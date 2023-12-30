/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
    void bind_add_asm(py::module& m);
    void bind_asm_header(py::module& m);
    void bind_asm_position(py::module& m);
    void bind_blob_msg_sink_b(py::module& m);
    void bind_blob_msg_source_b(py::module& m);
    void bind_block_prefix_bb(py::module& m);
    void bind_byte_to_softbits_bf(py::module& m);
    void bind_constellation_ccsds_bpsk(py::module& m);
    void bind_constellation_ccsds_qpsk_axis(py::module& m);
    void bind_constellation_ccsds_qpsk(py::module& m);
    void bind_conv_decode27(py::module& m);
    void bind_conv_encode27_bb(py::module& m);
    //void bind_conv_puncturing27(py::module& m);
    void bind_decoder_verbosity(py::module& m);
    void bind_dll_cc(py::module& m);
    void bind_doppler_correction_cc(py::module& m);
    void bind_frame_generation_config(py::module& m);
    void bind_frame_sync_config(py::module& m);
    void bind_frame_sync(py::module& m);
    void bind_ldpc_decoder(py::module& m);
    void bind_ldpc_encoder(py::module& m);
    void bind_ldpc(py::module& m);
    void bind_message_info(py::module& m);
    void bind_mpsk_ambiguity_resolver_f(py::module& m);
    void bind_mpsk_detector_soft_cf(py::module& m);
    void bind_mpsk_mod_bc(py::module& m);
    void bind_mpsk_preamble_cc(py::module& m);
    void bind_msg_null_src(py::module& m);
    void bind_pack_bit_to_bytes(py::module& m);
    void bind_pll_cc(py::module& m);
    void bind_randomiser(py::module& m);
    void bind_randomiser_softbits(py::module& m);
    void bind_reversebits(py::module& m);
    void bind_rs_decode(py::module& m);
    void bind_rs_encode(py::module& m);
    void bind_snr_estimation_cf(py::module& m);
    void bind_snr_est(py::module& m);
    void bind_snr_interpolation(py::module& m);
    void bind_snr(py::module& m);
    void bind_softbit_msg_sink_f(py::module& m);
    void bind_softbit_msg_source_f(py::module& m);
    void bind_softbits_msg_to_bytes_b(py::module& m);
    //void bind_softbits(py::module& m);
    void bind_softbittobit(py::module& m);
    void bind_soft_to_hard_message(py::module& m);
    void bind_sort_frames(py::module& m);
    void bind_tag_storage(py::module& m);
    void bind_trunk_tail(py::module& m);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(ccsds_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");
    py::module::import("gnuradio.digital");
    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_add_asm(m);
    bind_asm_header(m);
    bind_asm_position(m);
    bind_blob_msg_sink_b(m);
    bind_blob_msg_source_b(m);
    bind_block_prefix_bb(m);
    bind_byte_to_softbits_bf(m);
    bind_constellation_ccsds_bpsk(m);
    bind_constellation_ccsds_qpsk_axis(m);
    bind_constellation_ccsds_qpsk(m);
    bind_conv_decode27(m);
    bind_conv_encode27_bb(m);
    //bind_conv_puncturing27(m);
    bind_decoder_verbosity(m);
    bind_dll_cc(m);
    bind_doppler_correction_cc(m);
    bind_frame_generation_config(m);
    bind_frame_sync_config(m);
    bind_frame_sync(m);
    bind_ldpc_decoder(m);
    bind_ldpc_encoder(m);
    bind_ldpc(m);
    bind_message_info(m);
    bind_mpsk_ambiguity_resolver_f(m);
    bind_mpsk_detector_soft_cf(m);
    bind_mpsk_mod_bc(m);
    bind_mpsk_preamble_cc(m);
    bind_msg_null_src(m);
    bind_pack_bit_to_bytes(m);
    bind_pll_cc(m);
    bind_randomiser(m);
    bind_randomiser_softbits(m);
    bind_reversebits(m);
    bind_rs_decode(m);
    bind_rs_encode(m);
    bind_snr_estimation_cf(m);
    bind_snr_est(m);
    bind_snr_interpolation(m);
    bind_snr(m);
    bind_softbit_msg_sink_f(m);
    bind_softbit_msg_source_f(m);
    bind_softbits_msg_to_bytes_b(m);
    //bind_softbits(m);
    bind_softbittobit(m);
    bind_soft_to_hard_message(m);
    bind_sort_frames(m);
    bind_tag_storage(m);
    bind_trunk_tail(m);
    // ) END BINDING_FUNCTION_CALLS
}
