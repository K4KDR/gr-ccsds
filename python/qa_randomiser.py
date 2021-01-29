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
import os
import pmt
import time

class qa_randomiser(gr_unittest.TestCase):

    def setUp (self):
            self.tb = gr.top_block ()

    def tearDown (self):
            self.tb = None

    def to_hex(self, dec):
            return '0x%02X' % (dec)

    def runSingleExperiment(self, messages_in, messages_out_exp,poly=0x95,seed=0xFF):
        #print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
        #raw_input ('Press Enter to continue: ')


        ##################################################
        # Blocks
        ##################################################
        self.null_src = ccsds.msg_null_src()
        self.randomizer = ccsds.randomiser(poly, seed)
        self.dbg = blocks.message_debug()

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(self.null_src, "out", self.randomizer, "in")
        self.tb.msg_connect(self.randomizer, "out", self.dbg, "store")
        #self.tb.msg_connect(self.randomizer, "out", self.dbg, "print")

        # start flowgraph
        self.tb.start()

        block = self.randomizer.to_basic_block()
        port = pmt.intern("in")

        num_messages = 0
        
        for i in range(len(messages_in)) :
            msg_in = pmt.make_u8vector(len(messages_in[i]), 0x00)
            for j in range(len(messages_in[i])) :
                    pmt.u8vector_set(msg_in, j, messages_in[i][j])

            meta = pmt.make_dict()
            meta = pmt.dict_add(meta, pmt.intern("frame_number"), pmt.from_long(i))
            
            block._post( port, pmt.cons(meta, msg_in) )
            num_messages = num_messages+1
        
        block._post( port, pmt.PMT_EOF )

        timeout = 50
        while(self.dbg.num_messages() < num_messages+1 and timeout > 0):
            time.sleep(0.1)
            timeout -= 1
        
        self.tb.stop()
        self.tb.wait()

        # test that all messages have been processed
        self.assertEqual (self.dbg.num_messages(), num_messages+1, '%d messages in store, %d expected.' % (self.dbg.num_messages(),(num_messages+1)))
        
        # test for EOF
        eof_msg = self.dbg.get_message(num_messages)
        self.assertEqual (pmt.is_eof_object(eof_msg), True, 'EOF block not at expected position')

        # test the blobs
        for i in range(len(messages_out_exp)):
            dbg_msg_in = self.dbg.get_message(i)
            dbg_msg = pmt.cdr(dbg_msg_in)
            dbg_hdr = pmt.car(dbg_msg_in)

            dbg_frame_num = int(pmt.to_long(pmt.dict_ref(dbg_hdr, pmt.intern("frame_number"), pmt.from_long(num_messages+100))))

            self.assertEqual (i, dbg_frame_num, 'Frame number %d does not match the expected %d' %        (dbg_frame_num, i))

            dbg_data = []
            [dbg_data.append(pmt.u8vector_ref(dbg_msg, j)) for j in range(pmt.length(dbg_msg))]
            dbg_data = tuple(dbg_data)

            out_hex = map(self.to_hex,dbg_data)
            exp_hex = map(self.to_hex,messages_out_exp[i])
            out_str = str(dbg_data)
            exp_str = str(messages_out_exp[i])

            self.assertEqual (messages_out_exp[i], dbg_data, 'BLOB data mismatch at BLOB no %d/%d:\n  out (hex): %s\n  exp (hex): %s\n  out (plain): %s\n  exp (plain): %s' %
                                (i+1, num_messages, out_hex, exp_hex, out_str, exp_str))


    def runChainExperiment(self, messages_in, poly=0x95,seed=0xFF):
        #print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
        #raw_input ('Press Enter to continue: ')


        ##################################################
        # Blocks
        ##################################################
        self.null_src = ccsds.msg_null_src()
        self.randomizer = ccsds.randomiser(poly, seed)
        self.derandomizer = ccsds.randomiser(poly, seed)
        self.dbg = blocks.message_debug()

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(self.null_src, "out", self.randomizer, "in")
        self.tb.msg_connect(self.randomizer, "out", self.derandomizer, "in")
        self.tb.msg_connect(self.derandomizer, "out", self.dbg, "store")
        #self.tb.msg_connect(self.derandomizer, "out", self.dbg, "print")

        # start flowgraph
        self.tb.start()

        block = self.randomizer.to_basic_block()
        port = pmt.intern("in")

        num_messages = 0
        
        for i in range(len(messages_in)) :
            msg_in = pmt.make_u8vector(len(messages_in[i]), 0x00)
            for j in range(len(messages_in[i])) :
                    pmt.u8vector_set(msg_in, j, messages_in[i][j])

            meta = pmt.make_dict()
            meta = pmt.dict_add(meta, pmt.intern("frame_number"), pmt.from_long(i))

            block._post( port, pmt.cons(meta, msg_in) )
            num_messages = num_messages+1
        
        block._post( port, pmt.PMT_EOF )

        timeout = 50
        while(self.dbg.num_messages() < num_messages+1 and timeout > 0):
            time.sleep(0.1)
            timeout -= 1
        
        self.tb.stop()
        self.tb.wait()

        # test that all messages have been processed
        self.assertEqual (self.dbg.num_messages(), num_messages+1, '%d messages in store, %d expected.' % (self.dbg.num_messages(),(num_messages+1)))
        
        # test for EOF
        eof_msg = self.dbg.get_message(num_messages)
        self.assertEqual (pmt.is_eof_object(eof_msg), True, 'EOF block not at expected position')

        # test the blobs
        for i in range(len(messages_in)):
            dbg_msg_in = self.dbg.get_message(i)
            dbg_msg = pmt.cdr(dbg_msg_in)
            dbg_hdr = pmt.car(dbg_msg_in)

            dbg_frame_num = int(pmt.to_long(pmt.dict_ref(dbg_hdr, pmt.intern("frame_number"), pmt.from_long(num_messages+100))))

            self.assertEqual (i, dbg_frame_num, 'Frame number %d does not match the expected %d' %        (dbg_frame_num, i))

            dbg_data = []
            [dbg_data.append(pmt.u8vector_ref(dbg_msg, j)) for j in range(pmt.length(dbg_msg))]
            dbg_data = tuple(dbg_data)

            out_hex = map(self.to_hex,dbg_data)
            exp_hex = map(self.to_hex,messages_in[i])
            out_str = str(dbg_data)
            exp_str = str(messages_in[i])

            self.assertEqual (messages_in[i], dbg_data, 'BLOB data mismatch at BLOB no %d/%d:\n  out (hex): %s\n  exp (hex): %s\n  out (plain): %s\n  exp (plain): %s' %
                                        (i+1, num_messages, out_hex, exp_hex, out_str, exp_str))



    ##################################################
    # Single Experiments
    ##################################################

    #'''
    def test_zero_in(self) :
        #ECSS defines the first 40 bits of the pseudo-random sequence. By
        #using zero as neutral element of XOR an input sequence of all
        #zeros should yield the pseudo-random sequence as output.
        
        messages_in = ( (0x00,0x00,0x00,0x00,0x00) ,)

        messages_out_exp = ( (0xFF,0x48,0x0E,0xC0,0x9A), )
        self.runSingleExperiment(messages_in, messages_out_exp,poly=0x95,seed=0xFF)
    #'''

    #'''
    def test_one_in(self) :
        #ECSS defines the first 40 bits of the pseudo-random sequence. To
        #check that XOR is used now a stream of all ones is inserted.
        #This should lead to the inverted pseudo-random sequence

        messages_in = ( (0xFF,0xFF,0xFF,0xFF,0xFF) ,)

        messages_out_exp = ( (0x00,0xB7,0xF1,0x3F,0x65), )
        self.runSingleExperiment(messages_in, messages_out_exp,poly=0x95,seed=0xFF)
    #'''

    #TODO For a complete test the whole sequence should be tested, not just
    # the first 40 bits. This requires the complete sequence from an
    # independent source (either by hand or a different implementation)

    ##################################################
    # Chain Experiments
    ##################################################

    #'''
    def test_chain_zeros(self) :
        #Test the concatenation of two randomisers which should output
        #the original input message.

        messages_in = ( (0x00,0x00,0x00,0x00,0x00) ,)*5

        self.runChainExperiment(messages_in, poly=0x95,seed=0xFF)
    #'''

    #'''
    def test_chain_ones(self) :
        #Test the concatenation of two randomisers which should output
        #the original input message.
        
        messages_in = ( (0xFF,0xFF,0xFF,0xFF,0xFF) ,)*10

        self.runChainExperiment(messages_in, poly=0x95,seed=0xFF)
    #'''

    #'''
    def test_chain_random(self) :
        #Test the concatenation of two randomisers which should output
        #the original input message.
        
        # 10 messages with 100 random bytes in each
        messages_in = (
                (5, 82, 139, 85, 105, 88, 138, 165, 207, 211, 56, 152, 35, 11, 200, 117, 121, 63, 9, 5, 169, 43, 151, 73, 253, 169, 101, 138, 11, 21, 201, 236, 76, 244, 135, 251, 11, 158, 76, 32, 18, 103, 228, 2, 249, 241, 56, 222, 95, 28, 144, 14, 115, 50, 79, 103, 164, 177, 189, 206, 198, 208, 39, 68, 155, 117, 57, 40, 81, 187, 94, 182, 232, 255, 125, 250, 141, 1, 187, 4, 176, 171, 67, 2, 152, 173, 204, 157, 86, 241, 203, 155, 222, 189, 235, 50, 124, 59, 197, 106),
                (213, 174, 135, 35, 255, 113, 81, 45, 255, 179, 16, 66, 21, 1, 8, 254, 193, 194, 100, 194, 171, 83, 38, 67, 187, 121, 106, 226, 186, 203, 39, 250, 197, 51, 206, 96, 124, 129, 194, 56, 74, 60, 117, 173, 234, 74, 119, 76, 235, 143, 78, 174, 103, 240, 65, 5, 177, 187, 247, 126, 34, 124, 190, 151, 44, 107, 117, 207, 13, 218, 97, 10, 90, 30, 0, 129, 17, 57, 51, 255, 213, 84, 197, 118, 138, 56, 72, 87, 223, 102, 72, 115, 223, 41, 247, 25, 61, 77, 161, 206),
                (155, 65, 42, 75, 154, 100, 88, 59, 216, 229, 238, 83, 31, 51, 220, 142, 208, 194, 115, 18, 237, 182, 146, 63, 156, 25, 222, 149, 236, 162, 252, 40, 10, 110, 231, 78, 181, 69, 190, 47, 91, 239, 252, 16, 102, 4, 215, 23, 229, 99, 40, 93, 159, 81, 99, 215, 131, 146, 184, 2, 84, 72, 76, 188, 46, 135, 179, 255, 240, 76, 233, 41, 128, 213, 200, 150, 72, 25, 178, 103, 176, 9, 149, 237, 153, 205, 176, 131, 69, 143, 48, 197, 228, 48, 208, 146, 58, 32, 60, 7),
                (17, 13, 33, 236, 31, 149, 134, 246, 190, 173, 118, 26, 199, 135, 81, 7, 104, 219, 108, 164, 229, 171, 13, 162, 95, 148, 138, 156, 146, 15, 157, 34, 191, 152, 231, 241, 77, 9, 71, 21, 88, 133, 196, 199, 177, 87, 211, 232, 92, 226, 36, 36, 24, 189, 180, 64, 203, 3, 88, 85, 26, 163, 36, 58, 248, 62, 30, 165, 168, 253, 152, 163, 177, 207, 211, 111, 75, 74, 211, 72, 31, 44, 227, 46, 224, 236, 184, 210, 136, 127, 121, 170, 195, 193, 159, 103, 50, 212, 196, 237),
                (9, 114, 154, 124, 58, 228, 195, 119, 244, 7, 221, 164, 117, 101, 134, 28, 161, 22, 220, 194, 70, 252, 136, 149, 34, 89, 120, 73, 87, 50, 151, 59, 255, 61, 163, 128, 64, 218, 48, 124, 243, 233, 225, 107, 58, 238, 82, 119, 169, 114, 211, 235, 29, 40, 209, 200, 159, 57, 200, 17, 77, 190, 156, 151, 171, 83, 176, 188, 130, 245, 146, 226, 244, 111, 13, 181, 107, 215, 239, 158, 132, 244, 250, 158, 74, 53, 238, 98, 111, 109, 5, 206, 218, 171, 231, 114, 110, 0, 161, 160),
                (198, 36, 127, 137, 129, 130, 146, 3, 128, 20, 47, 182, 196, 74, 47, 98, 224, 174, 103, 92, 197, 228, 77, 18, 64, 255, 94, 110, 178, 11, 118, 250, 171, 254, 179, 242, 119, 82, 244, 250, 224, 31, 70, 91, 11, 247, 54, 56, 108, 56, 84, 28, 210, 122, 182, 141, 22, 59, 22, 77, 136, 171, 137, 225, 82, 188, 98, 221, 223, 205, 33, 90, 192, 44, 248, 253, 34, 52, 29, 229, 237, 181, 156, 98, 45, 248, 204, 52, 191, 150, 76, 211, 214, 57, 102, 43, 164, 230, 242, 82),
                (238, 94, 203, 221, 180, 237, 72, 83, 131, 174, 15, 247, 85, 230, 226, 217, 214, 253, 199, 182, 117, 233, 78, 104, 186, 185, 238, 117, 127, 4, 193, 56, 214, 38, 36, 63, 88, 236, 195, 240, 42, 108, 193, 153, 234, 84, 49, 13, 252, 218, 227, 182, 189, 237, 66, 163, 125, 246, 174, 17, 251, 25, 175, 55, 223, 119, 214, 233, 217, 242, 73, 213, 121, 24, 209, 123, 87, 5, 183, 102, 82, 143, 211, 120, 164, 178, 42, 216, 220, 82, 98, 72, 89, 34, 146, 42, 60, 252, 217, 73),
                (156, 223, 101, 57, 118, 43, 30, 68, 35, 0, 171, 112, 52, 77, 185, 38, 16, 224, 158, 255, 154, 79, 164, 105, 7, 241, 199, 122, 123, 1, 7, 158, 66, 26, 9, 144, 79, 28, 106, 178, 119, 117, 161, 167, 85, 119, 2, 21, 44, 215, 216, 47, 183, 177, 255, 230, 169, 22, 184, 74, 115, 237, 35, 84, 180, 168, 228, 121, 122, 161, 218, 25, 24, 163, 103, 62, 62, 237, 75, 62, 35, 53, 67, 58, 42, 191, 182, 79, 131, 105, 174, 246, 222, 175, 54, 51, 41, 180, 21, 80),
                (223, 102, 150, 89, 228, 82, 99, 106, 32, 247, 240, 211, 149, 70, 223, 173, 215, 205, 22, 40, 54, 65, 87, 242, 165, 233, 121, 113, 9, 229, 16, 0, 11, 65, 229, 186, 156, 184, 251, 116, 43, 137, 40, 179, 18, 203, 83, 21, 142, 240, 44, 171, 42, 228, 124, 10, 91, 226, 187, 41, 177, 102, 144, 17, 7, 83, 71, 252, 69, 143, 138, 108, 16, 46, 204, 212, 57, 177, 15, 218, 181, 183, 1, 37, 190, 42, 253, 116, 158, 88, 29, 198, 170, 79, 9, 183, 255, 86, 71, 140), (230, 51, 186, 78, 232, 193, 171, 198, 106, 6, 133, 136, 240, 207, 1, 85, 19, 9, 76, 83, 187, 228, 82, 107, 251, 6, 9, 99, 63, 69, 229, 162, 58, 238, 59, 25, 132, 80, 78, 64, 230, 82, 103, 145, 43, 213, 223, 232, 113, 97, 98, 255, 27, 4, 26, 169, 248, 218, 223, 218, 61, 40, 237, 175, 69, 103, 37, 1, 198, 111, 165, 155, 16, 153, 140, 195, 45, 101, 199, 33, 205, 212, 73, 65, 179, 123, 32, 33, 65, 40, 23, 244, 207, 25, 156, 185, 179, 184, 102, 13)
                )

        self.runChainExperiment(messages_in, poly=0x95,seed=0xFF)
    #'''

if __name__ == '__main__':
        gr_unittest.run (qa_randomiser)
