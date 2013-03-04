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
from cmath import rect
from math import pi, sqrt



class qa_mpsk_demod2_cb (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def to_hex(self, dec):
		return '0x%02X' % (dec)

	def checkBits(self, src_data, exp_data, M):
		##################################################
		# Blocks
		##################################################
		self.src = gr.vector_source_c(src_data, False, 1)
		self.snk_sym = gr.vector_sink_b(1)
		self.snk_conf = gr.vector_sink_b(1)
		self.detector = ccsds_swig.mpsk_demod2_cb(M,8)

		##################################################
		# Connections
		##################################################
		self.tb.connect((self.src, 0), (self.detector, 0))
		self.tb.connect((self.detector, 0), (self.snk_sym, 0))
		self.tb.connect((self.detector, 1), (self.snk_conf, 0))

		self.tb.run ()

		# check symbol bit
		self.assertEqual(exp_data, self.snk_sym.data(),
					'Output symbols are not matching the expexted result:\nOutput  : %s\nExpected: %s\nInput   : %s' %
					( str(self.snk_sym.data()), str(exp_data), str(src_data) )
				)


	def checkConf(self, src_data, exp_conf, M):
		##################################################
		# Blocks
		##################################################
		self.src = gr.vector_source_c(src_data, False, 1)
		self.snk_sym = gr.vector_sink_b(1)
		self.snk_conf = gr.vector_sink_b(1)
		self.detector = ccsds_swig.mpsk_demod2_cb(M,8)

		##################################################
		# Connections
		##################################################
		self.tb.connect((self.src, 0), (self.detector, 0))
		self.tb.connect((self.detector, 0), (self.snk_sym, 0))
		self.tb.connect((self.detector, 1), (self.snk_conf, 0))

		self.tb.run ()

		# check confidence value
		self.assertEqual(exp_conf, self.snk_conf.data(),
					'Output confidence values are not matching the expexted result:\nOutput  : %s\nExpected: %s\nInput   : %s' %
					( str(self.snk_conf.data()), str(exp_conf), str(src_data) )
				)


	def checkWithMod(self, M):
		src_data = tuple(range(M))
		src = gr.vector_source_b(src_data, False, 1)
		mod = ccsds_swig.mpsk_mod_bc(int(M))
		demod = ccsds_swig.mpsk_demod2_cb (int(M),8)
		snk_sym  = gr.vector_sink_b ()
		snk_conf = gr.vector_sink_b(1)
		
		self.tb.connect (src, mod)
		if(M != 4) :
			self.tb.connect (mod, demod)
		else :
			rot = gr.multiply_const_cc(rect(1,-0.25*pi))
			self.tb.connect (mod, rot)
			self.tb.connect (rot, demod)

		self.tb.connect ((demod,0), snk_sym)
		self.tb.connect ((demod,1), snk_conf)

		self.tb.run ()

		self.assertEqual(src_data, snk_sym.data(),
					'Output symbols of the detector output are not matching the modulator input:\nOutput: %s\nInput:  %s' %
					( str(snk_sym.data()), str(src_data)  )
				)



	##################################################
	# BPSK
	##################################################

	#'''
	def test_demod_bpsk_sym (self):
		src_data =     (1+0j, -1+0j, 1+10j, 1-10j, 0.1+0j, 0.1+10j, -1+10j, -1-10j, -0.1+0j, -0.1+10j )
		expected_sym = (   0,     1,     0,     0,      0,       0,      1,      1,       1,        1 )
		self.checkBits(src_data, expected_sym, 2)
	#'''

	#'''
	def test_demod_bpsk_conf (self):
		src_data =     (1+0j, -1+0j, 1+10j, 0+0j )
		expected_conf = ( 255,   255,   255,    0 )
		self.checkConf(src_data, expected_conf, 2)
	#'''


	##################################################
	# QPSK
	##################################################

	#'''
	def test_demod_qpsk_sym (self):
		src_data =        (1+0j, -1+0j, 0+1j, 0-1j, 1+10j, 1-10j, 0.1+0j, 0.1+10j, -10+9j, -10-7j, -0.1+0j, -0.1+10j )
		expected_result = (0   ,     3,     2,   1,     2,     1,      0,       2,      3,      3,       3,        2 )
		self.checkBits(src_data, expected_result, 4)
	#'''


	##################################################
	# 8-PSK
	##################################################

	#'''
	def test_demod_8psk (self):
		M = 8.0
		# symbols
		phases = tuple(2.0*p*pi/M        for p in range(0,int(M)))
		# right after next zone
		phases += tuple((2.0*p+0.9)*pi/M for p in range(0,int(M)))
		#right before next zone
		phases += tuple((2.0*p+1.1)*pi/M for p in range(0,int(M)))
	
		powers = (0.1,1,1.1,5)
	
		gray  = (0, 1, 3, 2, 6, 7, 5, 4)
		gray +=	(0, 1, 3, 2, 6, 7, 5, 4)
		gray +=	(1, 3, 2, 6, 7, 5, 4, 0)
	

		src_data = ()
		expected_result = ()
		for power in powers:
			src_data += tuple(rect(power,p) for p in phases)
			expected_result += gray

		self.checkBits(src_data, expected_result, int(M))
	#'''


	##################################################
	# 16-PSK
	##################################################

	#'''
	def test_demod_16psk (self):
		M = 16.0
		# symbols
		phases = tuple(2.0*p*pi/M        for p in range(0,int(M)))
		# right after next zone
		phases += tuple((2.0*p+0.9)*pi/M for p in range(0,int(M)))
		#right before next zone
		phases += tuple((2.0*p+1.1)*pi/M for p in range(0,int(M)))

		powers = (0.1,1,1.1,5)
	
		gray  = (0, 1, 3, 2, 6, 7, 5,  4, 12, 13, 15, 14, 10, 11, 9, 8)
		gray +=	(0, 1, 3, 2, 6, 7, 5,  4, 12, 13, 15, 14, 10, 11, 9, 8)
		gray +=	(1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11,  9, 8, 0)
	

		src_data = ()
		expected_result = ()
		for power in powers:
			src_data += tuple(rect(power,p) for p in phases)
			expected_result += gray

		self.checkBits(src_data, expected_result, int(M))
	#'''


	##################################################
	# Mod-Detector chain
	##################################################

	#'''
	def test_demod_with_mod_bpsk (self):
		self.checkWithMod(2)

	def test_demod_with_mod_qpsk (self):
		self.checkWithMod(4)

	def test_demod_with_mod_8psk (self):
		self.checkWithMod(8)

	def test_demod_with_mod_16psk (self):
		self.checkWithMod(16)
	#'''

if __name__ == '__main__':
	gr_unittest.main ()
