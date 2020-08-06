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

#include "ccsds_utils.h"
#include "hexstring_to_binary.h"

#include <gnuradio/io_signature.h>
#include "block_prefix_bb_impl.h"

namespace gr {
  namespace ccsds {

    block_prefix_bb::sptr
    block_prefix_bb::make(std::string asm_hex, size_t block_len)
    {
      return gnuradio::get_initial_sptr
        (new block_prefix_bb_impl(asm_hex, block_len));
    }

    /*
     * The private constructor
     */
    block_prefix_bb_impl::block_prefix_bb_impl(std::string asm_hex, size_t block_len)
      : gr::block("block_prefix_bb",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(uint8_t))),
        d_PREFIX( byteutils::decode_hex(asm_hex) ),
        d_PREFIX_LEN( d_PREFIX.size() ),
        d_BLOCK_LEN( block_len ),
        d_OUTPUT_BLOCK_LEN ( d_PREFIX_LEN + d_BLOCK_LEN )
    {
      d_block_counter = 0u;

      set_output_multiple(static_cast<int>(d_OUTPUT_BLOCK_LEN));
    }

    /*
     * Our virtual destructor.
     */
    block_prefix_bb_impl::~block_prefix_bb_impl()
    {
    }

    void
    block_prefix_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      assert(noutput_items > 0);
      size_t num_out_remaining = static_cast<size_t>(noutput_items);
      size_t num_inputs_planned = 0u;
      
      // first (potentially incomplete) block
      if (d_block_counter < d_PREFIX_LEN) {
        // a full block is missing
        
        // generate prefix
        num_out_remaining -= utils::pick_smaller(d_PREFIX_LEN-d_block_counter, num_out_remaining);
        
        // generate block
        const size_t to_copy = utils::pick_smaller(num_out_remaining, d_BLOCK_LEN);
        num_inputs_planned += to_copy;
        num_out_remaining -= to_copy;
      }
      
      // full blocks
      const size_t num_full_blocks = num_out_remaining / d_OUTPUT_BLOCK_LEN;
      num_out_remaining -= num_full_blocks*d_OUTPUT_BLOCK_LEN;
      num_inputs_planned += num_full_blocks*d_BLOCK_LEN;

      // last (incomplete) block
      if (num_out_remaining > 0u) {
        // Prefix does not require input
        num_out_remaining -= utils::pick_smaller(num_out_remaining, d_PREFIX_LEN);

        // Next frame
        const size_t to_copy = utils::pick_smaller(num_out_remaining, d_BLOCK_LEN);
        num_out_remaining -= to_copy;
        num_inputs_planned += to_copy;
      }

      ninput_items_required[0] = static_cast<int>(num_inputs_planned);
    }

    int
    block_prefix_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = static_cast<const uint8_t*>(input_items[0]);
      uint8_t *out = static_cast<uint8_t*>(output_items[0]);

      size_t num_out_total = 0u;
      size_t num_in_total = 0u;

      assert(noutput_items > 0);
      const size_t num_out_available_total = static_cast<size_t>(noutput_items);
      const size_t num_in_available_total = static_cast<size_t>(ninput_items[0]);

      while (num_out_total < num_out_available_total) {
        assert(num_in_total <= num_in_available_total);
        const size_t num_in_available = num_in_available_total - num_in_total;
        assert(num_out_total <= num_out_available_total);
        const size_t num_out_available = num_out_available_total - num_out_total;

        if (num_out_available == 0u) {
          break;
        }

        size_t num_produced = 0u;
        size_t num_consumed = 0u;

        if (d_block_counter < d_PREFIX_LEN) {
          const size_t to_copy = utils::pick_smaller(d_PREFIX_LEN - d_block_counter, num_out_available);
          std::memcpy(&out[num_out_total], &d_PREFIX[d_block_counter], to_copy);
          num_produced += to_copy;
        } else {
          assert(d_block_counter <= d_OUTPUT_BLOCK_LEN);
          const size_t num_out_possible = utils::pick_smaller(d_OUTPUT_BLOCK_LEN - d_block_counter, num_out_available);
          const size_t to_copy = utils::pick_smaller(num_out_possible, num_in_available);
          std::memcpy(&out[num_out_total], &in[num_in_total], to_copy);
          num_produced += to_copy;
          num_consumed += to_copy;
        }
        num_in_total += num_consumed;
        num_out_total += num_produced;

        d_block_counter += num_produced;

        assert(d_block_counter <= d_OUTPUT_BLOCK_LEN);
        d_block_counter = (d_block_counter == d_PREFIX_LEN+d_BLOCK_LEN) ? 0u : d_block_counter;

        assert(num_out_total > 0u);  // we should have made some progress here
        assert(num_out_total <= num_out_available_total);  // but don't overdo it

        if (num_produced == 0u) {
          break;
        }
      }
      
      consume_each (static_cast<int>(num_in_total));

      // Tell runtime system how many output items we produced.
      return static_cast<int>(num_out_total);
    }

  } /* namespace ccsds */
} /* namespace gr */

