#include "ticp/TicpServer.hpp"

#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <csignal>

static unsigned int FRAME_LEN; //bytes
static double BIT_RATE; // bits/usec
static unsigned int FRAME_TIME; // in usec
static unsigned int msg_count = 1;
static bool init = false;

static unsigned int rate_frame_count = 0;
static timeval rate_time_ref; // Point in time to which diff refers

static sig_atomic_t last_signal= 0;

//#define CCSDS_LATENCY_SENDER_DEBUG
 
#ifdef CCSDS_LATENCY_SENDER_DEBUG
	/*! \brief File pointer for debugging. */
	FILE *dbg_file;

	/* \brief Time value of block start. */
	timeval dbg_time_start;
#endif

void handler_function (int parameter) {
	last_signal = parameter;
}

//////////////////////////////// Time function /////////////////////////////////

#define G 1000000000
#define M 1000000
#define K 1000

inline unsigned long time_diff_nano(timeval *a, timeval *b) {
	return (unsigned long) ((long)(a->tv_sec-b->tv_sec)*G+((long)(a->tv_usec)-(long)(b->tv_usec))*K);
}

inline void time_diff_to_timespec(timespec *spec, unsigned long diff) {
	spec->tv_sec  = diff / G;
	spec->tv_nsec = diff % G;

	return;
}

inline void time_reset_timespec(timespec *spec) {
	spec->tv_sec  = 0;
	spec->tv_nsec = 0;
	return;
}

inline void time_add_usecs(timeval *t, unsigned long diff) {
	const unsigned long usec_sum = t->tv_usec+diff;

	t->tv_sec += usec_sum / M;
	t->tv_usec = usec_sum % M;

	return ;
}

inline unsigned long time_to_nano(timespec *spec) {
	return spec->tv_sec*G+spec->tv_nsec;
}

////////////////////////////// END Time function ///////////////////////////////

void flowControl(void) {

	if(!init) {
		gettimeofday(&rate_time_ref, 0);
		init = true;
	}


	timeval now;
	gettimeofday(&now, 0);

	long time_offset = (long)rate_frame_count * (long)FRAME_TIME * 1000l; //ns
	long time_elapsed = time_diff_nano(&now, &rate_time_ref); //ns

	// is it already time to send the next frame, or do we need to wait
	if(time_offset > time_elapsed) {
		// wait

		//printf("Rate control: Need to sleep for %lu ns\n",time_offset-time_elapsed);

		// storage for timespecs
		timespec sleep_spec, remain_spec;

		// set initial sleep time
		time_diff_to_timespec(&sleep_spec, time_offset-time_elapsed);

		//printf("initial sleep_spec: ");
		//ccsds_time_print_timespec(&sleep_spec);
		//printf("\n");
			

		// sleep until the time is over (sleep at least once since we have to waist some time)
		do {
			time_reset_timespec(&remain_spec);

			/*
			printf("nanosleep( ");
			ccsds_time_print_timespec(&sleep_spec);
			printf(" , ");					
			ccsds_time_print_timespec(&remain_spec);
			printf(" )\n");
			//*/

			int ret = nanosleep(&sleep_spec, &remain_spec);

			/*
			printf("nanosleep returned code: %d and remain_spec: ",ret);
			ccsds_time_print_timespec(&remain_spec);
			printf("\n");
			//*/
			if(ret == EINVAL) {
				fprintf(stderr,"WARNING TICP FRAME SOURCE: Timespec for nanosleep is invalid.\n");
			} else if (ret == EINTR) {
				printf("\t system signal catched during nanosleep\n");
			}
			// if there is time remaining, this is the new sleep spec
			sleep_spec = remain_spec;
			//printf("\t%lu ns sleep remaining\n",ccsds_time_to_nano(&sleep_spec));

		} while(time_to_nano(&sleep_spec) > 0); // sleep until there is no more need to sleep
	}

	// We are about to send the frame, increase counter
	rate_frame_count++;

	// After a couple of iterations, move the reference time
	// to always keep it quite close to the current time so
	// the time differences in nanoseconds don't grow too
	// large.
	if(rate_frame_count>=10000) {
		rate_frame_count = 1;
		time_add_usecs(&rate_time_ref, time_offset/1000);
	}

	#ifdef CCSDS_LATENCY_SENDER_DEBUG
		if(!dbg_file) {
			dbg_file = fopen("/tmp/ccsds_latency_sender_debug.dat","w");
			gettimeofday(&dbg_time_start, 0);
		}

		timeval dbg_now;
		gettimeofday(&dbg_now, 0);
		fprintf(dbg_file,"%u,%lf,%lf,%lf\n",msg_count, time_diff_nano(&dbg_now,&rate_time_ref)/1e9, FRAME_TIME/1e6, time_offset/1e9);
	#endif

	return;
}

