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
import random
import numpy
import os
from gruel import pmt
import time
import math

class qa_ccsds_conv_decode27(gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
        	self.tb = None

	def to_hex(self, dec):
		return '0x%02X' % (dec)

    	def runExperiment(self, messages_in, messages_out_exp,poly1=0x79,poly2=0xDB,punct=ccsds_swig.NONE,ASM='1ACFFC1D'):
		#print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
		#raw_input ('Press Enter to continue: ')


		##################################################
		# Blocks
		##################################################
		self.null_src = ccsds_swig.msg_null_src()
		self.decoder = ccsds_swig.conv_decode27(poly1, poly2, punct, len(messages_out_exp[0])*8, ASM)
		self.dbg = gr.message_debug()

		##################################################
		# Asynch Message Connections
		##################################################
		self.tb.msg_connect(self.null_src, "out", self.decoder, "in")
		self.tb.msg_connect(self.decoder, "out", self.dbg, "store")
		#self.tb.msg_connect(self.decoder, "out", self.dbg, "print")

		# start flowgraph
		self.tb.start()

		block = self.decoder.to_basic_block()
		port = pmt.pmt_intern("in")

		num_messages = 0
		
		for i in xrange(len(messages_in)) :
			msg_in = pmt.pmt_make_f32vector(len(messages_in[i]), 0.0)
			for j in xrange(len(messages_in[i])) :
				pmt.pmt_f32vector_set(msg_in, j, messages_in[i][j])

			block._post( port, msg_in )
			num_messages = num_messages+1
		
		block._post( port, pmt.PMT_EOF )

		timeout = 50
		while(self.dbg.num_messages() < num_messages+1 and timeout > 0):
			time.sleep(0.1)
			timeout -= 1
		
		self.tb.stop()
		self.tb.wait()

		# test that all messages have been processed
		self.assertEqual (self.dbg.num_messages(), num_messages+1, '%d messages in store, %d expected.' % (self.dbg.num_messages(),(num_messages+1)))
		
		# test for EOF
		eof_msg = self.dbg.get_message(num_messages)
		self.assertEqual (pmt.pmt_is_eof_object(eof_msg), True, 'EOF block not at expected position')

		# test the blobs
		for i in xrange(len(messages_out_exp)):
			dbg_msg = self.dbg.get_message(i)
			dbg_data = []
			[dbg_data.append(pmt.pmt_u8vector_ref(dbg_msg, j)) for j in xrange(pmt.pmt_length(dbg_msg))]
			dbg_data = tuple(dbg_data)

			out_hex = map(self.to_hex,dbg_data)
			exp_hex = map(self.to_hex,messages_out_exp[i])
			out_str = str(dbg_data)
			exp_str = str(messages_out_exp[i])

			self.assertEqual (messages_out_exp[i], dbg_data, 'BLOB data mismatch at BLOB no %d/%d:\n  out (hex): %s\n  exp (hex): %s\n  out (plain): %s\n  exp (plain): %s' %
						(i+1, num_messages, out_hex, exp_hex, out_str, exp_str))


    	def runChainExperiment(self, poly1=0x79,poly2=0xDB,punct=ccsds_swig.NONE):
		##################################################
		# Variables
		##################################################
		self.M = M = 2
		self.ldM = ldM = int(math.log(M)/math.log(2))
		self.frame_len_byte = frame_len_byte = 8 #FIXME 210 if it should work for all puncturings
		self.num_frames = num_frames = 1

		data_in = []
		data_out = []

		for i in xrange(num_frames) :
			frame_data = map(int, numpy.random.randint(0, 256, frame_len_byte))

			# input vector (with ASM)
			[data_in.append(byte) for byte in [0x1A, 0xCF, 0xFC, 0x1D]] #ASM
			[data_in.append(byte) for byte in frame_data] #frame

			#output vector (without ASM)
			data_out.append(frame_data) #frame

		# Append one last ASM to reach decoder term state
		[data_in.append(byte) for byte in [0x1A, 0xCF, 0xFC, 0x1D]] #ASM

		#provide encoded ASM sequence
		asm_enc = {	ccsds_swig.NONE:	{ "asm":'81C971AA73D3E0', "asm_len": 52, "tail_len":12 }, # Rate 1/2
				ccsds_swig.ECSS_23:	{ "asm":'EAE0BF0A2A'    , "asm_len": 39, "tail_len": 9 }, # Rate 2/3
				ccsds_swig.ECSS_34:	{ "asm":'E0CAFD53A0'    , "asm_len": 35, "tail_len": 8 }, # Rate 3/4
				ccsds_swig.ECSS_56:	{ "asm":'AB2BE23A'      , "asm_len": 31, "tail_len": 8 }, # Rate 5/6
				ccsds_swig.ECSS_78:	{ "asm":'B10BE624'      , "asm_len": 30, "tail_len": 7 }  # Rate 7/8
			  }
		
		frame_len_sym = {	ccsds_swig.NONE:	int(2.0/1.0*frame_len_byte),
					ccsds_swig.ECSS_23:	int(3.0/2.0*frame_len_byte),
					ccsds_swig.ECSS_34:	int(4.0/3.0*frame_len_byte),
					ccsds_swig.ECSS_56:	int(6.0/5.0*frame_len_byte),
					ccsds_swig.ECSS_78:	int(8.0/7.0*frame_len_byte)
				}


		##################################################
		# Blocks
		##################################################
		self.src = gr.vector_source_b(data_in, False)
		self.encoder = ccsds_swig.conv_encode27_bb(poly1, poly2, punct)
		self.packed_to_unpacked = gr.packed_to_unpacked_bb(ldM, gr.GR_MSB_FIRST)
		self.mod = ccsds_swig.mpsk_mod_bc(M)
		self.demod = ccsds_swig.mpsk_detector_soft_cf(M)
		self.ar = ccsds_swig.mpsk_ambiguity_resolver_f(M,asm_enc[punct]['asm'],asm_enc[punct]['asm_len'],1,0.8,frame_len_sym[punct],asm_enc[punct]['tail_len'])
		self.decoder = ccsds_swig.conv_decode27(poly1, poly2, punct, frame_len_byte*8,'1ACFFC1D')
		self.dbg = gr.message_debug()
		
		##################################################
		# Connections
		##################################################
		self.tb.connect((self.src, 0), (self.encoder, 0))
		self.tb.connect((self.encoder, 0), (self.packed_to_unpacked, 0))
		self.tb.connect((self.packed_to_unpacked, 0), (self.mod, 0))
		self.tb.connect((self.mod, 0), (self.demod, 0))
		self.tb.connect((self.demod, 0), (self.ar, 0))
		
		##################################################
		# Asynch Message Connections
		##################################################
		self.tb.msg_connect(self.ar, "out", self.decoder, "in")
		self.tb.msg_connect(self.decoder, "out", self.dbg, "store")
		
		# start flowgraph
		self.tb.start()

		timeout = 50
		while(self.dbg.num_messages() < num_frames+1 and timeout > 0):
			time.sleep(0.1)
			timeout -= 1

		self.tb.stop()
		self.tb.wait()

		# test that all messages have been processed
		self.assertEqual (self.dbg.num_messages(), num_frames+1, '%d messages in store, %d expected.' % (self.dbg.num_messages(),(num_frames+1)))
		
		# test for EOF
		eof_msg = self.dbg.get_message(num_frames)
		self.assertEqual (pmt.pmt_is_eof_object(eof_msg), True, 'EOF block not at expected position')

		for i in xrange(num_frames) :
			dbg_msg = self.dbg.get_message(i)
			dbg_data = []
			[dbg_data.append(pmt.pmt_u8vector_ref(dbg_msg, j)) for j in xrange(pmt.pmt_length(dbg_msg))]
			dbg_data = tuple(dbg_data)

			out_hex = map(self.to_hex,dbg_data)
			exp_hex = map(self.to_hex,tuple(data_out[i]))
			out_str = str(dbg_data)
			exp_str = str(tuple(data_out[i]))

			err_str = 'Decoder output of length %u does not match encoder input of length %u for frame %u/%u\nexpected (hex): %s\noutput   (hex): %s\nexpected : %s\noutput   : %s' % (
					len(dbg_data), len(tuple(data_out[i])), i, num_frames, exp_hex, out_hex, exp_str, out_str
					)

			self.assertEqual (dbg_data, tuple(data_out[i]), err_str)


	##################################################
	# Rate 1/2
	##################################################

	#'''
	def test_1block_r12(self) :
		messages_in = [  [  	 1.0,-1.0,-1.0, 1.0,-1.0, 1.0, 1.0,-1.0,  -1.0,-1.0,-1.0, 1.0, 1.0, 1.0, 1.0, 1.0,   # 0xCA (with dependency on end of ASM (0x1D)
					 1.0,-1.0, 1.0, 1.0, 1.0,-1.0,-1.0,-1.0,  -1.0, 1.0,-1.0,-1.0, 1.0, 1.0,-1.0,-1.0,   # 0x0F
					 1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,   1.0,-1.0, 1.0, 1.0			  ], # start of ASM (0x1A)
			      ]
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp)
	#'''

	#'''
	def test_1block_r12_ber(self) :
		messages_in = [  [  	 1.0,-1.0,-1.0, 1.0,-1.0, 1.0, 1.0,-1.0,  -1.0,-1.0,-1.0, 1.0, 1.0, 1.0, 1.0, 1.0,   # 0xCA (with dependency on end of ASM (0x1D)
					 1.0,-1.0, 1.0, 1.0, 1.0,-1.0,-1.0,-1.0,  -1.0, 1.0,-1.0,-1.0, 1.0, 1.0,-1.0,-1.0,   # 0x0F
					 1.0, 1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,   1.0,-1.0, 1.0, 1.0			  ], # start of ASM (0x1A)
			      ]
		messages_in[0][5] *= -1.0
		messages_in = tuple(map(tuple,messages_in))
		
		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp)
	#'''

	#'''
	def test_1block_r12_c2noinvert(self) :
		messages_in = [  [	 1.0, 1.0,-1.0,-1.0,-1.0,-1.0, 1.0, 1.0,   -1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					 1.0, 1.0, 1.0,-1.0, 1.0, 1.0,-1.0, 1.0,   -1.0,-1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0,   # 0x0F
					 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,    1.0, 1.0, 1.0,-1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.NONE)
	#'''

	#'''
	def test_1block_r12_c2noinvert_ber(self) :
		messages_in = [  [	 1.0, 1.0,-1.0,-1.0,-1.0,-1.0, 1.0, 1.0,   -1.0, 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					 1.0, 1.0, 1.0,-1.0, 1.0, 1.0,-1.0, 1.0,   -1.0,-1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0,   # 0x0F
					 1.0,-1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,    1.0, 1.0, 1.0,-1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in[0][5] *= -1.0
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.NONE)
	#'''

	##################################################
	# Rate 2/3
	##################################################

	#'''
	def test_1block_r23(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,-1.0,-1.0,      1.0,   -1.0, 1.0,     -1.0, 1.0,-1.0,     -1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					 1.0, 1.0,     -1.0, 1.0, 1.0,      1.0,   -1.0,-1.0,      1.0, 1.0,-1.0,      1.0,   # 0x0F
					 1.0,-1.0,      1.0,-1.0, 1.0,      1.0,    1.0, 1.0,     -1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_23)
	#'''

	#'''
	def test_1block_r23_ber(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,-1.0,-1.0,      1.0,   -1.0, 1.0,     -1.0, 1.0,-1.0,     -1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					 1.0, 1.0,     -1.0, 1.0, 1.0,      1.0,   -1.0,-1.0,      1.0, 1.0,-1.0,      1.0,   # 0x0F
					 1.0,-1.0,      1.0,-1.0, 1.0,      1.0,    1.0, 1.0,     -1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in[0][5] *= -1.0
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_23)
	#'''

	
	##################################################
	# Rate 3/4
	##################################################

	#'''
	def test_1block_r34(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,-1.0,      1.0, 1.0,         1.0,-1.0,      1.0,-1.0,     -1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					 1.0,      1.0,-1.0,      1.0,-1.0,        -1.0,-1.0,      1.0, 1.0,     -1.0, 1.0,   # 0x0F
					     -1.0,-1.0,     -1.0, 1.0,      1.0,    1.0,      1.0,-1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_34)
	#'''

	#'''
	def test_1block_r34_ber(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,-1.0,      1.0, 1.0,         1.0,-1.0,      1.0,-1.0,     -1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					 1.0,      1.0,-1.0,      1.0,-1.0,        -1.0,-1.0,      1.0, 1.0,     -1.0, 1.0,   # 0x0F
					     -1.0,-1.0,     -1.0, 1.0,      1.0,    1.0,      1.0,-1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in[0][5] *= -1.0
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_34)
	#'''


	##################################################
	# Rate 5/6
	##################################################

	#'''
	def test_1block_r56(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,-1.0,           1.0,   -1.0,     -1.0,-1.0,     -1.0, 1.0,        # 0xCS (with dependency on end of ASM (0x1D)
					      1.0, 1.0,      1.0, 1.0,      1.0,   -1.0,           1.0, 1.0,     -1.0, 1.0,   # 0x0F
					     -1.0,-1.0,           1.0,-1.0,         1.0, 1.0,     -1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_56)
	#'''

	#'''
	def test_1block_r56_ber(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,-1.0,           1.0,   -1.0,     -1.0,-1.0,     -1.0, 1.0,        # 0xCS (with dependency on end of ASM (0x1D)
					      1.0, 1.0,      1.0, 1.0,      1.0,   -1.0,           1.0, 1.0,     -1.0, 1.0,   # 0x0F
					     -1.0,-1.0,           1.0,-1.0,         1.0, 1.0,     -1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in[0][5] *= -1.0
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_56)
	#'''

	##################################################
	# Rate 7/8
	##################################################

	#'''
	def test_1block_r78(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,     -1.0,      1.0,   -1.0,          -1.0, 1.0,      1.0,-1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					      1.0,     -1.0,      1.0,-1.0,             -1.0,-1.0,      1.0,-1.0,      1.0,   # 0x0F
					     -1.0,      1.0,-1.0,           1.0,    1.0,      1.0,-1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_78)
	#'''

	#'''
	def test_1block_r78_ber(self) :
		messages_in = [  [	 1.0, 1.0,     -1.0,     -1.0,      1.0,   -1.0,          -1.0, 1.0,      1.0,-1.0,   # 0xCS (with dependency on end of ASM (0x1D)
					      1.0,     -1.0,      1.0,-1.0,             -1.0,-1.0,      1.0,-1.0,      1.0,   # 0x0F
					     -1.0,      1.0,-1.0,           1.0,    1.0,      1.0,-1.0                     ], # start of ASM (0x1A)
			      ]
		messages_in[0][5] *= -1.0
		messages_in = tuple(map(tuple,messages_in))

		messages_out_exp = ( (0xCA, 0x0F), )
		self.runExperiment(messages_in, messages_out_exp,poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_78)
	#'''


	##################################################
	# Encoder / Decoder chain tests
	##################################################

	#'''
	def test_chain_r12(self) :
		self.runChainExperiment(poly1=0x79,poly2=0xDB,punct=ccsds_swig.NONE)
	#'''

	#'''
	def test_chain_r23(self) :
		self.runChainExperiment(poly1=0x79,poly2=0x5B,punct=ccsds_swig.ECSS_23)
	#'''

	##
	## TODO Implement chain tests for rate 3/4, 5/6, 7/8 as soon as the
	## unpuncturing is synchronized
	##
if __name__ == '__main__':
	gr_unittest.main ()
