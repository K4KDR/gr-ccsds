#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: File Transfer Demonstration
# Author: Martin Luelf
# Generated: Wed Jan 23 10:36:19 2013
##################################################

execfile("/home/ngs1/.grc_gnuradio/mpsk_demod.py")
execfile("/home/ngs1/.grc_gnuradio/mpsk_mod.py")
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import ccsds

class file_transfer_demo(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "File Transfer Demonstration")

		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 2e5
		self.usrp_send_ip = usrp_send_ip = '192.168.10.2'
		self.usrp_recv_ip = usrp_recv_ip = '192.168.10.2'
		self.osf = osf = 4
		self.lo_offset = lo_offset = 2.5*samp_rate
		self.freq = freq = 2.2e9
		self.frame_len = frame_len = 40
		self.alpha = alpha = 0.5
		self.M = M = 4
		self.ASM = ASM = '1ACFFC1D'

		##################################################
		# Message Queues
		##################################################
		ccsds_frame_sync_bb_0_msgq_out = ccsds_ticp_frame_sink_b_0_msgq_in = gr.msg_queue(2)

		##################################################
		# Blocks
		##################################################
		self.uhd_usrp_source_0 = uhd.usrp_source(
			device_addr="",
			stream_args=uhd.stream_args(
				cpu_format="fc32",
				channels=range(1),
			),
		)
		self.uhd_usrp_source_0.set_samp_rate(samp_rate)
		self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(freq,lo_offset), 0)
		self.uhd_usrp_source_0.set_gain(0, 0)
		self.uhd_usrp_source_0.set_antenna("RX2", 0)
		self.uhd_usrp_sink_0 = uhd.usrp_sink(
			device_addr='ip=' + usrp_send_ip,
			stream_args=uhd.stream_args(
				cpu_format="fc32",
				channels=range(1),
			),
		)
		self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
		self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(freq,lo_offset), 0)
		self.uhd_usrp_sink_0.set_gain(0, 0)
		self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
		self.mpsk_mod_0 = mpsk_mod(
			osf=osf,
			alpha=alpha,
			M=M,
			samp_rate=samp_rate,
		)
		self.mpsk_demod_0 = mpsk_demod(
			alpha=alpha,
			ASM=ASM,
			frame_len=frame_len,
			M=M,
			samp_rate=samp_rate,
			osf=osf,
		)
		self.ccsds_ticp_frame_source_b_0 = ccsds.ticp_frame_source_b("localhost", 1737, 44)
		self.ccsds_ticp_frame_sink_b_0 = ccsds.ticp_frame_sink_b(1739, 40, ccsds_ticp_frame_sink_b_0_msgq_in)
		self.ccsds_frame_sync_bb_0 = ccsds.frame_sync_bb(ASM,1,0,frame_len,ccsds_frame_sync_bb_0_msgq_out)

		##################################################
		# Connections
		##################################################
		self.connect((self.mpsk_demod_0, 0), (self.ccsds_frame_sync_bb_0, 0))
		self.connect((self.ccsds_frame_sync_bb_0, 0), (self.ccsds_ticp_frame_sink_b_0, 0))
		self.connect((self.ccsds_ticp_frame_source_b_0, 0), (self.mpsk_mod_0, 0))
		self.connect((self.mpsk_mod_0, 0), (self.uhd_usrp_sink_0, 0))
		self.connect((self.uhd_usrp_source_0, 0), (self.mpsk_demod_0, 0))

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.set_lo_offset(2.5*self.samp_rate)
		self.mpsk_mod_0.set_samp_rate(self.samp_rate)
		self.mpsk_demod_0.set_samp_rate(self.samp_rate)
		self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
		self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)

	def get_usrp_send_ip(self):
		return self.usrp_send_ip

	def set_usrp_send_ip(self, usrp_send_ip):
		self.usrp_send_ip = usrp_send_ip

	def get_usrp_recv_ip(self):
		return self.usrp_recv_ip

	def set_usrp_recv_ip(self, usrp_recv_ip):
		self.usrp_recv_ip = usrp_recv_ip

	def get_osf(self):
		return self.osf

	def set_osf(self, osf):
		self.osf = osf
		self.mpsk_mod_0.set_osf(self.osf)
		self.mpsk_demod_0.set_osf(self.osf)

	def get_lo_offset(self):
		return self.lo_offset

	def set_lo_offset(self, lo_offset):
		self.lo_offset = lo_offset
		self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(self.freq,self.lo_offset), 0)
		self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(self.freq,self.lo_offset), 0)

	def get_freq(self):
		return self.freq

	def set_freq(self, freq):
		self.freq = freq
		self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(self.freq,self.lo_offset), 0)
		self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(self.freq,self.lo_offset), 0)

	def get_frame_len(self):
		return self.frame_len

	def set_frame_len(self, frame_len):
		self.frame_len = frame_len
		self.mpsk_demod_0.set_frame_len(self.frame_len)

	def get_alpha(self):
		return self.alpha

	def set_alpha(self, alpha):
		self.alpha = alpha
		self.mpsk_mod_0.set_alpha(self.alpha)
		self.mpsk_demod_0.set_alpha(self.alpha)

	def get_M(self):
		return self.M

	def set_M(self, M):
		self.M = M
		self.mpsk_mod_0.set_M(self.M)
		self.mpsk_demod_0.set_M(self.M)

	def get_ASM(self):
		return self.ASM

	def set_ASM(self, ASM):
		self.ASM = ASM
		self.mpsk_demod_0.set_ASM(self.ASM)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = file_transfer_demo()
	tb.run()

