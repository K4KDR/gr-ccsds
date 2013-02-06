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

class qa_ccsds_msg_blob (gr_unittest.TestCase):

    def setUp (self):
	self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def runExperiment(self, num_blobs, blob_len):
	data = tuple(map(int, numpy.random.randint(0, 256, num_blobs*blob_len)))
	src = self.random_source_x_0 = gr.vector_source_b(data, False)
	sink = self.gr_vector_sink_x_0 = gr.vector_sink_b(1)
	self.ccsds_msg_blob_source_b_0 = ccsds_swig.msg_blob_source_b(blob_len)
	self.ccsds_msg_blob_sink_b_0 = ccsds_swig.msg_blob_sink_b(blob_len)

	##################################################
	# Connections
	##################################################
	self.tb.connect((self.ccsds_msg_blob_source_b_0, 0), (self.gr_vector_sink_x_0, 0))
	self.tb.connect((self.random_source_x_0, 0), (self.ccsds_msg_blob_sink_b_0, 0))

	##################################################
	# Asynch Message Connections
	##################################################
	self.tb.msg_connect(self.ccsds_msg_blob_sink_b_0, "out", self.ccsds_msg_blob_source_b_0, "in")	

	self.tb.run()
	
	return ( data, sink.data() )
	
    def test_msg_blob_once_1(self):
	print 'Blocked waiting for GDB attach (pid = %d)' % (os.getpid(),)
	raw_input ('Press Enter to continue: ')
	(src_data, sink_data) = self.runExperiment(1, 1)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for BLOB length 1')

    def test_msg_blob_once_10(self):
	(src_data, sink_data) = self.runExperiment(1, 10)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for BLOB length 10')

    def test_msg_blob_once_101(self):
	(src_data, sink_data) = self.runExperiment(1, 101)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for BLOB length 101')

    def test_msg_blob_once_1024(self):
	(src_data, sink_data) = self.runExperiment(1, 1024)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for BLOB length 1024')

    def test_msg_blob_multiple_large(self):
	(src_data, sink_data) = self.runExperiment(500, 5000)
	self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for BLOB length 1024')

    def test_msg_blob_multiple_random(self):
	random.seed()

	for t in range(0, 10):
		num_blobs = random.randint(2,100)
		blob_len  = random.randint(1,1000)
		(src_data, sink_data) = self.runExperiment(num_blobs, blob_len)
		self.assertEqual (src_data, sink_data, 'Output of MSG BLOB source does not match MSG BLOB sink for %d BLOBs of length %d' % (num_blobs, blob_len))


if __name__ == '__main__':
    gr_unittest.main ()
