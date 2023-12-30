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


#ifndef INCLUDED_CCSDS_BYTE_TO_SOFTBITS_BF_H
#define INCLUDED_CCSDS_BYTE_TO_SOFTBITS_BF_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief Convert packed bytes to softbits
     * \ingroup ccsds
     *
     */
    class CCSDS_API byte_to_softbits_bf : virtual public gr::sync_interpolator
    {
     public:
      typedef std::shared_ptr<byte_to_softbits_bf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::byte_to_softbits_bf.
       *
       * To avoid accidental use of raw pointers, ccsds::byte_to_softbits_bf's
       * constructor is in a private implementation
       * class. ccsds::byte_to_softbits_bf::make is the public interface for
       * creating new instances.
       * 
       * \param initial_n0 Assume this noise variance to create the softbits.
       * 
       * TODO: Read SNR tags to update n0
       */
      static sptr make(float initial_n0);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_BYTE_TO_SOFTBITS_BF_H */

