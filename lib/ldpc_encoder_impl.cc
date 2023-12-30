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
#include "ldpc_encoder_impl.h"

#include <vector>

namespace gr {
  namespace ccsds {

    ldpc_encoder::sptr
    ldpc_encoder::make(const char *generator_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos)
    {
      return gnuradio::get_initial_sptr
        (new ldpc_encoder_impl(generator_file, systype, puncttype, num_punct, punct_pos));
    }

    /*
     * The private constructor
     */
    ldpc_encoder_impl::ldpc_encoder_impl(const char *generator_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos)
      : gr::sync_block("ldpc_encoder",
              gr::io_signature::make(0, 0, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, sizeof(uint8_t)))
    {
        uint64_t *punct_pos_in = NULL;
        bool punct_pos_in_allocated = false;
        if(puncttype==LDPC_PUNCT_CUSTOM) {
            if(num_punct != punct_pos.size()) {
                fprintf(stderr,"ERROR LDPC ENCODER: Number of punctures is set to %lu, but vector with puncturing positions has %lu elements.\n", num_punct, punct_pos.size());
                return;
            }
        
            punct_pos_in = new uint64_t[num_punct];
            punct_pos_in_allocated=true;
            for(size_t i=0; i<num_punct; i++) {
                punct_pos_in[i] = punct_pos[i];
            }
        }
        // Create LDPC encoder
        this->d_punctconf = new ldpc::puncturing::conf_t((ldpc::puncturing::puncturing_t)puncttype, num_punct, punct_pos_in);
        this->d_encoder = new ldpc::encoder(generator_file, (ldpc::systematic::systematic_t)systype, this->d_punctconf);
        
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
    ldpc_encoder_impl::~ldpc_encoder_impl()
    {
        delete this->d_encoder;
        delete this->d_punctconf;
    }

    void ldpc_encoder_impl::process_frame(pmt::pmt_t msg_in) {
        
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING LDPC ENCODER: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING LDPC ENCODER: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a blob
    	if(!pmt::is_blob(msg)) {
    		fprintf(stderr,"WARNING LDPC ENCODER: expecting message data of type blob, skipping.\n");
    		return;
    	}
    
    	const unsigned int blob_len = (unsigned int) pmt::blob_length(msg);
        const uint64_t K_bytes = this->d_encoder->get_num_input();
        const uint64_t M_punct_bytes = this->d_encoder->get_num_output();
        
    	if(blob_len < K_bytes) {
    		fprintf(stderr,"ERROR LDPC ENCODER: blob message length of %u bytes is smaller than the expected length of %lu bytes.\n", blob_len, K_bytes);
    		return;
    	} else if(blob_len != K_bytes) {
    		fprintf(stderr,"WARNING LDPC ENCODER: blob message length of %u bytes does not match the expected length of %lu bytes.\n", blob_len, K_bytes);
    	}
    
    	// Message is BLOB
    	const uint8_t *data_in = (const unsigned char *) pmt::blob_data(msg);
        std::vector<uint8_t> data_out(M_punct_bytes);
        
    	// encode
        this->d_encoder->encode(data_out.data(), data_in);
        
    	// create output message data
    	pmt::pmt_t msg_out_data = pmt::make_blob(data_out.data(), M_punct_bytes);
    
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	// send generated codeblock
    	message_port_pub( pmt::mp("out"), msg_out );
    }
    
    int ldpc_encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &/*input_items*/,
        gr_vector_void_star &/*output_items*/)
    {
      return (noutput_items >= 0) ? 0 : -1;
    }
  } /* namespace ccsds */
} /* namespace gr */

