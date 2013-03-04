#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_softbits_msg_to_bytes_b.h>
#include <gr_io_signature.h>
#include <cstdio>


ccsds_softbits_msg_to_bytes_b_sptr ccsds_make_softbits_msg_to_bytes_b(bool /*dummy*/) {
    return ccsds_softbits_msg_to_bytes_b_sptr (new ccsds_softbits_msg_to_bytes_b() );
}

ccsds_softbits_msg_to_bytes_b::ccsds_softbits_msg_to_bytes_b (void)
  : gr_sync_block ("ccsds_softbits_msg_to_bytes_b",
	gr_make_io_signature (0, 0, sizeof(unsigned char)),
	gr_make_io_signature (1, 1, sizeof(unsigned char)))
{
	d_stop = false;

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_softbits_msg_to_bytes_b::process_message, this, _1));
}

ccsds_softbits_msg_to_bytes_b::~ccsds_softbits_msg_to_bytes_b () {
	// nothing to do
}

void ccsds_softbits_msg_to_bytes_b::process_message(pmt::pmt_t msg_in) {

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		d_stop = true;
		return;
	}

	// check if message has right format	
	if(!pmt::pmt_is_f32vector(msg_in)) {
		fprintf(stderr,"ERROR SOFTBIT MSG TO BYTES: expecting message of type f32vector, skipping.\n");
		return;
	}

	const unsigned int num_bits = pmt::pmt_length(msg_in);

	if(num_bits % 8 != 0) {
		fprintf(stderr,"WARNING SOFTBIT MSG TO BYTES: expecting message to contain a integer number of bytes. Actual length %d bits, last byte will be padded with zeros.\n", num_bits);
	}

	// Message is a f32vector with an integer multiple of 8 bits

	const unsigned int num_bytes = std::ceil(num_bits/8.0);

	// go through all output bytes
	for(unsigned int i=0;i<num_bytes;i++) {

		uint8_t byte = 0x00;

		// go through all bits of the current byte
		for(unsigned int j=0;j<8;j++) {
			// convert soft bit to hard decision
			const bool bit = (8*i+j < num_bits) ? (pmt::pmt_f32vector_ref(msg_in,i*8+j) > 0) : 0;

			// add bit to output
			byte = (byte<<1) | (bit ? 0x01 : 0x00);
		}

		d_queue.push(byte);
	}

	return;
}

int ccsds_softbits_msg_to_bytes_b::work (int                 noutput_items,
                                gr_vector_const_void_star&   /*input_items*/,
                                gr_vector_void_star&         output_items)
{

	if(d_stop && d_queue.size() == 0) {
		// signal EOF
		return -1;
	}

	unsigned char *out = (unsigned char *) output_items[0];

	unsigned int num_out;

	for(num_out = 0;num_out<(unsigned int)noutput_items && d_queue.size()>0;num_out++) {
		out[num_out] = d_queue.front();
		d_queue.pop();
	}

	// Tell runtime system how many output items we produced
	return num_out;
}

