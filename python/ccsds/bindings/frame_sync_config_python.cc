/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(frame_sync_config.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a866d8be7befbe2cf6c416416a9595d4)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/ccsds/frame_sync_config.h>
// pydoc.h is automatically generated in the build directory
#include <frame_sync_config_pydoc.h>

void bind_frame_sync_config(py::module& m)
{

    using frame_sync_config = ::gr::ccsds::frame_sync_config;


    py::class_<frame_sync_config, std::shared_ptr<frame_sync_config>>(
        m, "frame_sync_config", D(frame_sync_config))

        .def(py::init(&frame_sync_config::make),
             py::arg("constellation"),
             py::arg("asm_hex"),
             py::arg("asm_len_bits"),
             py::arg("payload_len_bits"),
             py::arg("head_len_bits"),
             py::arg("tail_len_bits"),
             D(frame_sync_config, make))


        .def("constellation",
             &frame_sync_config::constellation,
             D(frame_sync_config, constellation))


        .def("asm_num_bits",
             &frame_sync_config::asm_num_bits,
             D(frame_sync_config, asm_num_bits))


        .def("asm_num_symbols",
             &frame_sync_config::asm_num_symbols,
             D(frame_sync_config, asm_num_symbols))


        .def("asm_symbols",
             &frame_sync_config::asm_symbols,
             D(frame_sync_config, asm_symbols))


        .def("asm_symbols_conj",
             &frame_sync_config::asm_symbols_conj,
             D(frame_sync_config, asm_symbols_conj))


        .def("asm_bits", &frame_sync_config::asm_bits, D(frame_sync_config, asm_bits))


        .def("asm_bytes", &frame_sync_config::asm_bytes, D(frame_sync_config, asm_bytes))


        .def("block_num_bits",
             &frame_sync_config::block_num_bits,
             D(frame_sync_config, block_num_bits))


        .def("block_num_bytes",
             &frame_sync_config::block_num_bytes,
             D(frame_sync_config, block_num_bytes))


        .def("block_num_symbols",
             &frame_sync_config::block_num_symbols,
             D(frame_sync_config, block_num_symbols))


        .def("payload_num_bits",
             &frame_sync_config::payload_num_bits,
             D(frame_sync_config, payload_num_bits))


        .def("payload_num_bytes",
             &frame_sync_config::payload_num_bytes,
             D(frame_sync_config, payload_num_bytes))


        .def("payload_num_symbols",
             &frame_sync_config::payload_num_symbols,
             D(frame_sync_config, payload_num_symbols))


        .def("output_num_symbols",
             &frame_sync_config::output_num_symbols,
             D(frame_sync_config, output_num_symbols))


        .def("output_num_bits",
             &frame_sync_config::output_num_bits,
             D(frame_sync_config, output_num_bits))


        .def("head_num_bits",
             &frame_sync_config::head_num_bits,
             D(frame_sync_config, head_num_bits))


        .def("head_num_symbols",
             &frame_sync_config::head_num_symbols,
             D(frame_sync_config, head_num_symbols))


        .def("tail_num_bits",
             &frame_sync_config::tail_num_bits,
             D(frame_sync_config, tail_num_bits))


        .def("tail_num_symbols",
             &frame_sync_config::tail_num_symbols,
             D(frame_sync_config, tail_num_symbols))


        .def("history_for_n_frames",
             &frame_sync_config::history_for_n_frames,
             py::arg("n_frames"),
             D(frame_sync_config, history_for_n_frames))


        .def("modulate_symbol",
             &frame_sync_config::modulate_symbol,
             py::arg("symbol_bits"),
             D(frame_sync_config, modulate_symbol))


        .def("detect_ambiguity",
             &frame_sync_config::detect_ambiguity,
             py::arg("point"),
             D(frame_sync_config, detect_ambiguity))


        .def("get_ambiguity_symbol",
             &frame_sync_config::get_ambiguity_symbol,
             py::arg("ambiguity"),
             D(frame_sync_config, get_ambiguity_symbol))

        ;
}
