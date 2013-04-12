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
import ccsds_swig
import os
import math

class qa_ccsds_mpsk_ambiguity_resolver_bb (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def to_hex(self, dec):
		return '0x%02X' % (dec)

	def runBlock(self, src_data, exp_data, M, frame_len,ber_threshold,errors=False):
		##################################################
		# Variables
		##################################################
		self.M = M
		self.ldM = ldM = int(math.log(M)/math.log(2))
		self.frame_len = frame_len
		self.ASM = ASM = '1ACFFC1D'

		##################################################
		# Blocks
		##################################################
		self.unpacked_to_packed = gr.unpacked_to_packed_bb(ldM, gr.GR_MSB_FIRST)
		self.src = gr.vector_source_b(src_data, False, 1)
		self.snk = gr.vector_sink_b(1)
		self.packed_to_unpacked = gr.packed_to_unpacked_bb(ldM, gr.GR_MSB_FIRST)
		self.AR = ccsds_swig.mpsk_ambiguity_resolver_bb(M,ASM,0,ber_threshold,frame_len)

		##################################################
		# Connections
		##################################################
		self.tb.connect((self.AR, 0), (self.unpacked_to_packed, 0))
		self.tb.connect((self.src, 0), (self.packed_to_unpacked, 0))
		self.tb.connect((self.packed_to_unpacked, 0), (self.AR, 0))
		self.tb.connect((self.unpacked_to_packed, 0), (self.snk, 0))

		# start
		self.tb.start()
		self.tb.wait()

		out_str     = str(self.snk.data())
		exp_str     = str(exp_data) 
		out_str_hex = str(map(self.to_hex,self.snk.data()))
		exp_str_hex = str(map(self.to_hex,exp_data))
		
		if(not errors):
			self.assertEqual(exp_data, self.snk.data(),
				'Output is not matching the expexted result:\nOutput (hex)  : %s\nExpected (hex): %s\nOutput (dec)  : %s\nExpected (dec): %s' %
				(out_str_hex, exp_str_hex, out_str, exp_str)  )
		else:
			self.assertNotEqual(exp_data, self.snk.data(),
				'Output is matching the expexted result although mismatch was expected:\nOutput (hex)  : %s\nExpected (hex): %s\nOutput (dec)  : %s\nExpected (dec): %s' %
				(out_str_hex, exp_str_hex, out_str, exp_str)  )


	################################################################################
	################################################################################
	#################################             ##################################
	#################################    Tests    ##################################
	#################################             ##################################
	################################################################################
	################################################################################




	################################################################################
	# BPSK
	################################################################################

	#'''
	def test_bpsk_ofst3_amb0(self):
		src_data = [0x43,0x59,0xFF,0x83,0xB5,0x55,0x55,0x55,0x55] * 5
		src_data.append(0x40)
		src_data = tuple(src_data)

		exp_data = tuple([0x43,0x59,0xFF,0x83,0xB5,0x55,0x55,0x55,0x55] * 5)
		self.runBlock(src_data,exp_data,2,5,0)
	#'''

	#'''
	def test_bpsk_ofst1_amb1(self):
		src_data = [0x72,0x98,0x01,0xF1,0x2A,0xAA,0xAA,0xAA,0xAA] * 5
		src_data.append(0x80)
		src_data = tuple(src_data)

		exp_data = tuple([0x8D,0x67,0xFE,0x0E,0xD5,0x55,0x55,0x55,0x55] * 5)
		self.runBlock(src_data,exp_data,2,5,0)
	#'''


	################################################################################
	# 8-PSK
	################################################################################

	#'''
	# Send ASM with over 8-PSK with no symbol offset, one bit offset, ambiguity=0 and no BER.
	# Expect success
	def test_8psk_ofst0_bitofst1_amb0(self):
		src_data = [0x0D,0x67,0xFE,0x0E,0xD5,0x55,0x55,0x55,0x55] * 5
		src_data.append(0x40)
		src_data.append(0x00)
		src_data.append(0x00)
		src_data = tuple(src_data)

		exp_data = tuple([0x0D,0x67,0xFE,0x0E,0xD5,0x55,0x55,0x55,0x55] * 5)
		self.runBlock(src_data,exp_data,8,5,0)
	#'''

	#'''
	# Send ASM with over 8-PSK with no symbol offset, one bit offset, ambiguity=0 and 1 BER in the ASM.
	# Expect success because the AR ber threshold is set to one
	def test_8psk_ofst0_bitofst1_amb0_1ber_success(self):
		src_data = [0x0D,0x67,0xFE,0x1E,0xD5,0x55,0x55,0x55,0x55] * 5
		src_data.append(0x40)
		src_data.append(0x00)
		src_data.append(0x00)
		src_data = tuple(src_data)

		exp_data = tuple([0x0D,0x67,0xFE,0x1E,0xD5,0x55,0x55,0x55,0x55] * 5)
		self.runBlock(src_data,exp_data,8,5,1, False)
	#'''


	#'''
	# Send ASM with over 8-PSK with one symbol offset, one bit offset, ambiguity=5 and no BER.
	# Expect success
	def test_8psk_ofst1_bitofst1_amb5(self):
		src_data = [0x2B,0xD9,0xC0,0x8B,0x93,0x4D,0x34,0xD3,0x4D] * 5
		src_data.append(0x40)
		src_data.append(0x00)
		src_data.append(0x00)
		src_data = tuple(src_data)

		exp_data = [0xA1,0xAC,0xFF,0xC1,0xDA,0xAA,0xAA,0xAA,0xAA] * 5
		exp_data = tuple(exp_data)
		self.runBlock(src_data,exp_data,8,5,0)
	#'''

	#'''
	# Send ASM with over 8-PSK with one symbol offset, one bit offset, ambiguity=5 and 1 BER.
	# Expect fail
	def test_8psk_ofst1_bitofst1_amb5_1ber_fail(self):
		src_data = [0x2B,0xD9,0xC1,0x8B,0x93,0x4D,0x34,0xD3,0x4D] * 5
		src_data.append(0x40)
		src_data.append(0x00)
		src_data.append(0x00)
		src_data = tuple(src_data)

		exp_data = [0xA1,0xAC,0xFF,0xC1,0xDA,0xAA,0xAA,0xAA,0xAA] * 5
		exp_data = tuple(exp_data)
		self.runBlock(src_data,exp_data,8,5,0,True)
	#'''

	#'''
	# Send ASM with over 8-PSK with one symbol offset, one bit offset, ambiguity=5 and 1 BER.
	# Expect success (BER threshold set to one)
	def test_8psk_ofst1_bitofst1_amb5_1ber_success(self):
		src_data = [0x2B,0xD9,0xC4,0x8B,0x93,0x4D,0x34,0xD3,0x4D] * 5
		src_data.append(0x40)
		src_data.append(0x00)
		src_data.append(0x00)
		src_data = tuple(src_data)

		exp_data = [0xA1,0xAC,0xFE,0xC1,0xDA,0xAA,0xAA,0xAA,0xAA] * 5
		exp_data = tuple(exp_data)
		self.runBlock(src_data,exp_data,8,5,1,False)
	#'''

if __name__ == '__main__':
	gr_unittest.main ()
