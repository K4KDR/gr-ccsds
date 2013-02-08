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

	d_buffer = new unsigned char[d_BLOB_LEN];
	d_buffer_count = d_BLOB_LEN;

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_blob_msg_source_b::process_message, this, _1));
}

ccsds_blob_msg_source_b::~ccsds_blob_msg_source_b () {
	// release all memory from the queue
	for(unsigned int i=0;i<d_queue.size();i++) {
		unsigned char *ptr = d_queue.front();
		d_queue.pop();
		delete[] ptr;
	}

	delete[] d_buffer;
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
		return;
	}

	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		d_stop = true;
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

int ccsds_blob_msg_source_b::work (int               noutput_items,
                                gr_vector_const_void_star&   /*input_items*/,
                                gr_vector_void_star&         output_items)
{

	if(d_stop && d_queue.size() == 0 && d_buffer_count >= d_BLOB_LEN) {
		// signal EOF
		return -1;
	}

	unsigned char *out = (unsigned char *) output_items[0];

	unsigned int to_process = noutput_items;

	while(to_process > 0) {

		// empty buffer
		if(d_buffer_count < d_BLOB_LEN) {
			const unsigned int num_copy = std::min(d_BLOB_LEN-d_buffer_count, to_process);

			const unsigned int num_processed = noutput_items - to_process;

			// copy from buffer
			memcpy(&out[num_processed],&d_buffer[d_buffer_count], num_copy);
		
			// update counter
			d_buffer_count += num_copy;
			to_process -= num_copy;
		}

		// fill buffer if needed
		if(d_buffer_count >= d_BLOB_LEN) {
			// is there another message in the buffer?
			if(d_queue.size() == 0) {
				// queue is empty, return
				break;
			} else {
				// there is still a BLOB in the queue

				// fill buffer
				memcpy(d_buffer,d_queue.front(),d_BLOB_LEN);

				// free BLOB from memory
				delete[] d_queue.front();

				// remove BLOB pointer from queue
				d_queue.pop();

				// update counter
				d_buffer_count = 0;

			} // END queue is not empty
		} // END buffer needs to be filled
	} // END there are still samples to be processed

	// Tell runtime system how many output items we produced
	return noutput_items-to_process;
}

