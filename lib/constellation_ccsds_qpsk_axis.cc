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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/ccsds/constellation_ccsds_qpsk_axis.h>

namespace gr {
  namespace ccsds {
    
    constellation_ccsds_qpsk_axis::sptr constellation_ccsds_qpsk_axis::make() {
      return constellation_ccsds_qpsk_axis::sptr(new constellation_ccsds_qpsk_axis());
    }

    constellation_ccsds_qpsk_axis::constellation_ccsds_qpsk_axis() {
      d_constellation.resize(4);
      // Gray-coded

      d_constellation[0] = gr_complex(+1.0,  0.0);
      d_constellation[1] = gr_complex( 0.0, -1.0);
      d_constellation[2] = gr_complex( 0.0, +1.0);
      d_constellation[3] = gr_complex(-1.0,  0.0);

      d_pre_diff_code.resize(0);
      d_apply_pre_diff_code = false;
      
      d_rotational_symmetry = 4;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_ccsds_qpsk_axis::~constellation_ccsds_qpsk_axis() {}

    std::vector<float> constellation_ccsds_qpsk_axis::calc_soft_dec(gr_complex sample, float npwr) {
      const static float component = 1.0f/std::sqrt(2.0f);
      const static gr_complex rotation(component, component);

      const gr_complex rotated_sample = sample*rotation;
      return {
        -2.0f*component*std::real(rotated_sample)/npwr,
        -2.0f*component*std::imag(rotated_sample)/npwr
      };
    }

    unsigned int constellation_ccsds_qpsk_axis::decision_maker(const gr_complex* sample)
    {
      const float re = std::real(*sample);
      const float im = std::imag(*sample);
        
        return 2 * static_cast<unsigned int>(im > re) + static_cast<unsigned int>(im <= -re);
    }
  } /* namespace ccsds */
} /* namespace gr */