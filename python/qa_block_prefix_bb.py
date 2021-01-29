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

class qa_block_prefix_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def run_test(self, data_in, block_len, asm, expected_output):
        ##################################################
        # Blocks
        ##################################################
        self.src = blocks.vector_source_b(data_in, False)
        self.prefix = ccsds.block_prefix_bb(asm, block_len)
        self.snk = blocks.vector_sink_b(1)
                
        ##################################################
        # Connections
        ##################################################
        self.tb.connect((self.src, 0), (self.prefix, 0))
        self.tb.connect((self.prefix, 0), (self.snk, 0))

        ##################################################
        # Run flowgraph
        ##################################################
        self.tb.run()

        ##################################################
        # Check output
        ##################################################
        computed_output = self.snk.data()

        message = 'Computed {} bytes, expected {} bytes.\nExpected output: {}\nComputed output: {}'.format(
            len(expected_output), len(computed_output),
            str(['0x{:02X}'.format(b) for b in expected_output]),
            str(['0x{:02X}'.format(b) for b in computed_output])
        )

        self.assertTupleEqual(tuple(expected_output), computed_output, msg=message)
    
    def test_001_t (self):
        self.run_test(
            [0x00,]*100 + [0xFF,]*100,
            100,
            '1ACFFC1D',
            [0x1A,0xCF,0xFC,0x1D] + [0x00,]*100 + [0x1A,0xCF,0xFC,0x1D] + [0xFF,]*100
        )


    def test_002_t (self):
        self.run_test(
            list(range(256))+([0x00,]*256)+([0xFF,]*256)+([0xAA,]*256)+([0x0F,]*256),
            256,
            '1ACFFC1D',
            [0x1A,0xCF,0xFC,0x1D]+list(range(256))+[0x1A,0xCF,0xFC,0x1D]+[0x00,]*256+[0x1A,0xCF,0xFC,0x1D]+[0xFF,]*256+[0x1A,0xCF,0xFC,0x1D]+[0xAA,]*256+[0x1A,0xCF,0xFC,0x1D]+[0x0F,]*256,
        )


    def test_003_t (self):
        self.run_test(
            [0x00, 0xFF],
            1,
            '00AA',
            [0x00, 0xAA, 0x00, 0x00, 0xAA, 0xFF]
        )


if __name__ == '__main__':
    gr_unittest.run(qa_block_prefix_bb)
