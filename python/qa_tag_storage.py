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
import pmt
import ccsds_swig as ccsds

def make_tag(offset, key, value, source=pmt.PMT_F):
    return gr.tag_utils.python_to_tag( (offset, pmt.intern(key), value, source) )

class qa_tag_storage (gr_unittest.TestCase):

    def setUp (self):
        pass

    def tearDown (self):
        pass

    def test_001_t (self):
        """ Basic test with most important pmt types """
        self.run_test(50, [
            make_tag(0,  'test_uint', pmt.from_uint64(123456789)),
            make_tag(10, 'test_float', pmt.from_float(-0.1)),
            make_tag(20, 'test_str', pmt.intern('42')),
            make_tag(21, 'test_eof', pmt.PMT_EOF),
            make_tag(40, 'test_true', pmt.PMT_T),
        ])
    
    
    def test_002_t (self):
        """ Test mutiple tags at the same offset """
        self.run_test(50, [
            make_tag(0, 'test_uint', pmt.from_uint64(123456789)),
            make_tag(0, 'test_float', pmt.from_float(-0.1)),
            make_tag(20, 'test_str', pmt.intern('42')),
            make_tag(20, 'test_eof', pmt.PMT_EOF),
            make_tag(40, 'test_true', pmt.PMT_T),
        ])
    
    
    def test_003_t (self):
        """ Test that tags behind the actual data stream are dropped """
        self.run_test(300, [
            make_tag(0, 'test_uint', pmt.from_uint64(123456789)),
            make_tag(0, 'test_float', pmt.from_float(-0.1)),
            make_tag(200, 'test_str', pmt.intern('42')),
            make_tag(300, 'test_eof', pmt.PMT_EOF),
            make_tag(400, 'test_true', pmt.PMT_T),
        ])
    
    def run_test(self, length, tags):
        tb = gr.top_block ()
        
        ##################################################
        # Blocks
        ##################################################
        vector_source = blocks.vector_source_c((0, )*length, False, 1, tags)
        tag_storage = ccsds.tag_storage()

        ##################################################
        # Connections
        ##################################################
        tb.connect((vector_source, 0), (tag_storage, 0))
        
        tb.run()
        
        expected_tags = [t for t in tags if t.offset < length]
        computed_tags = tag_storage.get_tags()
        
        num_expected = len(expected_tags)
        num_computed = tag_storage.num_tags()
        
        self.assertEqual(num_expected, num_computed,
                         msg='Number of tags does not agree. Expected #={}, computed #={}.'.format(num_expected, num_computed)
        )
        
        self.assertEqual(num_expected, len(computed_tags),
                         msg='Number of returned tags from get_tags() is {} but num_tags() returned {}'.format(len(computed_tags), num_computed)
        )
        
        for i in range(num_expected):
            self.compare_tag(expected_tags[i], computed_tags[i], 'get_tags()[{}]'.format(i))
            
            computed_single = tag_storage.get_tag(i)
            self.compare_tag(expected_tags[i], computed_single, 'get_tag({})'.format(i))
            
            
            
            
            
    def compare_tag(self, comp, expt, tag_descr):
        self.assertEqual(expt.offset,       comp.offset, msg='Offset for tag {} is different.'.format(tag_descr))
        self.assertTrue(pmt.eqv(expt.key,   comp.key),   msg='Keys for tag {} are different.'.format(tag_descr))
        self.assertTrue(pmt.eqv(expt.value, comp.value), msg='Value for tag {} are different.'.format(tag_descr))
        self.assertTrue(pmt.eqv(expt.srcid, comp.srcid), msg='Source for tag {} are different.'.format(tag_descr))
        

if __name__ == '__main__':
    gr_unittest.run(qa_tag_storage, "qa_tag_storage.xml")
