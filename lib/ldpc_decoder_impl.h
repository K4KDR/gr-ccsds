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

#ifndef INCLUDED_CCSDS_LDPC_DECODER_IMPL_H
#define INCLUDED_CCSDS_LDPC_DECODER_IMPL_H

#include <ccsds/ldpc_decoder.h>

namespace gr {
  namespace ccsds {

    class ldpc_decoder_impl : public ldpc_decoder
    {
     private:
      /** Pointer to LDPC puncturing configuration */
      ldpc::puncturing::conf_t *d_punctconf;
      
      /** Pointer to LDPC decoder */
      ldpc::decoder *d_decoder;

      /** Store flag on whether or not invalid frames should be returned */
      const bool d_drop_invalid_frames;

        /** Store blocks verbosity level */
        const decoder_verbosity_t d_verbosity;
        
        /** Output file for debug information (only used when verbosity is set to DEBUG) */
        const std::string d_debug_file;
     public:
      ldpc_decoder_impl(const char *parity_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos, bool drop_invalid_frames, decoder_verbosity_t verbosity);
      ~ldpc_decoder_impl();

      void process_frame(pmt::pmt_t msg_in);
      
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_LDPC_DECODER_IMPL_H */

