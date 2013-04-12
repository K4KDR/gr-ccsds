#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_ticp_frame_source.h>
#include <gr_io_signature.h>
#include <ccsds_time.h>


ccsds_ticp_frame_source_sptr ccsds_make_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types, const double frame_rate) {
    return ccsds_ticp_frame_source_sptr (new ccsds_ticp_frame_source(hostname, port, frame_length, data_type, map_names, map_types, frame_rate) );
}

ccsds_ticp_frame_source::ccsds_ticp_frame_source (std::string hostname, unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types, const double frame_rate)
	: gr_block ("ccsds_ticp_frame_source",
		gr_make_io_signature (0, 0, 0),
		gr_make_io_signature (0, 0, 0)),
	d_FRAME_LEN(frame_length),
	d_DATA_TYPE(data_type),
	d_FRAME_RATE(frame_rate),
	d_RATE_CONTROL( (d_FRAME_RATE > 0.0) ),
	d_TIME_PER_FRAME( d_RATE_CONTROL ? (unsigned long)(1e9/d_FRAME_RATE) : 0ul)
{
	// register output port
	message_port_register_out(pmt::mp("out"));

	// create metadata dictionary
	if(map_names.size() != map_types.size()) {
		fprintf(stderr,"ERROR TICP FRAME SOURCE: Invalid metadata mapping, number of names and types are not equal.\n");
		exit(EXIT_FAILURE);
		return;
	}

	for(unsigned int i=0;i<map_types.size();i++) {
		if(map_types[i] == d_DATA_TYPE) {
			fprintf(stderr,"ERROR TICP FRAME SOURCE: Invalid metadata mapping, data type appears in metadata types as well.\n");
			exit(EXIT_FAILURE);
			return;
		}

		d_metadata_map[ map_types[i] ] = map_names[i];
	}
	if(d_metadata_map.size() != map_types.size()) {
		fprintf(stderr,"ERROR TICP FRAME SOURCE: Invalid metadata mapping, types are not unique\n");
		exit(EXIT_FAILURE);
		return;
	}


	#if TICP_FRAME_SOURCE_DEBUG == 1
		dbg_file = fopen("/tmp/ccsds_ticp_frame_source_debug.dat","w");
		dbg_file_timing = fopen("/tmp/ccsds_ticp_frame_source_debug_timing.dat","w");
		dbg_count = 0;
		gettimeofday(&dbg_time_start, 0);
	#endif

	d_frame_count = 0;

	#ifdef PROFILE_NUM_SAMPS
		profile_count = 0;
	#endif

	// Init rate control here just to be sure we dont play on random data
	// (real init will be done in asynchronous work)
	d_rate_frame_count = 0;
	gettimeofday(&d_rate_time_ref, 0);

	// create TICP Client
	d_ticp_sptr = boost::shared_ptr< ticp::Client<ticp::data_v2_t> >( new ticp::Client<ticp::data_v2_t>() );
	// Open connection
	d_ticp_sptr->ticpConnectBlocking(hostname, (unsigned short) port);

	d_stop = false;

	d_worker_thread = boost::shared_ptr<boost::thread>( new boost::thread(boost::bind(&ccsds_ticp_frame_source::asynchronous_work, this)) );
}

ccsds_ticp_frame_source::~ccsds_ticp_frame_source () {
	d_mutex.lock();
	d_stop = true;
	d_mutex.unlock();

	d_worker_thread->interrupt();
	d_worker_thread->join();

	d_ticp_sptr.reset(); // make sure the ticp Client is deleted as well.

	#if TICP_FRAME_SOURCE_DEBUG == 1
		fflush(dbg_file);
		fflush(dbg_file_timing);

		fclose(dbg_file);
		fclose(dbg_file_timing);
	#endif
}

