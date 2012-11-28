#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_dll_cc.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>

#define DLL_DEBUG

ccsds_dll_cc_sptr ccsds_make_dll_cc(unsigned int osf, float gamma) {
    return ccsds_dll_cc_sptr (new ccsds_dll_cc(osf,(float)gamma/osf) );
}

ccsds_dll_cc::ccsds_dll_cc (unsigned int osf, float gammab)
  : gr_block ("ccsds_dll_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	//gr_make_io_signature3 (1, 3, sizeof (gr_complex), sizeof (float), sizeof (float)))
	gr_make_io_signature (1, 1, sizeof (gr_complex))), d_OSF(osf), d_OSF_HALF((float)osf/2.0f), d_GAMMAB(gammab)
{
	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));

	// not initialized, we will init the other variables in general_work
	d_init = false;

	#ifdef DLL_DEBUG
		dbg_count = 0;

		dbg_file_o = fopen("debug_dll_orig.dat","w");
		dbg_file_i = fopen("debug_dll_intp.dat","w");
		if(dbg_file_o == NULL || dbg_file_i == NULL) {
			fprintf(stderr,"ERROR DLL: can not open debug file\n");
			exit(EXIT_FAILURE);
			return;
		}
		fprintf(dbg_file_o, "#k,real(y[k]),imag(y[k]),tau_hat\n");
		fprintf(dbg_file_i, "#k,t_i,real(y_hat[n]),imag(y_hat[n]),e_tau,l,mu\n");
	#endif
}

ccsds_dll_cc::~ccsds_dll_cc () {
	#ifdef DLL_DEBUG
		fflush(dbg_file_o);
		fflush(dbg_file_i);

		fclose(dbg_file_o);
		fclose(dbg_file_i);
	#endif
}

float ccsds_dll_cc::get_frac(float value) {
	return std::fmod(value,1.0f);
}

int ccsds_dll_cc::get_int(float value) {
	return (int) std::floor(value);
}


void ccsds_dll_cc::forecast(int noutput_items,gr_vector_int &ninput_items_required){
	// basically decimating by a factor of d_OSF, but for interpolation we
	// need to see 2 more samples "into the future"
	ninput_items_required[0] = d_OSF * noutput_items + 2;
}

inline void ccsds_dll_cc::to_real(float *out, const gr_complex *in, const unsigned int num) {
	for(unsigned int i=0;i<num;i++) {
		out[i] = std::real(in[i]);
	}
	return;
}

inline void ccsds_dll_cc::to_imag(float *out, const gr_complex *in, const unsigned int num) {
	for(unsigned int i=0;i<num;i++) {
		out[i] = std::imag(in[i]);
	}
	return;
}

/*
	Gardner timing error detector for passband signals

	e(k) = re{ [ y(kT-T+tau[k-1]) - y(kT+tau[k]) ] y*(kT-T/2+tau[k-1]) }
	Mengali p. 431
*/
float ccsds_dll_cc::gardner(gr_complex previous, gr_complex intermediate, gr_complex current) {
	return std::real( (previous-current) * std::conj(intermediate) );
}

gr_complex ccsds_dll_cc::interpolate_cvalue(const gr_complex *y, float mu) {
	const unsigned int num = 4;
	float real[num];
	float imag[num];

	to_real(real, y, num);
	to_imag(imag, y, num);

	return gr_complex(interpolate_value(real, mu), interpolate_value(imag, mu));
}

inline float ccsds_dll_cc::interpolate_value(const float *y, float mu) {
	float a,b,c,d;

	a = y[3] - y[2] - y[0] + y[1];
	b = y[0] - y[1] - a;
	c = y[2] - y[0];
	d = y[1];

	return(a*mu*mu*mu+b*mu*mu+c*mu+d);
}



int  ccsds_dll_cc::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	//float *freq = (float *) output_items[1];
	
	// how many samples can we process?
	unsigned int num = (d_OSF*noutput_items > (unsigned int) ninput_items[0] ) ? ninput_items[0] : noutput_items*d_OSF;
	
	// how many samples to output, when we want to have d_OSF spare samples
	// at the end and one at the front, to ensure the interpolator never has
	// to extrapolate
	int num_out = std::floor(num-1/d_OSF) -1;

	// not enough samples for one iteration
	if(num < 5 || num_out <= 0) {
		consume_each(0);
		return 0;
	}

	if(!d_init) {
		d_tau_hat = 0.0f;
		d_l = 1;
		d_mu = 0.0f;
		
		// retrieve first symbol
		// mu is zero, so take the sample directly
		d_last_interp[PREV] = in[1];

		// update indices
		d_l += get_int(d_OSF_HALF);
		d_mu = get_frac(d_mu + d_OSF_HALF);

		// retrieve intermediate sample
		d_last_interp[INTM] = interpolate_cvalue(&in[d_l-1], d_mu);

		// update indices
		d_l += get_int(d_mu + d_OSF_HALF);
		d_mu = get_frac(d_mu + d_OSF_HALF);

		// retrieve second symbol
		d_last_interp[CURR] = interpolate_cvalue(&in[d_l-1], d_mu);

		// update tau_hat
		float gamma_eps = d_GAMMAB * gardner(d_last_interp[0], d_last_interp[1], d_last_interp[2]);
		d_tau_hat += gamma_eps;

		// update indices
		d_l += get_int(d_mu + d_OSF_HALF + gamma_eps);
		d_mu = get_frac(d_mu + d_OSF_HALF + gamma_eps);

		// initialization done
		d_init = true;

		// output the two first symbols
		out[0] = d_last_interp[PREV];
		out[1] = d_last_interp[CURR];

		// Tell runtime how many input samples we used
		// next intermediate sample will have basepoint index d_l and
		// we nedd one previous sample for interpolation
		consume_each(d_l-1);

		// next block will contain exactly one sample prior to the
		// basepoint sample
		d_l = 1;

		// Tell runtime system how many output items we produced
		return 2;
	} // end init

	// save normalized timing error estimate
	float gamma_eps;

	for(unsigned int n=0;n<num_out;n++) {
		// drop last two interpolants and shift buffer
		d_last_interp[PREV] = d_last_interp[CURR];

		// retrieve intermediate sample
		d_last_interp[INTM] = interpolate_cvalue(&in[d_l-1], d_mu);

		// update indices
		d_l += get_int(d_mu + d_OSF_HALF);
		d_mu = get_frac(d_mu + d_OSF_HALF);

		// retrieve second symbol
		d_last_interp[CURR] = interpolate_cvalue(&in[d_l-1], d_mu);

		// output the new symbol
		out[n] = d_last_interp[CURR];

		// update tau_hat
		gamma_eps = d_GAMMAB * gardner(d_last_interp[0], d_last_interp[1], d_last_interp[2]);
		d_tau_hat += gamma_eps;

		// update indices
		d_l += get_int(d_mu + d_OSF_HALF + gamma_eps);
		d_mu = get_frac(d_mu + d_OSF_HALF + gamma_eps);
	}

	// Tell runtime how many input samples we used
	consume_each(d_l-1);

	// next block will contain exactly one sample prior to the
	// basepoint sample
	d_l = 1;

	// Tell runtime system how many output items we produced
	return num_out;
}
