/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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
#include "simple_bpsk_SNR_qf_impl.h"
#include <volk/volk.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {

    simple_bpsk_SNR_qf::sptr
    simple_bpsk_SNR_qf::make(size_t window_size)
    {
      return gnuradio::get_initial_sptr
        (new simple_bpsk_SNR_qf_impl(window_size));
    }

    /*
     * The private constructor
     */
    simple_bpsk_SNR_qf_impl::simple_bpsk_SNR_qf_impl(size_t window_size)
      : gr::block("simple_bpsk_SNR_qf",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float))),
	d_WINDOW_SIZE(window_size)
    {
    	const int alignment_multiple = 
	  volk_get_alignment() / sizeof(float);
	set_alignment(std::max(1,alignment_multiple));
    }

    /*
     * Our virtual destructor.
     */
    simple_bpsk_SNR_qf_impl::~simple_bpsk_SNR_qf_impl()
    {
    }

    void
    simple_bpsk_SNR_qf_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = noutput_items * d_WINDOW_SIZE;
    }

    int
    simple_bpsk_SNR_qf_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        float *out = (float *) output_items[0];
	
	// number of input items processed
	size_t nii = ninput_items[0];
	if (nii > noutput_items * d_WINDOW_SIZE)
	{
	nii = noutput_items * d_WINDOW_SIZE;
	}

	size_t align = volk_get_alignment();
	float *real = (float *) volk_malloc(nii * sizeof(float), align);
	float *imag = (float *) volk_malloc(nii * sizeof(float), align);
	float *mean_real = (float *) volk_malloc(noutput_items * sizeof(float),align);
	float sum_real=0;

	// get real part of points
	volk_32fc_deinterleave_real_32f(real,in,nii);
	
	for (int i=0; i < noutput_items; i++)
	{
        volk_32f_accumulator_s32f(&sum_real,real,d_WINDOW_SIZE);
	//(mean_real + i) = sum_real / d_WINDOW_SIZE;
	*(out + i) = sum_real / d_WINDOW_SIZE;
	}

	// Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (nii);

	// free allocated memory
	volk_free(mean_real);
	volk_free(real);
	volk_free(imag);

	printf("window_size: %d\tnout: %d\tnin: %d\n", d_WINDOW_SIZE, noutput_items, nii);
        // Tell runtime system how many output items we produced.
        return (noutput_items);
    }

  } /* namespace ccsds */
} /* namespace gr */

