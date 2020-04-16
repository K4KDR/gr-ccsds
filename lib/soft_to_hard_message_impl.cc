/* -*- c++ -*- */
/* 
 * Copyright 2018 Martin Luelf.
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

#include <ccsds/softbits.h>

#include <gnuradio/io_signature.h>
#include "soft_to_hard_message_impl.h"

#include <cassert>

namespace gr {
  namespace ccsds {

    soft_to_hard_message::sptr
    soft_to_hard_message::make()
    {
      return gnuradio::get_initial_sptr
        (new soft_to_hard_message_impl());
    }

    /*
     * The private constructor
     */
    soft_to_hard_message_impl::soft_to_hard_message_impl()
      : gr::sync_block("soft_to_hard_message",
              gr::io_signature::make(0, 0, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, sizeof(uint8_t))),
        d_stopped(false)
    {
      // register input
      message_port_register_in(pmt::mp("in"));
    	set_msg_handler(
          pmt::mp("in"),
          boost::bind(
            &soft_to_hard_message_impl::process_message,
            this,
            _1)
      );
      
      // register outnput type
    	message_port_register_out(pmt::mp("out"));
    }

    /*
     * Our virtual destructor.
     */
    soft_to_hard_message_impl::~soft_to_hard_message_impl()
    {
    }

    bool soft_to_hard_message_impl::stop() {
      this->d_stopped = true;
      // Signal EOF
    	message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    	return true;
    }
    
    void soft_to_hard_message_impl::process_message(pmt::pmt_t msg_in) {
      if(d_stopped) {
        // Do not accept new messages after EOF
        fprintf(stderr,"WARNING SOFTBIT TO HARDBYTE MSG: block already stopped, ignoring new messages.\n");
        return;
      }
      
      // check for EOF
      if(pmt::is_eof_object(msg_in)) {
        this->stop();
        return;
      }
    
      // check that input is a pair value
      if(!pmt::is_pair(msg_in)) {
        fprintf(stderr,"WARNING SOFTBIT TO HARDBYTE MSG: expecting message of type pair, skipping.\n");
        return;
      }
    
      const pmt::pmt_t hdr = pmt::car(msg_in);
      const pmt::pmt_t msg = pmt::cdr(msg_in);
    
      // check that input header is a dictionary
      if(!pmt::is_dict(hdr)) {
        fprintf(stderr,"WARNING SOFTBIT TO HARDBYTE MSG: expecting message header of type dict, skipping.\n");
        return;
      }
    
      // check that input data is a float vector
      if(!pmt::is_f32vector(msg)) {
        fprintf(stderr,"WARNING SOFTBIT TO HARDBYTE MSG: expecting message data of type f32vector, skipping.\n");
        return;
      }
      
      const size_t num_bits_in = pmt::length(msg);
      
      // check that input size is a multiple of 8 (and thus fits in bytes)
      if (num_bits_in % 8u > 0u) {
        fprintf(stderr,"WARNING SOFTBIT TO HARDBYTE MSG: expecting a multiple of 8 number of input softbit, got %lu %% 8 = %lu, skipping.\n", num_bits_in, num_bits_in % 8u);
        return;
      }
      
      assert(num_bits_in % 8 == 0);
      const size_t num_bytes_out = num_bits_in/8u;
      pmt::pmt_t msg_bytes = pmt::make_u8vector(num_bytes_out, 0x00);
      
      size_t num_elements_softbits;
      const float *softbit_values = pmt::f32vector_elements(msg, num_elements_softbits);
      assert(num_elements_softbits == num_bits_in);

      size_t num_elements_bytes;
      uint8_t *byte_values = pmt::u8vector_writable_elements(msg_bytes, num_elements_bytes);
      assert(num_elements_bytes == num_bytes_out);

      for(size_t i=0; i<num_bytes_out; i++) {
        for(size_t j=0; j<8; j++) {
          byte_values[i] |= softbits::hard_decision(softbit_values[8*i+j]) << (7-j);
        }
      }
    
      // Construct the new message using the received header
      pmt::pmt_t msg_out = pmt::cons(hdr, msg_bytes);

      // send message out
      message_port_pub( pmt::mp("out"), msg_out );
    }
    
    int
    soft_to_hard_message_impl::work(int /*noutput_items*/,
        gr_vector_const_void_star &/*input_items*/,
        gr_vector_void_star &/*output_items*/)
    {
      return (d_stopped) ? -1 : 0;
    }

  } /* namespace ccsds */
} /* namespace gr */

