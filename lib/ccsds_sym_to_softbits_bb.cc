#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_sym_to_softbits_bb.h>
#include <gr_io_signature.h>
#include <stdio.h>

ccsds_sym_to_softbits_bb_sptr
ccsds_make_sym_to_softbits_bb (unsigned int info_bits, unsigned char num_quant_bits) {
    return ccsds_sym_to_softbits_bb_sptr (new ccsds_sym_to_softbits_bb(info_bits, num_quant_bits));
}

ccsds_sym_to_softbits_bb::ccsds_sym_to_softbits_bb(unsigned int info_bits, unsigned char num_quant_bits)
  : gr_sync_block ("ccsds_sym_to_softbits_bb",
	gr_make_io_signature (2, 2, sizeof(unsigned char)),
	gr_make_io_signature (1, 1, sizeof(unsigned char))),
    d_NUM_BITS(info_bits),
    d_QUANT_MAX(num_quant_bits)
{
	set_fixed_rate(true);
	set_relative_rate(d_NUM_BITS);

	set_output_multiple(d_NUM_BITS);
}

ccsds_sym_to_softbits_bb::~ccsds_sym_to_softbits_bb ()
{
	// nothing to do
}

uint8_t ccsds_sym_to_softbits_bb::bit_to_softbit(bool data_bit, uint8_t confidence) {
	
	if(data_bit) {
		// one bit 128 if confidence = 0, or 129 - 255
		return 128 + (uint8_t)round(127.0f * confidence / d_QUANT_MAX);
	} else {
		// zero bit 0 - 127, or 128 for confidence = 0
		return 128 - (uint8_t)round(128.0f * confidence / d_QUANT_MAX);
	}
}

int  ccsds_sym_to_softbits_bb::work (int	noutput_items,
						gr_vector_const_void_star   &input_items,
						gr_vector_void_star         &output_items)
{
	const unsigned char *in_sym  = (const unsigned char *) input_items[0];
	const unsigned char *in_conf = (const unsigned char *) input_items[1];
	unsigned char *out = (unsigned char *) output_items[0];

	// go through all input symbols
	for(unsigned int i=0;i<(unsigned int)noutput_items;i++) {

		// go through all bits in the input symbol
		for(unsigned int j=0;j<d_NUM_BITS;j++) {

			// extract the infobit
			const bool bit = ( in_sym[i] & (0x01<<j) );

			// transform to softbit and output
			out[d_NUM_BITS*i+j] = bit_to_softbit(bit, in_conf[i]);
		}
	}

	return noutput_items;
}
