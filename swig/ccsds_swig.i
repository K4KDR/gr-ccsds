/* -*- c++ -*- */
#define CCSDS_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "ccsds_swig_doc.i"

%{
#include "ccsds/randomiser.h"
#include "ccsds/msg_null_src.h"
#include "ccsds/add_asm.h"
#include "ccsds/ticp_frame_source.h"
#include "ccsds/ticp_frame_sink.h"
#include "ccsds/trunk_tail.h"
#include "ccsds/pll_cc.h"
#include "ccsds/dll_cc.h"
#include "ccsds/blob_msg_sink_b.h"
#include "ccsds/blob_msg_source_b.h"
#include "ccsds/softbits_msg_to_bytes_b.h"
#include "ccsds/rs_decode.h"
////#include "ccsds/conv_encode27_bb.h"
////#include "ccsds/conv_decode27.h"
////#include "ccsds/conv_encode27_punct.h"
////#include "ccsds/conv_encode.h"
////#include "ccsds/rs_encode.h"
////#include "ccsds/mpsk_ambiguity_resolver_f.h"
////#include "ccsds/mpsk_mod_bc.h"
////#include "ccsds/mpsk_detector_soft_cf.h"
////#include "ccsds/mpsk_preamble_cc.h"
%}

// Puncturing definitions
////%include "ccsds/conv_encode27_punct.h"


%include "ccsds/randomiser.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,randomiser);

%include "ccsds/msg_null_src.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,msg_null_src);

%include "ccsds/add_asm.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,add_asm);

%include "ccsds/ticp_frame_source.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,ticp_frame_source);

%include "ccsds/ticp_frame_sink.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,ticp_frame_sink);

%include "ccsds/trunk_tail.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,trunk_tail);

%include "ccsds/pll_cc.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,pll_cc);

%include "ccsds/dll_cc.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,dll_cc);

%include "ccsds/blob_msg_sink_b.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,blob_msg_sink_b);

%include "ccsds/blob_msg_source_b.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,blob_msg_source_b);

%include "ccsds/softbits_msg_to_bytes_b.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,softbits_msg_to_bytes_b);

%include "ccsds/rs_decode.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,rs_decode);

////GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_ambiguity_resolver_f);
////%include "ccsds/mpsk_ambiguity_resolver_f.h"
////
////GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_mod_bc);
////%include "ccsds/mpsk_mod_bc.h"
////
////GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_detector_soft_cf);
////%include "ccsds/mpsk_detector_soft_cf.h"
////
////GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_preamble_cc);
////%include "ccsds/mpsk_preamble_cc.h"
////
////GR_SWIG_BLOCK_MAGIC2(ccsds,conv_encode27_bb);
////%include "ccsds/conv_encode27_bb.h"
////
////GR_SWIG_BLOCK_MAGIC2(ccsds,conv_decode27);
////%include "ccsds/conv_decode27.h"
////
////GR_SWIG_BLOCK_MAGIC2(ccsds,rs_encode);
////%include "ccsds/rs_encode.h"
