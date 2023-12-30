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
/* BINDTOOL_HEADER_FILE(msg_null_src.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(9aa8b0df3edd9623de35c4729789cd94)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/ccsds/msg_null_src.h>
// pydoc.h is automatically generated in the build directory
#include <msg_null_src_pydoc.h>

void bind_msg_null_src(py::module& m)
{

    using msg_null_src = ::gr::ccsds::msg_null_src;


    py::class_<msg_null_src,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<msg_null_src>>(m, "msg_null_src", D(msg_null_src))

        .def(py::init(&msg_null_src::make), D(msg_null_src, make))


        ;
}
