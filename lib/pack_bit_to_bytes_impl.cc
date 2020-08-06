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
#include "pack_bit_to_bytes_impl.h"

namespace {

constexpr uint8_t combine_bits(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, uint8_t bit4, uint8_t bit5, uint8_t bit6, uint8_t bit7) {
  return static_cast<uint8_t>(
      ((bit0 & 0x01) << 7u) | ((bit1 & 0x01) << 6u) | ((bit2 & 0x01) << 5u) | ((bit3 & 0x01) << 4u) |
      ((bit4 & 0x01) << 3u) | ((bit5 & 0x01) << 2u) | ((bit6 & 0x01) << 1u) | ((bit7 & 0x01) << 0u)
  );
}

}  // namespace

namespace gr {
  namespace ccsds {

    pack_bit_to_bytes::sptr
    pack_bit_to_bytes::make()
    {
      return gnuradio::get_initial_sptr
        (new pack_bit_to_bytes_impl());
    }

    /*
     * The private constructor
     */
    pack_bit_to_bytes_impl::pack_bit_to_bytes_impl()
      : gr::sync_decimator("pack_bit_to_bytes",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(uint8_t)), 8)
    {}

    /*
     * Our virtual destructor.
     */
    pack_bit_to_bytes_impl::~pack_bit_to_bytes_impl()
    {
    }

    int
    pack_bit_to_bytes_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      const size_t num_out = static_cast<size_t>(noutput_items);
      for (size_t i=0u; i<num_out; i++) {
        const size_t offset = i*8u;
        out[i] = combine_bits(in[offset+0u],in[offset+1u],in[offset+2u],in[offset+3u],in[offset+4u],in[offset+5u],in[offset+6u],in[offset+7u]);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace ccsds */
} /* namespace gr */

