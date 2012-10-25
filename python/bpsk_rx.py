#!/usr/bin/env python
#
# Copyright 2012
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gru
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from string import maketrans
# import scipy
# import numpy
# from numpy import asarray

# From gr-digital
from gnuradio import digital
from gnuradio.digital import digital_swig

# From gr-extras
from gnuradio import extras
from gnuradio.extras import extras_swig

# From gr-ccsds
import ccsds
from ccsds import ccsds_swig
import crc16

import gnuradio.gr.gr_threading as _threading

# for dynamicly named file output of the recorded live demod source
from datetime import datetime
# from current dir
from bpsk_demod import bpsk_demod

import struct
import sys

#import os
#print os.getpid()
#raw_input('Attach and press enter: ')

def hexstr2binstr(str,length=None):
	if length is None:
		length = len(str)*4
	return bin(int(str,16))[2:].zfill(length)

def int2binstr(num,length):
	return bin(num)[2:].zfill(length)

def hexstr2int(str):
	return int(str,16)

class frame_sync(gr.hier_block2):
	def __init__(self, options):
	# """
	# Hierarchical block for frame syncing.

		# @param options: command line and derived options
		# @type options: object
	# """
		gr.hier_block2.__init__(self, "frame_sync",
				gr.io_signature(1, 1, gr.sizeof_char), # Input signature
				gr.io_signature(0, 0, 0))					# Output signature

		access_code = options.access_code
		code_length = options.code_length

		self._access_code = hexstr2binstr(access_code)
		self._inv_access_code = self._access_code.translate(maketrans("10","01"))

		if options.viterbi27:
			# Convolutional encode the ASM so it can be searched for in the stream
			k = 7
			seed_length = k - 1
			length2encode = code_length - seed_length
			code_length = length2encode * 2
			self.encoder = ccsds.ccsds_conv_encode(hexstr2int(access_code) >> length2encode, k) #, 2, asarray([79, 91]))
			enc_access_code = self.encoder.encode(hexstr2int(access_code), length2encode, 79, 109)
			self._enc_access_code = int2binstr(enc_access_code,length2encode*2)
			self._enc_inv_access_code = self._enc_access_code.translate(maketrans("10","01"))
			# print "encoded ASM: %s" % str(hex(enc_access_code))
			# print "encoded ASM: %s" % str(self._enc_access_code)
			# print "encoded ASM: %s" % (enc_access_code)
			self._access_code = self._enc_access_code
			self._inv_access_code = self._enc_inv_access_code

		if options.sync_threshold == -1:
			threshold = code_length/10			  # default is 10% error count acceptable
		else:
			threshold = options.sync_threshold

		self.output_msgq = gr.msg_queue()		# the output message queue where identified frames will be 
												# sent in individual messages	
		# correlator looking for the non-inverted ASM
		self.correlator = digital_swig.correlate_access_code_bb(self._access_code, threshold)
		# correlator looking for the inverted ASM
		self.inv_correlator = digital_swig.correlate_access_code_bb(self._inv_access_code, threshold)

		# new framer that accepts parallel inputs from two correlators
		self.framer_sink = ccsds_swig.framer_sink_dual(self.output_msgq, options.frame_syms, options.asm_syms) 
																		# e.g. 20400, 64 or 10200, 32
		# connect the block input to the correlators
		# and the correlators to their respective port on the framer
		self.connect((self,0), self.correlator, (self.framer_sink,0))
		self.connect((self,0), self.inv_correlator, (self.framer_sink,1))
		# self.null_src = gr.null_source(gr.sizeof_char)
		# self.connect(self.null_src, (self.framer_sink,1))


class vit27_decode(gr.hier_block2):
	def __init__(self, options, input_msgq):
	# """
	# Hierarchical block for viterbi27 decoding.

		# @param options: command line and derived options
		# @type options: object
		# @param input_msgq: the output queue from the previous processing block
		# 						to be monitored for new messages that contain a single frame each,
		# @type input_msgq: gr_msg_queue_sptr
	# """
		gr.hier_block2.__init__(self, "vit27_decode",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0))					# Output signature
		# import os
		# print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
		# raw_input ('Press Enter to continue: ')
		self.output_msgq = gr.msg_queue()
		self._decoder = ccsds_swig.viterbi27_bb(self.output_msgq, options.frame_syms, hexstr2int(options.access_code), options.code_length);
		options.message_bits /= 2
		self._msg_input = ccsds_swig.msg_source(gr.sizeof_char, input_msgq)
		self.unpacker_bb = gr.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)
		self.connect(self._msg_input, self.unpacker_bb, self._decoder)


