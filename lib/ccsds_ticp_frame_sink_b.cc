#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_ticp_frame_sink_b.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>


ccsds_ticp_frame_sink_b_sptr ccsds_make_ticp_frame_sink_b(unsigned int port, const unsigned int frame_length, gr_msg_queue_sptr msgq) {
    return ccsds_ticp_frame_sink_b_sptr (new ccsds_ticp_frame_sink_b(port, frame_length, msgq) );
}

ccsds_ticp_frame_sink_b::ccsds_ticp_frame_sink_b (unsigned int port, const unsigned int frame_length, gr_msg_queue_sptr msgq)
  : gr_block ("ccsds_ticp_frame_sink_b",
	gr_make_io_signature (1, 1, sizeof(unsigned char)),
	gr_make_io_signature (0, 0, 0)), TicpServer(), d_FRAME_LEN(frame_length)
{

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		dbg_file = fopen("/tmp/ccsds_ticp_frame_sink_debug.dat","w");
		dbg_count = 0;
	#endif

	// store queue
	d_msgq = msgq;

	// Add getFrame method	
	ticpConnectGetFrameFunction(boost::bind(&ccsds_ticp_frame_sink_b::getFrame, this));

	// Open connection
	ticpStartServer(port, false);

}

ccsds_ticp_frame_sink_b::~ccsds_ticp_frame_sink_b () {
	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		fflush(dbg_file);
		fclose(dbg_file);
	#endif
}

const std::vector< unsigned char > ccsds_ticp_frame_sink_b::getFrame(void) {
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

int  ccsds_ticp_frame_sink_b::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	// Nothing to to, this block works completley asynchronous. Just consume
	// the input to make GNURadio happy
	consume_each(ninput_items[0]);

	return 0;
}
