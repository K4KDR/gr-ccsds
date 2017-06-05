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

class qa_softbit_msg_source (gr_unittest.TestCase):

    def setUp (self):
	self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def runExperiment(self, num_frames, frame_len):
	# blocks
	self.softbit_source = ccsds.softbit_msg_source_f(frame_len)
	self.snk = blocks.vector_sink_f(1)

	# connections
	self.tb.connect((self.softbit_source, 0), (self.snk, 0))

	# start flowgraph
	self.tb.start()

	src_data = [];
	port = pmt.intern("in")

	for i in xrange(num_frames):
		#generate random frame data
		data = map(float, numpy.random.random_sample(frame_len))

		# store frame data for later comparison
		[src_data.append(softbit) for softbit in data]

		#generate message
		msg = pmt.make_f32vector(frame_len, 0.0);
		# fill message with data
		for j in xrange(frame_len) :
			pmt.f32vector_set(msg, j, data[j])

		#post message
		meta = pmt.make_dict()
		meta = pmt.dict_add(meta, pmt.intern("frame_number"), pmt.from_long(i))
		
		self.softbit_source.to_basic_block()._post( port, pmt.cons(meta, msg) )
	
        
        #send EOF
	self.softbit_source.to_basic_block()._post( port, pmt.PMT_EOF )
	
	# wait for flowgraph to end
	self.tb.wait()
	
	self.assertFloatTuplesAlmostEqual (tuple(src_data), self.snk.data(), 4,  'Output of MSG Softbit Source does not match send data for %d frames of length %d' % (num_frames, frame_len))

    # send 0 Frames (just EOF) and return
    def test_eof(self):
	self.runExperiment(0, 1)

    # send one small Frame
    def test_one_small(self):
	self.runExperiment(1, 1)

    # send 100 Frames of 100 softbit
    def test_hundred(self):
	self.runExperiment(100, 100)

    # send one huge Frame
    def test_one_large(self):
	self.runExperiment(1, 10000)

    # send many small Frame
    def test_many_small(self):
	self.runExperiment(10000, 1)


if __name__ == '__main__':
    gr_unittest.run(qa_softbit_msg_source, "qa_softbit_msg_source.xml")