class pdr(gr.hier_block2):
	def __init__(self, options, input_msgq):
	# """
	# Hierarchical block for pseudo derandomiser.

		# @param options: command line and derived options
		# @type options: object
		# @param input_msgq: the output queue from the previous processing block
		# 						to be monitored for new messages that contain a single frame each,
		# @type input_msgq: gr_msg_queue_sptr
	# """
		gr.hier_block2.__init__(self, "pseudo de-randomiser",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0))					# Output signature
		# import os
		# print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
		# raw_input ('Press Enter to continue: ')
		self.output_msgq = gr.msg_queue()
		mask = 0xa9
		seed = 0xff # all ones
		length = 7 # workaround for bug in gnuradio core code
		self._pdr = ccsds_swig.randomiser_bb(self.output_msgq, options.message_bits, mask, seed, length);
		self._msg_input = ccsds_swig.msg_source(gr.sizeof_char, input_msgq)
		self.unpacker_bb = gr.packed_to_unpacked_bb(1,gr.GR_MSB_FIRST)
		self.connect(self._msg_input, self.unpacker_bb, self._pdr)


class reed_solomon(gr.hier_block2):
	def __init__(self, options, input_msgq):
	# """
	# Hierarchical block for RS decoder.

		# @param options: command line and derived options
		# @type options: object
		# @param input_msgq: the output queue from the previous processing block
		# 						to be monitored for new messages that contain a single frame each,
		# @type input_msgq: gr_msg_queue_sptr
	# """
		gr.hier_block2.__init__(self, "pseudo de-randomiser",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0))					# Output signature
		# import os
		# print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
		# raw_input ('Press Enter to continue: ')
		self.output_msgq = gr.msg_queue()
		self._rs = ccsds_swig.rs_decode_bb(self.output_msgq, options.message_bits)
		options.message_bits -= 5*32*8 # FIXME: do this properly
		self._msg_input = ccsds_swig.msg_source(gr.sizeof_char, input_msgq)
		self.connect(self._msg_input, self._rs)


class _queue_watcher_thread(_threading.Thread):
	def __init__(self, rcvd_pktq, callback, options):
		_threading.Thread.__init__(self)
		self.setDaemon(1)
		self.rcvd_pktq = rcvd_pktq
		self.callback = callback
		self.keep_running = True
		self.frame_check = options.frame_check
		self.start()


	def run(self):
		while self.keep_running:
			msg = self.rcvd_pktq.delete_head()
			payload = msg.to_string()
			ok = False
			if self.frame_check is True:
				(ok, m) = crc16.check_crc16(payload) # returns crc result and msg without crc field
			if self.callback:
				self.callback(ok, payload)


