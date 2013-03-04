#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_blob_msg_sink_b.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>
#include <cstdio>


ccsds_blob_msg_sink_b_sptr ccsds_make_blob_msg_sink_b(const unsigned int blob_len) {
    return ccsds_blob_msg_sink_b_sptr (new ccsds_blob_msg_sink_b(blob_len) );
}

ccsds_blob_msg_sink_b::ccsds_blob_msg_sink_b (const unsigned int blob_len)
  : gr_sync_block ("ccsds_blob_msg_sink_b",
	gr_make_io_signature (1, 1, sizeof(unsigned char)),
	gr_make_io_signature (0, 0, sizeof(unsigned char))), d_BLOB_LEN(blob_len)
{
	// register input type
	message_port_register_out(pmt::mp("out"));

	d_buffer = new unsigned char[d_BLOB_LEN];
	d_buffer_count = 0;
}

ccsds_blob_msg_sink_b::~ccsds_blob_msg_sink_b () {
	delete[] d_buffer;
}

bool ccsds_blob_msg_sink_b::stop(void) {
	// Signal EOF
	message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
	return true;
}

int ccsds_blob_msg_sink_b::work (int                 noutput_items,
                                gr_vector_const_void_star&   input_items,
                                gr_vector_void_star&         /*output_items*/)
{
	const unsigned char *in = (const unsigned char *) input_items[0];

	unsigned int to_process = noutput_items;

	while(to_process > 0) {

		// fill buffer

		// how many bytes to copy before either the buffer is full, or
		// we processed all input samples?
		const unsigned int num_copy = std::min(to_process, d_BLOB_LEN-d_buffer_count);

		// index of the input stream from where to start copying.
		const unsigned int num_processed = noutput_items - to_process;

		// fill buffer
		memcpy(&d_buffer[d_buffer_count], &in[num_processed], num_copy);

		// updated counters
		d_buffer_count += num_copy;
		to_process -= num_copy;


		// empty buffer, if full
		if(d_buffer_count >= d_BLOB_LEN) {
			// buffer is full, empty it
			
			// create message
			pmt::pmt_t msg_out = pmt::pmt_make_blob(d_buffer, d_BLOB_LEN);

			// send message out
			message_port_pub( pmt::mp("out"), msg_out );

			// reset counter
			d_buffer_count = 0;
		}
	}
	
	// We are a sink, so no data generated, but we are a sync block, so tell
	// runtime how many inputs we consumed.
	return noutput_items;
}

