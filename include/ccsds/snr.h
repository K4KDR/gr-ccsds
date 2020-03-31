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


#ifndef INCLUDED_CCSDS_SNR_H
#define INCLUDED_CCSDS_SNR_H

#include <ccsds/api.h>

#include <stdint.h>
#include <cstddef>

namespace gr {
  namespace ccsds {

    /*!
     * \brief Representation of a signal to noise ratio. Spcifically the energy (variance) per symbol to noise energy (variance) ratio.
     * 
     * sqrtEs refers to the square root of the signal energy (it's amplitude).
     * 
     * \ingroup ccsds
     *
     */
    class CCSDS_API snr
    {
    public:
      snr();
      snr(float Es, float N0);
      //snr(const snr &other) = default;
      
      ~snr() = default;

      bool valid() const;
      static const snr& pick_first_if_valid(const snr &first, const snr &second);
      
      float Es() const;
      float sqrtEs() const;
      float N0() const;
      float SNR_dB() const;

      static const snr INVALID;

    private:
      float d_sqrtEs;
      float d_N0;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SNR_H */

