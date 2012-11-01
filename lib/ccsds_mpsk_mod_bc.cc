#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_mpsk_mod_bc.h>
#include <gr_io_signature.h>
#include <stdio.h>
#include <math.h>

ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc (unsigned int M)
{
    return ccsds_mpsk_mod_bc_sptr (new ccsds_mpsk_mod_bc (M));
}

static const int MIN_IN = 1;    // mininum number of input streams
static const int MAX_IN = 1;    // maximum number of input streams
static const int MIN_OUT = 1;   // minimum number of output streams
static const int MAX_OUT = 1;   // maximum number of output streams

ccsds_mpsk_mod_bc::ccsds_mpsk_mod_bc (unsigned int M)
  : gr_block ("mpsk_mod_bc",
	gr_make_io_signature (MIN_IN, MAX_IN, sizeof (char)),
	gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (gr_complex)))
{
	d_M = M;

	// create phasor for rotation between the symbols
	const gr_complex diff_phasor = gr_complex(cos(2*M_PI/d_M),sin(2*M_PI/d_M));
	// initial phasor is on the real axis, except for QPSK (according to ECSS standard)
	gr_complex phasor = (d_M!=4) ? gr_complex(1.0,0.0) : gr_complex(sqrt(2.0),sqrt(2.0));

	// allocate memory for the constellation points
	d_constellation = (gr_complex *)malloc(d_M * sizeof(gr_complex));
	if (d_constellation == 0) {
		printf("ERROR: Out of memory\n");
		return;
	}

	// go through all constellation points
	for(unsigned int i=0;i<d_M;i++) {
		// store constellation point
		d_constellation[binaryToGray(i)] = phasor;
		// rotate to next point
		phasor *= diff_phasor;
	}
}

ccsds_mpsk_mod_bc::~ccsds_mpsk_mod_bc ()
{
	// free memory allocated for the constellation
	free(d_constellation);
}

int  ccsds_mpsk_mod_bc::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const char *in = (const char *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	// counter
	unsigned int i;
	for(i=0;i<noutput_items && i<ninput_items[0];i++) {
		out[i] = d_constellation[((unsigned int)in[i]) % d_M];
	}

	consume_each(i);

	// Tell runtime system how many output items we produced.
	return i;
}
