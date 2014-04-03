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
add_library(gnuradio-ccsds SHARED
	add_asm_impl.cc
	msg_null_src_impl.cc
	ticp_frame_source_impl.cc
	asynchronous_start.cc
	ticp_frame_sink_impl.cc
	trunk_tail_impl.cc
	lpf2.cc
	pll_cc_impl.cc
	dll_cc_impl.cc
	blob_msg_sink_b_impl.cc
	blob_msg_source_b_impl.cc
	softbits_msg_to_bytes_b_impl.cc
	rs_encode_impl.cc
	rs_decode_impl.cc
	mpsk_ambiguity_resolver_f_impl.cc
	conv_decode27_impl.cc
	reversebits_impl.cc
	mpsk_mod_bc_impl.cc
	mpsk_detector_soft_cf_impl.cc
	conv_encode27_bb_impl.cc
	conv_encode27_aux.cc
	mpsk_preamble_cc_impl.cc
	randomiser_impl.cc)
target_link_libraries(gnuradio-ccsds ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES} ticp fec)
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
find_package(Boost COMPONENTS unit_test_framework system)

include(GrTest)
set(GR_TEST_TARGET_DEPS gnuradio-ccsds)
#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

add_executable(qa_ccsds_conv_encode27_aux qa_ccsds_conv_encode27_aux.cc conv_encode27_aux.cc)
target_link_libraries(qa_ccsds_conv_encode27_aux ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_conv_encode27_aux qa_ccsds_conv_encode27_aux)


##add_executable(qa_ccsds_asm_operator qa_ccsds_asm_operator.cc)
##target_link_libraries(qa_ccsds_asm_operator ${Boost_LIBRARIES})
##GR_ADD_TEST(qa_ccsds_asm_operator qa_ccsds_asm_operator)
