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


#ifndef INCLUDED_CCSDS_BLOCK_PREFIX_BB_H
#define INCLUDED_CCSDS_BLOCK_PREFIX_BB_H

#include <ccsds/api.h>
#include <gnuradio/block.h>

#include <string>

namespace gr {
  namespace ccsds {

    /*!
     * \brief Add a periodic prefix before every block.
     * 
     * \ingroup ccsds
     *
     */
    class CCSDS_API block_prefix_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<block_prefix_bb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::block_prefix_bb.
       *
       * To avoid accidental use of raw pointers, ccsds::block_prefix_bb's
       * constructor is in a private implementation
       * class. ccsds::block_prefix_bb::make is the public interface for
       * creating new instances.
       * 
       * \param asm_hex Prefix as hexstring (must be an even number of characters)
       * \param block_len Number of bytes between each prefix
       */
      static sptr make(std::string asm_hex, size_t block_len);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_BLOCK_PREFIX_BB_H */

