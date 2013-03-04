#include <ccsds_conv_encode27_aux.h>
#include <ccsds_conv_encode27_punct.h>
#include <stdio.h>

ccsds_conv_encode27_punct_aux::ccsds_conv_encode27_punct_aux(uint8_t gen_poly_c1, uint8_t gen_poly_c2, ccsds_conv_encode27_punct::punct_t puncturing) :
		d_INVERT_C1( (gen_poly_c1 & 0x80) == 0x80 ),
		d_INVERT_C2( (gen_poly_c2 & 0x80) == 0x80 ),
		d_POLY_C1(gen_poly_c1 & 0x7F),
		d_POLY_C2(gen_poly_c2 & 0x7F),
		d_PUNCT_TYPE(puncturing),
		d_PUNCT_LEN(ccsds_conv_encode27_punct::get_pattern_len(puncturing))
{
	d_state = 0;
	d_punct_count = 0;

	d_punct_pattern = ccsds_conv_encode27_punct::get_pattern(d_PUNCT_TYPE);

	/*
	printf("CONV ENCODE constructor:\n");
	printf("  in poly c1: %X\n",gen_poly_c1);
	printf("  in poly c2: %X\n",gen_poly_c2);
	printf("  invert c1: %s\n",(d_INVERT_C1) ? "yes":"no");
	printf("  invert c2: %s\n",(d_INVERT_C2) ? "yes":"no");
	printf("  poly c1: %X\n",d_POLY_C1);
	printf("  poly c2: %X\n",d_POLY_C2);
	printf("  puncturing type: %u\n",d_PUNCT_TYPE);
	printf("\n");
	//*/
}

ccsds_conv_encode27_punct_aux::~ccsds_conv_encode27_punct_aux() {
	// nothing to do
}

void ccsds_conv_encode27_punct_aux::encode(uint16_t& output_bits, unsigned char input_byte) {

	// Keep current byte and all memory slots within the constraint length
	const uint16_t state_mask_shift = 0x7FFF;
	// make sure state vector is ready for the next byte, only keep the
	// memory bytes within the constraint length and make sure that the
	// space for the current byte is zero
	const uint16_t state_mask_end   = 0x7F00;

	// read in new byte at the lowest 8 bit of the state vector
	d_state = d_state | (uint16_t)input_byte;

	output_bits = 0;

	// mask to select the input bit at time t. If shifted to left by
	// k the input bit at time t-k is selected.
	const uint16_t bit_mask = 1 << 7;

	for(unsigned int bit_in=0; bit_in<8; bit_in++) {

		/*
		printf("Input for bit %u : ",bit_in);
		for(unsigned int i=0;i<7;i++) {
			const uint32_t bit = (d_state & (bit_mask << i)) ? 1 : 0;
			printf("i(t-%u)=%u ",i,bit);
		}
		printf("\n");//*/

		uint8_t out_c1 = 0;
		uint8_t out_c2 = 0;

		// go through all shift registers
		for(unsigned int t=0;t<7;t++) {

			// if shift register holds a zero, the output will not
			// change regardless if there is a connection to the
			// output or not, so only proceed with this bit, if it
			// is one.

			if(d_state & (bit_mask << t)) {
				// there is a one in this state register, check
				// if this one affects the output

				// mask to select the right bit in the poly mask
				uint8_t poly_mask = 0x40 >> t; // 0100 0000 >> t

				
				// if output is affected, output bit will be flipped
				out_c1 ^= (poly_mask & d_POLY_C1) ? 0x01 : 0x00;
				out_c2 ^= (poly_mask & d_POLY_C2) ? 0x01 : 0x00;

				//printf("bit %u, t-%u, poly_mask=%2X, poly_a=%3u, poly_b=%3u, tmp_c1=%u, tmp_c2=%u\n",bit_in,t,poly_mask,(poly_mask & d_POLY_C1),(poly_mask & d_POLY_C1),out_c1, out_c2);

			}
		}

		//printf("out: c1=%u, c2=%u, not(c2)=%u\n",out_c1,out_c2,out_c2^0x01); 

		// we have the two out bits, add them to the output stream by
		// first shifting the old output out of the LSB (output_bits<<1)
		// and then setting the new bit to the LSB (with OR since the
		// LSB is now zero). Make sure that the new bit is really just a
		// bit with (masking with 0x0001) and invert bit before, if the
		// bit needs to be inverted

		// c1 first
		output_bits = (output_bits<<1) | (uint16_t) ( ((d_INVERT_C1) ? out_c1^0x01 : out_c1) & 0x01 );
		// then c2
		output_bits = (output_bits<<1) | (uint16_t) ( ((d_INVERT_C2) ? out_c2^0x01 : out_c2) & 0x01 );

		//printf("output stream: %X\n", output_bits);

		// proceed with next input bit
		d_state = (d_state << 1) & state_mask_shift;
	}

	// make sure the current byte positions are all zero (cleanup)	
	d_state = d_state & state_mask_end;

	//printf("%X => %X\n\n\n\n",input_byte,output_bits);

	return;
}

void ccsds_conv_encode27_punct_aux::puncture(uint16_t& output_bits, uint8_t& num_out_bits, uint16_t input_bits) {
	
	// count number of output bits
	num_out_bits = 0;

	// start with zero and set the bits one by one.
	output_bits = 0;

	//printf("punct in: %X\n",input_bits);

	// go through all input bits and apply keep drop pattern
	for(unsigned int i=0;i<16;i++) {

		
		/*
		printf("  bit %2u: ",i);
		if(input_bits & (0x8000>>i)) {
			printf("is one  ");
		} else {
			printf("is zero ");
		}
		//*/

		// do we keep this bit
		if(d_punct_pattern[d_punct_count]) {
			// yes we keep it

			//printf("keep it ");

			// shift output to left to make space for the new bit
			output_bits = output_bits << 1;

			const uint16_t bit_mask = 0x8000>>i;

			// is it a one
			if(input_bits & bit_mask) {
				//set last bit to one
				output_bits |= 0x0001;
			}

			// in any case shift count the output bit
			num_out_bits++;
		}

		d_punct_count = (d_punct_count+1) % d_PUNCT_LEN;

		//printf("\n");
	}
}


void ccsds_conv_encode27_punct_aux::encode_punct(uint16_t& output_bits, uint8_t& num_out_bits, unsigned char input_byte) {

	// buffer for encoded and unpunctured bits	
	uint16_t encoded;

	// encode
	encode(encoded, input_byte);

	// puncturing
	if(d_PUNCT_TYPE == ccsds_conv_encode27_punct::NONE) {
		output_bits = encoded;
		num_out_bits = 16;
		//printf("CONV ENCODER: no punct: %02X => %04X (%u bits)\n",input_byte, output_bits, num_out_bits);
	} else {
		puncture(output_bits, num_out_bits, encoded);
		//printf("CONV ENCODER: punct: %02X => %04X => %04X (%u bits)\n",input_byte, encoded, output_bits, num_out_bits);
	}

	return;
}

