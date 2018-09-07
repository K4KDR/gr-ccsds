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

#ifndef INCLUDED_CCSDS_SOFT_TO_HARD_MESSAGE_IMPL_H
#define INCLUDED_CCSDS_SOFT_TO_HARD_MESSAGE_IMPL_H

#include <ccsds/soft_to_hard_message.h>

namespace gr {
  namespace ccsds {

    class soft_to_hard_message_impl : public soft_to_hard_message
    {
     private:
      bool d_stopped;

     public:
      soft_to_hard_message_impl();
      ~soft_to_hard_message_impl();
      
      bool stop(void);
      
      void process_message(pmt::pmt_t msg_in);
      
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SOFT_TO_HARD_MESSAGE_IMPL_H */

