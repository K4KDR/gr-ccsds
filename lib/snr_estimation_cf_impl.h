/* -*- c++ -*- */
/*
 * Copyright 2021 Martin Luelf <mail@mluelf.de>.
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

#ifndef INCLUDED_CCSDS_SNR_ESTIMATION_CF_IMPL_H
#define INCLUDED_CCSDS_SNR_ESTIMATION_CF_IMPL_H

#include <ccsds/snr_estimation_cf.h>
#include <ccsds/snr_est.h>

namespace gr {
  namespace ccsds {

    class snr_estimation_cf_impl : public snr_estimation_cf
    {
     private:
        const size_t d_DECIMATION;
        snr_est d_snr_estimator;

     public:
      snr_estimation_cf_impl(size_t decimation);
      ~snr_estimation_cf_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SNR_ESTIMATION_CF_IMPL_H */

