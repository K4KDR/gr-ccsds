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

class qa_mpsk_demod_cb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_demod_bpsk (self):
	src_data =        (1+0j, -1+0j, 1+10j, 1-10j, 0.1+0j, 0.1+10j, -1+10j, -1-10j, -0.1+0j, -0.1+10j )
	expected_result = (   0,     1,     0,     0,      0,       0,      1,      1,       1,        1 )
	src = gr.vector_source_c (src_data)
	sqr = ccsds_swig.mpsk_demod_cb (2)
	dst = gr.vector_sink_b ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  BPSK  =====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertEqual (expected_result, result_data, 'BPSK constellation does not match')


    def test_demod_qpsk (self):
	src_data =        (1+0j, -1+0j, 0+1j, 0-1j, 1+10j, 1-10j, 0.1+0j, 0.1+10j, -10+9j, -10-7j, -0.1+0j, -0.1+10j )
	expected_result = (0   ,     3,     1,   2,     1,     2,      0,       1,      3,      3,       3,        1 )
	src = gr.vector_source_c (src_data)
	sqr = ccsds_swig.mpsk_demod_cb (4)
	dst = gr.vector_sink_b ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  QPSK  =====================================\n"
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertEqual (expected_result, result_data, 'QPSK constellation does not match')

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

	src = gr.vector_source_c (src_data)
	sqr = ccsds_swig.mpsk_demod_cb (int(M))
	dst = gr.vector_sink_b ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "=====  8-PSK =====================================\n"
	#print "phases: %s\n" % (tuple(8.0*p/pi for p in phases), )
	#print "input: %s\n" % (src_data, )
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertEqual (expected_result, result_data, '8-PSK constellation does not match')

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

	src = gr.vector_source_c (src_data)
	sqr = ccsds_swig.mpsk_demod_cb (int(M))
	dst = gr.vector_sink_b ()
	self.tb.connect (src, sqr)
	self.tb.connect (sqr, dst)
	self.tb.run ()
	result_data = dst.data ()
	#print "===== 16-PSK =====================================\n"
	#print "phases: %s\n" % (tuple(8.0*p/pi for p in phases), )
	#print "input: %s\n" % (src_data, )
	#print "expected data: %s\n" % (expected_result, )
	#print "received data: %s\n" % (result_data, )
	#print "==================================================\n"
	self.assertEqual (expected_result, result_data, '16-PSK constellation does not match')



if __name__ == '__main__':
    gr_unittest.main ()
