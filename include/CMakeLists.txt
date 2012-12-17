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
    ccsds_api.h
    ccsds_randomiser_bb.h
    ccsds_viterbi27_bb.h
    ccsds_framer_sink_dual.h
    ccsds_msg_source.h
    ccsds_rs_decode_bb.h
    ccsds_local_oscillator_cc.h
    ccsds_fll_cc.h
    ccsds_pll_cc.h
    ccsds_dll_cc.h
    ccsds_mpsk_mod_bc.h
    ccsds_mpsk_demod_cb.h
    ccsds_mpsk_demod2_cb.h
    ccsds_moving_average.h
    ccsds_lpf.h
    ccsds_lpf2.h
    ccsds_crc16.h DESTINATION include/ccsds
)
