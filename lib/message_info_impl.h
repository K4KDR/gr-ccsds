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

#ifndef INCLUDED_CCSDS_MESSAGE_INFO_IMPL_H
#define INCLUDED_CCSDS_MESSAGE_INFO_IMPL_H

#include <gnuradio/ccsds/message_info.h>

namespace gr {
  namespace ccsds {

    class message_info_impl : public message_info
    {
     private:
        const mode_t d_MODE;
        bool d_eof_recv;
        const std::string d_PREFIX;
        
        void process_message(pmt::pmt_t msg);

     public:
      message_info_impl(std::string prefix, mode_t mode);
      ~message_info_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MESSAGE_INFO_IMPL_H */

