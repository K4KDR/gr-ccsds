/* -*- c++ -*- */

#define CCSDS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ccsds_swig_doc.i"

%{
#include "ccsds_randomiser_bb.h"
#include "ccsds_viterbi27_bb.h"
#include "ccsds_conv_encode.h"
#include "ccsds_framer_sink_dual.h"
#include "ccsds_rs_encode.h"
#include "ccsds_rs_decode_bb.h"
#include "ccsds_rs_decode.h"
#include "ccsds_crc16.h"
#include "ccsds_local_oscillator_cc.h"
#include "ccsds_fll_cc.h"
#include "ccsds_pll_cc.h"
#include "ccsds_dll_cc.h"
#include "ccsds_mpsk_ambiguity_resolver_bb.h"
#include "ccsds_mpsk_mod_bc.h"
#include "ccsds_mpsk_demod_cb.h"
#include "ccsds_mpsk_demod2_cb.h"
#include "ccsds_mpsk_preamble_cc.h"
#include "ccsds_frame_sync_b.h"
#include "ccsds_add_asm.h"
#include "ccsds_blob_msg_source_b.h"
#include "ccsds_blob_msg_sink_b.h"
#include "ccsds_ticp_frame_source.h"
#include "ccsds_ticp_frame_source_b.h"
#include "ccsds_ticp_frame_sink.h"
%}

%include "ccsds_crc16.i"


GR_SWIG_BLOCK_MAGIC(ccsds,local_oscillator_cc);
%include "ccsds_local_oscillator_cc.h"

GR_SWIG_BLOCK_MAGIC(ccsds,fll_cc);
%include "ccsds_fll_cc.h"

GR_SWIG_BLOCK_MAGIC(ccsds,pll_cc);
%include "ccsds_pll_cc.h"

GR_SWIG_BLOCK_MAGIC(ccsds,dll_cc);
%include "ccsds_dll_cc.h"

GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_ambiguity_resolver_bb);
%include "ccsds_mpsk_ambiguity_resolver_bb.h"

GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_mod_bc);
%include "ccsds_mpsk_mod_bc.h"

GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_demod_cb);
%include "ccsds_mpsk_demod_cb.h"

GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_demod2_cb);
%include "ccsds_mpsk_demod2_cb.h"

GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_preamble_cc);
%include "ccsds_mpsk_preamble_cc.h"

GR_SWIG_BLOCK_MAGIC(ccsds,frame_sync_b);
%include "ccsds_frame_sync_b.h"

GR_SWIG_BLOCK_MAGIC(ccsds,add_asm);
%include "ccsds_add_asm.h"

GR_SWIG_BLOCK_MAGIC(ccsds,blob_msg_source_b);
%include "ccsds_blob_msg_source_b.h"

GR_SWIG_BLOCK_MAGIC(ccsds,blob_msg_sink_b);
%include "ccsds_blob_msg_sink_b.h"

GR_SWIG_BLOCK_MAGIC(ccsds,ticp_frame_source);
%include "ccsds_ticp_frame_source.h"

GR_SWIG_BLOCK_MAGIC(ccsds,ticp_frame_source_b);
%include "ccsds_ticp_frame_source_b.h"

GR_SWIG_BLOCK_MAGIC(ccsds,ticp_frame_sink);
%include "ccsds_ticp_frame_sink.h"

GR_SWIG_BLOCK_MAGIC(ccsds,randomiser_bb);
%include "ccsds_randomiser_bb.h"

GR_SWIG_BLOCK_MAGIC(ccsds,viterbi27_bb);
%include "ccsds_viterbi27_bb.h"

//GR_SWIG_BLOCK_MAGIC(ccsds,conv_encode);
%include <numpy.i>
// %init %{
// import_array();
// %}
// %apply (int* IN_ARRAY1, int DIM1) {(int* polys, int rate)};
// %typemap(in) (const int rate, int *polys) {
  // int i;
  // if (!PyList_Check($input)) {
    // PyErr_SetString(PyExc_ValueError, "Expecting a list");
    // return NULL;
  // }
  // $1 = PyList_Size($input);
  // $2 = (int *) malloc(($1)*sizeof(int));
  // for (i = 0; i < $1; i++) {
    // PyObject *s = PyList_GetItem($input,i);
    // if (!PyInt_Check(s)) {
        // free($2);
        // PyErr_SetString(PyExc_ValueError, "List items must be integers");
        // return NULL;
    // }
    // $2[i] = PyInt_AsLong(s);
  // }
// }

// %typemap(freearg) (const int rate, int *polys) {
   // if ($2) free($2);
// }

%include "ccsds_conv_encode.h"

GR_SWIG_BLOCK_MAGIC(ccsds,framer_sink_dual);
%include "ccsds_framer_sink_dual.h"

GR_SWIG_BLOCK_MAGIC(ccsds,rs_encode);
%include "ccsds_rs_encode.h"

GR_SWIG_BLOCK_MAGIC(ccsds,rs_decode_bb);
%include "ccsds_rs_decode_bb.h"

GR_SWIG_BLOCK_MAGIC(ccsds,rs_decode);
%include "ccsds_rs_decode.h"
