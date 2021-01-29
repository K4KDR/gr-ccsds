#
# Copyright 2012
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gru
from ccsds import ccsds_swig
import struct

def gen_and_append_crc16(s):
    crc = ccsds_swig.crc16(s)
    return s + struct.pack(">H", gru.hexint(crc) & 0xFFFF)

def check_crc16(s):
        if len(s) < 2:
                return (False, '')
        msg = s[:-2]
        # print "msg = \n%s\n" % (msg,)
        actual = ccsds_swig.crc16(msg)
        (expected,) = struct.unpack("!H", s[-2:])
        # print "actual =", hex(actual), "expected =", hex(expected)
        return (actual == expected, msg)
