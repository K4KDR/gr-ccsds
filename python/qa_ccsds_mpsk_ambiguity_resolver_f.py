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

class qa_ccsds_mpsk_ambiguity_resolver_f (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def to_hex(self, dec):
		return '0x%02X' % (dec)

	def runBlock(self, src_data, exp_data, M, ASM='CA', ASM_len=8, ber_thresh=0.8, frame_len=1, num_tail=0, errors=False):
		#print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
		#raw_input ('Press Enter to continue: ')
		
		##################################################
		# Variables
		##################################################
		self.M = M
		self.ldM = ldM = int(math.log(M)/math.log(2))

		##################################################
		# Blocks
		##################################################
		self.src = gr.vector_source_f(src_data, False, 1)
		self.snk = gr.vector_sink_b(1)
		self.ccsds_softbits_msg_to_bytes_b_0 = ccsds_swig.softbits_msg_to_bytes_b()
		self.ccsds_mpsk_ambiguity_resolver_f_0 = ccsds_swig.mpsk_ambiguity_resolver_f(M,ASM,ASM_len,1,ber_thresh,frame_len,num_tail)

		##################################################
		# Connections
		##################################################
		self.tb.connect((self.src, 0), (self.ccsds_mpsk_ambiguity_resolver_f_0, 0))
		self.tb.connect((self.ccsds_softbits_msg_to_bytes_b_0, 0), (self.snk, 0))

		##################################################
		# Asynch Message Connections
		##################################################
		self.tb.msg_connect(self.ccsds_mpsk_ambiguity_resolver_f_0, "out", self.ccsds_softbits_msg_to_bytes_b_0, "in")

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

	'''
	def test_bpsk_ofst0_amb0(self):
		src_data =     [ 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM   0xCA
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,  # Frame 0x00
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM   0xCA
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,  # Frame 0xFF
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM   0xCA
				 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # Frame 0xAA
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0 ] # ASM   0xCA
		src_data = tuple(src_data)

		exp_data = (0x00,0xFF,0xAA)
		self.runBlock(src_data,exp_data,2)
	#'''

	'''
	def test_bpsk_ofst1_amb0(self):
		src_data =     [      1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0 ]
		src_data = tuple(src_data)

		exp_data = (0xFF,0xAA)
		self.runBlock(src_data,exp_data,2)
	#'''

	'''
	def test_bpsk_ofst0_amb1(self):
		src_data =     [-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0 ]
		src_data = tuple(src_data)

		exp_data = (0x00,0xFF,0xAA)
		self.runBlock(src_data,exp_data,M=2)
	#'''

	'''
	def test_bpsk_ofst1_amb1(self):
		src_data =     [     -1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0 ]
		src_data = tuple(src_data)

		exp_data = (0xFF,0xAA)
		self.runBlock(src_data,exp_data,M=2)
	#'''

	'''
	def test_bpsk_ofst3_amb0_ber(self):
		src_data =     [               -1.0, 1.0,-1.0, 1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
				 1.0,-1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0,
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0 ]
		src_data = tuple(src_data)

		exp_data = (0xFF,)
		self.runBlock(src_data,exp_data,2)
	#'''




	################################################################################
	# 8-PSK
	################################################################################

	'''
	def test_8psk_ofst0_amb0(self):
		src_data =     [ 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,  # FD1  0x00
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,  # FD2  0xFF
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM
				 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # FD3  0xAA
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0           ] # ASM
		src_data = tuple(src_data)

		exp_data = (0x00,0xFF,0xAA)
		self.runBlock(src_data,exp_data,8)
	#'''

	'''
	def test_8psk_ofst6_amb5(self):
		src_data =     [                               1.0, 1.0, # 0x00
				-1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, # ASM
				 1.0,-1.0,-1.0, 1.0,-1.0,-1.0, 1.0,-1.0, # FD1  0x00
				 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,-1.0, # ASM
				 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, # FD2  0xFF
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0, # ASM
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0,-1.0, # FD3  0xAA
				 1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0      ]# ASM
		src_data = tuple(src_data)

		#FIXME the ambiguity stream mimics the (very short) ASM.
		# in the current AR implementation lower ambiguity numbers have
		# priority and thus the "wrong" message 0x4A is returned instead
		# of 0x00. The main problem is that a different implementation
		# could prefer the right ambiguity and return 0x00. This would
		# make the test fail although the behaviour of the AR block was
		# fine, since both 0x4A and 0x00 are correct returns.
		#exp_data = (0x00,0xFF,0xAA)
		exp_data = (0x4A,0xFF,0xAA)

		self.runBlock(src_data,exp_data,8)
	#'''

	'''
	def test_8psk_ofst6_amb5_reord(self):
		src_data =     [                               1.0, 1.0, # 0x00
				-1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0, 1.0, # ASM
				 1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, # FD2  0xFF
				 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0, # ASM
				 1.0,-1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0, # FD1  0x00
				 1.0, 1.0, 1.0, 1.0,-1.0, 1.0,-1.0, 1.0, # ASM
				-1.0,-1.0, 1.0, 1.0,-1.0, 1.0,-1.0,-1.0, # FD3  0xAA
				 1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0      ]# ASM
		src_data = tuple(src_data)

		# Reordered data in a way that the ambiguity sequence no longer
		# mimics the ASM. (exchanged FD1 and FD2) tis test is
		# independent of the AR implementation in contrast to the test
		# above
		exp_data = (0xFF,0x00,0xAA)

		self.runBlock(src_data,exp_data,M=8)
	#'''

	'''
	def test_8psk_ofst2_amb1(self):
		src_data =     [          -1.0, 1.0, 1.0, 1.0, 1.0,-1.0,  # ASM
				-1.0,-1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,  # FD1  0x00
				 1.0,-1.0,-1.0,-1.0, 1.0,-1.0,-1.0,-1.0,  # ASM
				 1.0,-1.0, 1.0,-1.0, 1.0, 1.0,-1.0, 1.0,  # FD2  0xFF
				 1.0, 1.0, 1.0, 1.0, 1.0,-1.0, 1.0,-1.0,  # ASM
				-1.0, 1.0, 1.0,-1.0, 1.0,-1.0,-1.0,-1.0,  # FD3  0xAA
				 1.0,-1.0,-1.0, 1.0, 1.0, 1.0, 1.0,-1.0 ] # ASM
		src_data = tuple(src_data)

		exp_data = (0xFF,0xAA)

		self.runBlock(src_data,exp_data,8)
	#'''


	################################################################################
	# BPSK with tails
	################################################################################

	#'''
	def test_bpsk_ofst2_amb0(self):
		src_data =     [ 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM   0xCA
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,  # Frame 0x00
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM   0xCA
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,  # Frame 0xFF
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # ASM   0xCA
				 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0,  # Frame 0xAA
				 1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0 ] # ASM   0xCA
		src_data = tuple(src_data)

		exp_data = (0x00,0xC0,0xFF,0xC0,0xAA,0xC0)

		print 'This test will generate 3 warnings about non integer number of bytes. These warnings are safe to ignore for this test.'

		self.runBlock(src_data,exp_data,2,ASM='28',ASM_len=6,num_tail=2)
	#'''

if __name__ == '__main__':
	gr_unittest.main ()
