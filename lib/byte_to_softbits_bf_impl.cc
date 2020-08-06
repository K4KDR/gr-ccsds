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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "byte_to_softbits_bf_impl.h"

#include "ccsds_utils.h"
#include <ccsds/softbits.h>

namespace gr {
  namespace ccsds {

    byte_to_softbits_bf::sptr
    byte_to_softbits_bf::make(float initial_n0)
    {
      return gnuradio::get_initial_sptr
        (new byte_to_softbits_bf_impl(initial_n0));
    }

    /*
     * The private constructor
     */
    byte_to_softbits_bf_impl::byte_to_softbits_bf_impl(float initial_n0)
      : gr::sync_interpolator("byte_to_softbits_bf",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(float)), 8)
    {
      d_n0 = initial_n0;
    }

    /*
     * Our virtual destructor.
     */
    byte_to_softbits_bf_impl::~byte_to_softbits_bf_impl()
    {
    }

    int
    byte_to_softbits_bf_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      float *out = (float *) output_items[0];

      assert(noutput_items >= 0);
      assert(noutput_items%8 == 0);
      const size_t num_in = static_cast<size_t>(noutput_items)/8u;

      for (size_t i=0u; i<num_in; i++) {
        const uint8_t byte = in[i];
        for (size_t j=0u; j<8u; j++) {
          out[i*8u+j] = softbits::create_from_bit(utils::extract_bit_left(byte, j), d_n0);
        }
      }

      // Tell runtime system how many output items we produced.
      return static_cast<int>(num_in*8u);
    }

  } /* namespace ccsds */
} /* namespace gr */

