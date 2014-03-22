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

########################################################################
# Install public header files
########################################################################
install(FILES
	api.h
	randomiser.h
	reversebits.h
	msg_null_src.h
	add_asm.h
	ticp_frame_source.h
	ticp_frame_sink.h
	trunk_tail.h
	pll_cc.h
	dll_cc.h
	blob_msg_sink_b.h
	blob_msg_source_b.h
	softbits_msg_to_bytes_b.h
	rs_decode.h
	DESTINATION include/ccsds
)
#	conv_encode27_bb.h
#	conv_decode27.h
#	conv_encode27_punct.h
#	rs_encode.h
#	mpsk_ambiguity_resolver_f.h
#	mpsk_mod_bc.h
#	mpsk_detector_soft_cf.h
#	mpsk_preamble_cc.h
#)
