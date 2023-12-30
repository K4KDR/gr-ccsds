# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT ccsds_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT ccsds_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND ccsds_python_files
    add_asm_python.cc
    asm_header_python.cc
    asm_position_python.cc
    blob_msg_sink_b_python.cc
    blob_msg_source_b_python.cc
    block_prefix_bb_python.cc
    byte_to_softbits_bf_python.cc
    constellation_ccsds_bpsk_python.cc
    constellation_ccsds_qpsk_axis_python.cc
    constellation_ccsds_qpsk_python.cc
    conv_decode27_python.cc
    conv_encode27_bb_python.cc
    # conv_puncturing27_python.cc
    decoder_verbosity_python.cc
    dll_cc_python.cc
    doppler_correction_cc_python.cc
    frame_generation_config_python.cc
    frame_sync_config_python.cc
    frame_sync_python.cc
    ldpc_decoder_python.cc
    ldpc_encoder_python.cc
    ldpc_python.cc
    message_info_python.cc
    mpsk_ambiguity_resolver_f_python.cc
    mpsk_detector_soft_cf_python.cc
    mpsk_mod_bc_python.cc
    mpsk_preamble_cc_python.cc
    msg_null_src_python.cc
    pack_bit_to_bytes_python.cc
    pll_cc_python.cc
    randomiser_python.cc
    randomiser_softbits_python.cc
    reversebits_python.cc
    rs_decode_python.cc
    rs_encode_python.cc
    snr_estimation_cf_python.cc
    snr_est_python.cc
    snr_interpolation_python.cc
    snr_python.cc
    softbit_msg_sink_f_python.cc
    softbit_msg_source_f_python.cc
    softbits_msg_to_bytes_b_python.cc
    # softbits_python.cc
    softbittobit_python.cc
    soft_to_hard_message_python.cc
    sort_frames_python.cc
    tag_storage_python.cc
    trunk_tail_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(ccsds
   ../../..
   gr::ccsds
   "${ccsds_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET ccsds_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ccsds_python>
    ${PROJECT_BINARY_DIR}/test_modules/gnuradio/ccsds/
)

install(TARGETS ccsds_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/ccsds COMPONENT pythonapi)
