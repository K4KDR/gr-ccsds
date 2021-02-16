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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>

#include "snr_estimation_cf_impl.h"

namespace gr {
    namespace ccsds {

        snr_estimation_cf::sptr
        snr_estimation_cf::make(size_t decimation) {
            return gnuradio::get_initial_sptr
            (new snr_estimation_cf_impl(decimation));
        }


        /*
         * The private constructor
         */
        snr_estimation_cf_impl::snr_estimation_cf_impl(size_t decimation)
            : gr::sync_decimator("snr_estimation_cf",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)),
                    gr::io_signature::make(1, 1, sizeof(float)), decimation),
            d_DECIMATION(decimation),
            d_snr_estimator()
        {}

        /*
         * Our virtual destructor.
         */
        snr_estimation_cf_impl::~snr_estimation_cf_impl()
        {
        }

        int snr_estimation_cf_impl::work(int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items) {
            const gr_complex *in = (const gr_complex *) input_items[0];
            float *out = (float *) output_items[0];
            
            const size_t num_out = static_cast<size_t>(noutput_items);
            
            for (size_t i=0lu; i<num_out; i++) {
                d_snr_estimator.update(d_DECIMATION, &in[d_DECIMATION*i]);
                out[i] = d_snr_estimator.get_snr().SNR_dB();
            }

            // Tell runtime system how many output items we produced.
            return noutput_items;
        }

    } /* namespace ccsds */
} /* namespace gr */