void ccsds_ticp_frame_source::asynchronous_work(void)
{

	d_astart.wait_for_start(); // blocking

	// Init rate control
	if(d_RATE_CONTROL) {
		d_rate_frame_count = 0;
		gettimeofday(&d_rate_time_ref, 0);
	}

	bool stop;

	do {

		// Do the rate control
		if(d_RATE_CONTROL) {
			timeval now;
			gettimeofday(&now, 0);

			unsigned long time_offset = d_rate_frame_count * d_TIME_PER_FRAME; //ns

			unsigned long time_elapsed = ccsds_time_diff_nano(&now, &d_rate_time_ref); //ns

			// is it already time to send the next frame, or do we need to wait
			if(time_offset > time_elapsed) {
				boost::this_thread::sleep_for(boost::chrono::nanoseconds(time_offset-time_elapsed));
			}

			// We are about to send the frame, increase counter
			d_rate_frame_count++;

			// After a couple of iterations, move the reference time
			// to always keep it quite close to the current time so
			// the time differences in nanoseconds don't grow too
			// large.
			if(d_rate_frame_count>=100) {
				d_rate_frame_count = 1;
				ccsds_time_add_usecs(&d_rate_time_ref, time_offset/1000);
			}
		}

		#if TICP_FRAME_SOURCE_DEBUG == 1
			timeval now;
			gettimeofday(&now, 0);
			fprintf(dbg_file_timing,"%u,%lf,%lu,%lu\n",dbg_count, ccsds_time_diff_seconds(&now,&dbg_time_start),ccsds_time_to_micro(&dbg_time_start),ccsds_time_to_micro(&now));
		#endif

		// buffer for the frame
		ticp::data_v2_t frame;

		// get new Frame
		d_ticp_sptr->ticpGetFrameBlocking(frame); //blocking

		ticp::data_v1_t *frame_maindata = &frame[d_DATA_TYPE];

		// Check that frames have the same size in both systems
		if(frame_maindata->size() != d_FRAME_LEN) {
			fprintf(stderr,"ERROR TICP FRAME SOURCE: frame lengths do not match TICP:%lu, Internal:%u\n",frame_maindata->size(),d_FRAME_LEN);
			exit(EXIT_FAILURE);
			return;
		}

		// create blob from data
		pmt::pmt_t msg_out_data = pmt::pmt_make_blob(&(*frame_maindata)[0], d_FRAME_LEN);

		#if TICP_FRAME_SOURCE_DEBUG == 1
			fprintf(dbg_file,"%3u  ",dbg_count);
			for(unsigned int i=0;i<d_FRAME_LEN;i++) {
				fprintf(dbg_file,"%2X ",(*frame_maindata)[i]);
			}
			fprintf(dbg_file,"\n");
			dbg_count++;
		#endif

		// create header
		pmt::pmt_t dict = pmt::pmt_make_dict();
		for(ticp::data_v2_t::const_iterator it = frame.begin(); it != frame.end(); it++) {

			// skip the real data (which has been handled above)
			if(it->first == d_DATA_TYPE) {
				continue;
			}

			// skip data types we are not interested in (and have no name for it)
			if(d_metadata_map.count(it->first) == 0) {

				continue;
			}

			// at this point we have valid metadata that we want, so
			// add it to the dictionary

			// create blob from metadata
			pmt::pmt_t metadata_data = pmt::pmt_make_blob(&(it->second[0]), it->second.size());

			// create pmt name
			pmt::pmt_t metadata_name = pmt::pmt_string_to_symbol( d_metadata_map[it->first] );

			// add to the directory
			dict = pmt::pmt_dict_add(dict, metadata_name, metadata_data);
		}

		// Construct the new message using the received header
		pmt::pmt_t msg_out = pmt::pmt_cons(dict, msg_out_data);

		// enqueue message
		message_port_pub( pmt::mp("out"), msg_out );

		#ifdef PROFILE_NUM_SAMPS
			profile_count += d_FRAME_LEN;
			if(profile_count >= PROFILE_NUM_SAMPS) {
				d_mutex.lock();
				d_stop = true;
				d_mutex.unlock();
			}
			printf("%7lu/%7u bytes processed\n",profile_count,PROFILE_NUM_SAMPS);
		#endif

		//printf("TICP frame source processed %lu frames so far.\n", ++d_frame_count);

		try {
			boost::this_thread::interruption_point();
		} catch(...) {
			// do nothing, we just want to be able to be interrupted
		}

		d_mutex.lock();
		stop = d_stop or d_astart.is_stopped();
		d_mutex.unlock();
	} while(!stop);

	// stop asynchronous work

	// delete TICP client, so this object can be deconstructed
	d_ticp_sptr.reset();

	return;
}
