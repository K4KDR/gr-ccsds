#!/usr/bin/env python3
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
import numpy as np
import math

class qa_mpsk_mod_demod_chain(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def runChainExperiment(self, data_in, M):
        ##################################################
        # Variables
        ##################################################
        self.M = M
        self.ldM = ldM = int(math.log(M)/math.log(2))
        offset = 0.5 if M==4 else 0.0
        num_bits = len(data_in)*8
        self.assertEqual(0, num_bits % ldM)
        
        ##################################################
        # Blocks
        ##################################################
        self.src = blocks.vector_source_b(data_in, False)
        self.packed_to_unpacked = blocks.packed_to_unpacked_bb(ldM, gr.GR_MSB_FIRST)
        self.mod = ccsds.mpsk_mod_bc(M)
        self.demod = ccsds.mpsk_detector_soft_cf(M, offset)
        self.decision = ccsds.softbittobit()
        self.unpacked_to_packed = ccsds.pack_bit_to_bytes()
        self.sink = blocks.vector_sink_b()
        
        ##################################################
        # Connections
        ##################################################
        self.tb.connect((self.src, 0), (self.packed_to_unpacked, 0))
        self.tb.connect((self.packed_to_unpacked, 0), (self.mod, 0))
        self.tb.connect((self.mod, 0), (self.demod, 0))
        self.tb.connect((self.demod, 0), (self.decision, 0))
        self.tb.connect((self.decision, 0), (self.unpacked_to_packed, 0))
        self.tb.connect((self.unpacked_to_packed, 0), (self.sink, 0))
        
        # start flowgraph
        self.tb.run()
        
        data_computed = self.sink.data()
        
        data_expected_hex = str(['0x{:02X}'.format(e) for e in data_in])
        data_computed_hex = str(['0x{:02X}'.format(e) for e in data_computed])
        message = 'Computed data is not matching expected data for M={}.\nExpected: {} ({} elements)\nComputed: {} ({} elements)'.format(M, data_expected_hex, len(data_in), data_computed_hex, len(data_computed))
        
        self.assertComplexTuplesAlmostEqual(data_in, data_computed, msg=message)

    ##################################################
    # Test cases
    ##################################################

    def test_bpsk_0(self) :
        self.runChainExperiment([0x00, 0x00, 0xFF, 0xFF, 0xAA, 0xAA, 0x55, 0x55], 2)

    def test_bpsk_1(self) :
        np.random.seed(1)
        self.runChainExperiment(np.random.randint(0, 256, 1024), 2)

    def test_bpsk_2(self) :
        np.random.seed(2)
        self.runChainExperiment(np.random.randint(0, 256, 10240), 2)

    def test_bpsk_3(self) :
        np.random.seed(3)
        self.runChainExperiment(np.random.randint(0, 256, 512), 2)

    def test_bpsk_4(self) :
        np.random.seed(4)
        self.runChainExperiment(np.random.randint(0, 256, 5120), 2)

    def test_bpsk_5(self) :
        np.random.seed(5)
        self.runChainExperiment(np.random.randint(0, 256, 1024), 2)


    def test_qpsk_0(self) :
        self.runChainExperiment([0x00, 0x00, 0xFF, 0xFF, 0xAA, 0xAA, 0x55, 0x55], 4)

    def test_qpsk_1(self) :
        np.random.seed(1)
        self.runChainExperiment(np.random.randint(0, 256, 1024), 4)

    def test_qpsk_2(self) :
        np.random.seed(2)
        self.runChainExperiment(np.random.randint(0, 256, 10240), 4)

    def test_qpsk_3(self) :
        np.random.seed(3)
        self.runChainExperiment(np.random.randint(0, 256, 512), 4)

    def test_qpsk_4(self) :
        np.random.seed(4)
        self.runChainExperiment(np.random.randint(0, 256, 5120), 4)

    def test_qpsk_5(self) :
        np.random.seed(5)
        self.runChainExperiment(np.random.randint(0, 256, 1024), 4)


    def test_8psk_0(self) :
        self.runChainExperiment([0x00, 0x00, 0xFF, 0xFF, 0xAA, 0xAA, 0x55, 0x55, 0x12], 8)

    def test_8psk_1(self) :
        np.random.seed(1)
        self.runChainExperiment(np.random.randint(0, 256, 3*1024), 8)

    def test_8psk_2(self) :
        np.random.seed(2)
        self.runChainExperiment(np.random.randint(0, 256, 3*10240), 8)

    def test_8psk_3(self) :
        np.random.seed(3)
        self.runChainExperiment(np.random.randint(0, 256, 3*512), 8)

    def test_8psk_4(self) :
        np.random.seed(4)
        self.runChainExperiment(np.random.randint(0, 256, 3*5120), 8)

    def test_8psk_5(self) :
        np.random.seed(5)
        self.runChainExperiment(np.random.randint(0, 256, 3*1024), 8)

if __name__ == '__main__':
        gr_unittest.run(qa_mpsk_mod_demod_chain)
