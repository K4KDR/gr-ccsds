#include "ccsds_conv_encode27_aux.h"

int main(void) {
	ccsds_conv_encode27_punct_aux encoder;

	const unsigned int LEN=8;
	uint8_t in[LEN] = {0xF0,0xAA,0xFF,0xFF,0x00,0x00,0xF0,0xAA};
	uint16_t out[LEN];
	uint8_t num_bits;

	for(unsigned int i=0;i<LEN;i++) {
		encoder.encode_punct(out[i], num_bits, in[i]);
		printf("%02X => %04X (%2d bits)\n",in[i],out[i],num_bits);
	}

	return 0;
}
