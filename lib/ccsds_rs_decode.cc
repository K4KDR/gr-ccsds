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

	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_DEBUG
		dbg_file_in 	   = fopen("/tmp/ccsds_rs_decode_debug_in.dat","w");
		dbg_file_in_valid  = fopen("/tmp/ccsds_rs_decode_debug_in_valid.dat","w");
		dbg_file_out	   = fopen("/tmp/ccsds_rs_decode_debug_out.dat","w");
	#endif
}

ccsds_rs_decode::~ccsds_rs_decode () {
	delete[] d_buf_in;
	delete[] d_buf_data;
	delete[] d_buf_out;

	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_DEBUG
		fflush(dbg_file_in);
		fflush(dbg_file_in_valid);
		fflush(dbg_file_out);

		fclose(dbg_file_in);
		fclose(dbg_file_in_valid);
		fclose(dbg_file_out);
	#endif
}

void ccsds_rs_decode::process_frame(pmt::pmt_t msg_in) {

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

	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_FRAMEERR
		const unsigned long frame_number = pmt::pmt_to_long(pmt::pmt_dict_ref(hdr, pmt::mp("frame_number"), pmt::pmt_from_long(0)));
	#endif

	const unsigned int blob_len = (unsigned int) pmt::pmt_blob_length(msg);

	if(blob_len < d_IN_LEN) {
		fprintf(stderr,"ERROR RS DECODE: blob message length of %u bytes is smaller than the expected length of %u bytes.\n", blob_len, d_IN_LEN);
		return;
	} else if(blob_len != d_IN_LEN) {
		fprintf(stderr,"WARNING RS DECODE: blob message length of %u bytes does not match the expected length of %u bytes.\n", blob_len, d_IN_LEN);
	}

	// Message is BLOB
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg);

	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_DEBUG
		// Write input to debug file
		for(unsigned int i=0;i<blob_len;i++) {
			fprintf(dbg_file_in, "%02X ",data_in[i]);
		}
		fprintf(dbg_file_in, "\n");
	#endif

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
			#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_FRAMEERR
				printf("Invalid RS codeblock, dropping frame number %lu\n",frame_number);
			#endif
			return;
		} else {
			#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_FRAMEINFO
				if(ret > 0) {
					printf("Corrected %d symbols in frame number %lu\n",ret, frame_number);
				}
			#endif

			// copy to data buffer (without parity) for an easier
			// 2nd interleaving step
			memcpy(&d_buf_data[i*d_k], &d_buf_in[i*(d_k+d_2E)], d_k);
		}
	}

	// perform interleaving (2nd step)
	for(unsigned int i=0;i<d_DATA_LEN;i++) {
		d_buf_out[ccsds_interl_indx(i, d_I, d_DATA_LEN)] = d_buf_data[i];
	}

	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_DEBUG
		// Write input to debug file (at this point the frame is valid)
		for(unsigned int i=0;i<blob_len;i++) {
			fprintf(dbg_file_in_valid, "%02X ",data_in[i]);
		}
		fprintf(dbg_file_in_valid, "\n");

		// Write output to debug file
		for(unsigned int i=0;i<d_DATA_LEN;i++) {
			fprintf(dbg_file_out, "%02X ",d_buf_out[i]);
		}
		fprintf(dbg_file_out, "\n");
	#endif

	// create output message data
	pmt::pmt_t msg_out_data = pmt::pmt_make_blob(d_buf_out, d_DATA_LEN);

	// Construct the new message using the received header
	pmt::pmt_t msg_out = pmt::pmt_cons(hdr, msg_out_data);

	// send generated codeblock
	message_port_pub( pmt::mp("out"), msg_out );
}

