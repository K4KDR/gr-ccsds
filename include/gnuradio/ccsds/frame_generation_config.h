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


#ifndef INCLUDED_CCSDS_FRAME_GENERATION_CONFIG_H
#define INCLUDED_CCSDS_FRAME_GENERATION_CONFIG_H

#include <gnuradio/ccsds/api.h>

#include <boost/shared_ptr.hpp>

#include <cstdint>


namespace gr {
  namespace ccsds {

      /*!
      * \brief Options how frames should be generated when an ASM is found
      * 
      * \ingroup ccsds
      *
      */
      class CCSDS_API frame_generation_config {
        public:
          typedef std::shared_ptr<frame_generation_config> sptr;
          
          virtual ~frame_generation_config() {};

          virtual size_t frames_before() const =0;
          virtual size_t frames_after() const =0;
          virtual float min_correlation_primary_pct() const =0;
          virtual float min_correlation_secondary_pct() const =0;

          /**
           * Create a new generation config
           * 
           * This config determins what should happen when an attached sync marker (ASM) is found in the stream.
           * The frame associated with a found ASM is called a primary frame. Based on the found offset and
           * ambiguity it is also possible to predict the position of previous, or following frames, assuming that
           * they are spaced full block lengths away at the same ambiguity. These predicted frames are called secondary
           * frames.
           * 
           * \param num_frames_before When an ASM is found also create this many secondary frames at the predicted locations prior to the found ASM
           * \param num_frames_after When an ASM is found also create this many secondary frames at the predicted locations after the found ASM
           * \param min_correlation_pct_primary Discard any primary frames that have a relative correlation that is lower than this number (in percent)
           * \param min_correlation_pct_secondary Discard any secondary frames that have a relative correlation that is lower than this number (in percent)
           */
          static sptr make(size_t num_frames_before, size_t num_frames_after, float min_correlation_pct_primary=0.0f, float min_correlation_pct_secondary=0.0f);

        //protected:
        //  frame_generation_config() = default;
      };


  }  // namespace ccsds
}  // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_GENERATION_CONFIG_H */

