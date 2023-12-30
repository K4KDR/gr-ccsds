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

class qa_reversebits(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def to_hex(self, dec):
        return '0x%02X' % (dec)

    def runSingleExperiment(self, messages_in, messages_out_exp,indices=(0,)):
        #print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
        #raw_input ('Press Enter to continue: ')


        ##################################################
        # Blocks
        ##################################################
        self.null_src = ccsds.msg_null_src()
        self.reversebits = ccsds.reversebits(len(messages_in[0]), indices)
        self.dbg = blocks.message_debug()

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(self.null_src, "out", self.reversebits, "in")
        self.tb.msg_connect(self.reversebits, "out", self.dbg, "store")
        #self.tb.msg_connect(self.reversebits, "out", self.dbg, "print")

        # start flowgraph
        self.tb.start()

        block = self.reversebits.to_basic_block()
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


    ##################################################
    # Single Experiments
    ##################################################

    #'''
    def test_zero_in(self) :
        # Zeroes should always lead to zeros again
        
        messages_in = ( (0x00,0x00,0x00,0x00,0x00) ,)

        messages_out_exp = ( (0x00,0x00,0x00,0x00,0x00), )
        self.runSingleExperiment(messages_in, messages_out_exp,indices=(0,1))
    #'''

    #'''
    def test_ones_in(self) :
        # All ones should always lead to all ones again
        
        messages_in = ( (0xFF,0xFF) ,)

        messages_out_exp = ( (0xFF,0xFF), )
        self.runSingleExperiment(messages_in, messages_out_exp,indices=(0,))
    #'''

    #'''
    def test_complete_sequence_in(self) :
        # Input sequence of 0x00 0x01 0x02 0x0F 0xF0 0xAA 0x42 where all bytes should be reversed
        # Output (by hand)  0x00 0x80 0x40 0xF0 0x0F 0x55 0x42

        # And for the fun of it, lets test it twice (block has no memory so it's kind of senseless, but possible)

        messages_in = ( (0x00,0x01,0x02,0x0F,0xF0,0xAA,0x42) ,(0x00,0x01,0x02,0x0F,0xF0,0xAA,0x42))

        messages_out_exp = ( (0x00,0x80,0x40,0xF0,0x0F,0x55,0x42), (0x00,0x80,0x40,0xF0,0x0F,0x55,0x42))
        self.runSingleExperiment(messages_in, messages_out_exp,indices=range(7))
    #'''

    #'''
    def test_partial_sequence_in(self) :
        # Input sequence of 0x00 0x01 0x02 0x0F 0xF0 0xAA 0x42 where the following bytes should be reversed
        #                       0    1    2         4    5     
        # Output (by hand)  0x00 0x80 0x40 0x0F 0x0F 0x55 0x42

        # And for the fun of it, lets test it twice (block has no memory so it's kind of senseless, but possible)

        messages_in = ( (0x00,0x01,0x02,0x0F,0xF0,0xAA,0x42) ,(0x00,0x01,0x02,0x0F,0xF0,0xAA,0x42))

        messages_out_exp = ( (0x00,0x80,0x40,0x0F,0x0F,0x55,0x42), (0x00,0x80,0x40,0x0F,0x0F,0x55,0x42))
        self.runSingleExperiment(messages_in, messages_out_exp,indices=(0,1,2,4,5))
    #'''


if __name__ == '__main__':
    gr_unittest.run (qa_reversebits)
