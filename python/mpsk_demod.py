#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: M-PSK Demodulation Chain
# Author: Martin Luelf
# Description: A block for synchronization and demodulation of M-PSK signals.
# Generated: Mon Jan 21 18:04:54 2013
##################################################

from gnuradio import gr
from gnuradio.gr import firdes
import ccsds

class mpsk_demod(gr.hier_block2):

	def __init__(self, alpha=0.5, ASM='1ACFFC1D', frame_len=40, M=4, samp_rate=32000, osf=4):
		gr.hier_block2.__init__(
			self, "M-PSK Demodulation Chain",
			gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
			gr.io_signature(1, 1, gr.sizeof_char*1),
		)

		##################################################
		# Parameters
		##################################################
		self.alpha = alpha
		self.ASM = ASM
		self.frame_len = frame_len
		self.M = M
		self.samp_rate = samp_rate
		self.osf = osf

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = samp_rate/osf

		##################################################
		# Message Queues
		##################################################
		ccsds_pll_cc_0_msgq_out = ccsds_local_oscillator_cc_0_msgq_in = gr.msg_queue(2)

		##################################################
		# Blocks
		##################################################
		self.root_raised_cosine_filter_0_1 = gr.fir_filter_ccf(1, firdes.root_raised_cosine(
			1, samp_rate, sym_rate, alpha, 11*osf))
		self.gr_throttle_0_0_0 = gr.throttle(gr.sizeof_gr_complex*1, sym_rate)
		self.ccsds_pll_cc_0 = ccsds.pll_cc(M, 1e-2,ccsds_pll_cc_0_msgq_out)
		self.ccsds_mpsk_demod_cb_0 = ccsds.mpsk_demod_cb(M)
		self.ccsds_mpsk_ambiguity_resolver_bb = ccsds.mpsk_ambiguity_resolver_bb(M,ASM,1,frame_len)
		self.ccsds_local_oscillator_cc_0 = ccsds.local_oscillator_cc(4,osf,ccsds_local_oscillator_cc_0_msgq_in)
		self.ccsds_dll_cc_0 = ccsds.dll_cc(osf,0.1)

		##################################################
		# Connections
		##################################################
		self.connect((self.ccsds_mpsk_ambiguity_resolver_bb, 0), (self, 0))
		self.connect((self.ccsds_local_oscillator_cc_0, 0), (self.root_raised_cosine_filter_0_1, 0))
		self.connect((self.root_raised_cosine_filter_0_1, 0), (self.ccsds_dll_cc_0, 0))
		self.connect((self, 0), (self.ccsds_local_oscillator_cc_0, 0))
		self.connect((self.ccsds_dll_cc_0, 0), (self.gr_throttle_0_0_0, 0))
		self.connect((self.gr_throttle_0_0_0, 0), (self.ccsds_pll_cc_0, 0))
		self.connect((self.ccsds_pll_cc_0, 0), (self.ccsds_mpsk_demod_cb_0, 0))
		self.connect((self.ccsds_mpsk_demod_cb_0, 0), (self.ccsds_mpsk_ambiguity_resolver_bb, 0))

	def get_alpha(self):
		return self.alpha

	def set_alpha(self, alpha):
		self.alpha = alpha
		self.root_raised_cosine_filter_0_1.set_taps(firdes.root_raised_cosine(1, self.samp_rate, self.sym_rate, self.alpha, 11*self.osf))

	def get_ASM(self):
		return self.ASM

	def set_ASM(self, ASM):
		self.ASM = ASM

	def get_frame_len(self):
		return self.frame_len

	def set_frame_len(self, frame_len):
		self.frame_len = frame_len

	def get_M(self):
		return self.M

	def set_M(self, M):
		self.M = M

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.set_sym_rate(self.samp_rate/self.osf)
		self.root_raised_cosine_filter_0_1.set_taps(firdes.root_raised_cosine(1, self.samp_rate, self.sym_rate, self.alpha, 11*self.osf))

	def get_osf(self):
		return self.osf

	def set_osf(self, osf):
		self.osf = osf
		self.set_sym_rate(self.samp_rate/self.osf)
		self.root_raised_cosine_filter_0_1.set_taps(firdes.root_raised_cosine(1, self.samp_rate, self.sym_rate, self.alpha, 11*self.osf))

	def get_sym_rate(self):
		return self.sym_rate

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.root_raised_cosine_filter_0_1.set_taps(firdes.root_raised_cosine(1, self.samp_rate, self.sym_rate, self.alpha, 11*self.osf))


