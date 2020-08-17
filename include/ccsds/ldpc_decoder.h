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


#ifndef INCLUDED_CCSDS_LDPC_DECODER_H
#define INCLUDED_CCSDS_LDPC_DECODER_H

#include <ccsds/api.h>
#include <ccsds/decoder_verbosity.h>
#include <gnuradio/sync_block.h>
#include <ldpc/ldpc.h>
#include <ldpc/decoder.h>
#include <ccsds/ldpc.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief <+description of block+>
     * \ingroup ccsds
     *
     */
    class CCSDS_API ldpc_decoder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<ldpc_decoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::ldpc_decoder.
       *
       * To avoid accidental use of raw pointers, ccsds::ldpc_decoder's
       * constructor is in a private implementation
       * class. ccsds::ldpc_decoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char *parity_file, sys_t systype, punct_t puncttype, uint64_t num_punct, std::vector<size_t> punct_pos, bool drop_invalid_frames, decoder_verbosity_t verbosity);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_LDPC_DECODER_H */

