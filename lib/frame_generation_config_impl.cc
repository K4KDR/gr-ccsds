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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "frame_generation_config_impl.h"

namespace gr {
  namespace ccsds {

    frame_generation_config_impl::frame_generation_config_impl(size_t num_frames_before, size_t num_frames_after, float min_correlation_pct_primary, float min_correlation_secondary)
      : d_num_frames_before(num_frames_before),
        d_num_frames_after(num_frames_after),
        d_min_correlation_primary_pct(min_correlation_pct_primary),
        d_min_correlation_secondary_pct(min_correlation_secondary)
      {
        // Nothing to do
      }

    size_t frame_generation_config_impl::frames_before() const {
      return d_num_frames_before;
    }
    
    size_t frame_generation_config_impl::frames_after() const {
      return d_num_frames_after;
    }

    float frame_generation_config_impl::min_correlation_primary_pct() const {
      return d_min_correlation_primary_pct;
    }

    float frame_generation_config_impl::min_correlation_secondary_pct() const {
      return d_min_correlation_secondary_pct;
    }

    frame_generation_config::sptr frame_generation_config::make(size_t num_frames_before, size_t num_frames_after, float min_correlation_pct_primary, float min_correlation_secondary) {
      return sptr(new frame_generation_config_impl(num_frames_before, num_frames_after, min_correlation_pct_primary, min_correlation_secondary));
    }


  } // namespace ccsds
} // namespace gr
