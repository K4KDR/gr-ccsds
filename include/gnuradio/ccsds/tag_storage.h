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


#ifndef INCLUDED_CCSDS_TAG_STORAGE_H
#define INCLUDED_CCSDS_TAG_STORAGE_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/block.h>

#include <vector>

namespace gr {
  namespace ccsds {

    /*!
     * \brief Store all tags from the input stream, ignore inpt samples
     * \ingroup ccsds
     *
     */
    class CCSDS_API tag_storage : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<tag_storage> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::tag_storage.
       *
       * To avoid accidental use of raw pointers, ccsds::tag_storage's
       * constructor is in a private implementation
       * class. ccsds::tag_storage::make is the public interface for
       * creating new instances.
       */
      static sptr make();

      virtual size_t num_tags() const =0;

      virtual std::vector<gr::tag_t> get_tags() const =0;

      virtual gr::tag_t get_tag(size_t num) const =0;

      virtual bool finished() const =0;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_TAG_STORAGE_H */

