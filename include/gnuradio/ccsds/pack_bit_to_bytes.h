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


#ifndef INCLUDED_CCSDS_PACK_BIT_TO_BYTES_H
#define INCLUDED_CCSDS_PACK_BIT_TO_BYTES_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief <+description of block+>
     * \ingroup ccsds
     *
     */
    class CCSDS_API pack_bit_to_bytes : virtual public gr::sync_decimator
    {
     public:
      typedef std::shared_ptr<pack_bit_to_bytes> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::pack_bit_to_bytes.
       *
       * To avoid accidental use of raw pointers, ccsds::pack_bit_to_bytes's
       * constructor is in a private implementation
       * class. ccsds::pack_bit_to_bytes::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_PACK_BIT_TO_BYTES_H */

