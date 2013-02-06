#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_msg_blob_source_b.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>
#include <cstdio>


ccsds_msg_blob_source_b_sptr ccsds_make_msg_blob_source_b(const unsigned int blob_len) {
    return ccsds_msg_blob_source_b_sptr (new ccsds_msg_blob_source_b(blob_len) );
}

ccsds_msg_blob_source_b::ccsds_msg_blob_source_b (const unsigned int blob_len)
  : gr_block ("ccsds_msg_blob_source_b",
	gr_make_io_signature (0, 0, sizeof(unsigned char)),
	gr_make_io_signature (1, 1, sizeof(unsigned char))), d_BLOB_LEN(blob_len)
{
	// only process BLOBs as a whole
	set_output_multiple(d_BLOB_LEN);

	d_stop = -1; // No EOF received so far

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_msg_blob_source_b::process_message, this, _1));
}

ccsds_msg_blob_source_b::~ccsds_msg_blob_source_b () {
	// release all memory from the queue
	for(unsigned int i=0;i<d_queue.size();i++) {
		unsigned char *ptr = d_queue.front();
		d_queue.pop();
		delete[] ptr;
	}

}

bool ccsds_msg_blob_source_b::start(void) {
	d_astart.start();
}

bool ccsds_msg_blob_source_b::stop(void) {
	d_astart.stop();
}

void ccsds_msg_blob_source_b::process_message(pmt::pmt_t msg_in) {

	if(d_stop == 0) {
		return;
	}

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		d_stop = d_queue.size();
		return;
	}

	// check if message has right format	
	if(!pmt::pmt_is_blob(msg_in)) {
		fprintf(stderr,"ERROR MSG BLOB SOURCE: expecting message of type blob, skipping.\n");
		return;
	}

	// Message is BLOB

	// pointer to frame
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg_in);

	// message for the buffer
	unsigned char *tmp = new unsigned char[d_BLOB_LEN];

	// fill buffer
	memcpy(tmp, data_in, d_BLOB_LEN*sizeof(unsigned char));

	// enqueue
	d_queue.push(tmp);

	return;
}

int ccsds_msg_blob_source_b::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{

	if(d_stop == 0) {
		// signal EOF
		return -1;
	}

	unsigned char *out = (unsigned char *) output_items[0];

	// how many BLOBs can we process
	const unsigned int num_blobs = std::min(noutput_items/d_BLOB_LEN, (unsigned int)d_queue.size());

	// is there soemthing to do?
	if(num_blobs == 0) {
		return 0;
	}

	for(unsigned int i=0;i<num_blobs;i++) {
		unsigned char *data_in = d_queue.front();

		memcpy(&out[i*d_BLOB_LEN],data_in,d_BLOB_LEN*sizeof(unsigned char));

		d_queue.pop();
		delete[] data_in;

		// Did we receive an EOF message
		if(d_stop > 0) {
			// Yes, just finish the messages in the queue
			d_stop--;

			// Was it the last message?
			if(d_stop == 0) {
				// Yes, break out of loop to return the
				// generated stream. On next re-entry into work
				// EOF will be signalled
				break;
			}
		}
	}

	// Tell runtime system how many output items we produced
	return num_blobs*d_BLOB_LEN;
}

