#!/usr/bin/env python
# 
# Copyright 2012 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import ccsds_swig as ccsds
import os

class qa_conv_encode27_bb (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def to_hex(self, dec):
		return '0x%02X' % (dec)

	def runBlock(self, src_data, exp_data, poly_c1, poly_c2, punct_t):
		##################################################
		# Blocks
		##################################################
		self.src = blocks.vector_source_b(src_data, False)
		self.snk = blocks.vector_sink_b(1)
		self.encoder = ccsds.conv_encode27_bb(poly_c1, poly_c2, punct_t)
	
		##################################################
		# Connections
		##################################################
		self.tb.connect((self.src, 0), (self.encoder, 0))
		self.tb.connect((self.encoder, 0), (self.snk, 0))

		# start
		self.tb.start()
		self.tb.wait()

		out_str     = str(self.snk.data())
		exp_str     = str(exp_data) 
		out_str_hex = str(map(self.to_hex,self.snk.data()))
		exp_str_hex = str(map(self.to_hex,exp_data))
		self.assertEqual(exp_data, self.snk.data(),
			'Output is not matching the expexted result:\nOutput (hex)  : %s\nExpected (hex): %s\nOutput (dec)  : %s\nExpected (dec): %s' %
			(out_str_hex, exp_str_hex, out_str, exp_str)  )



	################################################################################
	################################################################################
	#################################             ##################################
	#################################    Tests    ##################################
	#################################             ##################################
	################################################################################
	################################################################################





	##################################################
	# Rate 1/2
	##################################################

	#'''
	def test_zeros_r12(self):
		src_data = (0x00,0x00,0x00,0x00)
		exp_data = (0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55)
		self.runBlock(src_data,exp_data,0x79,0xDB,ccsds.NONE)
	#'''

	#'''
	def test_ones_r12(self):
		src_data = (0xFF,0xFF,0xFF,0xFF)
		exp_data = (0x8C,0x1A,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA)
		self.runBlock(src_data,exp_data,0x79,0xDB,ccsds.NONE)
	#'''

	#'''
	def test_asm_r12(self):
		src_data = (0x1A,0xCF,0xFC,0x1D)
		exp_data = (0x56,0x08,0x1C,0x97,0x1A,0xA7,0x3D,0x3E)
		self.runBlock(src_data,exp_data,0x79,0xDB,ccsds.NONE)
	#'''



	##################################################
	# Rate 2/3
	##################################################

	#'''
	def test_zeros_r23(self):
		src_data = (0x00,0x00,0x00,0x00)
		exp_data = (0x00,0x00,0x00,0x00,0x00,0x00)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_23)
	#'''

	#'''
	def test_asm_r23(self):
		src_data = (0x1A,0xCF,0xFC,0x1D)
		exp_data = (0x05,0xF5,0x70,0x5F,0x85,0x15)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_23)
	#'''

	##################################################
	# Rate 3/4
	##################################################

	#'''
	def test_zeros_r34(self):
		src_data = (0x00,0x00,0x00)
		exp_data = (0x00,0x00,0x00,0x00)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_34)
	#'''

	#'''
	def test_asm_r34(self):
		src_data = (0x1A,0xCF,0xFC,0x1D,0x00,0x00)
		exp_data = (0x0E,0xE0,0xCA,0xFD,0x53,0xA7,0x60,0x00)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_34)
	#'''


	##################################################
	# Rate 5/6
	##################################################

	#'''
	def test_zeros_r56(self):
		src_data = (0x00,0x00,0x00,0x00,0x00)
		exp_data = (0x00,0x00,0x00,0x00,0x00,0x00)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_56)
	#'''

	#'''
	def test_asm_r56(self):
		src_data = (0x1A,0xCF,0xFC,0x1D,0x00)
		exp_data = (0x09,0xAB,0x2B,0xE2,0x3A,0xBC)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_56)
	#'''



	##################################################
	# Rate 7/8
	##################################################

	#'''
	def test_zeros_r78(self):
		src_data = (0x00,0x00,0x00,0x00,0x00,0x00,0x00)
		exp_data = (0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_78)
	#'''

	#'''
	def test_asm_r78(self):
		src_data = (0x1A,0xCF,0xFC,0x1D,0x00,0x00,0x00)
		exp_data = (0x0B,0x62,0x17,0xCC,0x4A,0xF0,0x00,0x00)
		self.runBlock(src_data,exp_data,0x79,0x5B,ccsds.ECSS_78)
	#'''



if __name__ == '__main__':
	gr_unittest.run(qa_conv_encode27_bb, "qa_conv_encode27_bb.xml")
