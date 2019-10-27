#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 <+YOU OR YOUR COMPANY+>.
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import ccsds_swig as ccsds

class qa_simple_bpsk_SNR_qf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        print 'simple_bpsk_SNR_qf unit test not implemented, yet.'
        """
        # set up fg
        src_data = (1+0j, -1+0j,) # TODO add test sequence
        expected_result = (1,)
        src = blocks.vector_source_c (src_data)
        snr = ccsds.simple_bpsk_SNR_qf (2)
        dst = blocks.vector_sink_f ()
        self.tb.connect(src,snr)
        self.tb.connect(snr,dst)
        
        self.tb.run ()
        result_data = dst.data ()
        # check data
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 4, 'SNR does not match')
        """
        
if __name__ == '__main__':
    gr_unittest.run(qa_simple_bpsk_SNR_qf, "qa_simple_bpsk_SNR_qf.xml")
