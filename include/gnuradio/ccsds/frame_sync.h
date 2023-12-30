/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf.
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


#ifndef INCLUDED_CCSDS_FRAME_SYNC_H
#define INCLUDED_CCSDS_FRAME_SYNC_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/ccsds/frame_sync_config.h>
#include <gnuradio/ccsds/frame_generation_config.h>
#include <gnuradio/ccsds/asm_position.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/digital/constellation.h>


namespace gr {
  namespace ccsds {

    /*!
     * \brief Symbol based frame synchronization with ambiguity resolution
     * \ingroup ccsds
     *
     */
    class CCSDS_API frame_sync : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<frame_sync> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::frame_sync.
       *
       * To avoid accidental use of raw pointers, ccsds::frame_sync's
       * constructor is in a private implementation
       * class. ccsds::frame_sync::make is the public interface for
       * creating new instances.
       * 
       * \param sync_conf Frame configuration (ASM, block lenght)
       * \param gen_conf Configuration on which frames should be generated
       * \param additional_asm If the position and ambiguity of ASMs that are already known (e.g. from a previous iteration in post processing). Each position provided here will be copied to the output as secondary frame.
       */
      static sptr make(std::shared_ptr<frame_sync_config> sync_conf, // Note: use explicit std::shared_ptr instead of constellation_sptr typedef because the latter confuses SWIG
                       std::shared_ptr<frame_generation_config> gen_conf,
                       std::shared_ptr<asm_position_list> additional_asm = asm_position_list::make()
                      );
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_SYNC_H */

