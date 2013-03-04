#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_sym_to_softbits_bf.h>
#include <gr_io_signature.h>
#include <stdio.h>

ccsds_sym_to_softbits_bf_sptr
ccsds_make_sym_to_softbits_bf (unsigned int info_bits, unsigned char num_quant_bits) {
    return ccsds_sym_to_softbits_bf_sptr (new ccsds_sym_to_softbits_bf(info_bits, num_quant_bits));
}

ccsds_sym_to_softbits_bf::ccsds_sym_to_softbits_bf(unsigned int info_bits, unsigned char num_quant_bits)
  : gr_block ("ccsds_sym_to_softbits_bf",
	gr_make_io_signature (2, 2, sizeof(unsigned char)),
	gr_make_io_signature (1, 1, sizeof(float))),
    d_NUM_BITS(info_bits),
    d_QUANT_MAX((unsigned int)(pow(2,num_quant_bits)-1))
{
	//set_relative_rate(d_NUM_BITS);

	set_output_multiple(d_NUM_BITS);
}

ccsds_sym_to_softbits_bf::~ccsds_sym_to_softbits_bf ()
{
	// nothing to do
}

void ccsds_sym_to_softbits_bf::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
	const int num_in = std::max((int)std::ceil((float)noutput_items / d_NUM_BITS), 1);

	ninput_items_required[0] = num_in;
	ninput_items_required[1] = num_in;
	
	return;
}

float ccsds_sym_to_softbits_bf::bit_to_softbit(const bool data_bit, const uint8_t confidence) {
	return (data_bit ? 1.0f : -1.0f) * confidence / d_QUANT_MAX;
}

int  ccsds_sym_to_softbits_bf::general_work (int	noutput_items,
							gr_vector_int               &ninput_items,
			                                gr_vector_const_void_star   &input_items,
                			                gr_vector_void_star         &output_items)
{
	const unsigned char *in_sym  = (const unsigned char *) input_items[0];
	const unsigned char *in_conf = (const unsigned char *) input_items[1];
	float *out = (float *) output_items[0];

	const unsigned int num_in = std::min(std::min(ninput_items[0],ninput_items[1]), (int)std::floor((float)noutput_items/d_NUM_BITS));

	// go through all input symbols
	for(unsigned int i=0;i<num_in;i++) {
		// go through all bits in the input symbol
		for(unsigned int j=0;j<d_NUM_BITS;j++) {
			// extract the infobit
			const bool bit = ( in_sym[i] & (0x01<<j) );

			// transform to softbit and output
			out[d_NUM_BITS*i+j] = bit_to_softbit(bit, in_conf[i]);
		}
	}


	consume_each(num_in);
	return num_in*d_NUM_BITS;
}
