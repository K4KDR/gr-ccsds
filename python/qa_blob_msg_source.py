#!/usr/bin/env python
# 
# Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

class qa_blob_msg_source (gr_unittest.TestCase):

    def setUp (self):
	self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def runExperiment(self, num_blobs, blob_len):
	# blocks
	self.blob_source = ccsds.blob_msg_source_b(blob_len)
	self.snk = blocks.vector_sink_b(1)

	# connections
	self.tb.connect((self.blob_source, 0), (self.snk, 0))

	# start flowgraph
	self.tb.start()

	src_data = [];
	port = pmt.intern("in")

	for i in xrange(num_blobs):
		#generate random blob data
		data = map(int, numpy.random.randint(0, 256, blob_len))

		# store blob data for later comparison
		[src_data.append(byte) for byte in data]

		#generate message
		msg = pmt.make_u8vector(blob_len, 0x00);
		# fill message with data
		for j in xrange(blob_len) :
			pmt.u8vector_set(msg, j, data[j])

		#post message
		meta = pmt.make_dict()
		meta = pmt.dict_add(meta, pmt.intern("frame_number"), pmt.from_long(i))
		
		self.blob_source.to_basic_block()._post( port, pmt.cons(meta, msg) )
	
        
        #send EOF
	self.blob_source.to_basic_block()._post( port, pmt.PMT_EOF )
	
	# wait for flowgraph to end
	self.tb.wait()
	
	self.assertEqual (tuple(src_data), self.snk.data(), 'Output of MSG BLOB source does not match send data for %d blobs of length %d' % (num_blobs, blob_len))

    # send 0 BLOBs (just EOF) and return
    def test_eof(self):
	self.runExperiment(0, 1)

    # send one small BLOB
    def test_one_small(self):
	self.runExperiment(1, 1)

    # send 100 BLOBs of 100 byte
    def test_hundred(self):
	self.runExperiment(100, 100)

    # send one huge BLOB
    def test_one_large(self):
	self.runExperiment(1, 100000)

    # send many small BLOB
    def test_many_small(self):
	self.runExperiment(100000, 1)


if __name__ == '__main__':
    gr_unittest.run(qa_blob_msg_source, "qa_blob_msg_source.xml")
