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
	gr_make_io_signature (1, 1, sizeof (unsigned char))), TicpClient(), d_FRAME_LEN(frame_length)
{

	#if TICP_FRAME_SOURCE_B_DEBUG == 1
		dbg_file = fopen("/tmp/ccsds_ticp_frame_source_b_debug.dat","w");
		dbg_count = 0;
	#endif

	#ifdef PROFILE_NUM_SAMPS
		profile_count = 0;
	#endif

	// Open connection
	ticpConnectBlocking(hostname, (unsigned short) port);

	// We will output frames
	set_output_multiple(d_FRAME_LEN);

	d_stop = false;
}

ccsds_ticp_frame_source_b::~ccsds_ticp_frame_source_b () {
	#if TICP_FRAME_SOURCE_B_DEBUG == 1
		fflush(dbg_file);
		fclose(dbg_file);
	#endif
}

void ccsds_ticp_frame_source_b::forecast(int noutput_items,gr_vector_int &ninput_items_required){
	// We are a source, so we don't need input samples to generate output
	return;
}

bool ccsds_ticp_frame_source_b::stop( void ) {
	d_stop = true;
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


	while(num_out < num_allocated && !d_stop) {

		// buffer for the frame
		vector< unsigned char > frame;

		// get new Frame
		ticpGetFrameBlocking(frame); //blocking

		// Check that frames have the same size in both systems
		if(frame.size() != d_FRAME_LEN) {
			fprintf(stderr,"ERROR TICP FRAME SOURCE: frame lengths do not match TICP:%lu, Internal:%u\n",frame.size(),d_FRAME_LEN);
			exit(EXIT_FAILURE);
			return 0;
		}

		#if TICP_FRAME_SOURCE_B_DEBUG == 1
			fprintf(dbg_file,"%3u  ",dbg_count);
		#endif

		const unsigned int ofst = num_out*d_FRAME_LEN;

		// copy frame to output
		for(unsigned int i=0u;i<d_FRAME_LEN;i++) {

			out[ofst+i] = frame[i];

			#if TICP_FRAME_SOURCE_B_DEBUG == 1
				fprintf(dbg_file,"%2X ",frame[i]);
			#endif
		}

		#if TICP_FRAME_SOURCE_B_DEBUG == 1
			fprintf(dbg_file,"\n");
			dbg_count++;
		#endif

		num_out++;
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
