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


#ifndef INCLUDED_CCSDS_SNR_INTERPOLATION_H
#define INCLUDED_CCSDS_SNR_INTERPOLATION_H

#include <ccsds/api.h>
#include <ccsds/snr.h>

#include <stdint.h>
#include <cstddef>

namespace gr {
  namespace ccsds {

    /*!
     * \brief Linear SNR interpolation block
     * 
     * Allows the linear interpolation within one block length between a start and end SNR
     * 
     * The interpolation can be used with absolute offsets, or with local indices counting
     * from the first symbol in the block. The relative offset functions are marked by the
     * _window suffix.
     * 
     * \ingroup ccsds
     *
     */
    class CCSDS_API snr_interpolation {
    public:
      snr_interpolation();
      snr_interpolation(const snr &start, const snr &end, uint64_t offset_start, size_t block_length);
      snr_interpolation(const snr_interpolation &other) = default;
      snr_interpolation(const snr_interpolation &other, size_t shift);

      float getFractionAbs(uint64_t pos) const;
      float getFractionWindow(uint64_t pos) const;

      float Es(uint64_t pos) const;
      float sqrtEs(uint64_t pos) const;
      float N0(uint64_t pos) const;
      float SNR_dB(uint64_t pos) const;

      float Es_window(size_t pos) const;
      float sqrtEs_window(size_t pos) const;
      float N0_window(size_t pos) const;
      float SNR_dB_window(size_t pos) const;

      float Es_start() const;
      float Es_slope() const;
      float sqrtEs_start() const;
      float sqrtEs_slope() const;
      float N0_start() const;
      float N0_slope() const;
      float SNR_dB_start() const;

      bool valid() const;

      uint64_t offset() const;
      size_t block_length() const;
      float block_length_scale() const;


    protected:
      uint64_t d_OFFSET_START;
      float d_BLOCK_LEN_SCALE;
      float d_sqrtEs_0;
      float d_N0_0;
      float d_sqrtEs_m;
      float d_Es_m;
      float d_N0_m;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SNR_INTERPOLATION_H */

