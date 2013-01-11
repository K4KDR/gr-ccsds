#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_mpsk_constellation.h"
#include <ccsds_mpsk_demod_cb.h>
#include <gr_io_signature.h>
#include <stdio.h>
#include <math.h>



ccsds_mpsk_demod_cb_sptr ccsds_make_mpsk_demod_cb (unsigned int M)
{
    return ccsds_mpsk_demod_cb_sptr (new ccsds_mpsk_demod_cb (M));
}

ccsds_mpsk_demod_cb::ccsds_mpsk_demod_cb (unsigned int M)
  : gr_sync_block ("ccsds_mpsk_demod_cb",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature (1, 1, sizeof (char))), d_M(M)
{
	if(M==2) {
		detect_symbol = &ccsds_mpsk_demod_cb::detect_bpsk_symbol;
	} else if (M==4) {
		detect_symbol = &ccsds_mpsk_demod_cb::detect_qpsk_symbol;
	} else {
		detect_symbol = &ccsds_mpsk_demod_cb::detect_mpsk_symbol;
	}

	d_constellation = ccsds_make_mpsk_const(M);	
}

ccsds_mpsk_demod_cb::~ccsds_mpsk_demod_cb ()
{
	// free memory allocated for the constellation
	delete[] d_constellation;
}

char ccsds_mpsk_demod_cb::detect_bpsk_symbol(gr_complex symbol) {
	if(std::real(symbol) > 0) {
		return 0;
	} else {
		return 1;
	}
}

char ccsds_mpsk_demod_cb::detect_qpsk_symbol(gr_complex symbol) {
	const float re = std::real(symbol);
	const float im = std::imag(symbol);


	if(im>re) {
		if(re>-im) {
			return 2;
		} else { // re <= -im
			return 3;
		}
	} else { // im <= re)
		if(re>-im) {
			return 0;
		} else { // re <= -im
			return 1;
		}
	}
	
}

// Brute Force per symbol
char ccsds_mpsk_demod_cb::detect_mpsk_symbol(gr_complex symbol) {
	float min_dist = INFINITY;
	float dist;
	char min_byte = 0;
	//printf("Detecting symbol: (%f, %f)\n",real(symbol),imag(symbol));

	for(char byte=0;(unsigned int)byte<d_M;byte++) {

		dist = std::abs(d_constellation[(unsigned int)byte]-symbol);
		min_byte = (dist >= min_dist) ? min_byte : byte;
		min_dist = (dist >= min_dist) ? min_dist : dist;
		//printf("  trying byte %2u: (%f,%f), distance=%f, min_dist=%f, selected=%2u\n",(unsigned int)byte, real(d_constellation[(unsigned int)byte]),
		//		imag(d_constellation[(unsigned int)byte]), dist, min_dist, (unsigned int)min_byte);
	}
	//printf("\n");

	return min_byte;
}

int  ccsds_mpsk_demod_cb::work (int                     noutput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	char *out = (char *) output_items[0];

	const unsigned int num = noutput_items;

	for(unsigned int i=0;i<num;i++) {
		out[i] = (this->*detect_symbol)(in[i]);
	}

	//consume_each(i);

	/* when profiling, process a fixed amount of samples and terminate afterwards
	count += i;
	if(count > 10000000) {
		fprintf(stdout,"M-PSK Demod: terminating\n");
		exit(EXIT_FAILURE);
		return i;
	}
	//*/

	// Tell runtime system how many output items we produced.
	return num;
}
