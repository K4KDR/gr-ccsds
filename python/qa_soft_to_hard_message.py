#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2018 Martin Luelf.
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
import pmt
import time

class qa_soft_to_hard_message(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None
    
    """
        Run test with multiple PDUs, so all three inputs must be lists
        of dict, float list and byte list
    """
    def runTest(self, header_dict, bits_in, bytes_out_exp):
        
        """ Setup flowgraph """
        
        # blocks
        self.converter = ccsds.soft_to_hard_message()
        self.debug = blocks.message_debug()
        
        # connections
        self.tb.msg_connect(self.converter, "out", self.debug, "store")
        
        
        """ Run flowgraph """
        
        self.tb.start()
        
        block = self.converter.to_basic_block()
        port = pmt.intern("in")
        
        NUM_PDUS = len(header_dict)
        if(len(bits_in) != NUM_PDUS):
            raise ValueError('Inconsistent number of Header and Softbit lists')
        if(len(bytes_out_exp) != NUM_PDUS):
            raise ValueError('Inconsistent number of Header and expected output byte lists')
        
        for i in xrange(NUM_PDUS):
            # compile softbit PDU
            
            # dict
            meta = pmt.make_dict()
            for k,v in header_dict[i].items():
                meta = pmt.dict_add(meta,
                                    pmt.intern(k),
                                    pmt.to_pmt(v))
            
            data_soft = pmt.init_f32vector(len(bits_in[i]), bits_in[i])
            msg_post = pmt.cons(meta, data_soft)
            # send to block
            block._post( port, msg_post )
        
        # and signal EOF
        block._post( port, pmt.PMT_EOF )
        
        # wait for block to finish
        timeout = 50
        while(self.debug.num_messages() < NUM_PDUS+1 and timeout > 0):
            time.sleep(0.1)
            timeout -= 1
        
        self.tb.stop()
        self.tb.wait()
        
        
        """ check data """
        
        # test that all messages have been processed
        self.assertEqual (self.debug.num_messages(), NUM_PDUS+1, '%d messages in store, %d expected.' % (self.debug.num_messages(),(NUM_PDUS+1)))
        
        # test for EOF
        eof_msg = self.debug.get_message(NUM_PDUS)
        self.assertEqual (pmt.is_eof_object(eof_msg), True, 'EOF block not at expected position')
        
        # test PDUs
        for i in xrange(NUM_PDUS):
            _recv_msg = self.debug.get_message(i)
            
            self.assertEqual(pmt.is_pair(_recv_msg), True, "Message %d is not a pair" % i)
            
            _recv_payload = pmt.cdr(_recv_msg)
            _recv_header  = pmt.car(_recv_msg)
            
            """ Check header """
            for k,v in header_dict[i].items():
                _pmt_key = pmt.intern(k)
                self.assertEqual(
                  pmt.dict_has_key(_recv_header, _pmt_key),
                  True,
                  'Received header of PDU number %d is lacking the key "%s"' % (i, k)
                )
                
                _val_recv = pmt.to_python(
                    pmt.dict_ref(_recv_header,_pmt_key, pmt.PMT_NIL))
                
                self.assertEqual(v, _val_recv, 'Header value mismatch for key "%s": expected "%s", got "%s"' % (k, v, _val_recv))
            
            """ Check value """
            self.assertEqual(pmt.is_u8vector(_recv_payload), True, "Message %d payload is not a uint8 vector" % i)
            
            self.assertEqual(
                  pmt.length(_recv_payload),
                  len(bytes_out_exp[i]),
                  'Message %d payload does not have the right length. Expected %d, got %d' %
                    (i,
                     len(bytes_out_exp[i]),
                     pmt.length(_recv_payload)
                    )
            )
            
            for j in xrange(len(bytes_out_exp[i])):
                _byte_recv = pmt.u8vector_ref(_recv_payload, j)
                
                self.assertEqual(bytes_out_exp[i][j], _byte_recv,
                        'Byte %d in PDU %d does not match expected value. Expected %d, got %d' % (j,i,bytes_out_exp[i][j], _byte_recv))
    def test_001_t(self):
        """ Test correct bit conversion """
        self.runTest( 
            [{},],
            [
              [ -1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3,
                -0.2, -0.1,-0.01, 0.01,  0.1,  0.2,  0.3,  0.4,
                0.5,  0.6,  0.7,  0.8,  0.9,  1.0,  1.0,  1.0],
            ],
            [
              [0x00, 0x1F, 0xFF],
            ])
    
    def test_002_t(self):
        """ Test PDU conversion """
        o = -0.8 # kind of looks like a 0
        l = 0.8  # kind of looks like a 1
        self.runTest(
            [
              {'frame_number': 0, 'some_float': 1.23, 'some_flag': True, 'some_int': 1234567890},
              {'frame_number': 1, 'some_float': 1.24, 'some_flag': True, 'some_int': 123456789012},
              {'frame_number': 2, 'some_float': 1.25, 'some_flag': True, 'some_int': 12345678901234},
              {'frame_number': 3, 'some_float': 1.26, 'some_flag': True, 'some_int': 123456789056},
            ],
            [
              [o,o,o,o,o,o,o,o,  l,l,l,l,l,l,l,l,  o,l,o,l,o,l,o,l],
              [l,o,l,o,l,o,l,o,  o,o,l,l,l,o,o,l,  l,l,o,o,o,o,o,l],[o,o,o,o,l,l,l,l,  l,l,o,o,o,o,l,l,  o,o,l,l,o,o,l,l],[l,l,o,o,l,l,o,o,  o,o,l,l,o,o,l,l,  o,l,l,o,l,o,o,l],
            ],
            [
              [0x00, 0xFF, 0x55],
              [0xAA, 0x39, 0xC1],
              [0x0F, 0xC3, 0x33],
              [0xCC, 0x33, 0x69],
            ])

if __name__ == '__main__':
    gr_unittest.run(qa_soft_to_hard_message, "qa_soft_to_hard_message.xml")
