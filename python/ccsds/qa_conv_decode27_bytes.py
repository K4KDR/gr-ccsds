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
import random
import numpy
import os
import pmt
import time
import math

class qa_conv_decode27_bytes(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
            self.tb = None

    def to_hex(self, dec):
        return '0x%02X' % (dec)

    def run_test(self, frame_len_bits, frames_encoded, expected_output, poly1=0x79, poly2=0xDB, punct=ccsds.NONE ,ASM='1ACFFC1D'):
        data_in = []

        frame_len_enc_bytes = len(frames_encoded[0])
        frame_len_dec_bytes = len(expected_output[0])

        for frame in frames_encoded:
            data_in += frame

            assert(frame_len_enc_bytes == len(frame))
        
        for frame in expected_output:
            assert(frame_len_dec_bytes == len(frame))
        
        ##################################################
        # Blocks
        ##################################################
        self.source = blocks.vector_source_b(data_in, False)
        self.conversion = ccsds.byte_to_softbits_bf(1e-3)
        self.msg_sink = ccsds.softbit_msg_sink_f(frame_len_enc_bytes*8)
        self.decoder = ccsds.conv_decode27(poly1, poly2, punct, frame_len_dec_bytes*8, ASM)
        self.dbg = blocks.message_debug()

        ##################################################
        # Connections
        ##################################################
        self.tb.connect( (self.source, 0), (self.conversion, 0) )
        self.tb.connect( (self.conversion, 0), (self.msg_sink, 0) )

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(self.msg_sink, "out", self.decoder, "in")
        self.tb.msg_connect(self.decoder, "out", self.dbg, "store")

        ##################################################
        # Run Flowgraph
        ##################################################
        self.tb.start()

        timeout = 50
        num_messages = len(expected_output)
        while(self.dbg.num_messages() < num_messages+1 and timeout > 0):
            time.sleep(0.1)
            timeout -= 1
        
        self.tb.stop()
        self.tb.wait()

        ##################################################
        # Check results
        ##################################################
        # test correct number of received frames
        self.assertEqual (self.dbg.num_messages(), num_messages+1, '%d messages in store, %d expected.' % (self.dbg.num_messages(),(num_messages+1)))
        
        # test for EOF
        eof_msg = self.dbg.get_message(num_messages)
        self.assertEqual (pmt.is_eof_object(eof_msg), True, 'EOF block not at expected position')

        # test the blobs
        for i in range(len(expected_output)):
            dbg_msg_in = self.dbg.get_message(i)
            dbg_msg = pmt.cdr(dbg_msg_in)
            #dbg_hdr = pmt.car(dbg_msg_in)

            computed_output = pmt.u8vector_elements(dbg_msg)
            
            message = 'Computed data ({} bytes) does not match expected data ({} bytest).\nInput: {}\nComputed: {}\nExpected: {}'.format(
                len(expected_output[i]),
                len(computed_output),
                str(['0x{:02X}'.format(b) for b in frames_encoded[i]]),
                str(['0x{:02X}'.format(b) for b in expected_output[i]]),
                str(['0x{:02X}'.format(b) for b in computed_output])
            )
            self.assertTupleEqual (expected_output[i], computed_output, msg=message)

    def test_r12_01(self):
        self.run_test(12,
            (
                (0x42, 0x25, 0x55, 0x55, 0x55, 0x55, 0x8C, 0x1A, 0xAA, 0xAA, 0xAA, 0xAA, 0x92, 0xE9, 0x99, 0x99, 0x99, 0x99, 0x40, 0xD6, 0x66, 0x66, 0x66, 0x66, 0x5D, 0x78),
                (0xA3, 0x2A, 0x10, 0xE9, 0xE8, 0xD6, 0x00, 0x57, 0xD9, 0xE3, 0x1A, 0x1B, 0x25, 0xF0, 0x15, 0x14, 0x29, 0x99, 0x40, 0xD6, 0x66, 0x66, 0x66, 0x66, 0x5D, 0x78)
            ),
            (
                (0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55),
                (0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xAA, 0x55, 0x55, 0x55)
            ),
            poly1=0x79, poly2=0xDB, punct=ccsds.NONE ,ASM='1ACFFC1D'
        )
    
if __name__ == '__main__':
    gr_unittest.run(qa_conv_decode27_bytes)
