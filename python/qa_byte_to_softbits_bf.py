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

class qa_byte_to_softbits_bf (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def run_test(self, input_data, expected_output, noise_level):
        self.source = blocks.vector_source_b(input_data, False)
        self.conversion = ccsds.byte_to_softbits_bf(noise_level)
        self.sink = blocks.vector_sink_f()

        self.tb.connect( (self.source, 0), (self.conversion, 0) )
        self.tb.connect( (self.conversion, 0), (self.sink, 0) )

        self.tb.run()

        computed_data = self.sink.data()
        message = 'Computed output ({} floats) does not match expected output ({} floats).\nInput: {}\nExpected output: {}\nComputed output: {}'.format(
            len(computed_data),
            len(expected_output),
            str(['0x{:02X}'.format(b) for b in input_data]),
            str(expected_output),
            str(computed_data)
        )

        self.assertFloatTuplesAlmostEqual(expected_output, computed_data, 2, msg=message)
    
    def test_001_t (self):
        noise_power = 1e-3
        # 2*sample/noise_power
        mag = 2.0*1.0/noise_power
        
        self.run_test(
            (0x00, 0x01, 0x85, 0xAA, 0x55, 0xFF),
            (
                -mag, -mag, -mag, -mag, -mag, -mag, -mag, -mag,
                -mag, -mag, -mag, -mag, -mag, -mag, -mag, +mag,
                +mag, -mag, -mag, -mag, -mag, +mag, -mag, +mag,
                +mag, -mag, +mag, -mag, +mag, -mag, +mag, -mag,
                -mag, +mag, -mag, +mag, -mag, +mag, -mag, +mag,
                +mag, +mag, +mag, +mag, +mag, +mag, +mag, +mag,
            ),
            noise_power
        )

    def test_002_t (self):
        noise_power = 0.5
        # 2*sample/noise_power
        mag = 2.0*1.0/noise_power
        
        self.run_test(
            (0x00, 0x01, 0x85, 0xAA, 0x55, 0xFF),
            (
                -mag, -mag, -mag, -mag, -mag, -mag, -mag, -mag,
                -mag, -mag, -mag, -mag, -mag, -mag, -mag, +mag,
                +mag, -mag, -mag, -mag, -mag, +mag, -mag, +mag,
                +mag, -mag, +mag, -mag, +mag, -mag, +mag, -mag,
                -mag, +mag, -mag, +mag, -mag, +mag, -mag, +mag,
                +mag, +mag, +mag, +mag, +mag, +mag, +mag, +mag,
            ),
            noise_power
        )


if __name__ == '__main__':
    gr_unittest.run(qa_byte_to_softbits_bf)
