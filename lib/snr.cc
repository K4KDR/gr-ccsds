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

#include <ccsds/snr.h>

#include <limits>
#include <cassert>
#include <cmath>

#include <cstdio>

namespace gr {
  namespace ccsds {

    snr::snr()
    : d_sqrtEs(std::numeric_limits<float>::quiet_NaN()),
      d_N0(std::numeric_limits<float>::quiet_NaN())
    {
      // Nothing to do
    }

    snr::snr(float Es, float N0)
    : d_sqrtEs(std::sqrt(Es)),
      d_N0(N0)
    {
      // Nothing to do
    }

    const snr snr::INVALID = snr(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());

    const snr& snr::pick_first_if_valid(const snr &first, const snr &second) {
        assert(first.valid() || second.valid());
        return first.valid() ? first : second;
    }

    bool snr::valid() const {
        return std::isnan(d_sqrtEs)==false && std::isnan(d_N0)==false;
    }

    float snr::Es() const {
      return d_sqrtEs*d_sqrtEs;
    }

    float snr::sqrtEs() const {
      return d_sqrtEs;
    }

    float snr::N0() const {
      return d_N0;
    }

    float snr::SNR_dB() const {
      return 10.0f*std::log10(Es()/N0());
    }

  } /* namespace ccsds */
} /* namespace gr */