class receive_path(gr.hier_block2):
	def __init__(self, rx_callback, options):
		gr.hier_block2.__init__(self, "receive_path",
				gr.io_signature(1, 1, gr.sizeof_char),
				gr.io_signature(0, 0, 0))
		
		# options = copy.copy(options)	# make a copy so we can destructively modify

		self._verbose	 = options.verbose

		self._rx_callback = rx_callback  # this callback is fired when a frame arrives

		# Display some information about the setup
		# if self._verbose:
			# self._print_verbage()
		if options.frame_sync:
			self.frame_sync = frame_sync(options)
			input_msgq = self.frame_sync.output_msgq
			# connect block input to the frame receiver
			self.connect((self,0), (self.frame_sync,0))
		else: 
			input_msgq = gr.msg_queue()
			self.packer_bb = gr.unpacked_to_packed_bb(1,gr.GR_MSB_FIRST)
			self.msg_sink = gr.message_sink(gr.sizeof_char,input_msgq,False) # block if msgq is full
			self.connect((self,0), self.packer_bb, self.msg_sink)

		if options.viterbi27:
			self.decoder = vit27_decode(options,input_msgq)
			input_msgq = self.decoder.output_msgq
			self.connect(self.decoder)
		if options.derandomise:
			self.pdr = pdr(options,input_msgq)
			input_msgq = self.pdr.output_msgq
			self.connect(self.pdr)
		if options.reed_solomon:
			self.rs = reed_solomon(options,input_msgq)
			input_msgq = self.rs.output_msgq
			self.connect(self.rs)

		if options.inspect_queue:
			self._watcher = _queue_watcher_thread(input_msgq, rx_callback, options)
		elif options.to_file:
			self.file_sink = gr.file_sink(gr.sizeof_char, options.to_file)
			self._msg_input = ccsds_swig.msg_source(gr.sizeof_char, input_msgq)
			self.connect(self._msg_input, self.file_sink)

	
	def _print_verbage(self):
		"""
		Prints information about the receive path
		"""
		# print "\nReceive Path:"
		# print "modulation:	  %s"	% (self._demod_class.__name__)
		# print "bitrate:		 %sb/s" % (eng_notation.num_to_str(self._bitrate))
		# print "samples/symbol:  %.4f"	% (self.samples_per_symbol())
		# print "Differential:	%s"	% (self.differential())

class my_top_block(gr.top_block):
	def __init__(self, rx_callback, options):
		gr.top_block.__init__(self)

		# Set up receive path
		self.rxpath = receive_path(rx_callback, options) 
		if (options.from_usrp) :
			self.source = bpsk_demod(options.centre_freq, options.usrp_gain,
										options.sps, options.sym_rate)
			self.connect(self.source, self.rxpath)
			# record live demod source to file
			self.file_path = file_path = options.file_path
			self.file_out = file_out = file_path + datetime.now().strftime("%Y.%m.%d.%H.%M.%S") + ".dat"
			self.gr_file_sink = gr.file_sink(gr.sizeof_char, file_out)
			self.gr_file_sink.set_unbuffered(False)
			self.packer = gr.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
			self.connect(self.source, self.packer, self.gr_file_sink)

		elif(options.from_file is not None):
			sys.stderr.write(("Reading samples from '%s'.\n\n" % (options.from_file)))
			self.source = gr.file_source(gr.sizeof_char, options.from_file, options.repeat_file)
			self.unpacker_bb = gr.packed_to_unpacked_bb(1,gr.GR_MSB_FIRST)
			self._throttle = gr.throttle(gr.sizeof_char,1e6)
			self.connect(self.source, 
				# self._throttle, 
				self.unpacker_bb)
			self.connect(self.unpacker_bb, (self.rxpath, 0))
			
		else:
			sys.stderr.write("No source defined, pulling samples from null source.\n\n")
			self.source = gr.null_source(gr.sizeof_char)
			self.connect(self.source, self.rxpath)


# /////////////////////////////////////////////////////////////////////////////
#								   main
# /////////////////////////////////////////////////////////////////////////////

global n_rcvd, n_right

