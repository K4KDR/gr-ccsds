#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_fll_cc.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <gr_rotator.h>
#include <stdio.h>
#include <math.h>
#include <ccsds_moving_average.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>

ccsds_fll_cc_sptr
ccsds_make_fll_cc ()
{
    return ccsds_fll_cc_sptr (new ccsds_fll_cc ());
}

ccsds_fll_cc::ccsds_fll_cc ()
  : gr_block ("ccsds_fll_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	//gr_make_io_signature3 (1, 3, sizeof (gr_complex), sizeof (float), sizeof (float)))
	gr_make_io_signature (1, 1, sizeof (gr_complex)))
{
	// initialize on real axis
	d_last_sample = gr_complex(1.0,0.0);
	d_last_sample_valid = false;

	d_phase = 0.0f;
	
	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));
}

ccsds_fll_cc::~ccsds_fll_cc ()
{
    // nothing else required in this example
}

const double ccsds_fll_cc::D_TWOPI=2.0f*M_PI;

void ccsds_fll_cc::calc_diffs(gr_complex *tmp_c, const gr_complex *in, const unsigned int num) {
	//* use volk
	gr_complex *in_shift = (gr_complex*) fftw_malloc(num * sizeof(gr_complex));
	if(in_shift == 0) {
		fprintf(stderr,"ERROR: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return;
	}

	in_shift[0] = d_last_sample;
	memcpy(&in_shift[1], &in[0], (num-1)*sizeof(gr_complex) );

	if(is_unaligned()) {
		volk_32fc_x2_multiply_conjugate_32fc_u(tmp_c, in, in_shift, num);
	} else {
		volk_32fc_x2_multiply_conjugate_32fc_a(tmp_c, in, in_shift, num);
	}
	
	fftw_free(in_shift);
	//*/

	/* without volk
	// calculate first difference manually
	tmp_c[0] = in[0] * conj(d_last_sample);
	for(unsigned int i=1;i<num;i++) {
		tmp_c[i] = conj(in[i-1]) * in[i];
	}
	//*/

	return;
}

void ccsds_fll_cc::calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num) {
	// tmp_c and tmp_f are guaranteed to be aligned
	volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, 1.0f, num);

	/* without volk
	for(unsigned int i=0;i<num;i++) {
			tmp_f[i] = std::arg(tmp_c[i]);
	}
	//*/
	return;
}

void ccsds_fll_cc::calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num) {
	//* with volk	
	gr_complex *rot = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*num);

	// temp variable for sine and cosine part of rotator
	float tmp_s, tmp_c;

	// create the inidividual rotos
	for(unsigned int i=0;i<num;i++) {
		// confine phase between 0 and 2 PI, to ensure that we do not
		// encounter float over or underruns when iterating to long
		d_phase = std::fmod(d_phase-tmp_f[i], D_TWOPI);
		
		// calculate sine and cosine values for this phase. joint cal-
		// culation for the same angle is faster, than two individual
		// calls to sin and cos.
		sincosf(d_phase,&tmp_s, &tmp_c);

		// assemble the rotator and store it in an array
		rot[i] = gr_complex(tmp_c,tmp_s);
	}

	if(is_unaligned()) {
		volk_32fc_x2_multiply_32fc_u(out, in, rot, num);
	} else {
		volk_32fc_x2_multiply_32fc_a(out, in, rot, num);
	}

	fftw_free(rot);
	/*/

	/* without volk
	gr_complex rotator;
	for(unsigned int i=0;i<num;i++) {
		// do the phase integration in double precision
		d_phase = std::fmod(d_phase-(double)tmp_f[i], D_TWOPI);
		// the rotation itself can be single precision
		rotator = std::polar(1.0f,(float)d_phase);
		out[i] = in[i] * rotator;
	}
	//*/
	return;
}

int  ccsds_fll_cc::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	//float *freq = (float *) output_items[1];
	//float *freq_filtered = (float *) output_items[2];

	// how many samples can we process?
	unsigned int num = (noutput_items > ninput_items[0]) ? ninput_items[0] : noutput_items;

	// auxilliary variables
	gr_complex *tmp_c;
	float *tmp_f;

	// allocate temporary memory
	tmp_c = (gr_complex *) fftw_malloc(num * sizeof(gr_complex));
	tmp_f = (float *)      fftw_malloc(num * sizeof(float));
	if (tmp_c == 0 || tmp_f == 0) {
		fprintf(stderr,"ERROR: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return 0;
	}

	//
	// do the synchronization
	//
	
	// calculate difference phasors
	calc_diffs(tmp_c, in, num);

	// take the calculated phasors and calculate the phase difference.
	calc_phases(tmp_f, tmp_c, num);
	
	
	// Put these calculations into the filter
	if(d_last_sample_valid) { // should the first sample be filtered?
		// no, filter everything
		d_filter.filter(tmp_f,num);
	} else if (num > 1) {
		// more than one sample, skip first value
		d_filter.filter(&tmp_f[1],num-1);
	} // else there is only the one sample that should be skipped

	// rotate the samples according to the filtered frequency
	calc_rotation(out, in, tmp_f, num);

	// buffer last sample
	d_last_sample = in[num-1];

	// free resources
	fftw_free(tmp_f);
	fftw_free(tmp_c);

	// Tell runtime how many input samples we used
	consume_each(num);

	// Tell runtime system how many output items we produced
	return num;
}
