# Copyright 2011 Free Software Foundation, Inc.
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
install(FILES
	ccsds_randomiser.xml
	ccsds_reversebits.xml
	ccsds_msg_null_src.xml
	ccsds_ticp_frame_source.xml
	ccsds_ticp_frame_sink.xml 
	ccsds_blob_msg_source_b.xml
	ccsds_blob_msg_sink_b.xml
	ccsds_conv_decode27.xml
	ccsds_mpsk_ambiguity_resolver_f.xml
	ccsds_rs_encode.xml
	ccsds_rs_decode.xml
	ccsds_pll_cc.xml
	ccsds_dll_cc.xml
	ccsds_add_asm.xml
	ccsds_trunk_tail.xml
	ccsds_softbits_msg_to_bytes_b.xml
	ccsds_mpsk_detector_soft_cf.xml
	ccsds_mpsk_mod_bc.xml
	ccsds_conv_encode27_bb.xml
	ccsds_mpsk_preamble_cc.xml
	ccsds_softbittobit.xml
	DESTINATION share/gnuradio/grc/blocks
)

