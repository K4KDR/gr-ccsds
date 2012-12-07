#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_pll_cc.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <stdio.h>
#include <math.h>
#include <ccsds_moving_average.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>

#define PLL_DEBUG

ccsds_pll_cc_sptr ccsds_make_pll_cc(unsigned int m, float loop_bandwidth) {
    return ccsds_pll_cc_sptr (new ccsds_pll_cc(m, loop_bandwidth) );
}

ccsds_pll_cc::ccsds_pll_cc (unsigned int m, float loop_bandwidth)
  : gr_block ("ccsds_pll_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	//gr_make_io_signature3 (1, 3, sizeof (gr_complex), sizeof (float), sizeof (float)))
	gr_make_io_signature (1, 1, sizeof (gr_complex))), d_M(m)
{
	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));

	d_filter = ccsds_make_lpf2(loop_bandwidth, 0.5, 1.0);
	d_phi_hat = 0.0f;

	#ifdef PLL_DEBUG
		dbg_count = 0;

		dbg_file = fopen("debug_pll.dat","w");
		if(dbg_file == NULL) {
			fprintf(stderr,"ERROR PLL: can not open debug file\n");
			exit(EXIT_FAILURE);
			return;
		}
		fprintf(dbg_file, "#k,phi_raw,phi_hat,arg(in),arg(in^M)\n");
	#endif
}

ccsds_pll_cc::~ccsds_pll_cc ()
{
	delete d_filter;

	#ifdef DLL_DEBUG
		fflush(dbg_file);

		fclose(dbg_file);
	#endif
}

const double ccsds_pll_cc::D_TWOPI=2.0f*M_PI;

void ccsds_pll_cc::rotate_constellation(gr_complex *out, const gr_complex *in, const float angle, const unsigned int num) {
	gr_complex rot = std::polar(1.0f, angle);

	if(is_unaligned()) {
		volk_32fc_s32fc_multiply_32fc_u(out, in, rot, num);
	} else {
		volk_32fc_s32fc_multiply_32fc_a(out, in, rot, num);
	}
}

void ccsds_pll_cc::remove_modulation(gr_complex *out, const gr_complex *in, const unsigned int num) {
	// FIXME volk power block seems to calculate wrong results 
	// (e.g. 1 squared is -1)	
	if(true || is_unaligned()) {
		for(unsigned int i=0;i<num;i++) {
			out[i] = std::pow(in[i],d_M);
		}
	} else {
		volk_32fc_s32f_power_32fc_a(out,in,(float)d_M,num);
	}
	return;
}

void ccsds_pll_cc::calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num) {
	// tmp_c and tmp_f are guaranteed to be aligned
	volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, (float)d_M, num);
	//volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, 1.0f, num);

	/* without volk
	for(unsigned int i=0;i<num;i++) {
			tmp_f[i] = std::arg(tmp_c[i]);
	}
	//*/
	return;
}

void ccsds_pll_cc::calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num) {
	//* with volk	
	gr_complex *rot = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*num);

	// temp variable for sine and cosine part of rotator
	float tmp_s, tmp_c;
	float phase_offset = 0.0f;

	if(d_M == 4) { // QPSK constellation rotated by 45 degree
		phase_offset = M_PI/4.0f;
	}

	// create the inidividual rotos
	for(unsigned int i=0;i<num;i++) {
		// calculate sine and cosine values for this phase. joint cal-
		// culation for the same angle is faster, than two individual
		// calls to sin and cos.
		sincosf(-(tmp_f[i]+phase_offset),&tmp_s, &tmp_c);

		// assemble the rotator and store it in an array
		rot[i] = gr_complex(tmp_c,tmp_s);
	}

	if(is_unaligned()) {
		volk_32fc_x2_multiply_32fc_u(out, in, rot, num);
	} else {
		volk_32fc_x2_multiply_32fc_a(out, in, rot, num);
	}

	fftw_free(rot);
	// */

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

int  ccsds_pll_cc::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	
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
	
	if(d_M == 4) { // QPSK constellation rotated by 45 degree, fix it
		rotate_constellation(tmp_c, in, -M_PI/4.0f, num);
	} else { // no fix necessarry, just move the samples to the next buffer
		memcpy(tmp_c,in,num*sizeof(gr_complex));
	}

	// remove the modulation
	remove_modulation(tmp_c,tmp_c,num);

	#ifdef PLL_DEBUG
		gr_complex *mod_removed;
		mod_removed = new gr_complex[num];
		memcpy(mod_removed, tmp_c, num*sizeof(gr_complex));
	#endif

	// take the calculated phasors and calculate the phase
	calc_phases(tmp_f, tmp_c, num);
	
	#ifdef PLL_DEBUG
		float *phase_unfiltered;
		phase_unfiltered = new float[num];
		memcpy(phase_unfiltered, tmp_f, num*sizeof(float));
	#endif

	// Put these calculations into the filter
	d_filter->filter_wrapped(tmp_f,M_PI/(float)d_M,num);
	
	//float e_phi;
	
	#ifdef PLL_DEBUG
		for(unsigned int i=0;i<num;i++) {
			fprintf(dbg_file, "%d,%f,%f,%f,%f\n",dbg_count++,phase_unfiltered[i]/M_PI,tmp_f[i]/M_PI,std::arg(in[i])/M_PI,std::arg(mod_removed[i])/M_PI);
		}
		delete[] phase_unfiltered;
		delete[] mod_removed;
	#endif

	// rotate the samples according to the filtered phase
	calc_rotation(out, in, tmp_f, num);

	// free resources
	fftw_free(tmp_f);
	fftw_free(tmp_c);

	// Tell runtime how many input samples we used
	consume_each(num);

	// Tell runtime system how many output items we produced
	return num;
}
