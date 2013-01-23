#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: M-PSK Modulation Chain
# Author: Martin Luelf
# Description: A block for modulation and signal forming of an M-PSK signal.
# Generated: Wed Jan 23 10:35:24 2013
##################################################

from gnuradio import gr
from gnuradio.gr import firdes
import ccsds
import math

class mpsk_mod(gr.hier_block2):

	def __init__(self, osf=4, alpha=0.5, M=4, samp_rate=32000):
		gr.hier_block2.__init__(
			self, "M-PSK Modulation Chain",
			gr.io_signature(1, 1, gr.sizeof_char*1),
			gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
		)

		##################################################
		# Parameters
		##################################################
		self.osf = osf
		self.alpha = alpha
		self.M = M
		self.samp_rate = samp_rate

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = samp_rate/osf
		self.ldM = ldM = int(math.log(M)/math.log(2))

		##################################################
		# Blocks
		##################################################
		self.root_raised_cosine_filter_0_0 = gr.interp_fir_filter_ccf(osf, firdes.root_raised_cosine(
			osf*0.7, samp_rate, sym_rate, 0.5, 11*osf))
		self.gr_packed_to_unpacked_xx_0 = gr.packed_to_unpacked_bb(ldM, gr.GR_MSB_FIRST)
		self.ccsds_mpsk_mod_bc_0 = ccsds.mpsk_mod_bc(M)

		##################################################
		# Connections
		##################################################
		self.connect((self.ccsds_mpsk_mod_bc_0, 0), (self.root_raised_cosine_filter_0_0, 0))
		self.connect((self.root_raised_cosine_filter_0_0, 0), (self, 0))
		self.connect((self, 0), (self.gr_packed_to_unpacked_xx_0, 0))
		self.connect((self.gr_packed_to_unpacked_xx_0, 0), (self.ccsds_mpsk_mod_bc_0, 0))

	def get_osf(self):
		return self.osf

	def set_osf(self, osf):
		self.osf = osf
		self.set_sym_rate(self.samp_rate/self.osf)
		self.root_raised_cosine_filter_0_0.set_taps(firdes.root_raised_cosine(self.osf*0.7, self.samp_rate, self.sym_rate, 0.5, 11*self.osf))

	def get_alpha(self):
		return self.alpha

	def set_alpha(self, alpha):
		self.alpha = alpha

	def get_M(self):
		return self.M

	def set_M(self, M):
		self.M = M
		self.set_ldM(int(math.log(self.M)/math.log(2)))

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.set_sym_rate(self.samp_rate/self.osf)
		self.root_raised_cosine_filter_0_0.set_taps(firdes.root_raised_cosine(self.osf*0.7, self.samp_rate, self.sym_rate, 0.5, 11*self.osf))

	def get_sym_rate(self):
		return self.sym_rate

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.root_raised_cosine_filter_0_0.set_taps(firdes.root_raised_cosine(self.osf*0.7, self.samp_rate, self.sym_rate, 0.5, 11*self.osf))

	def get_ldM(self):
		return self.ldM

	def set_ldM(self, ldM):
		self.ldM = ldM


