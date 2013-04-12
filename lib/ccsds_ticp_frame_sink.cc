#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_ticp_frame_sink.h>
#include <gr_io_signature.h>
#include <gr_msg_queue.h>
#include <gr_message.h>


ccsds_ticp_frame_sink_sptr ccsds_make_ticp_frame_sink(unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types) {
    return ccsds_ticp_frame_sink_sptr (new ccsds_ticp_frame_sink(port, frame_length, data_type, map_names, map_types) );
}

ccsds_ticp_frame_sink::ccsds_ticp_frame_sink (unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types)
  : gr_block ("ccsds_ticp_frame_sink",
	gr_make_io_signature (0, 0, sizeof(unsigned char)),
	gr_make_io_signature (0, 0, 0)),
	d_FRAME_LEN(frame_length),
	d_DATA_TYPE(data_type)
{

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		dbg_file = fopen("/tmp/ccsds_ticp_frame_sink_debug.dat","w");
		dbg_count = 0;
	#endif

	// create metadata dictionary
	if(map_names.size() != map_types.size()) {
		fprintf(stderr,"ERROR TICP FRAME SINK: Invalid metadata mapping, number of names and types are not equal.\n");
		exit(EXIT_FAILURE);
		return;
	}
	for(unsigned int i=0;i<map_types.size();i++) {
		if(map_types[i] == d_DATA_TYPE) {
			fprintf(stderr,"ERROR TICP FRAME SINK: Invalid metadata mapping, data type appears in metadata types as well.\n");
			exit(EXIT_FAILURE);
			return;
		}
		d_metadata_map[ map_names[i] ] = map_types[i];
	}
	if(d_metadata_map.size() != map_types.size()) {
		fprintf(stderr,"ERROR TICP FRAME SINK: Invalid metadata mapping, names are not unique\n");
		exit(EXIT_FAILURE);
		return;
	}

	d_frame_count = 0;

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_ticp_frame_sink::process_message, this, _1));

	// create TICP Client
	d_ticp_sptr = boost::shared_ptr< ticp::Server<ticp::data_v2_t> >( new ticp::Server<ticp::data_v2_t>() );
	
	// Add getFrame method	
	d_ticp_sptr->ticpConnectGetFrameFunction(boost::bind(&ccsds_ticp_frame_sink::getFrame, this));

	// Open connection
	d_ticp_sptr->ticpStartServer(port, false);
}

ccsds_ticp_frame_sink::~ccsds_ticp_frame_sink () {
	d_ticp_sptr.reset(); // make sure the ticp Server is deleted as well.

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		fflush(dbg_file);
		fclose(dbg_file);
	#endif
}

const ticp::data_v2_t ccsds_ticp_frame_sink::getFrame(void) {

	// Lock before accessing the queue
	boost::mutex::scoped_lock lock(d_mutex);

	// Wait until there is at least one frame in the queue
	while(d_queue.size() == 0) {
		d_condition_var.wait(lock);
	}

	// okay, there is a frame available now
	ticp::data_v2_t frame = d_queue.front();

	// remove frame
	d_queue.pop();

	// we are done with the queue, release lock early
	lock.unlock();

	return frame;

	/*
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
	*/
}

void ccsds_ticp_frame_sink::process_message(pmt::pmt_t msg_in) {
	
	// check for EOF
	if(pmt::pmt_is_eof_object(msg_in)) {
		return;
	}
	
	// check that input is a pair value
	if(!pmt::pmt_is_pair(msg_in)) {
		fprintf(stderr,"WARNING TICP FRAME SINK: expecting message of type pair, skipping.\n");
		return;
	}

	const pmt::pmt_t hdr = pmt::pmt_car(msg_in);
	const pmt::pmt_t msg = pmt::pmt_cdr(msg_in);

	// check that input header is a dictionary
	if(!pmt::pmt_is_dict(hdr)) {
		fprintf(stderr,"WARNING TICP FRAME SINK: expecting message header of type dict, skipping.\n");
		return;
	}

	// check that input data is a blob
	if(!pmt::pmt_is_blob(msg)) {
		fprintf(stderr,"WARNING TICP FRAME SINK: expecting message data of type blob, skipping.\n");
		return;
	}

	// Message is BLOB

	const unsigned int blob_len = (unsigned int) pmt::pmt_blob_length(msg);

	if(blob_len != d_FRAME_LEN) {
		fprintf(stderr,"WARNING TICP FRAME SINK: blob message length of %u bytes does not match the expected length of %u bytes.\n", blob_len, d_FRAME_LEN);
	}

	// generate frame
	ticp::data_v2_t frame;

	// extract data
	ticp::data_v1_t data((unsigned char *)pmt::pmt_blob_data(msg), (unsigned char *)pmt::pmt_blob_data(msg)+d_FRAME_LEN);

	// insert main data
	frame[d_DATA_TYPE] = data;

	// process metadata

	// get metadata keys
	pmt::pmt_t hdr_keys = pmt::pmt_dict_keys(hdr);

	// number of available metadata elements
	const unsigned int hdr_len = pmt::pmt_length(hdr_keys); // evaluate length once, instead of every loop iteration

	for(unsigned int i=0;i<hdr_len;i++) {
		const pmt::pmt_t key = pmt::pmt_nth(i, hdr_keys);
		const pmt::pmt_t value = pmt::pmt_dict_ref(hdr, key, pmt::PMT_NIL);

		std::string name = pmt::pmt_symbol_to_string(key);

		if(!pmt::pmt_is_blob(value)) {
			fprintf(stderr,"WARNING TICP FRAME SINK: expecting metadata with key %s to be of type blob, skipping.\n", name.c_str());
			continue;
		}

		ticp::data_v1_t mdata((unsigned char *)pmt::pmt_blob_data(value), (unsigned char *)pmt::pmt_blob_data(value)+pmt::pmt_length(value));

		frame[ d_metadata_map[name] ] = mdata;
	}

	// Data structure generated, store into queue

	// Get lock for queue access
	boost::mutex::scoped_lock lock(d_mutex);

	// Check if the queue is empty (if so wake up the consumer later)
	const bool was_empty = d_queue.empty();

	// store the frame
	d_queue.push(frame);

	// unlock
	lock.unlock();

	// If queue was empty, we need to notify the consumer that there is data
	// available now
	if(was_empty) {
		d_condition_var.notify_one();
	}

	return;
}


