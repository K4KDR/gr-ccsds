#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import ccsds_swig as ccsds

class qa_pack_bit_to_bytes (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def runTest(self, data_in, expected_output):
        ##################################################
        # Blocks
        ##################################################
        self.src = blocks.vector_source_b(data_in, False)
        self.pack_bit_to_bytes = ccsds.pack_bit_to_bytes()
        self.sink = blocks.vector_sink_b()
        
        ##################################################
        # Connections
        ##################################################
        self.tb.connect((self.src, 0), (self.pack_bit_to_bytes, 0))
        self.tb.connect((self.pack_bit_to_bytes, 0), (self.sink, 0))
        
        ##################################################
        # Run flowgraph
        ##################################################
        self.tb.run()

        ##################################################
        # Compare results
        ##################################################
        self.assertTupleEqual(expected_output, self.sink.data())

    def test_001_t (self):
        self.runTest(
            (0,0,0,0,0,0,0,0,  1,1,1,1,1,1,1,1,  0,1,0,1,0,1,0,1,  1,0,1,0,1,0,1,0),
            (0x00, 0xFF, 0x55, 0xAA)
        )

    def test_002_t (self):
        self.runTest(
            (0,0,0,0,0,0,0,1,  0,0,0,0,0,0,1,0,  0,0,0,0,0,1,0,0,  0,0,0,0,1,0,0,0,  0,0,0,1,0,0,0,0,  0,0,1,0,0,0,0,0,  0,1,0,0,0,0,0,0,  1,0,0,0,0,0,0,0),
            (1, 2, 4, 8, 16, 32, 64, 128)
        )


if __name__ == '__main__':
    gr_unittest.run(qa_pack_bit_to_bytes)
