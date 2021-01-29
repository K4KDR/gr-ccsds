#!/usr/bin/env python3
# 
# Copyright 2020 Martin Luelf <mail@mluelf.de>.
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
from math import pi, sqrt, cos, sin

class qa_mpsk_detector_soft_cf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None
    
    def run_test(self, M, offset, test_data):
        """ test data is a tuple of tuples
            the first element is an angle [degrees] of the test point,
            the second element is another tuple with the expected bits
        """
        def cplx_at(deg):
            rad = pi/180.0*deg
            return cos(rad)+1j*sin(rad)
        src_data = [cplx_at(d[0]) for d in test_data]
        expected_result = []
        for d in test_data:
            expected_result += d[1]
        expected_result = tuple(expected_result)
        
        src = blocks.vector_source_c (src_data)
        detector = ccsds.mpsk_detector_soft_cf (M, offset)
        conversion = ccsds.softbittobit ()
        sink = blocks.vector_sink_b ()
        self.tb.connect (src, detector)
        self.tb.connect (detector, conversion)
        self.tb.connect (conversion, sink)
        
        self.tb.run ()
        
        result_data = sink.data ()
        
        message = 'Computed data is not matching expected data for M={} and an offset of {} symbols.\nExpected: {} ({} elements)\nComputed: {} ({} elements)'.format(M, offset, str(expected_result), len(expected_result), str(result_data), len(result_data))
        
        self.assertTupleEqual (expected_result, result_data, msg=message)
    
    def test_demod_bpsk(self):
        delta = 0.1
        test_data = (
            ( (  0.0       ), (0,) ),
            ( (180.0       ), (1,) ),
            ( (270.0+delta ), (0,) ),
            ( ( 90.0-delta ), (0,) ),
            ( ( 90.0+delta ), (1,) ),
            ( (270.0-delta ), (1,) ),
        )
        
        self.run_test(2, 0.0, test_data)
    
    def test_demod_qpsk_ccsds (self):
        delta = 0.1
        test_data = (
            ( ( 45.0      ), (0,0) ),
            ( (135.0      ), (1,0) ),
            ( (225.0      ), (1,1) ),
            ( (315.0      ), (0,1) ),
            ( (  0.0+delta), (0,0) ),
            ( ( 90.0-delta), (0,0) ),
            ( ( 90.0+delta), (1,0) ),
            ( (180.0-delta), (1,0) ),
            ( (180.0+delta), (1,1) ),
            ( (270.0-delta), (1,1) ),
            ( (270.0+delta), (0,1) ),
            ( (  0.0-delta), (0,1) ),
        )
        
        self.run_test(4, 0.5, test_data)

    def test_demod_qpsk_axis (self):
        delta = 0.1
        test_data = (
            ( (  0.0      ), (0,0) ),
            ( ( 90.0      ), (1,0) ),
            ( (180.0      ), (1,1) ),
            ( (270.0      ), (0,1) ),
            ( (315.0+delta), (0,0) ),
            ( ( 45.0-delta), (0,0) ),
            ( ( 45.0+delta), (1,0) ),
            ( (135.0-delta), (1,0) ),
            ( (135.0+delta), (1,1) ),
            ( (225.0-delta), (1,1) ),
            ( (225.0+delta), (0,1) ),
            ( (315.0-delta), (0,1) ),
        )
        
        self.run_test(4, 0.0, test_data)

if __name__ == '__main__':
    gr_unittest.run(qa_mpsk_detector_soft_cf)