def main():
	global n_rcvd, n_right

	n_rcvd = 0
	n_lost = 0
	p_frameno = 256 # an impossible frame number to start with (8 bit counter, 0-255)
	lock = 0
	
	def rx_callback(ok, payload):
		global n_rcvd, n_lost, p_frameno, lock
		# print "\n\nnew msg\n%s" % (digital.packet_utils.conv_packed_binary_string_to_1_0_string(payload[2:3]))
		# print len(payload[2:3])
		(frameno,) = struct.unpack('!B', payload[2:3]) # payload is a packed binary string
		if n_rcvd:
			if lock < 3:
				if frameno == p_frameno + 1:
					lock += 1
				else:
					lock = 0
			elif frameno > p_frameno:
				if frameno != p_frameno + 1:
					n_lost += frameno-p_frameno-1
			else:
				if p_frameno != 255:
					n_lost += 255-p_frameno-1
				if frameno > 0:
					n_lost += frameno-1
		else:
			n_lost = 0
			lock = 0
		p_frameno = frameno
		n_rcvd += 1

		print "ok = %5s  frame_count = %4d  n_rcvd = %4d  n_lost = %4d" % (
			ok, frameno, n_rcvd, n_lost)


	# FIXME: implement a configuration file interface instead of command line options
	# Create Options Parser:
	parser = OptionParser (option_class=eng_option, conflict_handler="resolve")
	# expert_grp = parser.add_option_group("Expert")

	parser.add_option("","--from-usrp", action="store_true", default=False,
					  help="take live feed from USRP", dest="from_usrp")
	parser.add_option("", "--freq", default=2235e6, type="int",
					  help="USRP: centre frequency [default: %default]", dest="centre_freq")
	parser.add_option("", "--usrp-gain", default=10, type="int",
					  help="USRP: internal gain setting [default: %default]", dest="usrp_gain")
	parser.add_option("", "--sps", default=2, type="int",
					  help="USRP: samples per symbol [default: %default]", dest="sps")
	parser.add_option("", "--sym-rate", default=2e6, type="int",
					  help="USRP: symbol rate [default: %default]", dest="sym_rate")
	parser.add_option("", "--demod-out-filepath", default="/home/ngs1/demod_out/",
					  help="file path to the directory for recording demod output\
					   [default: %default]", dest="file_path")
	parser.add_option("","--from-file", default=None,
					  help="input file of samples to demod", dest="from_file")
	parser.add_option("-r", "--repeat-file", action="store_true", 
					  help="Repeat file contents as input?", dest="repeat_file")
	parser.add_option("", "--ASM", default="1ACFFC1D",
					  help="Attached Sync Marker as hex string [default: %default]", dest="access_code")
	parser.add_option("", "--threshold", default=5, type="int",
					  help="max number of allowable bit errors in ASM detection\
					  [default: %default, set to -1 for 10% of ASM length]", dest="sync_threshold")
	parser.add_option("", "--frame-syms", default=20400, type="int",
					  help="expected number of channel symbols per frame \
					  [default: %default]", dest="frame_syms")
	parser.add_option("", "--asm-syms", default=64, type="int",
					  help="expected number of channel symbols per sync marker \
					  [default: %default]", dest="asm_syms")
	parser.add_option("-I", "--inspect-queue", action="store_true", default=False, 
					  help="monitor the results of the decoding in the console,\
					  prevents the output of any decoded data to a file,\
					  i.e. overrides the following option", dest="inspect_queue")
	parser.add_option("", "--to-file", dest="to_file", 
					  help="record the decoded frames to a binary file", default=None)
	parser.add_option("-F", "--frame-sync", action="store_true", default=False, dest="frame_sync")
	parser.add_option("-V", "--viterbi27", action="store_true", default=False, dest="viterbi27")
	parser.add_option("-P", "--pseudo-derandomise", action="store_true", default=False, dest="derandomise")
	parser.add_option("-R", "--reed-solomon", action="store_true", default=False, dest="reed_solomon")
	parser.add_option("-C", "--frame-check", action="store_true", default=False, dest="frame_check")
	parser.add_option("-v", "--verbose", action="store_true", default=False)

	(options, args) = parser.parse_args ()

	if len(args) != 0:
		parser.print_help(sys.stderr)
		sys.exit(1)

	if options.from_usrp is not True and options.from_file is None:
		sys.stderr.write("You must specify --from-usrp or --from-file\n")
		parser.print_help(sys.stderr)
		sys.exit(1)

	if options.from_usrp is True and options.frame_sync is False:
		parser.print_help(sys.stderr)
		raw_input("\n\nYou have defined the USRP as the source but not activated the Frame Sync, is this correct?\nHit [Enter] if yes, Ctrl-C if not.\n")
		# sys.exit(1)

	if options.inspect_queue is not True and options.to_file is None:
		sys.stderr.write("You must specify --inspect-queue or --to-file\n")
		parser.print_help(sys.stderr)
		sys.exit(1)
		
	options.message_bits = options.frame_syms
	if options.access_code is None:
		options.access_code = '1ACFFC1D'
	options.code_length = (len(options.access_code)*4)
	
	# build the graph
	tb = my_top_block(rx_callback, options)

	r = gr.enable_realtime_scheduling()
	if r != gr.RT_OK:
		print "Warning: Failed to enable realtime scheduling."

	tb.start()		# start flow graph
	tb.wait()		 # wait for it to finish

if __name__ == '__main__':
	try:
		main()
	except KeyboardInterrupt:
		pass
