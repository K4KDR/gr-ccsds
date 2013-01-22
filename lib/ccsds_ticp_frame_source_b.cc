#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_ticp_frame_source_b.h>
#include <gr_io_signature.h>



ccsds_ticp_frame_source_b_sptr ccsds_make_ticp_frame_source_b(std::string hostname, unsigned int port, const unsigned int frame_length) {
    return ccsds_ticp_frame_source_b_sptr (new ccsds_ticp_frame_source_b(hostname, port, frame_length) );
}

ccsds_ticp_frame_source_b::ccsds_ticp_frame_source_b (std::string hostname, unsigned int port, const unsigned int frame_length)
  : gr_block ("ccsds_ticp_frame_source_b",
	gr_make_io_signature (0, 0, 0),
	gr_make_io_signature (1, 1, sizeof (unsigned char))), TicpClient(), d_FRAME_LEN(frame_length), d_BUFFER_SIZE(4)
{

	#if TICP_FRAME_SOURCE_DEBUG == 1
		dbg_file = fopen("/tmp/ccsds_ticp_frame_source_debug.dat","w");
		dbg_count = 0;
	#endif

	#ifdef PROFILE_NUM_SAMPS
		profile_count = 0;
	#endif

	// Open connection
	ticpConnectBlocking(hostname, (unsigned short) port);

	// We will output frames
	set_output_multiple(d_FRAME_LEN);

	const unsigned int buf_len = d_BUFFER_SIZE*d_FRAME_LEN;
	d_frame_buffer = new unsigned char[buf_len];
	d_frames_buffered = 0;

	d_stop = false;

	d_ticp_add_function = boost::bind(&ccsds_ticp_frame_source_b::addFrame, this, _1);
}

ccsds_ticp_frame_source_b::~ccsds_ticp_frame_source_b () {
	delete[] d_frame_buffer;
	#if TICP_FRAME_SOURCE_DEBUG == 1
		fflush(dbg_file);
		fclose(dbg_file);
	#endif
}

void ccsds_ticp_frame_source_b::forecast(int noutput_items,gr_vector_int &ninput_items_required){
	// We are a source, so we don't need input samples to generate output
	return;
}

bool ccsds_ticp_frame_source_b::stop( void ) {
//	printf("try to get lock to stop()\n");
	d_mutex.lock();
	d_stop = true;
	d_mutex.unlock();
//	printf("stop() finished, release lock()");
}

void ccsds_ticp_frame_source_b::addFrame(const std::vector< unsigned char > &frame) {
	/*	
	printf("Received Frame: ");
	for(unsigned int i=0;i<frame.size();i++) {
		printf("%2X ",frame[i]);
	}
	printf("\n");
	//*/

	#if TICP_FRAME_SOURCE_DEBUG == 1
		fprintf(dbg_file,"%3u  ",dbg_count);
		for(unsigned int i=0u;i<frame.size();i++) {
			fprintf(dbg_file,"%2X ",frame[i]);
		}
		fprintf(dbg_file,"\n");
		dbg_count++;
	#endif

//	printf("addFrame() waiting for lock\n");
	// Make sure the Gnuradio loop is not using the buffer right now
	d_mutex.lock();

	// Is the block stil running?
	if(d_stop) {
		d_mutex.unlock();
		return;
	}

	// Check that frames have the same size in both systems
	if(frame.size() != d_FRAME_LEN) {
		fprintf(stderr,"ERROR TICP FRAME SOURCE: frame lengths do not match TICP:%lu, Internal:%u\n",frame.size(),d_FRAME_LEN);
		d_mutex.unlock();
		exit(EXIT_FAILURE);
		return;
	}

	// Check that there is still space in the buffer
	if(d_frames_buffered >= d_BUFFER_SIZE) {
		fprintf(stderr,"ERROR TICP FRAME SOURCE: No space left in buffer\n");
		d_mutex.unlock();
		exit(EXIT_FAILURE);
		return;
	}

	// Write Frame to buffer
	const unsigned int ofst = d_frames_buffered*d_FRAME_LEN;
	for(unsigned int i=0;i<d_FRAME_LEN;i++) {
		d_frame_buffer[ofst+i] = frame[i];
	}

	// Update counters
	d_frames_buffered++;
	d_frames_requested--;

	// Unlock again
	d_mutex.unlock();

//	printf("addFrame() finished, release lock.\n");

	return;
}

int  ccsds_ticp_frame_source_b::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	// Output stream
	unsigned char *out = (unsigned char *) output_items[0];
	
	// Number of frames that have been put to the output stream
	unsigned int num_out = 0;

	// Number of frames that fit into the allocated memory of the output
	// stream
	const unsigned int num_allocated = std::floor(noutput_items / d_FRAME_LEN);


	while(num_out == 0) {
//		printf("TICP Source general_work() waiting for lock\n");
		d_mutex.lock();
//		printf("TICP Source general_work() got lock. %u frames buffered, %u frames requested\n",d_frames_buffered,d_frames_requested);
		// We got the lock

		// How many frames can we output right now?
		num_out = std::min(d_frames_buffered, num_allocated);

		// Do we need to request new data?
		while(d_frames_buffered+d_frames_requested < d_BUFFER_SIZE && !d_stop) {
//			printf("Requesting new frame\n");
			ticpGetFrame(d_ticp_add_function);
			d_frames_requested++;
		}

		// is there a new frame to output?
		if(num_out == 0u) {
//			printf("TICP Source general_work() has an empty buffer, sleeping (releasing lock)\n");
			d_mutex.unlock();

			// If we have to try again, wait a bit to give the addFrame
			// threads a chance to lock in first.
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));

			continue;
		}

		//
		// there is something to output
		//

		// copy from buffer
		memcpy(out, d_frame_buffer, d_FRAME_LEN*num_out*sizeof(unsigned char));

		// How many framese are left in the buffer?
		d_frames_buffered = std::max((int)d_frames_buffered-(int)num_out, 0);
		
		// Are there elements left in the buffer?
		if(d_frames_buffered > 0u) {
			// Move buffered frames to the start of the buffer.
			memmove(&d_frame_buffer[0], &d_frame_buffer[num_out], d_FRAME_LEN*d_frames_buffered*sizeof(unsigned char));
		}

		// At this point there are d_frames_buffered frames at the start
		// of d_frame_buffer.

		// We are done, get out here
//		printf("TICP Source general_work() done, release lock\n");
		d_mutex.unlock();

		
	}
	
	#ifdef PROFILE_NUM_SAMPS
		profile_count += num_out*d_FRAME_LEN;
		if(profile_count >= PROFILE_NUM_SAMPS) {
			exit(EXIT_SUCCESS);
			return num_out*d_FRAME_LEN;
		}
		printf("%7lu/%7u bytes processed\n",profile_count,PROFILE_NUM_SAMPS);
	#endif
			

	// Tell runtime system how many output items we produced
	return num_out*d_FRAME_LEN;
}
