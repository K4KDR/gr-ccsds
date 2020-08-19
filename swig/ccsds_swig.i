/* -*- c++ -*- */

#define DIGITAL_API
#define CCSDS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ccsds_swig_doc.i"

// Puncturing definitions for convolutional coding
%include "ccsds/conv_puncturing27.h"

// Puncturing/systematics definitions for LDPC
%include "ccsds/ldpc.h"

// Verbosity definitions
%include "ccsds/decoder_verbosity.h"

// Message info constants
%include "ccsds/message_info.h"

%{
#include "ccsds/randomiser.h"
#include "ccsds/randomiser_softbits.h"
#include "ccsds/reversebits.h"
#include "ccsds/msg_null_src.h"
#include "ccsds/add_asm.h"
#include "ccsds/trunk_tail.h"
#include "ccsds/pll_cc.h"
#include "ccsds/dll_cc.h"
#include "ccsds/blob_msg_sink_b.h"
#include "ccsds/blob_msg_source_b.h"
#include "ccsds/softbit_msg_sink_f.h"
#include "ccsds/softbit_msg_source_f.h"
#include "ccsds/softbits_msg_to_bytes_b.h"
#include "ccsds/rs_encode.h"
#include "ccsds/rs_decode.h"
#include "ccsds/mpsk_ambiguity_resolver_f.h"
#include "ccsds/conv_decode27.h"
#include "ccsds/mpsk_mod_bc.h"
#include "ccsds/mpsk_detector_soft_cf.h"
#include "ccsds/conv_encode27_bb.h"
#include "ccsds/mpsk_preamble_cc.h"
#include "ccsds/softbittobit.h"
#include "ccsds/ldpc_encoder.h"
#include "ccsds/ldpc_decoder.h"
#include "ccsds/message_info.h"
#include "ccsds/soft_to_hard_message.h"
#include "ccsds/doppler_correction_cc.h"
#include "ccsds/tag_storage.h"
#include "gnuradio/digital/constellation.h"
#include "ccsds/constellation_ccsds_bpsk.h"
#include "ccsds/constellation_ccsds_qpsk.h"
#include "ccsds/constellation_ccsds_qpsk_axis.h"
#include "ccsds/snr.h"
#include "ccsds/snr_interpolation.h"
#include "ccsds/frame_sync_config.h"
#include "ccsds/frame_generation_config.h"
#include "ccsds/asm_header.h"
#include "ccsds/asm_position.h"
#include "ccsds/snr_est.h"
#include "ccsds/frame_sync.h"
#include "ccsds/pack_bit_to_bytes.h"
#include "ccsds/block_prefix_bb.h"
#include "ccsds/byte_to_softbits_bf.h"
#include "ccsds/sort_frames.h"
%}



%include "ccsds/randomiser.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,randomiser);

%include "ccsds/randomiser_softbits.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,randomiser_softbits);

%include "ccsds/reversebits.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,reversebits);

%include "ccsds/msg_null_src.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,msg_null_src);

%include "ccsds/add_asm.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,add_asm);

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

%include "ccsds/softbit_msg_sink_f.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,softbit_msg_sink_f);

%include "ccsds/softbit_msg_source_f.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,softbit_msg_source_f);

%include "ccsds/softbits_msg_to_bytes_b.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,softbits_msg_to_bytes_b);

%include "ccsds/rs_encode.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,rs_encode);

%include "ccsds/rs_decode.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,rs_decode);

%include "ccsds/mpsk_ambiguity_resolver_f.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_ambiguity_resolver_f);

%include "ccsds/conv_decode27.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,conv_decode27);

%include "ccsds/mpsk_detector_soft_cf.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_detector_soft_cf);

%include "ccsds/mpsk_mod_bc.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_mod_bc);

%include "ccsds/conv_encode27_bb.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,conv_encode27_bb);

%include "ccsds/mpsk_preamble_cc.h"
GR_SWIG_BLOCK_MAGIC2(ccsds,mpsk_preamble_cc);

%include "ccsds/softbittobit.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, softbittobit);

%include "ccsds/ldpc_encoder.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, ldpc_encoder);
%include "ccsds/ldpc_decoder.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, ldpc_decoder);
%include "ccsds/message_info.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, message_info);
%include "ccsds/soft_to_hard_message.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, soft_to_hard_message);
%include "ccsds/doppler_correction_cc.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, doppler_correction_cc);
%include "ccsds/tag_storage.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, tag_storage);

%include "gnuradio/digital/constellation.h"
%include "ccsds/constellation_ccsds_bpsk.h"
%include "ccsds/constellation_ccsds_qpsk.h"
%include "ccsds/constellation_ccsds_qpsk_axis.h"

%include "ccsds_constellation.i"

%include "ccsds/snr.h"
%include "ccsds/snr_interpolation.h"

%include "ccsds/frame_sync_config.h"
%include "ccsds/frame_generation_config.h"
%include "ccsds_frame_config.i"

%include "ccsds/asm_header.h"
%include "ccsds/asm_position.h"
%template(asm_position_list_sptr) boost::shared_ptr<gr::ccsds::asm_position_list>;
%pythoncode %{
asm_position_list_sptr.__repr__ = lambda self: self.to_string()
asm_position_list = asm_position_list.make;
%}

%include "ccsds/snr_est.h"

%include "ccsds/frame_sync.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, frame_sync);
%include "ccsds/pack_bit_to_bytes.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, pack_bit_to_bytes);
%include "ccsds/block_prefix_bb.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, block_prefix_bb);
%include "ccsds/byte_to_softbits_bf.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, byte_to_softbits_bf);
%include "ccsds/sort_frames.h"
GR_SWIG_BLOCK_MAGIC2(ccsds, sort_frames);
