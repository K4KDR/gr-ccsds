/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ldpc_decoder_impl.h"
#include "ldpc_decoder_common.h"

#include <vector>

namespace gr {
  namespace ccsds {

    ldpc_decoder::sptr
    ldpc_decoder::make(const char *parity_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos, bool drop_invalid_frames, decoder_verbosity_t verbosity)
    {
      return gnuradio::get_initial_sptr
        (new ldpc_decoder_impl(parity_file, systype, puncttype, num_punct, punct_pos, drop_invalid_frames, verbosity));
    }

    /*
     * The private constructor
     */
    ldpc_decoder_impl::ldpc_decoder_impl(const char *parity_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos, bool drop_invalid_frames, decoder_verbosity_t verbosity)
      : gr::sync_block("ldpc_decoder",
              gr::io_signature::make(0, 0, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, sizeof(uint8_t))),
        d_drop_invalid_frames(drop_invalid_frames),
        d_verbosity(verbosity),
        d_debug_file("/tmp/ldpc_dec_out.bin")
    {
        uint64_t *punct_pos_in = NULL;
        bool punct_pos_in_allocated = false;
        if(puncttype==LDPC_PUNCT_CUSTOM) {
            if(num_punct != punct_pos.size()) {
                fprintf(stderr,"ERROR LDPC DECODER: Number of punctures is set to %lu, but vector with puncturing positions has %lu elements.\n", num_punct, punct_pos.size());
                return;
            }
        
            punct_pos_in = new uint64_t[num_punct];
            punct_pos_in_allocated=true;
            for(size_t i=0; i<num_punct; i++) {
                punct_pos_in[i] = punct_pos[i];
            }
        }
        // Create LDPC decoder
        this->d_punctconf = new ldpc::puncturing::conf_t((ldpc::puncturing::puncturing_t)puncttype, num_punct, punct_pos_in);
        this->d_decoder = new ldpc::decoder(parity_file, (ldpc::systematic::systematic_t)systype, this->d_punctconf);
        
        if(punct_pos_in_allocated) {
            delete[] punct_pos_in;
        }
        
        // register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->process_frame(msg); });
        
    }

    /*
     * Our virtual destructor.
     */
    ldpc_decoder_impl::~ldpc_decoder_impl()
    {
        delete this->d_decoder;
        delete this->d_punctconf;
    }

    void ldpc_decoder_impl::process_frame(pmt::pmt_t msg_in) {
        
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING LDPC DECODER: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING LDPC DECODER: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a softbit (float) vector
    	if(!pmt::is_f32vector(msg)) {
    		fprintf(stderr,"WARNING LDPC DECODER: expecting message data of type f32_vector, skipping.\n");
    		return;
    	}
    
    	const unsigned int vec_len = (unsigned int) pmt::length(msg);
        const uint64_t K = this->d_decoder->get_num_output();
        const uint64_t M_punct = this->d_decoder->get_num_input();
        
    	if(vec_len < M_punct) {
    		fprintf(stderr,"ERROR LDPC DECODER: message length of %u softbits is smaller than the expected length of %lu softbits.\n", vec_len, M_punct);
    		return;
    	} else if(vec_len != M_punct) {
    		fprintf(stderr,"WARNING LDPC DECODER: message length of %u softbits does not match the expected length of %lu softbits.\n", vec_len, M_punct);
    	}
    
    	// Message is f32_vector
    	std::vector<float> data_in = pmt::f32vector_elements(msg);
        std::vector<float> data_out(K);
        
	// Convert gr-ccsds prop1 LLR to prob0 LLR from libLDPC
	for(size_t i=0; i<M_punct; i++) {
		data_in[i] = -data_in[i];
	}

    	// decode
        ldpc::decoder::metadata_t meta;
        
        if (d_verbosity >= DECODER_VERBOSITY_DEBUG) {
            this->d_decoder->decode(data_out.data(), data_in.data(), &meta, d_debug_file.c_str()); // Create debug output
        } else {
            this->d_decoder->decode(data_out.data(), data_in.data(), &meta);
        }
        
        if (d_verbosity >= DECODER_VERBOSITY_DEBUG) {
            printf("LDPC DECODER: Decoding %s after %lu iterations. %lu bits corrected.\n", meta.success ? "SUCCESSFULL" : "FAILED", meta.num_iterations, meta.num_corrected);
        }

        const std::string frame_number_str = pmt::write_string(pmt::dict_ref(hdr, pmt::intern("frame_number"), pmt::PMT_NIL));
        if (meta.success == false && d_drop_invalid_frames) {
            if (d_verbosity >= DECODER_VERBOSITY_DROPPED) {
                printf("LDPC decoder: DROPPING frame number %s because it is INVALID.\n", frame_number_str.c_str());
            }
            return;
        } else if(meta.success == false && d_verbosity >= DECODER_VERBOSITY_FAILED) {
            printf("LDPC decoder: Frame number %s is INVALID.\n", frame_number_str.c_str());
        } else if (d_verbosity >= DECODER_VERBOSITY_ALL) {
            printf("LDPC decoder: Frame number %s is %s.\n", frame_number_str.c_str(), (meta.success ? "VALID" : "INVALID"));
        }

        ldpc_common::attachMetadata(hdr, &meta);
        
    	// create output message data
    	pmt::pmt_t msg_out_data = pmt::make_f32vector(K, 0.0f);
        for(size_t i=0; i<K; i++) {
		// Invert prob0 LLR from libLDPS back to gr-ccsds prob1 LLR by inverting the sign
            pmt::f32vector_set(msg_out_data, i, -data_out[i]);
        }
        
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	// send generated codeblock
    	message_port_pub( pmt::mp("out"), msg_out );
    }
    
    
    int ldpc_decoder_impl::work(int noutput_items,
        gr_vector_const_void_star &/*input_items*/,
        gr_vector_void_star &/*output_items*/)
    {
      return (noutput_items >= 0) ? 0 : -1;
    }

  } /* namespace ccsds */
} /* namespace gr */

