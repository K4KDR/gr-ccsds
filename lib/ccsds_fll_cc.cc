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

ccsds_fll_cc_sptr
ccsds_make_fll_cc ()
{
    return ccsds_fll_cc_sptr (new ccsds_fll_cc ());
}

ccsds_fll_cc::ccsds_fll_cc ()
  : gr_block ("ccsds_fll_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature3 (1, 3, sizeof (gr_complex), sizeof(float), sizeof(float)))
{
	// initialize on real axis
	d_last_sample = gr_complex(1.0,0.0);
	d_last_sample_valid = false;

	d_rotator = gr_complex(1.0,0.0);
	
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
	float *freq_out = (float *) output_items[1];
	float *debug_out = (float *) output_items[2];

	// how many samples can we process?
	unsigned int num = (noutput_items > ninput_items[0]) ? ninput_items[0] : noutput_items;

	// auxilliary variables
	gr_complex *tmp_c;
	float *tmp_f;

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

	// calculate first difference manually
	tmp_c[0] = conj(d_last_sample) * in[0];

	// calculate the difference phasors
	for(size_t i=1;i<num;i++) {
		tmp_c[i] = conj(in[i-1]) * in[i];
	}

	// take the calculated phasors and calculate the phase difference. Put these calculations into the filter
	bool filter = d_last_sample_valid; // should the first sample be filtered?	
	
	for(size_t i=0;i<num;i++) {
		// calculate the phase difference
		float diff = std::arg(tmp_c[i]);
		tmp_f[i] = (filter) ? d_filter.filter(diff) : diff;

		if(!filter) {
			// we want to skip only the first sample
			// once we are here we have already skiped one
			filter = true;
		}

		d_rotator *= std::polar(1.0f,-tmp_f[i]);
		out[i] = in[i] * d_rotator;
		freq_out[i] = tmp_f[i];
		debug_out[i] = diff;
	}


	// buffer last sample
	d_last_sample = in[num-1];

	// free resources
	free(tmp_f);
	free(tmp_c);

	consume_each(num);

	// Tell runtime system how many output items we produced.
	return num;
}
