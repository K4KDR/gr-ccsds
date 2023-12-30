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

#ifndef INCLUDED_CCSDS_FRAME_CONFIG_IMPL_H
#define INCLUDED_CCSDS_FRAME_CONFIG_IMPL_H

#include <gnuradio/ccsds/frame_generation_config.h>



namespace gr {
  namespace ccsds {

    class CCSDS_API frame_generation_config_impl : virtual public frame_generation_config {
        public:
          frame_generation_config_impl(size_t num_frames_before, size_t num_frames_after, float min_correlation_pct_primary=0.0f, float min_correlation_secondary=0.0f);

          size_t frames_before() const override;
          size_t frames_after() const override;
          float min_correlation_primary_pct() const override;
          float min_correlation_secondary_pct() const override;


        protected:
          size_t d_num_frames_before;
          size_t d_num_frames_after;
          float d_min_correlation_primary_pct;
          float d_min_correlation_secondary_pct;
      };

  }  // namespace ccsds
}  // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_CONFIG_IMPL_H */
