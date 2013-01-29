# Copyright 2011 Free Software Foundation, Inc.
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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
add_library(gnuradio-ccsds SHARED ccsds_randomiser_bb.cc ccsds_viterbi27_bb.cc viterbi27_sse.c ccsds_framer_sink_dual.cc ccsds_rs_decode_bb.cc decode_rs_8.c decode_rs_ccsds.c ccsds_globals.c ccsds_crc16.cc ccsds_local_oscillator_cc.cc ccsds_fll_cc.cc ccsds_pll_cc.cc ccsds_dll_cc.cc ccsds_mpsk_ambiguity_resolver_bb.cc ccsds_mpsk_mod_bc.cc ccsds_mpsk_demod_cb.cc ccsds_mpsk_demod2_cb.cc ccsds_frame_sync_b.cc ccsds_moving_average.cc ccsds_lpf.cc ccsds_lpf2.cc ccsds_add_asm.cc ccsds_ticp_frame_source.cc ccsds_ticp_frame_source_b.cc ccsds_ticp_frame_sink_b.cc)
target_link_libraries(gnuradio-ccsds ${Boost_LIBRARIES} ${GRUEL_LIBRARIES} ${GNURADIO_CORE_LIBRARIES} fftw3 ticp)
set_target_properties(gnuradio-ccsds PROPERTIES DEFINE_SYMBOL "gnuradio_ccsds_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-ccsds
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
find_package(Boost COMPONENTS unit_test_framework)

include(GrTest)
set(GR_TEST_TARGET_DEPS gnuradio-ccsds)
#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

add_executable(qa_ccsds_randomiser_bb qa_ccsds_randomiser_bb.cc)
target_link_libraries(qa_ccsds_randomiser_bb gnuradio-ccsds ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_randomiser_bb qa_ccsds_randomiser_bb)

add_executable(qa_ccsds_viterbi27_bb qa_ccsds_viterbi27_bb.cc)
target_link_libraries(qa_ccsds_viterbi27_bb gnuradio-ccsds ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_viterbi27_bb qa_ccsds_viterbi27_bb)

add_executable(qa_ccsds_framer_sink_dual qa_ccsds_framer_sink_dual.cc)
target_link_libraries(qa_ccsds_framer_sink_dual gnuradio-ccsds ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_framer_sink_dual qa_ccsds_framer_sink_dual)

add_executable(qa_ccsds_rs_decode_bb qa_ccsds_rs_decode_bb.cc)
target_link_libraries(qa_ccsds_rs_decode_bb gnuradio-ccsds ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_rs_decode_bb qa_ccsds_rs_decode_bb)
