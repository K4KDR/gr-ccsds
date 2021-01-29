#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# 
# Copyright 2020 Martin Luelf.
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
from gnuradio import digital
from gnuradio import analog
import pmt
import math
import time
import cmath

import ccsds_swig as ccsds

class qa_frame_sync(gr_unittest.TestCase):
    
    ### BPSK
    
    def test_bpsk_001 (self):
        # Low noise
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=1e-4, interruption_len=0, amb_slip=0)
        
    def test_bpsk_002 (self):
        # No noise (infinite SNR)
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=0, amb_slip=0)
    
    
    def test_bpsk_003 (self):
        # Ambiguity
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=1e-4, interruption_len=0, amb_slip=0)
        
    def test_bpsk_004 (self):
        # Ambiguity no noise
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=0.0, interruption_len=0, amb_slip=0)
    
    def test_bpsk_005 (self):
        # Small interruption
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=1e-4, interruption_len=15, amb_slip=0)
        
    def test_bpsk_006 (self):
        # Large interruption
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=700, amb_slip=0)
    
    def test_bpsk_007 (self):
        # Ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=1e-4, interruption_len=0, amb_slip=1)
        
    def test_bpsk_008 (self):
        # Small interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=15, amb_slip=1)
    
    def test_bpsk_009 (self):
        # Small interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=0.0, interruption_len=17, amb_slip=1)
    
    def test_bpsk_010 (self):
        # Large interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=0.001, interruption_len=1712, amb_slip=1)
    
    def test_bpsk_011 (self):
        # Large interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_bpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=712, amb_slip=1)
    
    
    ### QPSK
    
    def test_qpsk_001 (self):
        # Low noise
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=1e-4, interruption_len=0, amb_slip=0)
        
    def test_qpsk_002 (self):
        # No noise (infinite SNR)
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=0, amb_slip=0)
    
    
    def test_qpsk_003 (self):
        # Ambiguity
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=1e-4, interruption_len=0, amb_slip=0)
        
    def test_qpsk_004 (self):
        # Ambiguity no noise
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=0.0, interruption_len=0, amb_slip=0)
    
    def test_qpsk_005 (self):
        # Small interruption
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=1e-4, interruption_len=15, amb_slip=0)
        
    def test_qpsk_006 (self):
        # Large interruption
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=700, amb_slip=0)
    
    def test_qpsk_007 (self):
        # Ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=1e-4, interruption_len=0, amb_slip=1)
        
    def test_qpsk_008 (self):
        # Small interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=15, amb_slip=1)
    
    def test_qpsk_009 (self):
        # Small interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=0.0, interruption_len=17, amb_slip=1)
    
    def test_qpsk_010 (self):
        # Large interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=1, noise_amp=0.001, interruption_len=1712, amb_slip=1)
    
    def test_qpsk_011 (self):
        # Large interruption and ambiguity slip
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*2*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(0, 0, 0.0)
        self.run_test(sync_conf, gen_conf, ambiguity=0, noise_amp=0.0, interruption_len=712, amb_slip=1)
    
    
    ### Test frame generation
    def test_qpsk_frame_gen_1 (self):
        sync_conf = ccsds.frame_sync_config(ccsds.constellation_ccsds_qpsk().base(), '1ACFFC1D', 32, 4*255*8, 0, 0)
        gen_conf = ccsds.frame_generation_config(2, 3, 0.0)
        
        block_len_bits = sync_conf.block_num_bits()
        
        payload_len_bytes = sync_conf.payload_num_bytes()
        
        # 500 symbols / ( 4 symbols per byte)
        self.generate_symbols([0,]*int(500/4), sync_conf.constellation(), ambiguity=0, noise_amp=1e-3)
        
        # First frame
        self.queue_frame(
            #[b%256 for b in range(payload_len_bytes)],
            [0xFF,]*payload_len_bytes,
            sync_conf, ambiguity=0, noise_amp=1e-3,
            exp_header={
                'asm_offset':      pmt.from_uint64(1000),
                'asm_number':      pmt.from_uint64(0),
                'asm_ambiguity':   pmt.from_uint64(0),
                'frame_complete':  pmt.PMT_T,
                'asm_primary':     pmt.PMT_T,
        })
        
        # Secondary frames afterwards
        self.queue_expected(
            [0x55,]*(100-4) + [0x70, 0x9A, 0xA9, 0x7B] + [0xAA,]*(payload_len_bytes-100),
            {
                'asm_offset':      pmt.from_uint64(9192),
                'asm_number':      pmt.from_uint64(1),
                'asm_ambiguity':   pmt.from_uint64(0),
                'frame_complete':  pmt.PMT_T,
                'asm_primary':     pmt.PMT_F,
        })
        
        self.queue_expected(
            [0xAA,]*(100-4) + [0x00,]*(payload_len_bytes-100+4),
            {
                'asm_offset':      pmt.from_uint64(17384),
                'asm_number':      pmt.from_uint64(2),
                'asm_ambiguity':   pmt.from_uint64(0),
                'frame_complete':  pmt.PMT_T,
                'asm_primary':     pmt.PMT_F,
        })
        
        self.queue_expected(
            [0x00,]*payload_len_bytes,
            {
                'asm_offset':      pmt.from_uint64(25576),
                'asm_number':      pmt.from_uint64(3),
                'asm_ambiguity':   pmt.from_uint64(0),
                'frame_complete':  pmt.PMT_T,
                'asm_primary':     pmt.PMT_F,
        })
        
        
        # 400 symbols / ( 4 symbols per byte)
        self.generate_symbols([0,]*int(400/4), sync_conf.constellation(), ambiguity=1, noise_amp=1e-3)
        
        ### Second frame
        self.queue_frame(
            #[(b+100)%256 for b in range(payload_len_bytes)],
            [0xFF,]*payload_len_bytes,
            sync_conf, ambiguity=1, noise_amp=1e-3,
            exp_header={
                'asm_offset':      pmt.from_uint64(9992),
                'asm_number':      pmt.from_uint64(1),
                'asm_ambiguity':   pmt.from_uint64(1),
                'frame_complete':  pmt.PMT_T,
                'asm_primary':     pmt.PMT_T,
        })
        
        # previous frame
        self.queue_expected(
            [0x55,]*920 + [0x00,]*100,
            {
            'asm_offset':      pmt.from_uint64(1800),
            'asm_number':      pmt.from_uint64(0),
            'asm_ambiguity':   pmt.from_uint64(1),
            'frame_complete':  pmt.PMT_T,
            'asm_primary':     pmt.PMT_F,
        })
    
        self.queue_expected(
            [0xAA,]*payload_len_bytes,
            {
            'asm_offset':      pmt.from_uint64(18184),
            'asm_number':      pmt.from_uint64(2),
            'asm_ambiguity':   pmt.from_uint64(1),
            'frame_complete':  pmt.PMT_T,
            'asm_primary':     pmt.PMT_F,
        })
    
        self.queue_expected(
            [0xAA,]*payload_len_bytes,
            {
            'asm_offset':      pmt.from_uint64(26376),
            'asm_number':      pmt.from_uint64(3),
            'asm_ambiguity':   pmt.from_uint64(1),
            'frame_complete':  pmt.PMT_T,
            'asm_primary':     pmt.PMT_F,
        })
    
        self.queue_expected(
            [0xAA,]*payload_len_bytes,
            {
            'asm_offset':      pmt.from_uint64(34568),
            'asm_number':      pmt.from_uint64(4),
            'asm_ambiguity':   pmt.from_uint64(1),
            'frame_complete':  pmt.PMT_T,
            'asm_primary':     pmt.PMT_F,
        })
        
        self.generate_symbols([0,]*int((4080+16)*3), sync_conf.constellation(), ambiguity=0, noise_amp=1e-3)
        
        #print('Generated {} samples'.format(len(self.symbols)))
        
        computed = self.compute_frame_sync_output(sync_conf, gen_conf)
        
        self.check_frames(computed)
    
    
    def setUp(self):
        self.symbols = []
        self.exp_headers = []
        self.frames = []
        
    def tearDown(self):
        self.symbols = []
        self.exp_headers = []
        self.frames = []
    
    def run_test(self, sync_conf, gen_conf, ambiguity, noise_amp, interruption_len=0, amb_slip=0):
        constellation = sync_conf.constellation()
        block_len_bits = sync_conf.block_num_bits()
        payload_len_bytes = sync_conf.payload_num_bytes()
        # First frame
        self.queue_frame(
            [b%256 for b in range(payload_len_bytes)],
            sync_conf, ambiguity=ambiguity, noise_amp=noise_amp,
            exp_header={
                'asm_offset':    pmt.from_uint64(0*block_len_bits),
                'asm_number':    pmt.from_uint64(0),
                'asm_ambiguity': pmt.from_uint64(ambiguity),
                'frame_complete':  pmt.PMT_T
        })
        
        # interruption_len "random" symbols
        # take the points from the constellation and repeat them as "random" interruption
        symbol_points = constellation.s_points()
        interruption_symbols = symbol_points*int(math.ceil(float(interruption_len)/len(symbol_points)))
        interruption_len_bits = interruption_len*constellation.bits_per_symbol()
        
        #print('queueing %d symbols' % len(interruption_symbols[:interruption_len]))
        self.queue_symbols(interruption_symbols[:interruption_len])
        fame_number_incr = int(math.floor(float(interruption_len_bits)/(block_len_bits)))
        
        # Next frame with increased offset
        self.queue_frame(
            [b%256 for b in range(payload_len_bytes)],
            #[0x00,]*payload_len_bytes,
            sync_conf, ambiguity=ambiguity, noise_amp=noise_amp,
            exp_header={
                'asm_offset':    pmt.from_uint64(1*block_len_bits+interruption_len_bits),
                'asm_number':    pmt.from_uint64(1+fame_number_incr),
                'asm_ambiguity': pmt.from_uint64(ambiguity),
                'frame_complete':  pmt.PMT_T
        })
            
        # Last frame with (possibly) different ambiguity
        new_ambiguity = (ambiguity+amb_slip)%constellation.arity()
        self.queue_frame(
            [0xAA,]*payload_len_bytes,
            sync_conf, ambiguity=new_ambiguity, noise_amp=noise_amp,
            exp_header={
                'asm_offset':    pmt.from_uint64(2*block_len_bits+interruption_len_bits),
                'asm_number':    pmt.from_uint64(2+fame_number_incr),
                'asm_ambiguity': pmt.from_uint64(new_ambiguity),
                'frame_complete':  pmt.PMT_T
        })

        #print('Generated {} samples'.format(len(self.symbols)))
        
        computed = self.compute_frame_sync_output(sync_conf, gen_conf)
        
        self.check_frames(computed)
    
    
    def to_hex_str(self, v):
        return '[{}]'.format(', '.join(['({:4d}): 0x{:02X}'.format(i, v[i]) for i in range(len(v))]))
    
    def dict_to_str(self, d):
            self.assertTrue(isinstance(d,dict), 'Header {} is not a dictionary'.format(d))
            elements = ['{}: {}'.format(k, v) for k, v in d.items()]
            return '{{ {} }}'.format(', '.join(elements))
    
    def check_frames(self, computed):
        #print('------------------------------\nExpected:')
        #for i in range(len(self.exp_headers)):
        ##for i in [0,]:
            #print('Frame {}'.format(i))
            #print(str(self.exp_headers[i]))
            #print(self.to_hex_str(self.frames[i]))
        #print('==============================\nComputed:')
        #for i in range(len(computed)):
            #print('  header: {}'.format(ascii(pmt.car(computed[i]))))
            #print('  body: {}'.format(self.to_hex_str(pmt.to_python(pmt.cdr(computed[i])))))
            #break
        #print('------------------------------')
        
        # expected frames and headers must have consistent length
        assert len(self.exp_headers) == len(self.frames)
        
        self.assertTrue(len(computed) >= len(self.frames)+1,
                        '{} messages computed, but at least {}+1 EOF expected.'.format(len(computed), len(self.frames))
        )
        
        last_element = computed[-1]
        self.assertTrue(pmt.is_eof_object(last_element), 'Last computed frame should be the EOF object, instead it was {}'.format(last_element))
        
        
        for i in range(len(self.exp_headers)):
            computed_indx = None
            for j in range(len(computed)-1):
                self.assertFalse(pmt.is_eof_object(computed[j]), 'Computed frame {} is the EOF object, but the EOF object is expected at {}'.format(j, len(self.frames)))
            
                self.assertTrue(pmt.is_pair(computed[j]), 'Computed frame {} is not a PDU (not a pair)'.format(j))
            
                header_comp = pmt.car(computed[j])
            
                self.assertTrue(pmt.is_dict(header_comp), 'Computed frame {} is not a PDU (first element not a dictionary)'.format(j))
                
                flag_matches = True
                for k, v in self.exp_headers[i].items():
                    key_pmt = pmt.intern(k)
                    
                    if not pmt.dict_has_key(header_comp, key_pmt):
                        flag_matches = False
                        break
                    
                    val_comp = pmt.dict_ref(header_comp, key_pmt, pmt.PMT_NIL)
                    
                    if not pmt.eqv(v, val_comp):
                        flag_matches = False
                        break
                
                if flag_matches:
                    computed_indx = j
                    break
            
            expected_header_str = self.dict_to_str(self.exp_headers[i])
            computed_headers = [pmt.to_python(pmt.car(computed[k])) for k in range(len(computed)-1)]
            computed_headers_str = '\n'.join([self.dict_to_str(d) for d in computed_headers if d is not None])
            self.assertTrue(computed_indx is not None, 'Cannot find expected headers for frame {} in computed headers.\nExpected:\n{}\n\nCandidates:\n{}'.format(i, expected_header_str, computed_headers_str))
            data_comp = pmt.cdr(computed[computed_indx])
            
            self.assertTrue(pmt.is_u8vector(data_comp), 'Computed frame {} is not a PDU (second element not a u8vector)'.format(computed_indx))
            frame_length_expected = len(self.frames[i])
            frame_length_computed = pmt.length(data_comp)
            self.assertTrue( frame_length_expected == frame_length_computed , 'Computed frame {} has incorrect length ({}). Expected: {}'.format(i, frame_length_computed, frame_length_expected))
            
            computed_bytes = pmt.u8vector_elements(data_comp)
            expected_bytes = tuple(self.frames[i])
            expected_hex = self.to_hex_str(self.frames[i])
            computed_hex = self.to_hex_str(computed_bytes)
            for j in range(frame_length_expected):
                if self.frames[i][j] != computed_bytes[j]:
                    self.assertTrue( False , 'Computed frame does not match expected frame {} at position {}.\nExpected:\n{}\n{}\nComputed:\n{}\n{}'.format(i, j, self.exp_headers[i], expected_hex, pmt.to_python(pmt.car(computed[computed_indx])), computed_hex))
            # Count manual comparison as sucessfull assertion
            self.assertTrue( True )
            
            #self.assertTupleEqual(computed_bytes, expected_bytes, 'Computed frames are not equal.\nExpected:\n{}\nComputed:\n{}'.format(expected_hex, computed_hex))
            #exp_dict = self.exp_headers[i]
            #for k, v in exp_dict.items():
                #key_pmt = pmt.intern(k)
                #self.assertTrue( pmt.dict_has_key(header_comp, key_pmt), 'Computed directory does not have the expected "{}" header entry. Computed entries are: {}'.format(k, pmt.dict_keys(header_comp)))
                
                #val_comp = pmt.dict_ref(header_comp, key_pmt, pmt.PMT_NIL)
                
                #self.assertTrue(pmt.eqv(v, val_comp), 'Header entry "{}" for frame {} differs. Expected {} ({}), computed {} ({})'.format(k, i, v, type(v), val_comp, type(val_comp)))
            
            #frame_num = pmt.dict_ref(header_comp, pmt.intern('frame_number'), pmt.PMT_NIL)
        return    
            
    def generate_symbols(self, data_bytes, constellation, ambiguity, noise_amp):
        tb = gr.top_block()
        
        ##################################################
        # Variables
        ##################################################

        amb_point = constellation.map_to_points_v(ambiguity)[0]
        amb_ref = constellation.map_to_points_v(0)[0]
        ambiguity_symbol = amb_point*amb_ref.conjugate()
        #print('Rotating samples by {} to obtain ambiguity {}. point[{}] = {}.'.format(ambiguity_symbol, ambiguity, ambiguity, amb_point))
        ##################################################
        # Blocks
        ##################################################
        unpack = blocks.packed_to_unpacked_bb(constellation.bits_per_symbol(), gr.GR_MSB_FIRST)
        source = blocks.vector_source_b(data_bytes, False, 1, [])
        sink = blocks.vector_sink_c(1, 1024)
        modulator = ccsds.mpsk_mod_bc(constellation.arity())
        insert_ambiguity = blocks.multiply_const_vcc((ambiguity_symbol, ))
        noise_source = analog.noise_source_c(analog.GR_GAUSSIAN, noise_amp, 0)
        blocks_add = blocks.add_vcc(1)
        
        ##################################################
        # Connections
        ##################################################
        tb.connect((blocks_add, 0), (insert_ambiguity, 0))
        tb.connect((insert_ambiguity, 0), (sink, 0))
        tb.connect((modulator, 0), (blocks_add, 0))
        tb.connect((source, 0), (unpack, 0))
        tb.connect((unpack, 0), (modulator, 0))
        tb.connect((noise_source, 0), (blocks_add, 1))
            
        tb.run()
        
        self.queue_symbols(sink.data())
        
    def queue_frame(self, frame, sync_conf, ambiguity, noise_amp, exp_header):
        self.generate_symbols(list(sync_conf.asm_bytes())+frame, sync_conf.constellation(), ambiguity, noise_amp)
        self.queue_expected(frame, exp_header)
    
    def queue_expected(self, frame, exp_header):
        self.frames.append(frame)
        self.exp_headers.append(exp_header)
        
    
    def queue_symbols(self, symbols):
        if len(symbols) == 0:
            return
        
        self.symbols += symbols

    def compute_frame_sync_output(self, sync_config, gen_config):
        tb = gr.top_block()
        
        block_len_symbos = sync_config.block_num_symbols()
        #print('Computing frame synchronization for {} bytes of data'.format(len(data_in)))
        
        ##################################################
        # Blocks
        ##################################################
        source = blocks.vector_source_c(self.symbols, False, 1, [])
        ccsds_frame_sync = ccsds.frame_sync(sync_config, gen_config)
        ccsds_soft_to_hard_message = ccsds.soft_to_hard_message()
        blocks_message_debug = blocks.message_debug()
        
        ##################################################
        # Connections
        ##################################################
        tb.connect((source, 0), (ccsds_frame_sync, 0))
        tb.msg_connect((ccsds_frame_sync, 'out'), (ccsds_soft_to_hard_message, 'in'))
        tb.msg_connect((ccsds_soft_to_hard_message, 'out'), (blocks_message_debug, 'store'))
        
        tb.start()
        
        timeout = 50
        while(timeout >= 0):
            time.sleep(0.1)
            timeout -= 1
            
            num_messages = blocks_message_debug.num_messages()
            if num_messages == 0:
                # not a single message yet, keep waiting
                continue
            
            last_message = blocks_message_debug.get_message(num_messages-1)
            if pmt.is_eof_object(last_message):
                # eof received, stop waiting
                #print('EOF received, aborting')
                break
            else:
                # no eof received yet, keep going
                continue
        if timeout < 0:
            print('Timeout')
            
        tb.stop()
        tb.wait()
        
        #print('first symbols from source: {}'.format(data_in[:5]))
        #print('first symbols into SNR est: {}'.format(sink_in.data()[:5]))
        #print('first symbols into frame sync: {}'.format(sink_out.data()[:5]))
        
        return [blocks_message_debug.get_message(i) for i in range(blocks_message_debug.num_messages())]
    
if __name__ == '__main__':
    gr_unittest.run(qa_frame_sync)
