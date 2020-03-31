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
#include <ccsds/snr_est.h>
#include "ccsds_utils.h"

namespace gr {
  namespace ccsds {

    snr_est::snr_est()
    : d_alpha(0.001),
      d_beta(1.0-d_alpha),
      d_y1(0.0),
      d_y2(0.0)
    {
      // Nothing to do
    }

    void snr_est::update(size_t num_input, const gr_complex *input) {
      for (size_t i=0; i < num_input; i++) {
        const double abs_input = std::abs(input[i]);
        assert(!std::isnan(abs_input));
        assert(abs_input >= 0.0);
        const double y1 = abs_input*abs_input;
        assert(!std::isnan(y1));
        assert(y1 >= 0.0);
        d_y1 = d_alpha * y1 + d_beta * d_y1;
        assert(!std::isnan(d_y1));

        const double y2 = y1*y1;
        assert(!std::isnan(y2));
        assert(y2 >= 0.0);
        d_y2 = d_alpha * y2 + d_beta * d_y2;
        assert(!std::isnan(d_y2));
      }
    }

    ccsds::snr snr_est::get_snr() const {
      return {signal_lin(), noise_lin()};
    }

    float snr_est::signal_lin() const {
      return std::sqrt(static_cast<float>(utils::minus_cap_zero(2.0*d_y1*d_y1, d_y2)));
    }

    float snr_est::noise_lin() const {
      return static_cast<float>(d_y1) - signal_lin();
    }

  } /* namespace ccsds */
} /* namespace gr */

