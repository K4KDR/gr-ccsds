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
	ccsds_randomiser.cc
	ccsds_conv_encode27_bb.cc
	ccsds_conv_encode27_aux.cc
	ccsds_conv_decode27.cc
	ccsds_rs_decode.cc
	ccsds_rs_encode.cc
	ccsds_pll_cc.cc
	ccsds_dll_cc.cc
	ccsds_mpsk_ambiguity_resolver_f.cc
	ccsds_mpsk_mod_bc.cc
	ccsds_mpsk_detector_soft_cf.cc
	ccsds_mpsk_preamble_cc.cc
	ccsds_lpf2.cc
	ccsds_add_asm.cc
	ccsds_msg_null_src.cc
	ccsds_softbits_msg_to_bytes_b.cc
	ccsds_blob_msg_source_b.cc
	ccsds_blob_msg_sink_b.cc
	ccsds_trunk_tail.cc
	ccsds_ticp_frame_source.cc
	ccsds_ticp_frame_sink.cc)
#target_link_libraries(gnuradio-ccsds ${Boost_LIBRARIES}  ${GNURADIO_RUNTIME_LIBRARIES} fftw3 ticp /usr/local/lib/libfec.a)
target_link_libraries(gnuradio-ccsds ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES} fftw3 ticp fec)
set_target_properties(gnuradio-ccsds PROPERTIES DEFINE_SYMBOL "gnuradio_ccsds_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-ccsds
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

add_executable (test_ccsds_conv_encode27_aux test_ccsds_conv_encode27_aux.cc ccsds_conv_encode27_aux.cc) 

########################################################################
# Build and register unit test
########################################################################
find_package(Boost COMPONENTS unit_test_framework system)

include(GrTest)
set(GR_TEST_TARGET_DEPS gnuradio-ccsds)
#turn each test cpp file into an executable with an int main() function
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN)

add_executable(qa_ccsds_conv_encode27_aux qa_ccsds_conv_encode27_aux.cc ccsds_conv_encode27_aux.cc)
target_link_libraries(qa_ccsds_conv_encode27_aux ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_conv_encode27_aux qa_ccsds_conv_encode27_aux)


add_executable(qa_ccsds_asm_operator qa_ccsds_asm_operator.cc)
target_link_libraries(qa_ccsds_asm_operator ${Boost_LIBRARIES})
GR_ADD_TEST(qa_ccsds_asm_operator qa_ccsds_asm_operator)