ticp::data_v2_t getLatencyMessage(void) {



	// Sleep until we can send again without overfilling the buffer.
	flowControl();

	timeval start;
	gettimeofday(&start, 0);
	
	//printf("Send message at %12ld seconds and %6ld micro seconds, that is %8ld usec after the last one\n",start.tv_sec,start.tv_usec,get_time_diff(start,last));
	//last = start;

	ticp::data_v1_t message;

	uint32_t num = msg_count++;
	
	uint64_t seconds = start.tv_sec;
	uint64_t micros  = start.tv_usec;

	message.push_back( (unsigned char) ( (num & 0xFF000000ul) >> 24) );
	message.push_back( (unsigned char) ( (num  & 0x00FF0000ul) >> 16) );
	message.push_back( (unsigned char) ( (num  & 0x0000FF00ul) >>  8) );
	message.push_back( (unsigned char) ( (num  & 0x000000FFul) >>  0) );

	message.push_back( (unsigned char) ( (seconds & 0xFF00000000000000ul) >> 56) );
	message.push_back( (unsigned char) ( (seconds & 0x00FF000000000000ul) >> 48) );
	message.push_back( (unsigned char) ( (seconds & 0x0000FF0000000000ul) >> 40) );
	message.push_back( (unsigned char) ( (seconds & 0x000000FF00000000ul) >> 32) );
	message.push_back( (unsigned char) ( (seconds & 0x00000000FF000000ul) >> 24) );
	message.push_back( (unsigned char) ( (seconds & 0x0000000000FF0000ul) >> 16) );
	message.push_back( (unsigned char) ( (seconds & 0x000000000000FF00ul) >>  8) );
	message.push_back( (unsigned char) ( (seconds & 0x00000000000000FFul) >>  0) );

	message.push_back( (unsigned char) ( (micros  & 0xFF00000000000000ul) >> 56) );
	message.push_back( (unsigned char) ( (micros  & 0x00FF000000000000ul) >> 48) );
	message.push_back( (unsigned char) ( (micros  & 0x0000FF0000000000ul) >> 40) );
	message.push_back( (unsigned char) ( (micros  & 0x000000FF00000000ul) >> 32) );
	message.push_back( (unsigned char) ( (micros  & 0x00000000FF000000ul) >> 24) );
	message.push_back( (unsigned char) ( (micros  & 0x0000000000FF0000ul) >> 16) );
	message.push_back( (unsigned char) ( (micros  & 0x000000000000FF00ul) >>  8) );
	message.push_back( (unsigned char) ( (micros  & 0x00000000000000FFul) >>  0) );

	for(unsigned int i=message.size();i<FRAME_LEN;i++) {
		message.push_back(0x00);
	}

	cout << "." << flush;

	ticp::data_v2_t frame;
	frame[0] = message;

	return frame;
}

int main(int argc, char* argv[])
{
	if(argc != 4) {
		cerr << "usage: latency_sender <frame length> <data rate> <num repetitions> <port>" << endl;
		cerr << endl;
		cerr << "frame length     Length of a frame in bytes" << endl;
		cerr << "data rate        Data rate in bits per second" << endl;
		cerr << "port             Port number to bind on" << endl;
		return 2;
	}

	// register message handler
	signal (SIGTERM,handler_function);
	signal (SIGINT, handler_function);

	stringstream *arg_parser;

	arg_parser = new stringstream(argv[1]);
	*arg_parser >> FRAME_LEN;
	delete arg_parser;

	arg_parser = new stringstream(argv[2]);
	unsigned int tmp;
	*arg_parser >> tmp;
	delete arg_parser;
	BIT_RATE = tmp/1e6;
	FRAME_TIME = (unsigned int) ((FRAME_LEN*8.0)/BIT_RATE);

	arg_parser = new stringstream(argv[3]);
	unsigned int port_num;
	*arg_parser >> port_num;
	delete arg_parser;


	ticp::Server<ticp::data_v2_t> ticpServer;
	
	/* start TICP server */
	ticpServer.ticpConnectGetFrameFunction(&getLatencyMessage);
	
	
	cout << "One frame with length of " << FRAME_LEN << " bytes will need ";
	cout << FRAME_TIME << " usec at a rate of " << BIT_RATE << " bit/usec." << endl;

	cout << "Server listening on port " << port_num << endl;
	ticpServer.ticpStartServer(port_num, false); // blocking

	
	while(last_signal == 0) {
		// sleep very long to not take CPU resources. glibc's sleep will
		// be interrupted by any signal
		sleep(999);
	}

	#ifdef CCSDS_LATENCY_SENDER_DEBUG
		fflush(dbg_file);

		fclose(dbg_file);
	#endif

	// exit
	return 0;
}
