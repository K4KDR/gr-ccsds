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

class qa_sym_to_softbits_bf (gr_unittest.TestCase):

	def setUp (self):
		self.tb = gr.top_block ()

	def tearDown (self):
		self.tb = None

	def to_hex(self, dec):
		return '0x%02X' % (dec)


	def runTest(self, sym_in, conf_in, out_exp, num_bits, num_quant):
		##################################################
		# Blocks
		##################################################
		self.sym = gr.vector_source_b(sym_in, False, 1)
		self.conf = gr.vector_source_b(conf_in, False, 1)
		self.encoder = ccsds_swig.sym_to_softbits_bf(num_bits,num_quant)
		self.snk = gr.vector_sink_f(1)

		##################################################
		# Connections
		##################################################
		self.tb.connect((self.sym, 0), (self.encoder, 0))
		self.tb.connect((self.conf, 0), (self.encoder, 1))
		self.tb.connect((self.encoder, 0), (self.snk, 0))

		self.tb.run()

		out = self.snk.data()

		fail_str = 'Converted softbits do not match the expected output.\nexpected: %s\noutput  : %s' %	(str(out_exp), str(self.snk.data()))
		self.assertFloatTuplesAlmostEqual(out, out_exp, msg=fail_str,places=2)

	def test_8bit_conf(self):
		in_sym  = (0x70,0xFF,0x00,0xAA)*3
		in_conf = (0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,128,128,128,128)

		out_exp = (	-1.0, 1.0, 1.0, 1.0,-1.0,-1.0,-1.0,-1.0, # 0x70 full conf
				 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, # 0xFF full conf
				-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0, # 0x00 full conf
				 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, 1.0,-1.0, # 0xAA full conf
				 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, # 0x70 no conf
				 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, # 0xFF no conf
				 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, # 0x00 no conf
				 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, # 0xAA no conf
				-0.5, 0.5, 0.5, 0.5,-0.5,-0.5,-0.5,-0.5, # 0x70 half conf
				 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, # 0xFF half conf
				-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5, # 0x00 half conf
				 0.5,-0.5, 0.5,-0.5, 0.5,-0.5, 0.5,-0.5  # 0xAA half conf
			  )

		self.runTest(in_sym,in_conf,out_exp,8,8)
		

if __name__ == '__main__':
	gr_unittest.main ()
