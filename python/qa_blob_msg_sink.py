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

class qa_blob_msg_sink (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def runExperiment(self, num_blobs, blob_len):
        # generate data
        blobs = []
        data = []
        for i in range(num_blobs) :
            blob = [int(i) for i in numpy.random.randint(0, 256, blob_len)]
            [data.append(byte) for byte in blob]
            blobs.append(blob)

            
        # blocks
        self.blob_sink = ccsds.blob_msg_sink_b(blob_len)
        self.src = blocks.vector_source_b(tuple(data), False)
        self.dbg = blocks.message_debug()

        # connections
        self.tb.connect((self.src, 0), (self.blob_sink, 0))
        self.tb.msg_connect(self.blob_sink, "out", self.dbg, "store")

        # start flowgraph
        self.tb.start()

        # timeout = 50
        # while(self.dbg.num_messages() < num_blobs+1 and timeout > 0):
        #     time.sleep(0.1)
        #     timeout -= 1
        # self.assertTrue(timeout > 0, 'Test timed out')
            
        # self.tb.stop()
        self.tb.wait()

        self.assertTrue(self.dbg.num_messages() >= num_blobs)

        # test for EOF
        # Disabled, because EOF messages tend to get lost in 3.8 TODO: re-enable once this issue has been solved
        #eof_msg = self.dbg.get_message(self.dbg.num_messages()-1)
        #self.assertEqual (pmt.is_eof_object(eof_msg), True, 'EOF block no at expected position')

        # test the blobs
        for i in range(min(self.dbg.num_messages()-1, num_blobs)):
            dbg_msg_in = self.dbg.get_message(i)
            dbg_msg = pmt.cdr(dbg_msg_in)
            dbg_data = []
            [dbg_data.append(pmt.u8vector_ref(dbg_msg, j)) for j in range(pmt.length(dbg_msg))]
            self.assertEqual (blobs[i], dbg_data, 'BLOB data mismatch in %d byte BLOB no %d/%d' % (blob_len, i+1, num_blobs))



    # send 0 BLOBs (just EOF) and return
    def test_eof(self):
        self.runExperiment(0, 0)

    # send one small BLOB
    def test_one_small(self):
        self.runExperiment(1, 1)

    # send 100 BLOBs of 100 byte
    def test_hundred(self):
        self.runExperiment(100, 100)

    # send one huge BLOB
    def test_one_large(self):
        self.runExperiment(1, 10000)

    # send many small BLOB
    def test_many_small(self):
        self.runExperiment(10000, 1)


if __name__ == '__main__':
    gr_unittest.run(qa_blob_msg_sink)
