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
import pmt
import numpy
import random
import time

class qa_rs_chain (gr_unittest.TestCase):

        def setUp (self):
                self.tb = gr.top_block ()

        def tearDown (self):
                self.tb = None

        # --message--> ccsds_rs_encode --message--> ccsds_rs_decode --message-->
        # BLOB Message source --stream--> vector source
        def runMessageTest(self, I, num_frames):
                data = tuple(map(int, numpy.random.randint(0, 256, 223*num_frames*I)))
                ##################################################
                # Blocks
                ##################################################
                self.src = blocks.vector_source_b(data, False)
                self.snk = blocks.vector_sink_b(1)
                self.encoder = ccsds.rs_encode(I)
                self.decoder = ccsds.rs_decode(I, int(0), int(ccsds.DECODER_VERBOSITY_QUIET))
                self.blob_source = ccsds.blob_msg_source_b(223*I)
                self.blob_sink = ccsds.blob_msg_sink_b(223*I)

                ##################################################
                # Connections
                ##################################################
                self.tb.connect((self.src, 0), (self.blob_sink, 0))
                self.tb.connect((self.blob_source, 0), (self.snk, 0))

                ##################################################
                # Asynch Message Connections
                ##################################################
                self.tb.msg_connect(self.blob_sink, "out", self.encoder, "in")
                self.tb.msg_connect(self.decoder, "out", self.blob_source, "in")
                self.tb.msg_connect(self.encoder, "out", self.decoder, "in")

                #self.dbg = gr.message_debug()
                #self.tb.msg_connect(self.blob_sink, "out", self.dbg, "print")

                # start
                self.tb.start()

                while(len(self.snk.data()) < 223*num_frames*I):
                        time.sleep(0.1)
                        #print "%d/%d samples arrived, sleep until all data arrived" % (len(self.snk.data()), 223*num_frames*I)

                # stop flowgraph
                self.tb.stop()
                self.tb.wait()

                self.assertEqual(data, self.snk.data(), 'Output is not matching input for I=%d (using ccsds_rs_decode)' % (I))

        

#        # Randm Source --stream--> BLOB Message Sink --message--> ccsds_rs_encode
#        # --message--> BLOB Message source --stream--> ccsds_rs_decode_bb --msg-->
#        # msg_source --stream--> vector source
#        def runStreamTest(self, num_frames):
#                I = 5
#                data = tuple(map(int, numpy.random.randint(0, 256, 223*num_frames*I)))
#                #data = tuple(map(int, numpy.random.randint(1, 2, 223*num_frames*I)))
#                ##################################################
#                # Message Queues
#                ##################################################
#                decoder_msgq_out = msg_to_byte_msgq_in = gr.msg_queue(2)
#
#                ##################################################
#                # Blocks
#                ##################################################
#                self.src = blocks.vector_source_b(data, False)
#                self.snk = blocks.vector_sink_b(1)
#                self.msg_to_byte = blocks.message_source(gr.sizeof_char*1, msg_to_byte_msgq_in)
#                self.encoder = ccsds.rs_encode(I)
#                self.decoder = ccsds.rs_decode_bb(decoder_msgq_out, 10200)
#                self.byte_to_blob_msg = ccsds.blob_msg_sink_b(223*I)
#                self.blob_msg_to_byte = ccsds.blob_msg_source_b(255*I)
#
#                ##################################################
#                # Connections
#                ##################################################
#                self.tb.connect((self.src, 0), (self.byte_to_blob_msg, 0))
#                self.tb.connect((self.blob_msg_to_byte, 0), (self.decoder, 0))
#                self.tb.connect((self.msg_to_byte, 0), (self.snk, 0))
#
#                ##################################################
#                # Asynch Message Connections
#                ##################################################
#                self.tb.msg_connect(self.byte_to_blob_msg, "out", self.encoder, "in")
#                self.tb.msg_connect(self.encoder, "out", self.blob_msg_to_byte, "in")
#
#                #self.dbg = gr.message_debug()
#                #self.tb.msg_connect(self.blob_sink, "out", self.dbg, "print")
#
#                # start
#                self.tb.start()
#
#                while(len(self.snk.data()) < 223*num_frames*I):
#                        time.sleep(0.1)
#                        #print "%d/%d samples arrived, sleep until all data arrived" % (len(self.snk.data()), 223*num_frames*I)
#
#                # stop flowgraph
#                self.tb.stop()
#                self.tb.wait()
#
#                self.assertEqual(data, self.snk.data(), 'Output is not matching input for I=%d (using ccsds_rs_decode_bb)' % (I))
#

        ##
        ## ccsds_rs_decode
        ##

        #'''
        # Send one uninterleaved frame
        def test_message_once_I1(self):
                self.runMessageTest(1,1)
        #'''

        #'''
        # Send two frames with I=1
        def test_message_twice_I1(self):
                self.runMessageTest(2,1)
        #'''

        #'''
        # Send two frames with I=2
        def test_message_twice_I2(self):
                self.runMessageTest(2,2)
        #'''

        #'''
        # Send two frames with I=3
        def test_message_twice_I3(self):
                self.runMessageTest(3,2)
        #'''

        #'''
        # Send two frames with I=4
        def test_message_twice_I4(self):
                self.runMessageTest(4,2)
        #'''

        #'''
        # Send two frames with I=5
        def test_message_twice_I5(self):
                self.runMessageTest(5,2)
        #'''

        #'''
        # Send two frames with I=8
        def test_message_twice_I8(self):
                self.runMessageTest(8,2)
        #'''

        #'''
        # Send multiple frames with I=1
        def test_message_multiple_I1(self):
                self.runMessageTest(1,20)
        #'''

        #'''
        # Send multiple frames with I=2
        def test_message_multiple_I2(self):
                self.runMessageTest(2,20)
        #'''

        #'''
        # Send multiple frames with I=3
        def test_message_multiple_I3(self):
                self.runMessageTest(3,20)
        #'''

        #'''
        # Send multiple frames with I=4
        def test_message_multiple_I4(self):
                self.runMessageTest(4,20)
        #'''

        #'''
        # Send multiple frames with I=5
        def test_message_multiple_I5(self):
                self.runMessageTest(5,20)
        #'''

        #'''
        # Send multiple frames with I=8
        def test_message_multiple_I8(self):
                self.runMessageTest(8,20)
        #'''

        ##
        ## ccsds_rs_decode_bb
        ##


#        # Send one uninterleaved frame
#        #'''
#        def test_stream_once(self):
#                self.runStreamTest(1)
#        #'''
#
#        #'''
#        # Send multiple frames
#        def test_stream_multiple(self):
#                self.runStreamTest(20)
#        #'''


if __name__ == '__main__':
        gr_unittest.run(qa_rs_chain)
