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
from cmath import rect, exp
from math import pi, sqrt

class qa_mpsk_detector_soft_cf (gr_unittest.TestCase):

    def setUp (self):
	self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_demod_bpsk (self):
	src_data = (1+0j, -1+0j, -0.9+0.1j, -0.9+0.1j)
	expected_result = (0, 1, 1, 1)
	src = blocks.vector_source_c (src_data)
	sqr = ccsds.mpsk_detector_soft_cf (2)
	con = ccsds.softbittobit ()
	dst = blocks.vector_sink_b ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, con)
	self.tb.connect (con, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  BPSK  =====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6, 'BPSK constellation does not match')

    def test_demod_qpsk (self):
	src_data = (0.7071+0.7071j, 0.7071-0.7071j, -0.7071-0.7071j, -0.7071+0.7071j, 0.05+0.95j, 0.95+0.05j, 0.95-0.05j, -0.05+0.95j)
	expected_result = (0,0,     0,1,            1,1,             1,0,             0,0,        0,0,        0,1,         1,0)
	src = blocks.vector_source_c (src_data)
	rot = blocks.multiply_const_vcc((exp(-1j*pi/4), ))
	sqr = ccsds.mpsk_detector_soft_cf (4)
	con = ccsds.softbittobit ()
	dst = blocks.vector_sink_b ()
	self.tb.connect (src, rot)
	self.tb.connect (rot, sqr)
	self.tb.connect (sqr, con)
	self.tb.connect (con, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  QPSK  =====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6, 'QPSK constellation does not match')

    def test_demod_qpsk_single (self):
	src_data = (0.7071+0.7071j,)
	expected_result = (0,0)
	src = blocks.vector_source_c (src_data)
	rot = blocks.multiply_const_vcc((exp(-1j*pi/4), ))
	sqr = ccsds.mpsk_detector_soft_cf (4)
	con = ccsds.softbittobit ()
	dst = blocks.vector_sink_b ()
	self.tb.connect (src, rot)
	self.tb.connect (rot, sqr)
	self.tb.connect (sqr, con)
	self.tb.connect (con, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  QPSK  single ==============================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6, 'QPSK constellation does not match')

if __name__ == '__main__':
    gr_unittest.run(qa_mpsk_detector_soft_cf, "qa_mpsk_detector_soft_cf.xml")
