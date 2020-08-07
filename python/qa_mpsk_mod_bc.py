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
from cmath import rect
from math import pi, sqrt

class qa_mpsk_mod_bc (gr_unittest.TestCase):

    def setUp (self):
	self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_mod_bpsk (self):
	src_data = (0, 1)
	expected_result = (1+0j, -1+0j)
	src = blocks.vector_source_b (src_data)
	sqr = ccsds.mpsk_mod_bc (2)
	dst = blocks.vector_sink_c ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  BPSK  =====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 4, 'BPSK constellation does not match')

    def test_mod_qpsk (self):
	src_data = (0, 1, 2, 3)
	expected_result = (1+1j, 1-1j, -1+1j, -1-1j)
	# scale to unit circle
	expected_result = tuple(p/sqrt(2.0) for p in expected_result)
	src = blocks.vector_source_b (src_data)
	sqr = ccsds.mpsk_mod_bc (4)
	dst = blocks.vector_sink_c ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  QPSK  =====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"

	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 4, 'QPSK constellation does not match')

    def test_mod_8psk (self):
	src_data = (0, 1, 2, 3, 4, 5, 6, 7)
	# gray     (0, 1, 3, 2, 6, 7, 5, 4)
	phases =   (0, 1, 3, 2, 7, 6, 4, 5) # phases *4/pi
	expected_result = tuple(rect(1.0,p*pi/4.0) for p in phases)
	src = blocks.vector_source_b (src_data)
	sqr = ccsds.mpsk_mod_bc (8)
	dst = blocks.vector_sink_c ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  8-PSK ====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "=================================================\n"

	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 4, '8-PSK constellation does not match')

    def test_mod_16psk (self):
	src_data = (0, 1, 2, 3, 4, 5, 6, 7,  8,  9, 10, 11, 12, 13, 14, 15)
	# gray     (0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11,  9,  8)
	phases =   (0, 1, 3, 2, 7, 6, 4, 5, 15, 14, 12, 13,  8,  9, 11, 10) # phases *8/pi
	expected_result = tuple(rect(1.0,p*pi/8.0) for p in phases)
	src = blocks.vector_source_b (src_data)
	sqr = ccsds.mpsk_mod_bc (16)
	dst = blocks.vector_sink_c ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "===== 16-PSK ====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "=================================================\n"

	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 4, '16-PSK constellation does not match')


if __name__ == '__main__':
    gr_unittest.run(qa_mpsk_mod_bc, "qa_mpsk_mod_bc.xml")
