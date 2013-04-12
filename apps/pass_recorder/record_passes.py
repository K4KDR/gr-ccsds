#!/usr/bin/env python

import subprocess
import re
import time
import logging
from optparse import OptionParser
import datetime
import sys

class PassCalcError(Exception):
	'''Exception when calculating pass information'''
	pass


def predict_next_pass_times(sat = 'PROBA-1', start_at = -1):
	'''
	Returns a tuple with the start and end time of the next pass of<sat>
	starting from now or from a given point in time (if <start_at> is given
	as a UNIX Timestamp). The returned start_time might be in the past (or
	smaller than the provided <start_at>) which means that there is a pass
	right now (or at the provided <start_at>)
	'''

	# line regex to match predict output
	line_match = re.compile(r"""
	^			# Start of line
	(\d+)			# Unix timestamp <-- Group 1
	\s+			# Whitespaces
	[A-Z][a-z]{2}		# Day of the week (e.g. TUE)
	\s+			# Whitespaces
	\d{2}[a-zA-Z]{3}\d{2}	# Date (e.g. 06Mar13)
	\s+			# Whitespaces
	\d{2}:\d{2}:\d{2}	# Time: Hour:Minute:Second
	(?:\s+\d+){7}		# Multiple values (elevation azimuth, orbital phase (mod 256), latitude (ssp), longitude (ssp), range, orbit number)
	\s+			# Whitespaces
	(?:\*|\+)?		# Spacecraft sunlight visibility
	$			# End of line
	""", re.VERBOSE)

	# argument list for predict
	args = ['predict', '-p', sat]

	if start_at >= 0 : # start time provided, append this to the argument list
		args.append(str(int(start_at)))

	# run predict
	process = subprocess.Popen(args, stdout=subprocess.PIPE)
	out,err = process.communicate()

	# find matching lines
	matches_lst = [line_match.match(line) for line in out.split('\n')]
	matches = [match for match in matches_lst if match is not None]

	# check if we found enough of them
	if len(matches) < 2:
		raise PassCalcError("Less than two lines found, this does not define a pass")

	# extract the times from the matches
	start_time = int(matches[ 0].group(1))
	end_time   = int(matches[-1].group(1))

	# return results
	return start_time, end_time

def get_tracking_start(sat = 'PROBA-1', min_el = 0, start = -1, end = -1):
	''' 
	Returns a tuple with start and end time of a pass taking the minimum
	elevation mask into account. If <start> time is not provided, the
	current time will be used. If <end> is not provided the time when the
	satellite falls below the minimum elevation for the first time is used.

	Returns (-1,-1) if no suitable pass is found.
	'''

	# line regex to match predict output
	line_match = re.compile(r"""
	^			# Start of line
	(\d+)			# Unix timestamp <-- Group 1
	\s+			# Whitespaces
	[A-Z][a-z]{2}		# Day of the week (e.g. TUE)
	\s+			# Whitespaces
	\d{2}[a-zA-Z]{3}\d{2}	# Date (e.g. 06Mar13)
	\s+			# Whitespaces
	\d{2}:\d{2}:\d{2}	# Time: Hour:Minute:Second
	\s+			# Whitespace
	(\d+)			# Elevation angle <-- Group 2
	(?:\s+\d+){6}		# Multiple values (azimuth, orbital phase (mod 256), latitude (ssp), longitude (ssp), range, orbit number)
	\s+			# Whitespaces
	(?:\*|\+)?		# Spacecraft sunlight visibility
	$			# End of line
	""", re.VERBOSE)

	# There might be more than one pass in the specified timeframe and we
	# are looking only for the first one that goes above the specified
	# elevation mask
	while True:
		try:		
			start_pass, end_pass = predict_next_pass_times(sat, start)
		except PassCalcError:
				
			if start < 0:
				# We want passes from now on. Since there is a
				# unsuitable pass right now, we need to look
				# into the future, so set the start time to now
				start = time.time()

			# How long into the future shall we look
			time_add = 60*15 # look 15 Minutes later (no satellite will be visible again that fast, so we should not miss anything during that)

			# Log this exception
			logging.debug("Found incomplete pass definition starting at %s, look for passes starting at %s",
				datetime.datetime.fromtimestamp(start).isoformat(),
				datetime.datetime.fromtimestamp(start+time_add).isoformat() )

			# Check again for a future time
			start += time_add

			continue


		# By now we should have found a valid path's start and end time,
		# so continue with checking

		if end > 0 : #End time has been specified
			if start_pass >= end: # No pass found before end
				return -1,-1
			elif end_pass > end: # Pass ends after requested end time
				end_pass = end

		# argument list for predict
		args = ['predict', '-f', str(sat), str(start_pass), str(end_pass)]

		# run predict
		out, __ = (subprocess.Popen(args, stdout=subprocess.PIPE)).communicate()

		# find matching lines
		matches = [line_match.match(line) for line in out.split('\n')]

		# check if we found enough of them
		if len(matches) < 2:
			raise PassCalcError("Less than two lines found, this does not define a pass")

		# extract times of lines with high enough elevation
		times   = [int(match.group(1)) for match in matches if match is not None and int(match.group(2)) >= min_el]
		
		# If there are less than two there is no pass with the minimum elevation or the pass will last less than a second, so ignore it
		if len(times) > 2:
			return (times[0], times[-1])

		start = end_pass

