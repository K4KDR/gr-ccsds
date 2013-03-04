#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_mpsk_constellation.h"
#include <ccsds_mpsk_preamble_cc.h>
#include <gr_io_signature.h>
#include <stdio.h>
#include <math.h>

ccsds_mpsk_preamble_cc_sptr
ccsds_make_mpsk_preamble_cc (const unsigned int M, const unsigned int num_symbols) {
    return ccsds_mpsk_preamble_cc_sptr (new ccsds_mpsk_preamble_cc (M, num_symbols));
}

ccsds_mpsk_preamble_cc::ccsds_mpsk_preamble_cc(const unsigned int M, const unsigned int num_symbols)
  : gr_block ("ccsds_mpsk_preamble_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature (1, 1, sizeof (gr_complex))), d_M(M), d_NUM_SYMBOLS(num_symbols)
{
	d_preamble_count = 0;

	if(M == 4) {
		float factor = 1.0f / sqrt(2.0f);
		d_preamble_syms[0] = gr_complex( factor, factor);
		d_preamble_syms[1] = gr_complex(-factor,-factor);
	} else {
		d_preamble_syms[0] = gr_complex( 1.0f,0.0f);
		d_preamble_syms[1] = gr_complex(-1.0f,0.0f);
	}
}

ccsds_mpsk_preamble_cc::~ccsds_mpsk_preamble_cc ()
{
	// nothing to do
}

void ccsds_mpsk_preamble_cc::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
	// are we still generating, or already copying?
	if(d_preamble_count >= d_NUM_SYMBOLS) {
		// copying
		ninput_items_required[0] = noutput_items;
	} else {
		// generating
		ninput_items_required[0] = 0;
	}
	return;
}

int  ccsds_mpsk_preamble_cc::general_work (int                     noutput_items,
			 gr_vector_int 		     &ninput_items,
                         gr_vector_const_void_star   &input_items,
                         gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];


	// do we still have to generate the preamble?
	if(d_preamble_count < d_NUM_SYMBOLS) {

		// yes, generate preamble
		const unsigned int num = std::min(d_NUM_SYMBOLS-d_preamble_count, (unsigned int)noutput_items);
		for(unsigned int i=0;i<num;i++) {
			out[i] = d_preamble_syms[d_preamble_count%2];
			d_preamble_count++;
		}

		// return here to don't handle overlapping cases in the later
		// copy code
		consume_each(0);
		return num;
	}

	// no, we can copy the input stream

	// how many samples can we process
	const unsigned int num = std::min(noutput_items, ninput_items[0]);

	// copy stream
	memcpy(out, in, num*sizeof(gr_complex));

	// consume input stream
	consume_each(num);

	// Tell runtime system how many output items we produced.
	return num;
}
