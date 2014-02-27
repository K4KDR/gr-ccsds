#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_trunk_tail.h>
#include <gr_io_signature.h>


ccsds_trunk_tail_sptr ccsds_make_trunk_tail(const unsigned int trunk_len, const unsigned int frame_len) {
    return ccsds_trunk_tail_sptr (new ccsds_trunk_tail(trunk_len, frame_len) );
}

ccsds_trunk_tail::ccsds_trunk_tail(const unsigned int trunk_len, const unsigned int frame_len)
  : gr_sync_block ("ccsds_trunk_tail",
	gr_make_io_signature (0, 0, 0),
	gr_make_io_signature (0, 0, 0)), d_TRUNK_LEN(trunk_len), d_FRAME_LEN(frame_len)
{

	d_stop = false;

	// register output port
	message_port_register_out(pmt::mp("out"));

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_trunk_tail::process_frame, this, _1));
}

ccsds_trunk_tail::~ccsds_trunk_tail () {
	return;
}



void ccsds_trunk_tail::process_frame(pmt::pmt_t msg_in) {

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
		d_stop = true;

		return;
	}

	// check that input is a pair value
	if(!pmt::pmt_is_pair(msg_in)) {
		fprintf(stderr,"WARNING TRUNK TAIL: expecting message of type pair, skipping.\n");
		return;
	}

	const pmt::pmt_t hdr = pmt::pmt_car(msg_in);
	const pmt::pmt_t msg = pmt::pmt_cdr(msg_in);

	// check that input header is a dictionary
	if(!pmt::pmt_is_dict(hdr)) {
		fprintf(stderr,"WARNING TRUNK TAIL: expecting message header of type dict, skipping.\n");
		return;
	}

	// check that input data is a blob
	if(!pmt::pmt_is_blob(msg)) {
		fprintf(stderr,"WARNING TRUNK TAIL: expecting message data of type blob, skipping.\n");
		return;
	}

	// TODO Check frame length
	
	// Message is BLOB
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg);

	pmt::pmt_t msg_out_data = pmt::pmt_make_blob(data_in, d_FRAME_LEN-d_TRUNK_LEN);

	// Construct the new message using the received header
	pmt::pmt_t msg_out = pmt::pmt_cons(hdr, msg_out_data);

	message_port_pub( pmt::mp("out"), msg_out );

}

int ccsds_trunk_tail::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
	return (d_stop ? -1 : 0);
}
