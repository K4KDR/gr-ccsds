#!/usr/bin/env python3
# 
# Copyright 2020 Martin Luelf <mail@mluelf.de>
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

class qa_mpsk_ambiguity_resolver_chain(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
            self.tb = None

    def to_hex(self, dec):
        return '0x%02X' % (dec)

    def run_test(self, data_in, data_expected, frame_len_byte, M, ambiguity, offset_bytes, asm_prefix=[0x1A, 0xCF, 0xFC, 0x1D], asm_ar=[0x1A, 0xCF, 0xFC, 0x1D]):
        ##################################################
        # Variables
        ##################################################
        ldM = int(math.log(M)/math.log(2))
        
        
        asm_prefix_str = ''.join(['{:02X}'.format(b) for b in asm_prefix])
        asm_ar_str = ''.join(['{:02X}'.format(b) for b in asm_ar])

        offset = 0.5 if M==4 else 0.0
        
        ambiguity_angle_rad = float(ambiguity)/float(M)*2.0*math.pi
        ambiguity_complex = math.cos(ambiguity_angle_rad)+1j*math.sin(ambiguity_angle_rad)
        data_prefixed = []
        num_frames = int(math.floor(len(data_in)/frame_len_byte))
        for i in range(num_frames):
            data_prefixed += asm_prefix + data_in[i*frame_len_byte:(i+1)*frame_len_byte]
        data_prefixed += data_in[num_frames*frame_len_byte:]
        source_data = [0x00,]*offset_bytes+data_prefixed
        
        ##################################################
        # Blocks
        ##################################################
        src = blocks.vector_source_b(source_data, False)
        packed_to_unpacked = blocks.packed_to_unpacked_bb(ldM, gr.GR_MSB_FIRST)
        mod = ccsds.mpsk_mod_bc(M)
        demod = ccsds.mpsk_detector_soft_cf(M, offset)
        ambiguity = blocks.multiply_const_cc(ambiguity_complex)
        ar = ccsds.mpsk_ambiguity_resolver_f(M,asm_ar_str,len(asm_ar)*8,1,0.8,frame_len_byte,0, ccsds.AR_OUTPUT_NONE)
        conversion = ccsds.softbits_msg_to_bytes_b()
        sink = blocks.vector_sink_b()
        #dbg = blocks.message_debug()

        ##################################################
        # Connections
        ##################################################
        self.tb.connect((src, 0), (packed_to_unpacked, 0))
        self.tb.connect((packed_to_unpacked, 0), (mod, 0))
        self.tb.connect((mod, 0), (ambiguity, 0))
        self.tb.connect((ambiguity, 0), (demod, 0))
        self.tb.connect((demod, 0), (ar, 0))
        self.tb.connect((conversion, 0), (sink, 0))
        
        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(ar, "out", conversion, "in")
        #self.tb.msg_connect(ar, "out", dbg, "print_pdu")
    
        ##################################################
        # Run flowgraph
        ##################################################
        self.tb.run()

        #print('M={}, frame_len={}, offset={} bytes'.format(M, frame_len_byte, offset_bytes))
        #print('{} bytes input items'.format(len(data_in)))
        #print('{} bytes with ASMs'.format(len(self.sink2.data())))
        #print('{} bits into modulator'.format(len(self.sink3.data())))
        #print('{} symbols from modulator items'.format(len(self.sink4.data())))
        #print('{} softbits from demod'.format(len(self.sink5.data())))

        #print('{} bytes output.\n\n'.format(len(self.sink.data())))
        
        ##################################################
        # Test results
        ##################################################
        computed_output = sink.data()
        message = 'Expected {} bytes, got {} bytes.\nExpected data: {}\nComputed data: {}'.format(
            len(data_expected), len(computed_output),
            str(['0x{:02X}'.format(b) for b in data_expected]),
            str(['0x{:02X}'.format(b) for b in computed_output]),
            )
        self.assertTupleEqual(tuple(data_expected), computed_output, msg=message)
    
    
    ##################################################
    # BPSK tests
    ##################################################
        
    def test_bpsk_no_asm(self) :
        "Test without ASM to detect false positives"
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            [],
            frame_len_byte=5,
            M=2,
            ambiguity=1,
            offset_bytes=0,
            asm_prefix=[]
        )
    
    def test_bpsk_simple(self):
        """ Test without ambiguity or offset """
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=2,
            ambiguity=0,
            offset_bytes=0
        )
    
    
    def test_bpsk_ambiguity(self) :
        """ Test ambiguity """
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=2,
            ambiguity=1,
            offset_bytes=0
        )

    
    def test_bpsk_offset(self) :
        """ Test offset """
        self.run_test(
                [0xFF,]*5 +
                [0x00,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0xFF,]*5 +
                [0x00,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=2,
            ambiguity=0,
            offset_bytes=5
        )
    
    ##################################################
    # QPSK tests
    ##################################################
        
    def test_qpsk_no_asm(self) :
        "Test without ASM to detect false positives"
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            [],
            frame_len_byte=5,
            M=4,
            ambiguity=1,
            offset_bytes=0,
            asm_prefix=[]
        )
    
    def test_qpsk_simple(self):
        """ Test without ambiguity or offset """
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=4,
            ambiguity=0,
            offset_bytes=0
        )
    
    
    def test_qpsk_ambiguity_1(self) :
        """ Test ambiguity """
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=4,
            ambiguity=1,
            offset_bytes=0
        )

    def test_qpsk_ambiguity_2(self) :
        """ Test ambiguity """
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=4,
            ambiguity=2,
            offset_bytes=0
        )

    def test_qpsk_ambiguity_3(self) :
        """ Test ambiguity """
        self.run_test(
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0x00,]*5 +
                [0xFF,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=4,
            ambiguity=3,
            offset_bytes=0
        )

    
    def test_qpsk_offset(self) :
        """ Test offset """
        self.run_test(
                [0xFF,]*5 +
                [0x00,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
                [0xFF,]*5 +
                [0x00,]*5 +
                [0xAA,]*5 +
                [0x55,]*5 +
                [0x00, 0x01, 0x02, 0x03, 0x04]
            ,
            frame_len_byte=5,
            M=4,
            ambiguity=0,
            offset_bytes=5
        )
    
    
if __name__ == '__main__':
    gr_unittest.run(qa_mpsk_ambiguity_resolver_chain)
