/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf <mail@mluelf.de>.
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

#ifndef INCLUDED_CCSDS_BLOCK_PREFIX_BB_IMPL_H
#define INCLUDED_CCSDS_BLOCK_PREFIX_BB_IMPL_H

#include <ccsds/block_prefix_bb.h>

namespace gr {
  namespace ccsds {

    class block_prefix_bb_impl : public block_prefix_bb
    {
     private:
      const std::vector<uint8_t> d_PREFIX;
      const size_t d_PREFIX_LEN;
      const size_t d_BLOCK_LEN;
      const size_t d_OUTPUT_BLOCK_LEN;

      size_t d_block_counter;

     public:
      block_prefix_bb_impl(std::string asm_hex, size_t block_len);
      ~block_prefix_bb_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_BLOCK_PREFIX_BB_IMPL_H */

