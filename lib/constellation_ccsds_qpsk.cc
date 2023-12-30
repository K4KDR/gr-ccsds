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
#include <gnuradio/ccsds/constellation_ccsds_qpsk.h>

namespace gr {
  namespace ccsds {
    
    constellation_ccsds_qpsk::sptr constellation_ccsds_qpsk::make() {
      return constellation_ccsds_qpsk::sptr(new constellation_ccsds_qpsk());
    }

    constellation_ccsds_qpsk::constellation_ccsds_qpsk() {
      d_constellation.resize(4);
      // Gray-coded

      const float component = 1.0f/std::sqrt(2.0f);
      d_constellation[0] = gr_complex(+component, +component);
      d_constellation[1] = gr_complex(+component, -component);
      d_constellation[2] = gr_complex(-component, +component);
      d_constellation[3] = gr_complex(-component, -component);

      d_pre_diff_code.resize(0);
      d_apply_pre_diff_code = false;
      
      d_rotational_symmetry = 4;
      d_dimensionality = 1;
      calc_arity();
    }

    constellation_ccsds_qpsk::~constellation_ccsds_qpsk() {}

    std::vector<float> constellation_ccsds_qpsk::calc_soft_dec(gr_complex sample, float npwr) {
      const float component = 1.0f/std::sqrt(2.0f);
      return {
        -2.0f*component*std::real(sample)/npwr,
        -2.0f*component*std::imag(sample)/npwr
      };
    }

    unsigned int constellation_ccsds_qpsk::decision_maker(const gr_complex* sample)
    {
        // Real component determines MSB.
        // Imag component determines LSB.
        return 2 * static_cast<unsigned int>(std::real(*sample) < 0) + static_cast<unsigned int>(std::imag(*sample) < 0);
    }

  } /* namespace ccsds */
} /* namespace gr */
