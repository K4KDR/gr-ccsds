#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2016 <+YOU OR YOUR COMPANY+>.
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
import os
import pmt
import time

class qa_ldpc_encoder (gr_unittest.TestCase):
    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def to_hex(self, dec):
        return '0x%02X' % (dec)
    
    def runTest(self, messages_in, messages_out_exp, generator, systype, puncttype, num_punct, punct_pos):
        #print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
        #raw_input ('Press Enter to continue: ')


        ##################################################
        # Blocks
        ##################################################
        self.null_src = ccsds.msg_null_src()
        self.encoder = ccsds.ldpc_encoder(generator, systype, puncttype, num_punct, punct_pos)
        self.dbg = blocks.message_debug()

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(self.null_src, "out", self.encoder, "in")
        self.tb.msg_connect(self.encoder, "out", self.dbg, "store")
        #self.tb.msg_connect(self.encoder, "out", self.dbg, "print")

        # start flowgraph
        self.tb.start()

        block = self.encoder.to_basic_block()
        port = pmt.intern("in")

        num_messages = 0
        
        for i in xrange(len(messages_in)) :
            msg_in = pmt.make_u8vector(len(messages_in[i]), 0x00)
            for j in xrange(len(messages_in[i])) :
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
        for i in xrange(len(messages_out_exp)):
            dbg_msg_in = self.dbg.get_message(i)
            dbg_msg = pmt.cdr(dbg_msg_in)
            dbg_hdr = pmt.car(dbg_msg_in)

            dbg_frame_num = int(pmt.to_long(pmt.dict_ref(dbg_hdr, pmt.intern("frame_number"), pmt.from_long(num_messages+100))))

            self.assertEqual (i, dbg_frame_num, 'Frame number %d does not match the expected %d' %    (dbg_frame_num, i))

            dbg_data = tuple([pmt.u8vector_ref(dbg_msg, j) for j in xrange(pmt.length(dbg_msg))])
            
            out_hex = map(self.to_hex,dbg_data)
            exp_hex = map(self.to_hex,messages_out_exp[i])
            out_str = str(dbg_data)
            exp_str = str(messages_out_exp[i])

            self.assertEqual (messages_out_exp[i], dbg_data, 'BLOB data mismatch at BLOB no %d/%d:\n  out (hex): %s\n  exp (hex): %s\n  out (plain): %s\n  exp (plain): %s' %
                        (i+1, num_messages, out_hex, exp_hex, out_str, exp_str))

    def test_001 (self):
        # Encode all zero frames
        NUM_REP = 2
        
        message_in = (0x00,)*128
        message_out_exp = (0x00,)*256
        
        messages_in = (message_in,)*NUM_REP
        messages_out_exp = (message_out_exp,)*NUM_REP
        
        # TODO Build common path for code definition files
        self.runTest(messages_in, messages_out_exp, '/home/v1tzl1/work/MOVE/LDPC/code/codes/gAR4JA_r12_k1024n.gen', ccsds.LDPC_SYS_FRONT, ccsds.LDPC_PUNCT_BACK, 512, [])

if __name__ == '__main__':
    gr_unittest.run(qa_ldpc_encoder, "qa_ldpc_encoder.xml")
