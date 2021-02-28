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

#include <ccsds/snr_interpolation.h>

#include "ccsds_utils.h"

#include <limits>
#include <cassert>
#include <cmath>

#include <cstdio>

namespace gr {
  namespace ccsds {

    bool snr_interpolation::valid() const {
      return (
           !std::isnan(d_sqrtEs_0)
        && !std::isnan(d_N0_0)
        && !std::isnan(d_sqrtEs_m)
        && !std::isnan(d_Es_m)
        && !std::isnan(d_N0_m)
      );
    }

    snr_interpolation::snr_interpolation()
    : d_OFFSET_START(0u),
      d_BLOCK_LEN_SCALE(0u),
      d_sqrtEs_0(std::numeric_limits<float>::quiet_NaN()),
      d_N0_0(std::numeric_limits<float>::quiet_NaN()),
      d_sqrtEs_m(std::numeric_limits<float>::quiet_NaN()),
      d_Es_m(std::numeric_limits<float>::quiet_NaN()),
      d_N0_m(std::numeric_limits<float>::quiet_NaN())
    {
      // Nothing to do
    }

    snr_interpolation::snr_interpolation(const snr &start, const snr &end, uint64_t offset_start, size_t block_length)
    : d_OFFSET_START(offset_start),
      d_BLOCK_LEN_SCALE(1.0f/static_cast<float>(utils::minus_cap_zero(block_length, 1lu))),
      d_sqrtEs_0(std::sqrt(snr::pick_first_if_valid(start, end).Es())),
      d_N0_0(snr::pick_first_if_valid(start, end).N0()),
      d_sqrtEs_m(std::sqrt(snr::pick_first_if_valid(end, start).Es())-std::sqrt(snr::pick_first_if_valid(start, end).Es())),
      d_Es_m(snr::pick_first_if_valid(end, start).Es()-snr::pick_first_if_valid(start, end).Es()),
      d_N0_m(snr::pick_first_if_valid(end, start).N0()-snr::pick_first_if_valid(start, end).N0())
    {
        assert( valid() );
    }

    snr_interpolation::snr_interpolation(const snr_interpolation &other, size_t shift)
    : d_OFFSET_START(other.offset()+shift),
      d_BLOCK_LEN_SCALE(other.block_length_scale()),
      d_sqrtEs_0(other.sqrtEs_window(shift)),
      d_N0_0(other.N0_window(shift)),
      d_sqrtEs_m(other.sqrtEs_slope()),
      d_Es_m(other.Es_slope()),
      d_N0_m(other.N0_slope())
    {
      assert(shift < block_length());
      assert( valid() );
    }

    float snr_interpolation::getFractionAbs(uint64_t pos) const {
      assert(pos >= d_OFFSET_START);
      const float frac = static_cast<float>(pos - d_OFFSET_START)*d_BLOCK_LEN_SCALE;
      assert(frac <= 1.0);
      return frac;
    }

    float snr_interpolation::getFractionWindow(size_t pos) const {
      return static_cast<float>(pos)*d_BLOCK_LEN_SCALE;
    }

    float snr_interpolation::sqrtEs(uint64_t pos) const {
      return d_sqrtEs_0 + d_sqrtEs_m*getFractionAbs(pos);
    }

    float snr_interpolation::Es(uint64_t pos) const {
      return d_sqrtEs_0*d_sqrtEs_0 + d_Es_m*getFractionAbs(pos);
    }

    float snr_interpolation::N0(uint64_t pos) const {
      return d_N0_0 + d_N0_m*getFractionAbs(pos);
    }

    float snr_interpolation::SNR_dB(uint64_t pos) const {
      return 10.0f*std::log10( Es(pos) / N0(pos) );
    }

    float snr_interpolation::sqrtEs_window(size_t pos) const {
      return d_sqrtEs_0 + d_sqrtEs_m*getFractionWindow(pos);
    }

    float snr_interpolation::Es_window(size_t pos) const {
      return d_sqrtEs_0*d_sqrtEs_0 + d_Es_m*getFractionWindow(pos);
    }

    float snr_interpolation::N0_window(size_t pos) const {
      return d_N0_0 + d_N0_m*getFractionWindow(pos);
    }

    float snr_interpolation::SNR_dB_window(size_t pos) const {
      return 10.0f*std::log10( Es_window(pos) / N0_window(pos) );
    }


    uint64_t snr_interpolation::offset() const {
      return d_OFFSET_START;
    }

    size_t snr_interpolation::block_length() const {
      return static_cast<size_t>(std::round(1.0/d_BLOCK_LEN_SCALE))+1lu;
    }

    float snr_interpolation::block_length_scale() const {
      return d_BLOCK_LEN_SCALE;
    }

    float snr_interpolation::Es_start() const {
      return d_sqrtEs_0*d_sqrtEs_0;
    }

    float snr_interpolation::Es_slope() const {
      return d_Es_m*d_BLOCK_LEN_SCALE;
    }

    float snr_interpolation::sqrtEs_start() const {
      return d_sqrtEs_0;
    }

    float snr_interpolation::sqrtEs_slope() const {
      return d_sqrtEs_m*d_BLOCK_LEN_SCALE;
    }

    float snr_interpolation::N0_start() const {
      return d_N0_0;
    }

    float snr_interpolation::N0_slope() const {
      return d_N0_m*d_BLOCK_LEN_SCALE;
    }

    float snr_interpolation::SNR_dB_start() const {
      return SNR_dB_window(0u);
    }

  } /* namespace ccsds */
} /* namespace gr */

