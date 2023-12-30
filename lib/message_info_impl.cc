/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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
#include "message_info_impl.h"
#include <sstream>

namespace gr {
  namespace ccsds {

    message_info::sptr
    message_info::make(std::string prefix, mode_t mode)
    {
      return gnuradio::get_initial_sptr
        (new message_info_impl(prefix, mode));
    }

    /*
     * The private constructor
     */
    message_info_impl::message_info_impl(std::string prefix, mode_t mode)
      : gr::sync_block("message_info",
              gr::io_signature::make(0, 0, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, sizeof(uint8_t))),
        d_MODE(mode),
        d_eof_recv(false),
        d_PREFIX(prefix)
    {
        // register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->process_message(msg); });
    }

    /*
     * Our virtual destructor.
     */
    message_info_impl::~message_info_impl()
    {
    }

    void message_info_impl::process_message(pmt::pmt_t msg_in) {
        if(this->d_eof_recv) {
    		// Do not accept new messages after EOF
    		return;
    	}
    
        // Do not make any printouts in silent mode
        if(d_MODE == MESSAGE_INFO_SILENT) {
            return;
        }
        
        // We do make a printout, lets start with the current time
        std::ostringstream s;
        s << this->d_PREFIX;
        
        time_t current_time;
        struct tm * time_info;
        char timeString[7];  // space for "HH:MM:SS\0"

        time(&current_time);
        time_info = localtime(&current_time);

        strftime(timeString, sizeof(timeString), "%H:%M ", time_info);
        s << std::string(timeString);
        
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
            s << "EOF message";
            std::cout << s.str() << std::endl;
    		d_eof_recv = true;
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		s << pmt::write_string(msg_in);
            std::cout << s.str() << std::endl;
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
        
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		s << "Message pair, but first element is not a dict.";
            std::cout << s.str() << std::endl;
    		return;
    	}
    	
    	if(d_MODE == MESSAGE_INFO_FULL || d_MODE == MESSAGE_INFO_HEADER_ONLY) {
            s << "-- Header ---------------\n";
            s << pmt::write_string(hdr) << "\n";
            s << "-- Body -----------------\n";
        } else if(d_MODE == MESSAGE_INFO_COMPACT) {
            s << pmt::length(pmt::dict_keys(hdr)) << " Header entries, ";
        }
        
        size_t len;
        if(pmt::is_blob(msg)) {
            len = pmt::blob_length(msg);
    		if(d_MODE == MESSAGE_INFO_COMPACT || d_MODE == MESSAGE_INFO_HEADER_ONLY) {
                s << len << " Bytes payload.";
            } else if(d_MODE == MESSAGE_INFO_FULL) {
                s << "ASCII (" << len << " elements): \n";
                s << pmt::write_string(msg) << "\n";
                s << ".........................\n";
                const std::vector<uint8_t> bytes = pmt::u8vector_elements(msg);
                s << "HEX (" << bytes.size() << " bytes): \n";
                for(auto byte : bytes) {
                    const uint8_t byte_high = (byte & 0xF0) >> 4;
                    const uint8_t byte_low  = (byte & 0x0F) >> 0;
                    constexpr char hex_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
                    s << "0x" << hex_chars[byte_high] << hex_chars[byte_low] << " ";
                }
                s << "\n-------------------------\n";
            }
    	} else if(pmt::is_f32vector(msg)) {
            len = pmt::length(msg);
    		if(d_MODE == MESSAGE_INFO_COMPACT || d_MODE == MESSAGE_INFO_HEADER_ONLY) {
                s << len << " Softbits payload.";
            } else if(d_MODE == MESSAGE_INFO_FULL) {
                s << pmt::write_string(msg) << "\n";
                s << "-------------------------\n";
            }
        } else {
            len = pmt::length(msg);
    		if(d_MODE == MESSAGE_INFO_COMPACT || d_MODE == MESSAGE_INFO_HEADER_ONLY) {
                s << len << " elements payload.";
            } else if(d_MODE == MESSAGE_INFO_FULL) {
                s << pmt::write_string(msg) << "\n";
                s << "-------------------------\n";
            }
        }
        
        // print
    	std::cout << s.str() << std::endl;
        
    	return;
    }
    
    int
    message_info_impl::work(int /*noutput_items*/,
        gr_vector_const_void_star &/*input_items*/,
        gr_vector_void_star &/*output_items*/)
    {
      return (this->d_eof_recv ? -1 : 0);
    }

  } /* namespace ccsds */
} /* namespace gr */

