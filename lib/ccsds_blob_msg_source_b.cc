#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_blob_msg_source_b.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>
#include <cstdio>


ccsds_blob_msg_source_b_sptr ccsds_make_blob_msg_source_b(const unsigned int blob_len) {
    return ccsds_blob_msg_source_b_sptr (new ccsds_blob_msg_source_b(blob_len) );
}

ccsds_blob_msg_source_b::ccsds_blob_msg_source_b (const unsigned int blob_len)
  : gr_sync_block ("ccsds_blob_msg_source_b",
	gr_make_io_signature (0, 0, sizeof(unsigned char)),
	gr_make_io_signature (1, 1, sizeof(unsigned char))), d_BLOB_LEN(blob_len)
{
	d_stop = false;

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_blob_msg_source_b::process_message, this, _1));
}

ccsds_blob_msg_source_b::~ccsds_blob_msg_source_b () {
	// nothing to do
}

bool ccsds_blob_msg_source_b::start(void) {
	d_astart.start();
	return true;
}

bool ccsds_blob_msg_source_b::stop(void) {
	d_astart.stop();
	return true;
}

void ccsds_blob_msg_source_b::process_message(pmt::pmt_t msg_in) {

	if(d_stop) {
		// Do not accept new messages after EOF
		return;
	}

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		d_stop = true;
		return;
	}

	// check that input is a pair value
	if(!pmt::pmt_is_pair(msg_in)) {
		fprintf(stderr,"WARNING MSG BLOB SOURCE: expecting message of type pair, skipping.\n");
		return;
	}

	const pmt::pmt_t hdr = pmt::pmt_car(msg_in);
	const pmt::pmt_t msg = pmt::pmt_cdr(msg_in);

	// check that input header is a dictionary
	if(!pmt::pmt_is_dict(hdr)) {
		fprintf(stderr,"WARNING MSG BLOB SOURCE: expecting message header of type dict, skipping.\n");
		return;
	}

	// check that input data is a float vector
	if(!pmt::pmt_is_blob(msg)) {
		fprintf(stderr,"WARNING MSG BLOB SOURCE: expecting message data of type blob, skipping.\n");
		return;
	}

	// check if message has right length
	if(pmt::pmt_length(msg) != d_BLOB_LEN) {
		fprintf(stderr,"WARNING MSG BLOB SOURCE: message of length %lu does not match the expected length of %u, skipping.\n",pmt::pmt_length(msg), d_BLOB_LEN);
		return;
	}

	// Message is BLOB and has the right length

	// pointer to frame
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg);

	for(unsigned int i=0;i<d_BLOB_LEN;i++) {
		d_queue.push(data_in[i]);
	}

	return;
}

int ccsds_blob_msg_source_b::work (int		               noutput_items,
				gr_vector_const_void_star&     /*input_items*/,
                                gr_vector_void_star&           output_items)
{

	if(d_stop && d_queue.size() == 0) {
		// signal EOF
		return -1;
	}

	unsigned char *out = (unsigned char *) output_items[0];

	const unsigned int num_out = std::min(noutput_items,(int)d_queue.size());

	for(unsigned int i=0;i<num_out;i++) {
		// Pull element from queue
		out[i] = d_queue.front();

		// Delete element from queue
		d_queue.pop();
	}

	return num_out;
}

