#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_rs_decode.h>
#include <ccsds_interleaving_aux.h>
#include <gr_io_signature.h>
#include <cstdio>


ccsds_rs_decode_sptr ccsds_make_rs_decode(const unsigned int I) {
    return ccsds_rs_decode_sptr (new ccsds_rs_decode(I) );
}

ccsds_rs_decode::ccsds_rs_decode(const unsigned int I)
  : gr_block ("ccsds_rs_decode",
	gr_make_io_signature (0, 0, 0),
	gr_make_io_signature (0, 0, 0)), d_I(I), d_k(223), d_n(255), d_2E(32), d_DATA_LEN(d_k*d_I), d_IN_LEN(d_DATA_LEN+d_I*d_2E)
{
	// create buffers
	d_buf_in   = new unsigned char[ d_IN_LEN ];
	d_buf_data = new unsigned char[ d_DATA_LEN ];
	d_buf_out  = new unsigned char[ d_DATA_LEN ];

	// register output port
	message_port_register_out(pmt::mp("out"));

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_rs_decode::process_frame, this, _1));
}

ccsds_rs_decode::~ccsds_rs_decode () {
	delete[] d_buf_in;
	delete[] d_buf_data;
	delete[] d_buf_out;
}

void ccsds_rs_decode::process_frame(pmt::pmt_t msg_in) {

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
		return;
	}

	if(!pmt::pmt_is_blob(msg_in)) {
		fprintf(stderr,"ERROR RS DECODE: expecting message of type blob, skipping.\n");
		return;
	}

	const unsigned int blob_len = (unsigned int) pmt::pmt_blob_length(msg_in);

	if(blob_len < d_IN_LEN) {
		fprintf(stderr,"ERROR RS DECODE: blob message length of %u bytes is smaller than the expected length of %u bytes.\n", blob_len, d_IN_LEN);
		return;
	} else if(blob_len != d_IN_LEN) {
		fprintf(stderr,"WARNING RS DECODE: blob message length of %u bytes does not match the expected length of %u bytes.\n", blob_len, d_IN_LEN);
	}

	// Message is BLOB
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg_in);

	// start interleaving (1st step)
	for(unsigned int i=0;i<d_IN_LEN;i++) {
		d_buf_in[i] = data_in[ccsds_interl_indx(i, d_I, d_IN_LEN)];
	}

	// decode
	for(unsigned int i=0;i<d_I;i++) {
		// no hints about erasures, use no padding
		int ret = decode_rs_ccsds(&d_buf_in[i*(d_k+d_2E)], NULL, 0, 0);
		if(ret < 0) {
			// unable to decode, drop this codeblock
			return;
		} else {
			// copy to data buffer (without parity) for an easier
			// 2nd interleaving step
			memcpy(&d_buf_data[i*d_k], &d_buf_in[i*(d_k+d_2E)], d_k);
		}
	}

	// perform interleaving (2nd step)
	for(unsigned int i=0;i<d_DATA_LEN;i++) {
		d_buf_out[ccsds_interl_indx(i, d_I, d_DATA_LEN)] = d_buf_data[i];
	}

	// create output message
	pmt::pmt_t msg_out = pmt::pmt_make_blob(d_buf_out, d_DATA_LEN);

	// send generated codeblock
	message_port_pub( pmt::mp("out"), msg_out );
	//printf("RS decoder: posting message\n");
}

