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
import ccsds_swig
from cmath import rect
from math import pi, sqrt
import random
import numpy
import os

class qa_ccsds_blob_msg_chain (gr_unittest.TestCase):

    def setUp (self):
	self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def runExperiment(self, num_blobs, blob_len):
	data = tuple(map(int, numpy.random.randint(0, 256, num_blobs*blob_len)))
	src = self.random_source_x_0 = gr.vector_source_b(data, False)
	sink = self.gr_vector_sink_x_0 = gr.vector_sink_b(1)
	self.ccsds_blob_msg_source_b_0 = ccsds_swig.blob_msg_source_b(blob_len)
	self.ccsds_blob_msg_sink_b_0 = ccsds_swig.blob_msg_sink_b(blob_len)

	##################################################
	# Connections
	##################################################
	self.tb.connect((self.ccsds_blob_msg_source_b_0, 0), (self.gr_vector_sink_x_0, 0))
	self.tb.connect((self.random_source_x_0, 0), (self.ccsds_blob_msg_sink_b_0, 0))

	##################################################
	# Asynch Message Connections
	##################################################
	self.tb.msg_connect(self.ccsds_blob_msg_sink_b_0, "out", self.ccsds_blob_msg_source_b_0, "in")	

	self.tb.start()
	#self.tb.stop()
	self.tb.wait()
	
	return ( data, sink.data() )
	
    # Test empty datastream (EOF only)
    def test_eof(self):
	(src_data, sink_data) = self.runExperiment(0, 1)
	self.assertEqual (sink_data, (), 'Expected empty dataset')

    # Send one small BLOB
    def test_blob_msg_once_small(self):
	(src_data, sink_data) = self.runExperiment(1, 1)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for a small single BLOB')

    # Send a few medium BLOBs
    def test_blob_msg_multiple_medium(self):
	(src_data, sink_data) = self.runExperiment(100, 100)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for multiple medium BLOBs')

    # Send on huge BLOB
    def test_blob_msg_once_huge(self):
	(src_data, sink_data) = self.runExperiment(1, 100000)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for a huge single BLOB')

    # Send multiple small BLOBs
    def test_blob_msg_multiple_small(self):
	(src_data, sink_data) = self.runExperiment(100000, 1)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for a huge number of small BLOBs')

if __name__ == '__main__':
    gr_unittest.main ()
