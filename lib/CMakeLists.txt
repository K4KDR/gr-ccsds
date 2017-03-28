# Copyright 2011,2012 Free Software Foundation, Inc.
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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND ccsds_sources
	asynchronous_start.cc
	add_asm_impl.cc
	msg_null_src_impl.cc
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
	randomiser_impl.cc
	randomiser_softbits_impl.cc
	softbittobit_impl.cc
   	simple_bpsk_SNR_qf_impl.cc
	ldpc_encoder_impl.cc
	ldpc_decoder_impl.cc
)

set(ccsds_sources "${ccsds_sources}" PARENT_SCOPE)
if(NOT ccsds_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT ccsds_sources)

add_library(gnuradio-ccsds SHARED ${ccsds_sources})
target_link_libraries(gnuradio-ccsds ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES} fec ldpc)
set_target_properties(gnuradio-ccsds PROPERTIES DEFINE_SYMBOL "gnuradio_ccsds_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-ccsds PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

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
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_ccsds_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ccsds.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_ccsds.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_conv_encode27_aux.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/conv_encode27_aux.cc
)

add_executable(test-ccsds ${test_ccsds_sources})

target_link_libraries(
  test-ccsds
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-ccsds
)

GR_ADD_TEST(test_ccsds test-ccsds)
