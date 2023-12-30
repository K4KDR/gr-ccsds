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


#ifndef INCLUDED_CCSDS_SNR_EST_H
#define INCLUDED_CCSDS_SNR_EST_H
#include <gnuradio/gr_complex.h>
#include <gnuradio/ccsds/api.h>
#include <gnuradio/ccsds/snr.h>
namespace gr {
  namespace ccsds {

    /*!
     * \brief SNR estimation block based on the gr::digital::mpsk_snr_est_m2m4 block
     *
     */
    class CCSDS_API snr_est {
    public:
      snr_est();
      ~snr_est() = default;

      /**
       * \brief Feed input signal into the estimator
       * 
       * \param num_inputs Number of samples to process
       * \param input arra with at least num_inputs samples
      */
      void update(size_t num_inputs, const gr_complex *input);

      /** Return current (linear) SNR estimate */
      snr get_snr() const;

      /** Return current (linear) signal variance estimate */
      float signal_lin() const;

      /** Return current (linear) noise variance estimate */
      float noise_lin() const;


    protected:
      const double d_alpha;
      const double d_beta;
      double d_y1;
      double d_y2;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SNR_EST_H */

