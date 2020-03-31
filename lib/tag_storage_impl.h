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

#ifndef INCLUDED_CCSDS_TAG_STORAGE_IMPL_H
#define INCLUDED_CCSDS_TAG_STORAGE_IMPL_H

#include <ccsds/tag_storage.h>

#include <list>

namespace gr {
  namespace ccsds {

    class tag_storage_impl : public tag_storage
    {
     private:
      const pmt::pmt_t d_KEY_EOF;
      boost::optional<uint64_t> d_eof_offset;
      std::list<gr::tag_t> d_tags;
      bool d_eof_signalled;

      void processTags(size_t num_inputs);


     public:
      tag_storage_impl();
      ~tag_storage_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required) override;

      // Where all the action really happens
      int general_work(int noutput_items,
         gr_vector_int &ninput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items
      ) override;
      
      size_t num_tags() const override;

      std::vector<gr::tag_t> get_tags() const override;

      gr::tag_t get_tag(size_t num) const override;

      bool finished() const override;

      bool stop() override;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_TAG_STORAGE_IMPL_H */

