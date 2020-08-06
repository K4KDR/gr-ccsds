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
import ccsds

tb = gr.top_block()

data_in = [
    0x1A, 0xCF, 0xFC, 0x1D, 
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55,
    0x1A, 0xCF, 0xFC, 0x1D,
    0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xAA, 0x55, 0x55, 0x55,
    0x1A, 0xCF, 0xFC, 0x1D,
]
src = blocks.vector_source_b(data_in, False)
encoder = ccsds.conv_encode27_bb(0x79,0xDB,ccsds.NONE)
snk = blocks.vector_sink_b(1)

##################################################
# Connections
##################################################
tb.connect((src, 0), (encoder, 0))
tb.connect((encoder, 0), (snk, 0))

# start
tb.run()

print('{}\n{}'.format(str(['0x{:02X}        '.format(d) for d in data_in]), str(['0x{:02X}'.format(d) for d in snk.data()])))
