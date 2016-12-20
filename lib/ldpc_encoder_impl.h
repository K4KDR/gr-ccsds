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

#ifndef INCLUDED_CCSDS_LDPC_ENCODER_IMPL_H
#define INCLUDED_CCSDS_LDPC_ENCODER_IMPL_H

#include <ccsds/ldpc_encoder.h>

namespace gr {
  namespace ccsds {

    class ldpc_encoder_impl : public ldpc_encoder
    {
     private:
      /** Pointer to LDPC puncturing configuration */
      ldpc::puncturing::conf_t *d_punctconf;
      
      /** Pointer to LDPC encoder */
      ldpc::encoder *d_encoder;

     public:
      ldpc_encoder_impl(const char *generator_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos);
      ~ldpc_encoder_impl();

      
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
      
      /*! \brief Asynchronous work function which is processing the queues. */
      void process_frame(pmt::pmt_t msg);

    };
    
  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_LDPC_ENCODER_IMPL_H */

