#!/usr/bin/env python
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
import pmt
import time

def pdu(payload, **kwargs):
    return pmt.cons(pmt.to_pmt(kwargs), pmt.to_pmt(payload))

class qa_sort_frames (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=True,
                        sort_field='asm_number',
                        score_field='asm_score',
                        pick_highest_score=True,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ]
        )

    def test_002_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='asm_score',
                        pick_highest_score=True,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ]
        )
    
    def test_003_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='asm_score',
                        pick_highest_score=False,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                        ]
        )
    
    
    def test_004_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='',
                        pick_highest_score=True,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                        ]
        )
    
    def test_005_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='',
                        pick_highest_score=False,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x01, 0xFF, 0x02, 0x00], asm_number=2, asm_score=1.5),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x01, 0xAA, 0x01, 0xFF], asm_number=2, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ]
        )
    
    def test_005_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='asm_score',
                        pick_highest_score=True,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                        ]
        )
    
    def test_006_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='asm_score',
                        pick_highest_score=False,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                        ]
        )
    
    def test_007_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='',
                        pick_highest_score=True,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                        ]
        )
    
    def test_008_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=False,
                        sort_field='asm_number',
                        score_field='',
                        pick_highest_score=False,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                        ]
        )
    
    def test_009_t (self):
        self.run_test(window_size=5,
                        allow_duplicates=True,
                        sort_field='asm_number',
                        score_field='',
                        pick_highest_score=False,
                        frames_in=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                        ],
                        frames_expected=[
                            pdu([0x00, 0xAA, 0x01, 0xFF], asm_number=1, asm_score=0.5),
                            pdu([0x00, 0xFF, 0x02, 0x00], asm_number=1, asm_score=1.5),
                            pdu([0x00, 0xFF, 0x03, 0x00], asm_number=1, asm_score=2.5),
                            pdu([0x00, 0xFF, 0x04, 0x00], asm_number=1, asm_score=0.2),
                            pdu([0x02, 0xAA, 0x01, 0xFF], asm_number=3, asm_score=0.5),
                            pdu([0x02, 0xFF, 0x02, 0x00], asm_number=3, asm_score=1.5),
                            pdu([0x03, 0xFF, 0x02, 0x00], asm_number=4, asm_score=1.5),
                            pdu([0x03, 0xAA, 0x01, 0xFF], asm_number=4, asm_score=0.5),
                            pdu([0x03, 0xFF, 0x03, 0x00], asm_number=4, asm_score=4.5),
                        ]
        )
    
    
    def run_test(self, window_size, allow_duplicates, sort_field, score_field, pick_highest_score, frames_in=None, frames_expected=None):
        
        self.assertTrue(frames_in is not None, 'Invalid test, no input frames defined.')
        self.assertTrue(frames_expected is not None, 'Invalid test, no expected frames defined.')
        
        ##################################################
        # Blocks
        ##################################################
        src = ccsds.msg_null_src()
        sort = ccsds.sort_frames(window_size, allow_duplicates, sort_field, score_field, pick_highest_score, ccsds.SORT_FRAME_OUTPUT_QUIET)
        store = blocks.message_debug()

        ##################################################
        # Asynch Message Connections
        ##################################################
        self.tb.msg_connect(src, 'out', sort, 'in')
        self.tb.msg_connect(sort, 'out', store, 'store')
        
        ##################################################
        # Run Flowgraph
        ##################################################
        
        self.tb.start()
        
        sort_block_basic = sort.to_basic_block()
        port = pmt.intern("in")
        
        for frame in frames_in:
            sort_block_basic._post( port, frame )
        
        sort_block_basic._post( port, pmt.PMT_EOF )
        
        
        timeout = 50
        while(timeout >= 0):
            time.sleep(0.1)
            timeout -= 1
            
            num_messages = store.num_messages()
            if num_messages == 0:
                # not a single message yet, keep waiting
                continue
            
            last_message = store.get_message(num_messages-1)
            if pmt.is_eof_object(last_message):
                # eof received, stop waiting
                #print('EOF received, aborting')
                break
            else:
                # no eof received yet, keep going
                continue
        
        self.assertTrue(timeout >= 0, 'Unittest timed out.')
        
        self.assertTrue( pmt.is_eof_object(store.get_message(store.num_messages()-1)), 'Last frame output must be an EOF object.')
        
        frames_computed = [store.get_message(i) for i in range(store.num_messages()-1)]
        
        #print('Expected:\n  {}'.format('\n  '.join([str(f) for f in frames_expected])))
        #print('Computed:\n  {}'.format('\n  '.join([str(f) for f in frames_computed])))
        
        for i in range(min(len(frames_expected), len(frames_computed))):
            hdr_computed = pmt.car(frames_computed[i])
            pld_computed = pmt.cdr(frames_computed[i])
            hdr_expected = pmt.car(frames_expected[i])
            pld_expected = pmt.cdr(frames_expected[i])
            
            self.compare_headers(hdr_expected, hdr_computed)
            
            self.compare_payload(pld_expected, pld_computed, hdr_expected)
            
        
        self.assertTrue(len(frames_expected) == len(frames_computed),
                        'Expected {} frames, but computed {}.'.format(len(frames_expected), len(frames_computed))
        )
        
        
    def compare_headers(self, expected, computed):
        self.assertTrue( pmt.is_dict(expected), 'Expected header is not a dictionary {}'.format(expected) )
        self.assertTrue( pmt.is_dict(computed), 'Computed header is not a dictionary {}'.format(computed) )
        
        expected_dict = pmt.to_python(expected)
        computed_dict = pmt.to_python(computed)
        
        expected_str = '{{ {} }}'.format(', '.join(['{}:{}'.format(k, expected_dict[k]) for k in sorted(expected_dict.keys())]))
        computed_str = '{{ {} }}'.format(', '.join(['{}:{}'.format(k, computed_dict[k]) for k in sorted(computed_dict.keys())]))
        
        expected_keys = set(expected_dict.keys())
        computed_keys = set(computed_dict.keys())
        
        self.assertTrue(expected_keys <= computed_keys, 'Header entry is missing the expected keys {}\n. Expected header: {}\nComputed header: {}'.format(
            expected_keys-(expected_keys & computed_keys),
            expected_str,
            computed_str
            ))
        
        self.assertTrue(computed_keys <= expected_keys, 'Header entry has additional unexpected keys {}\n. Expected header: {}\nComputed header: {}'.format(
            computed_keys-(computed_keys & expected_keys),
            expected_str,
            computed_str
            ))
        
        for k in expected_keys:
            self.assertTrue( expected_dict[k] == computed_dict[k], 'Header entry {} does not match. Expected "{}", computed "{}".\nExpected header: {}\nComputed header: {}'.format(
                k, expected_dict[k], computed_dict[k], expected_str, computed_str
            ))
    
    def compare_payload(self, expected, computed, header_exp_str):
        self.assertTrue( pmt.is_vector(expected), 'Expected payload is not a vector.')
        self.assertTrue( pmt.is_vector(computed), 'Computed payload is not a vector.')
        
        computed_len = pmt.length(computed)
        expected_len = pmt.length(expected)
        
        computed_str = 'payload with {:d} elements {}'.format(computed_len, str(computed))
        expected_str = 'payload with {:d} elements {}'.format(expected_len, str(expected))
        
        msg = 'Expected payload does not match the computed payload.\n         header: {} \n  computed payload: {}\n  expected_payload: {}'.format(header_exp_str, expected_str, computed_str)
        
        self.assertTrue( computed_len == expected_len, msg)
        
        expected_list = pmt.to_python(expected)
        computed_list = pmt.to_python(computed)
        
        for i in range(expected_len):
            self.assertEqual( expected_list[i], computed_list[i] , msg)

if __name__ == '__main__':
    gr_unittest.run(qa_sort_frames, "qa_sort_frames.xml")
