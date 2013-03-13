#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_ticp_frame_sink.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>


ccsds_ticp_frame_sink_sptr ccsds_make_ticp_frame_sink(unsigned int port, const unsigned int frame_length) {
    return ccsds_ticp_frame_sink_sptr (new ccsds_ticp_frame_sink(port, frame_length) );
}

ccsds_ticp_frame_sink::ccsds_ticp_frame_sink (unsigned int port, const unsigned int frame_length)
  : gr_block ("ccsds_ticp_frame_sink",
	gr_make_io_signature (0, 0, sizeof(unsigned char)),
	gr_make_io_signature (0, 0, 0)), TicpServer(), d_FRAME_LEN(frame_length)
{

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		dbg_file = fopen("/tmp/ccsds_ticp_frame_sink_debug.dat","w");
		dbg_count = 0;
	#endif

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_ticp_frame_sink::process_message, this, _1));

	// store queue
	d_msgq = gr_msg_queue_sptr(new gr_msg_queue(5));

	// Add getFrame method	
	ticpConnectGetFrameFunction(boost::bind(&ccsds_ticp_frame_sink::getFrame, this));

	// Open connection
	ticpStartServer(port, false);
}

ccsds_ticp_frame_sink::~ccsds_ticp_frame_sink () {
	d_msgq.reset();

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		fflush(dbg_file);
		fclose(dbg_file);
	#endif
}

const std::vector< unsigned char > ccsds_ticp_frame_sink::getFrame(void) {
	gr_message_sptr msg = d_msgq->delete_head(); // blocking

	if(msg->length() != d_FRAME_LEN) {
		fprintf(stderr,"ERROR TICP frame sink: frame lengths do not match. msg:%lu, internal:%u\n",msg->length(),d_FRAME_LEN);
		exit(EXIT_FAILURE);
		return std::vector< unsigned char >();
	}

	unsigned char *msg_data = msg->msg();
	std::vector< unsigned char > frame(d_FRAME_LEN, 0u);

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		fprintf(dbg_file,"%3u  ",dbg_count);
	#endif

	for(unsigned int i=0u;i<d_FRAME_LEN;i++) {
		frame[i] = msg_data[i];
		
		#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
			fprintf(dbg_file,"%2X ",frame[i]);
		#endif
	}

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		fprintf(dbg_file,"\n");
		dbg_count++;
	#endif

	return frame;
}

void ccsds_ticp_frame_sink::process_message(pmt::pmt_t msg_in) {
	
	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		return;
	}
	
	// check if message has right format	
	if(!pmt::pmt_is_blob(msg_in)) {
		fprintf(stderr,"ERROR TICP FRAME SINK: expecting message of type blob, skipping.\n");
		return;
	}

	// Message is BLOB

	// pointer to frame
	const unsigned char *data_in = (const unsigned char *) pmt::pmt_blob_data(msg_in);

	// message for the buffer
	gr_message_sptr msg_buf = gr_make_message(0, 0, 0, d_FRAME_LEN);

	// fill message
	memcpy(msg_buf->msg(), data_in, d_FRAME_LEN*sizeof(unsigned char));

	// enqueue
	d_msgq->insert_tail(msg_buf); // blocking

	return;
}


