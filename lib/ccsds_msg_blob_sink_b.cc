#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_msg_blob_sink_b.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>
#include <cstdio>


ccsds_msg_blob_sink_b_sptr ccsds_make_msg_blob_sink_b(const unsigned int blob_len) {
    return ccsds_msg_blob_sink_b_sptr (new ccsds_msg_blob_sink_b(blob_len) );
}

ccsds_msg_blob_sink_b::ccsds_msg_blob_sink_b (const unsigned int blob_len)
  : gr_block ("ccsds_msg_blob_sink_b",
	gr_make_io_signature (1, 1, sizeof(unsigned char)),
	gr_make_io_signature (0, 0, sizeof(unsigned char))), d_BLOB_LEN(blob_len)
{
	// register input type
	message_port_register_out(pmt::mp("out"));
}

ccsds_msg_blob_sink_b::~ccsds_msg_blob_sink_b () {
	// nothing to do
}

bool ccsds_msg_blob_sink_b::stop(void) {
	// Signal EOF
	message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
}

int ccsds_msg_blob_sink_b::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const unsigned char *in = (const unsigned char *) input_items[0];

	// how many BLOBs can we process
	const unsigned int num_blobs = ninput_items[0]/d_BLOB_LEN;

	// is there soemthing to do?
	if(num_blobs == 0) {
		return 0;
	}

	// process all blobs
	for(unsigned int i=0;i<num_blobs;i++) {
		// create message
		pmt::pmt_t msg_out = pmt::pmt_make_blob(&in[i*d_BLOB_LEN], d_BLOB_LEN);

		// send message out
		message_port_pub( pmt::mp("out"), msg_out );
	}

	// Tell runtime how many samples we used.
	consume_each(num_blobs*d_BLOB_LEN);

	// We are a sink, so no data generated
	return 0;
}

