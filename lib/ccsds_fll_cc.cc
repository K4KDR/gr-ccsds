#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_fll_cc.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <gr_rotator.h>
#include <stdio.h>
#include <math.h>

ccsds_fll_cc_sptr ccsds_make_fll_cc ()
{
    return ccsds_fll_cc_sptr (new ccsds_fll_cc ());
}

static const int MIN_IN = 1;    // mininum number of input streams
static const int MAX_IN = 1;    // maximum number of input streams
static const int MIN_OUT = 1;   // minimum number of output streams
static const int MAX_OUT = 1;   // maximum number of output streams

ccsds_fll_cc::ccsds_fll_cc ()
  : gr_block ("fll_cc",
          gr_make_io_signature (MIN_IN, MAX_IN, sizeof (gr_complex)),
          gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (gr_complex)))
{
    // nothing else required in this example
}

ccsds_fll_cc::~ccsds_fll_cc ()
{
    // nothing else required in this example
}

int  ccsds_fll_cc::general_work (int                     noutput_items,
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
	float freq;

	// allocate temporary memory
	tmp_c = (gr_complex *)malloc(num * sizeof(gr_complex));
	tmp_f = (float *)malloc(num * sizeof(float));
	if (tmp_c == 0 || tmp_f == 0) {
		printf("ERROR: Out of memory\n");
		return 1;
	}

	//
	// do the synchronization
	//

	if(! is_unaligned()) {
		// calculate phase difference phasor
		volk_32fc_x2_multiply_conjugate_32fc_a(tmp_c, &in[0], &in[1], num-1);
		// calcuate phase
		volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, 1.0, num-1);
		// add phases for average computation
		volk_32f_accumulator_s32f_a(&freq, tmp_f, num-1);
		freq = freq/(num-1);

		// FIXME volk rotator only available for gnuradio >= 3.6.2
		// translate (float) frequency offset into a per sample phasor
		//gr_complex initial_phase=gr_complex(1.0,0.0);
		//volk_32fc_s32fc_x2_rotator_32fc_a(out, in, gr_complex(cos(freq),sin(freq)), &initial_phase, num);

	} else {
		printf("FLL working with unaligned buffers");

		// calculate phase difference phasor
		volk_32fc_x2_multiply_conjugate_32fc_u(tmp_c, &in[0], &in[1], num-1);
		// calculate the phase and create the average
		float freq = 0.0;
		for(size_t i=0;i<num-1;i++) {
			// calculate phase difference (up to num-1 so we don't run out of array bounds)
			tmp_c[i] = conj(in[i]) * in[i+1];
			// calculate the phase out of this
			tmp_f[i] = atan2(imag(tmp_c[i]),real(tmp_c[i]));
			freq += tmp_f[i];
		}
		// normalize freq by number of samples
		freq = freq/(num-1);

	}

	// FIXME volk rotator only available for gnuradio >= 3.6.2
	// creatr rotator
	gr_rotator rot = gr_rotator();
	rot.set_phase_incr(gr_complex(cos(freq),sin(freq)));
	for(size_t i=0;i<num;i++) {
		out[i] = rot.rotate(in[i]);
	}

	// free resources
	free(tmp_f);
	free(tmp_c);

	consume_each(num);

	// Tell runtime system how many output items we produced.
	return num;
}
