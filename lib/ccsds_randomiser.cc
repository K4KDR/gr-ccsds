#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_randomiser.h>
#include <gr_io_signature.h>
#include <stdio.h>

ccsds_randomiser_sptr ccsds_make_randomiser(const uint8_t polynomial, const uint8_t seed) {
    return ccsds_randomiser_sptr (new ccsds_randomiser(polynomial, seed) );
}

ccsds_randomiser::ccsds_randomiser(const uint8_t polynomial, const uint8_t seed)
  : gr_sync_block ("ccsds_randomiser",
	gr_make_io_signature (0, 0, 0),
	gr_make_io_signature (0, 0, 0)), d_POLY(polynomial), d_SEED(seed)
{
	d_stop = false;

	d_state = d_SEED;

	// register output port
	message_port_register_out(pmt::mp("out"));

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_randomiser::process_frame, this, _1));
}

ccsds_randomiser::~ccsds_randomiser () {
}

void ccsds_randomiser::reset(void) {
	d_state = d_SEED;
}

uint8_t ccsds_randomiser::get_next_byte(void) {
	
	uint8_t out = 0x00;

	// loop 8 times to collect a new byte
	for(unsigned int i=0;i<8;i++) {
		// Bits from all registers that generate the next input bits
		uint8_t input_bits = d_state & d_POLY;

		// Buffer to store the XORed input bit in the LSB position
		uint8_t input_bit = 0x00;

		// go through all positions in input_bits and summ them up
		for(unsigned int j=0;j<8;j++) {
			input_bit ^= (input_bits>>j) & 0x01;
		}

		// collect the MSB as output before it is shifted out of scope
		out = (out<<1) | ((d_state>>7) & 0x01);

		// perform the shift and add new bit
		d_state = (d_state<<1) | input_bit;
	}

	// The state has been updated 8 times now, output the collected bits
	return out;
}

void ccsds_randomiser::process_frame(pmt::pmt_t msg_in) {

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
		d_stop = true;

		return;
	}

	if(!pmt::pmt_is_blob(msg_in)) {
		fprintf(stderr,"ERROR PSEUDO RANDOMIZER: expecting message of type blob, skipping.\n");
		return;
	}

	// Message is BLOB
	const unsigned int BLOB_LEN = pmt::pmt_length(msg_in);

	// Assign input and output pointer
	const uint8_t *data_in = (const uint8_t *) pmt::pmt_blob_data(msg_in);
	boost::shared_ptr<uint8_t[]> data_out = boost::shared_ptr<uint8_t[]>(new uint8_t[BLOB_LEN]);

	// Reset state before new block
	reset();

	// Go through all bytes of the block
	for(unsigned int i=0;i<BLOB_LEN;i++) {
		data_out[i] = (data_in[i] ^ get_next_byte());
	}

	// Generate output message
	pmt::pmt_t msg_out = pmt::pmt_make_blob(data_out.get(), BLOB_LEN);

	// Post message
	message_port_pub( pmt::mp("out"), msg_out );
}

int ccsds_randomiser::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
	return (d_stop ? -1 : 0);
}
