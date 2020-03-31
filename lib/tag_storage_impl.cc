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
#include "tag_storage_impl.h"

#include <algorithm>

namespace gr {
  namespace ccsds {

    tag_storage::sptr
    tag_storage::make()
    {
      return gnuradio::get_initial_sptr
        (new tag_storage_impl());
    }

    /*
     * The private constructor
     */
    tag_storage_impl::tag_storage_impl()
      : gr::block("tag_storage",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
      d_KEY_EOF(pmt::intern("EOF"))
    {
      d_eof_signalled = false;
    }

    /*
     * Our virtual destructor.
     */
    tag_storage_impl::~tag_storage_impl()
    {
    }

    void tag_storage_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
      assert(ninput_items_required.size() == 1u);
      ninput_items_required[0] = (noutput_items <= 1) ? 0 : noutput_items;
    }

    int tag_storage_impl::general_work(int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      assert(noutput_items == ninput_items[0]);
      processTags(noutput_items);

      const bool eof = d_eof_offset && nitems_read(0)+noutput_items >= d_eof_offset.value();
      
      consume_each(noutput_items);

      // Never set this back to false
      d_eof_signalled = d_eof_signalled || eof;
      
      // Tell runtime system how many output items we produced.
      return eof ? WORK_DONE : noutput_items;
    }

    void tag_storage_impl::processTags(size_t num_inputs) {
      std::vector<gr::tag_t> tags;

      const uint64_t start = nitems_read(0);
      
      // It is legal to look for tags in the future. To prevent some off by one weiredness, lets look at least 10 samps into the future
      const uint64_t end = start + num_inputs + 20;
      
      get_tags_in_range(tags, 0, start, end);
      
      for (size_t i=0u; i<tags.size(); i++) {
        auto it = std::find(d_tags.begin(), d_tags.end(), tags[i]);

        if (it == d_tags.end()) {
          // Only add tag if it is not in the list yet.
          d_tags.push_back(tags[i]);

          if (pmt::eqv(tags[i].key, d_KEY_EOF) && pmt::is_eof_object(tags[i].value)) {
            d_eof_offset = tags[i].offset;
          }
        }
      }
    }

    bool tag_storage_impl::stop() {
      // See if there are any tags left that we have not processed so far.
      processTags(0u);

      // mark block as done
      d_eof_signalled = true;
      
      return true;
    }

    size_t tag_storage_impl::num_tags() const {
      return d_tags.size();
    }

    gr::tag_t tag_storage_impl::get_tag(size_t num) const {
      assert(num < d_tags.size());
      
      auto it = d_tags.cbegin();
      for(size_t i=0u; i<num; i++) {
        it++;
      }

      return *it;
    }

    std::vector<gr::tag_t> tag_storage_impl::get_tags() const {
      std::vector<gr::tag_t> tags;

      for(auto tag : d_tags) {
        tags.push_back(tag);
      }

      return tags;
    }

    bool tag_storage_impl::finished() const {
      return d_eof_signalled;
    }

  } /* namespace ccsds */
} /* namespace gr */

