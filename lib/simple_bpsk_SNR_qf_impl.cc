/* -*- c++ -*- */
/* 
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
#include <boost/math/special_functions/sign.hpp>

namespace gr {
  namespace ccsds {

    simple_bpsk_SNR_qf::sptr
    simple_bpsk_SNR_qf::make(unsigned int window_size)
    {
      return gnuradio::get_initial_sptr
        (new simple_bpsk_SNR_qf_impl(window_size));
    }

    /*
     * The private constructor
     */
    simple_bpsk_SNR_qf_impl::simple_bpsk_SNR_qf_impl(unsigned int window_size)
      : gr::block("simple_bpsk_SNR_qf",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(float))),
	d_window_size(window_size)
    {
    	const int alignment_multiple = 
	  volk_get_alignment() / sizeof(gr_complex);
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
	ninput_items_required[0] = noutput_items * d_window_size;
    }
    
    inline void
    simple_bpsk_SNR_qf_impl::variance(float *variance, float *inputBuffer, const unsigned int num_points)
    {
	// numerical critical implementation !! ( but used in volk_32f_stddev_and_mean_32f_x2_generic)
	// NB: since the points are mean free, I don't subtract the mean here in the calculation !!
	float returnValue = 0;
	if(num_points > 0){
		const float* aPtr = inputBuffer;
		unsigned int number = 0;

		for(number = 0; number < num_points; number++){
			returnValue += (*aPtr) * (*aPtr);
			*aPtr++;
		}
		returnValue /= num_points;
	}
	*variance = returnValue;
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
	size_t nii = noutput_items * d_window_size;

	const size_t ALIGN = volk_get_alignment();

	float *real_part = (float *) volk_malloc(nii * sizeof(float), ALIGN);
	float *real_part_squared = (float *) volk_malloc(d_window_size * sizeof(float), ALIGN);
	gr_complex *positive = (gr_complex*) volk_malloc(nii * sizeof(gr_complex), ALIGN);
	gr_complex *mean_free = (gr_complex*) volk_malloc(d_window_size * sizeof(gr_complex), ALIGN);
	int8_t *sgn_vector = (int8_t *) volk_malloc(nii * sizeof(int8_t), ALIGN);

	float *snr_real_vector = (float *) volk_malloc(d_window_size * sizeof(float), ALIGN);
	float *snr_imag_vector = (float *) volk_malloc(d_window_size * sizeof(float), ALIGN);
	float *snr_magn_vector = (float *) volk_malloc(d_window_size * sizeof(float), ALIGN);

	float var_real=0;
	float var_imag=0;
	float var_magn=0;

	float mean_real=0;
	float squared_mean=0;
	int8_t sgn=0;
	const gr_complex minus_one = -1;
	uint16_t  window_offset = 0;

	const float *complexVectorPtr = (float*) in;
	float *positiveVectorPtr = (float*) positive;
	
	// flip negative real part
	for(size_t i=0; i<nii; i++)
	{
		//sgn = boost::math::sign(*complexVectorPtr);
		*(sgn_vector + i) = boost::math::sign(*complexVectorPtr);	
		// get the real part of the inputs (see volk_32fc_deinterleave_real_32f_generic implementation)
		*positiveVectorPtr++ = *complexVectorPtr++ * *(sgn_vector + i);
		// and copy the imaginary part
		*positiveVectorPtr++ = *complexVectorPtr++;
	}
	
	// get real part of points
	volk_32fc_deinterleave_real_32f(real_part,positive,nii);
	
	for (int i=0; i < noutput_items; i++)
	{
		window_offset = i * d_window_size;

        	volk_32f_accumulator_s32f(&mean_real,(real_part + window_offset), d_window_size);
		mean_real =mean_real / d_window_size;

		// calculate the signal power
		//volk_32f_s32f_power_32f(real_part_squared + window_offset, real_part, 2, d_window_size); 
        	//volk_32f_accumulator_s32f(&squared_mean, real_part_squared, d_window_size);
		//squared_mean = squared_mean / d_window_size;
		squared_mean = mean_real * mean_real;

		for(int j=0; j< d_window_size;j++)
		{
			sgn = *(sgn_vector + window_offset + j);
			*(mean_free +j)  = *(in + window_offset + j) - (sgn * mean_real);
		}

		volk_32fc_deinterleave_real_32f(snr_real_vector, mean_free, d_window_size);
		volk_32fc_deinterleave_imag_32f(snr_imag_vector, mean_free, d_window_size);
		volk_32fc_magnitude_32f(snr_magn_vector, mean_free, d_window_size);

		variance(&var_real, snr_real_vector, d_window_size);
		variance(&var_imag, snr_imag_vector, d_window_size);
		variance(&var_magn, snr_magn_vector, d_window_size);
		
		d_SNR_real = 10 * log10(squared_mean / var_real);
		d_SNR_imag = 10 * log10(squared_mean / var_imag);
		d_SNR_magn = 10 * log10(squared_mean / var_magn);
		
		*(out + i) = d_SNR_magn;

		printf("varianve real: %10e\t imag: %10e\t complex: %10e\n", var_real, var_imag, var_magn);
	}
	
	
	//printf("Window size: %d\n", d_window_size);
	// Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (nii);

	// free allocated memory
	volk_free(positive);
	volk_free(mean_free);
	volk_free(real_part);
	volk_free(real_part_squared);
	volk_free(sgn_vector);
	volk_free(snr_real_vector);
	volk_free(snr_imag_vector);
	volk_free(snr_magn_vector);

	//printf("window_size: %lu\tnout: %d\tnin: %lu\tni_given: %d\n", d_window_size, noutput_items, nii,ninput_items[0]);
        // Tell runtime system how many output items we produced.
        return (noutput_items);
    }

    void
    simple_bpsk_SNR_qf_impl::setup_rpc()
    {
    #ifdef GR_CTRLPORT
    	add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<simple_bpsk_SNR_qf, float>(
        alias(), "SNR_real",
        &simple_bpsk_SNR_qf::get_SNR_real,
        pmt::mp(-100.0f), pmt::mp(100.0f), pmt::mp(10.0f),
        "dB", "SNR of the real part", RPC_PRIVLVL_MIN,
        DISPTIME)));

    	add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<simple_bpsk_SNR_qf, float>(
        alias(), "SNR_imag",
        &simple_bpsk_SNR_qf::get_SNR_imag,
        pmt::mp(-1000.0f), pmt::mp(1000.0f), pmt::mp(10.0f),
        "dB", "SNR of the imaginary part", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTLOG)));

    	add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<simple_bpsk_SNR_qf, float>(
        alias(), "SNR_magn",
        &simple_bpsk_SNR_qf::get_SNR_magn,
        pmt::mp(-1000.0f), pmt::mp(100.0f), pmt::mp(10.0f),
        "dB", "complex SNR", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTLOG)));

    	add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<simple_bpsk_SNR_qf, unsigned int>(
        alias(), "d_window_size",
        &simple_bpsk_SNR_qf::get_window_size,
        pmt::mp(0), pmt::mp(10000), pmt::mp(0),
        "samples", "Window Size", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTLOG)));

    	add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<simple_bpsk_SNR_qf, unsigned int>(
        alias(), "d_window_size",
        &simple_bpsk_SNR_qf::set_window_size,
        pmt::mp(0), pmt::mp(10000), pmt::mp(0),
        "samples", "set Window Size", RPC_PRIVLVL_MIN,
        DISPNULL)));

    #endif /* GR_CTRLPORT */
    }

  } /* namespace ccsds */
} /* namespace gr */

