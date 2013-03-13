#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_rs_encode.h>
#include <ccsds_interleaving_aux.h>
#include <gr_io_signature.h>
#include <cstdio>



ccsds_rs_encode_sptr ccsds_make_rs_encode(const unsigned int I) {
    return ccsds_rs_encode_sptr (new ccsds_rs_encode(I) );
}

ccsds_rs_encode::ccsds_rs_encode(const unsigned int I)
  : gr_block ("ccsds_rs_encode",
	gr_make_io_signature (0, 0, 0),
	gr_make_io_signature (0, 0, 0)), d_I(I), d_k(223), d_n(255), d_2E(32), d_DATA_LEN(d_k*d_I), d_PARITY_LEN(d_2E*d_I), d_OUT_LEN(d_DATA_LEN+d_PARITY_LEN)
{
	// create buffers
	d_buf_data    = new unsigned char[ d_DATA_LEN ];
	d_buf_parity  = new unsigned char[ d_PARITY_LEN ];
	d_buf_parity2 = new unsigned char[ d_PARITY_LEN ];
	d_buf_out     = new unsigned char[ d_OUT_LEN ];

	// register output port
	message_port_register_out(pmt::mp("out"));

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_rs_encode::process_frame, this, _1));
}

ccsds_rs_encode::~ccsds_rs_encode () {
	delete[] d_buf_data;
	delete[] d_buf_parity;
	delete[] d_buf_parity2;
	delete[] d_buf_out;
}

void ccsds_rs_encode::process_frame(pmt::pmt_t msg_in) {
	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
		return;
	}

	// check that input is a pair value
	if(!pmt::pmt_is_pair(msg_in)) {
		fprintf(stderr,"WARNING RS DECODE: expecting message of type pair, skipping.\n");
		return;
	}

	const pmt::pmt_t hdr = pmt::pmt_car(msg_in);
	const pmt::pmt_t msg = pmt::pmt_cdr(msg_in);

	// check that input header is a dictionary
	if(!pmt::pmt_is_dict(hdr)) {
		fprintf(stderr,"WARNING RS DECODE: expecting message header of type dict, skipping.\n");
		return;
	}

	// check that input data is a blob
	if(!pmt::pmt_is_blob(msg)) {
		fprintf(stderr,"WARNING RS DECODE: expecting message data of type blob, skipping.\n");
		return;
	}

	const unsigned int blob_len = (unsigned int) pmt::pmt_blob_length(msg);

	if(blob_len < d_DATA_LEN) {
		fprintf(stderr,"ERROR RS ENCODE: blob message length of %u bytes is smaller than the expected length of %u bytes.\n", blob_len, d_DATA_LEN);
		return;
	} else if(blob_len != d_DATA_LEN) {
		fprintf(stderr,"WARNING RS ENCODE: blob message length of %u bytes does not match the expected length of %u bytes.\n", blob_len, d_DATA_LEN);
	}


	// Message is BLOB
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg);

	// instead of performing the second step of the interleaving where we
	// would mainly copy data arround to get it back into the order it came
	// in, we copy the input data to the output buffer and just perform the 
	// second interleaving step for the parity bits to get them into the
	// right order.

	// start interleaving (1st step)
	for(unsigned int i=0;i<d_DATA_LEN;i++) {
		d_buf_data[i] = data_in[ ccsds_interl_indx(i, d_I, d_DATA_LEN) ];
	}

	// encode
	for(unsigned int i=0;i<d_I;i++) {
		encode_rs_ccsds(&d_buf_data[i*d_k], &d_buf_parity[i*d_2E], 0); // use no padding
	}

	// perform interleaving (2nd step) only for parity
	for(unsigned int i=0;i<d_PARITY_LEN;i++) {
		d_buf_parity2[ccsds_interl_indx(i, d_I, d_PARITY_LEN)] = d_buf_parity[i];
	}

	// copy blocks
	memcpy(d_buf_out, data_in, d_DATA_LEN);
	memcpy(&d_buf_out[d_DATA_LEN], d_buf_parity2, d_PARITY_LEN);

	// create output message
	pmt::pmt_t msg_out_data = pmt::pmt_make_blob(d_buf_out, d_OUT_LEN);

	// Construct the new message using the received header
	pmt::pmt_t msg_out = pmt::pmt_cons(hdr, msg_out_data);

	// send generated codeblock
	message_port_pub( pmt::mp("out"), msg_out );
}