def record_pass(command, start, end):
		# Spend the last few seconds in a short sleep cycle.
		while time.time() < (start-0.1):
			time.sleep(0.1)

		
		# Log and print out the command invocation
		logging.info("Running command: %s",' '.join(command))
		print "Running command: %s" % ' '.join(command)

		# Run the command
		try:
			process = subprocess.Popen(command)
		except Exception as e:
			logging.critical("Error when starting command: %s\nScript will exit with return 1",str(e))
			sys.exit(1)
		
		# Wait until process returns, or end of pass is reached
		while process.poll() is None and time.time() < end:
			time.sleep(1.0)

		# if process is still running
		if process.returncode is None:

			# log that we finished			
			logging.info("Stop recording command by sending SIGINT")

			# send SIGTERM to exit gracefully
			process.terminate()

			# wait another 5 seconds for the process to exit
			timeout = 5.0
			while process.poll() is None and timeout > 0:
				time.sleep(0.5)
				timeout -= 0.5

			# process still did not finish
			if process.returncode is None:
				logging.warning("Command did not respond to SIGINT, sending SIGKILL now")

				process.kill()
		else:
			logging.debug("Record command did exit on its own")

		# log call to communicate
		logging.debug("Fetching all output of the command and wait for final termination (if not terminated yet)")

		# fetch all possible output and wait for process to end
		out,err = process.communicate()

		# log returncode
		logging.info("Record command returned %s",str(process.returncode))

if  __name__ =='__main__':
	###
	### Main
	###

	parser = OptionParser(usage="usage: %prog [Options] Satellite RecordCommand [Record Params]")
	parser.add_option("-l", "--logfile", dest="logfile", default="record-passes.log",
		          help="Name of a log file to use. Default is %default", metavar="FILE")
	parser.add_option("--loglevel", dest="loglevel", default="INFO",
		          help="Loglevel. Options are: DEBUG, INFO, WARNING, CRITICAL. Default is %default")
	parser.add_option("-n", "--num-pases", dest="num_passes", type="int", default=0,
		          help="Number of passes after which recording is stopped. By default an arbitrary large number of passes is recorded.")
	parser.add_option("-e", "--min_elevation", dest="min_e", type="int", default=0,
		          help="Minimum elevation angle (in degrees) that is required for a line of sight to the satellite. Default is %default")

	(options, args) = parser.parse_args()

	numeric_level = getattr(logging, options.loglevel.upper(), None)
	if not isinstance(numeric_level, int):
	    raise ValueError('Invalid log level: %s' % loglevel)

	# check if the satellite name was provided
	if len(args) < 2:
		parser.error("incorrect number of arguments")

	# Set up logging
	logging.basicConfig(filename=options.logfile,format='%(asctime)s %(levelname)s: %(message)s', datefmt='%d.%m.%Y %H:%M:%S', level=numeric_level)

	if options.num_passes is 0:
		logging.info("Recording started for '%s' with a minimum elevation mask of %d degree", args[0], options.min_e)
	else:
		logging.info("Recording of %d passes started for '%s' with a minimum elevation mask of %d degree", options.num_passes, args[0], options.min_e)

	# Keep track how many passes we recorded so far
	passes_recorded = 0

	# Generate append string for logging output
	if options.num_passes is 0:
		num_passes_str = ''
	else:
		num_passes_str = '/%d' % options.num_passes
	pass_str = 'pass'

	# Loop through the passes
	while options.num_passes is 0 or passes_recorded < options.num_passes:
		# Only start recording if start and stop are different time instances
		while True:
			now = time.time()
			start, end = get_tracking_start(sat = args[0], min_el=options.min_e, start=now)

			if end > start and start > 0 and end > now:
				break

			time.sleep(1.0)

		logging.debug("It is now %s, next pass starting at %s and ending at %s",
			datetime.datetime.fromtimestamp(now).isoformat(),
			datetime.datetime.fromtimestamp(start).isoformat(),
			datetime.datetime.fromtimestamp(end).isoformat() )

		# How long to we have to sleep in order not to miss the next pass (wake up 5 seconds earlier, to be sure)
		sleep_time = max(start-now-5, 0)

		# Log the long time sleep
		logging.debug("Next pass predicted at %s, sleeping for %d seconds in order to wake up 5 seconds before the pass", datetime.datetime.fromtimestamp(start).isoformat(), sleep_time)

		# Sleep for a (possible) long time
		time.sleep(sleep_time)

		# Log the fact, that we woke up
		logging.debug("Waking up now for the upcomming pass")

		# All other parameters must be the command itself and it's parameters
		record_pass(args[1:], start, end)

		# Increase pass counter
		passes_recorded += 1

		# Log recording of this pass
		logging.info('%d%s %s recorded',passes_recorded,num_passes_str,pass_str)

		# After the last logging, from now on we have more than one pass, so change text to plural
		pass_str = 'passes'

	logging.info("Recording complete, exit")
