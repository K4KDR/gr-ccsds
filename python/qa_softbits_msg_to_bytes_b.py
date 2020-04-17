#!/usr/bin/env python
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

def to_hexstr(data):
    return ' '.join(['0x{:02X}'.format(b) for b in data])

class qa_softbits_msg_to_bytes_b (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    # no frames (just EOF)
    def test_eof(self):
        self.runTest(
            [],
            []
        )

    # one small frame
    def test_one_small(self):
        self.runTest(
            [[-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
              -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
               1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
               1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
              -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,
              ],
            ],
            [
                0x00, 0x00, 0xFF, 0x80, 0x01
            ]
        )
    
    # one small frame non uniform intensity
    def test_one_small_non_uniform(self):
        self.runTest(
            [[-0.001, -0.001, -0.001, -0.020, -5.001, -0.001, -0.020, -0.101,
              -0.001, -0.020, -0.020, -5.020, -0.001, -0.001, -0.001, -0.001,
               0.020,  0.001,  0.001,  0.020,  0.020,  0.001,  4.001,  0.001,
               0.020, -0.020, -8.001, -0.001, -0.020, -0.020, -0.201, -0.020,
              -0.001, -0.001, -0.020, -0.020, -0.020, -0.020, -0.001,  0.001,
              ],
            ],
            [
                0x00, 0x00, 0xFF, 0x80, 0x01
            ]
        )
    
    # multiple frames
    def test_multiple(self):
        self.runTest(
            [
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,  # 0x01
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0, -1.0,  # 0x02
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,  1.0,  # 0x03
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0,  1.0, -1.0, -1.0,  # 0x04
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0,  1.0, -1.0,  1.0,  # 0x05
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,  # 0x01
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,  # 0x01
                ],
                [ 1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x80
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,  # 0x01
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0,  # 0x01
                 -1.0, -1.0, -1.0,  1.0, -1.0, -1.0, -1.0,  1.0,  # 0x11
                ],
                [-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                 -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  # 0x00
                ],
            ],
            [
                0x00, 0x00, 
                0x00, 0x01, 
                0x00, 0x02, 
                0x00, 0x03, 
                0x00, 0x04, 
                0x00, 0x05,
                0x01, 0x01, 
                0x80, 0x01, 
                0x01, 0x11, 
                0x00, 0x00,
            ]
        )

    def runTest(self, input_softbits, expected_bytes):
        num_frames = len(input_softbits)
        
        # blocks
        self.softbit_source = ccsds.softbits_msg_to_bytes_b()
        self.snk = blocks.vector_sink_b(1)

        # connections
        self.tb.connect((self.softbit_source, 0), (self.snk, 0))

        # start flowgraph
        self.tb.start()

        port = pmt.intern("in")

        for i in xrange(len(input_softbits)):
            frame_len = len(input_softbits[i])
            
            self.assertTrue(frame_len%8==0, 'Frame length for frame {} is not a multiple of 8'.format(i))
            
            frame_len_bytes = frame_len / 8
            
            #post message
            data = pmt.init_f32vector(frame_len, input_softbits[i])
            meta = pmt.make_dict()
            meta = pmt.dict_add(meta, pmt.intern("frame_number"), pmt.from_long(i))
            
            self.softbit_source.to_basic_block()._post( port, pmt.cons(meta, data) )
        
            
        #send EOF
        self.softbit_source.to_basic_block()._post( port, pmt.PMT_EOF )
        
        # wait for flowgraph to end
        self.tb.wait()
        
        computed_data = self.snk.data()
        for i in xrange(num_frames):
            frame_len = len(input_softbits[i])
            self.assertTrue(frame_len%8==0, 'Frame length for frame {} is not a multiple of 8'.format(i))
            frame_len_bytes = frame_len / 8
            
            frame_computed = tuple( computed_data[i*frame_len_bytes:(i+1)*frame_len_bytes])
            frame_expected = tuple(expected_bytes[i*frame_len_bytes:(i+1)*frame_len_bytes])
            
            self.assertTupleEqual(frame_expected, frame_computed,  'Output for frame {} does not match.\nExpected:\n{}\nComputed:\n{}'.format(i, to_hexstr(frame_expected), to_hexstr(frame_computed)))

    

if __name__ == '__main__':
    gr_unittest.run(qa_softbits_msg_to_bytes_b, "qa_softbits_msg_to_bytes_b.xml")
