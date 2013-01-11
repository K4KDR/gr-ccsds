#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_mpsk_constellation.h"
#include <ccsds_mpsk_mod_bc.h>
#include <gr_io_signature.h>
#include <stdio.h>
#include <math.h>

ccsds_mpsk_mod_bc_sptr
ccsds_make_mpsk_mod_bc (unsigned int M)
{
    return ccsds_mpsk_mod_bc_sptr (new ccsds_mpsk_mod_bc (M));
}

static const int MIN_IN = 1;    // mininum number of input streams
static const int MAX_IN = 1;    // maximum number of input streams
static const int MIN_OUT = 1;   // minimum number of output streams
static const int MAX_OUT = 1;   // maximum number of output streams

ccsds_mpsk_mod_bc::ccsds_mpsk_mod_bc (unsigned int M)
  : gr_sync_block ("ccsds_mpsk_mod_bc",
	gr_make_io_signature (MIN_IN, MAX_IN, sizeof (char)),
	gr_make_io_signature (MIN_OUT, MAX_OUT, sizeof (gr_complex))), d_M(M)
{
	d_constellation = ccsds_make_mpsk_const(M);
}

ccsds_mpsk_mod_bc::~ccsds_mpsk_mod_bc ()
{
	// free memory allocated for the constellation
	delete[] d_constellation;
}

int  ccsds_mpsk_mod_bc::work (int                     noutput_items,
                         gr_vector_const_void_star   &input_items,
                         gr_vector_void_star         &output_items)
{
	const char *in = (const char *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	const unsigned int num = noutput_items;

	for(unsigned int i=0;i<num;i++) {
		out[i] = d_constellation[(in[i]) % d_M];
	}

	//consume_each(i);

	// Tell runtime system how many output items we produced.
	return num;
}
