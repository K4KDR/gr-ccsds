/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_CCSDS_LDPC_ENCODER_H
#define INCLUDED_CCSDS_LDPC_ENCODER_H

#include <ccsds/api.h>
#include <gnuradio/sync_block.h>
#include <ldpc/ldpc.h>
#include <ldpc/encoder.h>
#include <ccsds/ldpc.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief <+description of block+>
     * \ingroup ccsds
     *
     */
    class CCSDS_API ldpc_encoder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<ldpc_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::ldpc_encoder.
       *
       * To avoid accidental use of raw pointers, ccsds::ldpc_encoder's
       * constructor is in a private implementation
       * class. ccsds::ldpc_encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char *generator_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_LDPC_ENCODER_H */

