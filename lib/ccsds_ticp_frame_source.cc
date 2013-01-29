#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_ticp_frame_source.h>
#include <gr_io_signature.h>



ccsds_ticp_frame_source_sptr ccsds_make_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length) {
    return ccsds_ticp_frame_source_sptr (new ccsds_ticp_frame_source(hostname, port, frame_length) );
}

ccsds_ticp_frame_source::ccsds_ticp_frame_source (std::string hostname, unsigned int port, const unsigned int frame_length)
  : gr_block ("ccsds_ticp_frame_source",
	gr_make_io_signature (0, 0, 0),
	gr_make_io_signature (0, 0, 0)), TicpClient(), d_FRAME_LEN(frame_length)
{
	printf("Constructor\n");

	// register output port
	message_port_register_out(pmt::mp("out"));

	#if TICP_FRAME_SOURCE_DEBUG == 1
		dbg_file = fopen("/tmp/ccsds_ticp_frame_source_debug.dat","w");
		dbg_count = 0;
	#endif

	#ifdef PROFILE_NUM_SAMPS
		profile_count = 0;
	#endif

	// Open connection
	ticpConnectBlocking(hostname, (unsigned short) port);

	d_stop = false;

	boost::thread t(boost::bind(&ccsds_ticp_frame_source::asynchronous_work, this));
}

ccsds_ticp_frame_source::~ccsds_ticp_frame_source () {
	#if TICP_FRAME_SOURCE_DEBUG == 1
		fflush(dbg_file);
		fclose(dbg_file);
	#endif
}

bool ccsds_ticp_frame_source::start(void) {
	printf("start()\n");
}

bool ccsds_ticp_frame_source::stop( void ) {
	d_mutex.lock();
	d_stop = true;
	printf("stop\n");
	d_mutex.unlock();
}

void ccsds_ticp_frame_source::asynchronous_work(void)
{
	printf("asynchronous_work()\n");
	bool stop;

	do {

		// buffer for the frame
		vector< unsigned char > frame;

		// get new Frame
		ticpGetFrameBlocking(frame); //blocking

		// Check that frames have the same size in both systems
		if(frame.size() != d_FRAME_LEN) {
			fprintf(stderr,"ERROR TICP FRAME SOURCE: frame lengths do not match TICP:%lu, Internal:%u\n",frame.size(),d_FRAME_LEN);
			exit(EXIT_FAILURE);
			return;
		}

		// buffer for message data
		unsigned char *out_data = new unsigned char[d_FRAME_LEN];

		#if TICP_FRAME_SOURCE_DEBUG == 1
			fprintf(dbg_file,"%3u  ",dbg_count);
		#endif

		// copy frame to message buffer
		for(unsigned int i=0;i<d_FRAME_LEN;i++) {
			out_data[i] = frame[i];
			#if TICP_FRAME_SOURCE_DEBUG == 1
				fprintf(dbg_file,"%2X",frame[i]);
			#endif
		}

		#if TICP_FRAME_SOURCE_DEBUG == 1
			fprintf(dbg_file,"\n");
			dbg_count++;
		#endif

		// create message from buffer
		pmt::pmt_t msg_out = pmt::pmt_make_blob(out_data, d_FRAME_LEN);

		// enqueue message
		message_port_pub( pmt::mp("out"), msg_out );

		// free buffer
		delete[] out_data;

		#ifdef PROFILE_NUM_SAMPS
			profile_count += d_FRAME_LEN;
			if(profile_count >= PROFILE_NUM_SAMPS) {
				d_mutex.lock();
				d_stop = true;
				d_mutex.unlock();
			}
			printf("%7lu/%7u bytes processed\n",profile_count,PROFILE_NUM_SAMPS);
		#endif

		d_mutex.lock();
		stop = d_stop;
		d_mutex.unlock();
	} while(!stop);
	
	return;
}
