/* -*- c++ -*- */

#define CCSDS_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "ccsds_swig_doc.i"

%{
#include "ccsds/randomiser.h"
////#include "ccsds_conv_encode27_bb.h"
////#include "ccsds_conv_decode27.h"
////#include "ccsds_conv_encode27_punct.h"
////#include "ccsds_conv_encode.h"
////#include "ccsds_rs_encode.h"
////#include "ccsds_rs_decode.h"
////#include "ccsds_pll_cc.h"
////#include "ccsds_dll_cc.h"
////#include "ccsds_mpsk_ambiguity_resolver_f.h"
////#include "ccsds_mpsk_mod_bc.h"
////#include "ccsds_mpsk_detector_soft_cf.h"
////#include "ccsds_mpsk_preamble_cc.h"
////#include "ccsds_softbits_msg_to_bytes_b.h"
////#include "ccsds_add_asm.h"
////#include "ccsds_msg_null_src.h"
////#include "ccsds_blob_msg_source_b.h"
////#include "ccsds_blob_msg_sink_b.h"
////#include "ccsds_trunk_tail.h"
////#include "ccsds_ticp_frame_source.h"
////#include "ccsds_ticp_frame_sink.h"
%}

// Puncturing definitions
////%include "ccsds_conv_encode27_punct.h"


//FIXME : Swig magic does not work at the moment .... dont know why
////GR_SWIG_BLOCK_MAGIC(ccsds, randomiser);
%include "ccsds/randomiser.h"

////GR_SWIG_BLOCK_MAGIC(ccsds,pll_cc);
////%include "ccsds_pll_cc.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,dll_cc);
////%include "ccsds_dll_cc.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_ambiguity_resolver_f);
////%include "ccsds_mpsk_ambiguity_resolver_f.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_mod_bc);
////%include "ccsds_mpsk_mod_bc.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_detector_soft_cf);
////%include "ccsds_mpsk_detector_soft_cf.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,mpsk_preamble_cc);
////%include "ccsds_mpsk_preamble_cc.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,softbits_msg_to_bytes_b);
////%include "ccsds_softbits_msg_to_bytes_b.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,add_asm);
////%include "ccsds_add_asm.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,trunk_tail);
////%include "ccsds_trunk_tail.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,msg_null_src);
////%include "ccsds_msg_null_src.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,blob_msg_source_b);
////%include "ccsds_blob_msg_source_b.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,blob_msg_sink_b);
////%include "ccsds_blob_msg_sink_b.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,ticp_frame_source);
////%include "ccsds_ticp_frame_source.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,ticp_frame_sink);
////%include "ccsds_ticp_frame_sink.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,conv_encode27_bb);
////%include "ccsds_conv_encode27_bb.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,conv_decode27);
////%include "ccsds_conv_decode27.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,rs_encode);
////%include "ccsds_rs_encode.h"
////
////GR_SWIG_BLOCK_MAGIC(ccsds,rs_decode);
////%include "ccsds_rs_decode.h"
