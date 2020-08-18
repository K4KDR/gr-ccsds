/* -*- c++ -*- */
/* 
 * Copyright 2019 Martin Luelf.
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

#ifndef INCLUDED_CCSDS_DOPPLER_CORRECTION_CC_IMPL_H
#define INCLUDED_CCSDS_DOPPLER_CORRECTION_CC_IMPL_H

#include <ccsds/doppler_correction_cc.h>

#include <vector>
#include <array>

#include "orbit_range_interp.h"

namespace gr {
  namespace ccsds {

    class doppler_correction_cc_impl : public doppler_correction_cc
    {
     private:
       const float d_scale;
       const unsigned int d_blocksize;
       orbits::RangeInterpolator d_range;
       float *d_tmp_f1;
       float *d_tmp_f2;
       gr_complex *d_tmp_c;
       double *d_tmp_d;
       
     public:
      doppler_correction_cc_impl(std::array<std::string,2> tles,
                                 double sampl_rate,
                                 double t_update,
                                 double frequency,
                                 std::array<double,3> lla,
                                 unsigned int block_length);
      ~doppler_correction_cc_impl();

      bool start() override;
      
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items) override;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_DOPPLER_CORRECTION_CC_IMPL_H */

