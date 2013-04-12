#include "ticp/TicpClient.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <time.h>
#include <cmath>
#include <numeric>


static unsigned int FRAME_LEN;
static unsigned int NUM_MESSAGES;
static unsigned int msg_count = 1;
static unsigned int last_num = 0;
static double *delays;

void readMsg(vector<unsigned char> const& msg)
{
	timeval stop;
	gettimeofday(&stop, 0);
	
	bool ok = true;

	if(msg.size() != FRAME_LEN) {
		cerr << "Received frame length " << msg.size();
		cerr << " does not match the expected length of " << FRAME_LEN << endl;
	}

	uint64_t seconds_stop = stop.tv_sec;
	uint64_t micros_stop  = stop.tv_usec;


	unsigned int i=0;

	uint32_t num = 0x00000000;
	num |= ((uint32_t)msg[i++]) << 24;
	num |= ((uint32_t)msg[i++]) << 16;
	num |= ((uint32_t)msg[i++]) <<  8;
	num |= ((uint32_t)msg[i++]) <<  0;

	uint64_t seconds_start = 0x0000000000000000;
	seconds_start |= ((uint64_t)msg[i++]) << 56;
	seconds_start |= ((uint64_t)msg[i++]) << 48;
	seconds_start |= ((uint64_t)msg[i++]) << 40;
	seconds_start |= ((uint64_t)msg[i++]) << 32;
	seconds_start |= ((uint64_t)msg[i++]) << 24;
	seconds_start |= ((uint64_t)msg[i++]) << 16;
	seconds_start |= ((uint64_t)msg[i++]) <<  8;
	seconds_start |= ((uint64_t)msg[i++]) <<  0;

	uint64_t micros_start = 0x0000000000000000;
	micros_start |= ((uint64_t)msg[i++]) << 56;
	micros_start |= ((uint64_t)msg[i++]) << 48;
	micros_start |= ((uint64_t)msg[i++]) << 40;
	micros_start |= ((uint64_t)msg[i++]) << 32;
	micros_start |= ((uint64_t)msg[i++]) << 24;
	micros_start |= ((uint64_t)msg[i++]) << 16;
	micros_start |= ((uint64_t)msg[i++]) <<  8;
	micros_start |= ((uint64_t)msg[i++]) <<  0;
	
	for(;i<FRAME_LEN;i++) {
		if(msg[i] != 0x00) {
			ok = false;
			cerr << "Received ";
			cerr << hex << msg[i];
			cerr << "instead of 00 as message fill" << endl;
		}
	}

	uint64_t seconds_diff = seconds_stop - seconds_start;
	long long micros_diff = micros_stop  - micros_start;
	if(micros_diff < 0) {
		micros_diff += 1000000; // Add a second
	}

	double delay = seconds_diff*1000.0 + (uint64_t)micros_diff/1000.0; // ms

	// cout << "Received command with " << delay << " ms delay" << endl;

	if(msg_count == 1) { //init
		last_num = num;
	} else if(num != ++last_num) {
		ok = false;
		cerr << "Expected frame number " << last_num << ", but got " << num << endl;
	}

	if(ok) {
		delays[msg_count++] = delay;
	}

}

int main(int argc, char* argv[])
{
	if(argc != 6) {
		cerr << "usage: latency_receiver <frame length> <num repetitions> <port> <filename> <identifier>" << endl;
		cerr << endl;
		cerr << "frame length     Length of a frame in bytes" << endl;
		cerr << "num repetitions  Number of measurements to perform" << endl;
		cerr << "port             Port number to bind on" << endl;
		cerr << "filename         Filename where to write the results" << endl;
		cerr << "identifier       A string that identifies the parameters of this measurement" << endl;
		return 2;
	}

	stringstream *arg_parser;

	arg_parser = new stringstream(argv[1]);
	*arg_parser >> FRAME_LEN;
	delete arg_parser;

	arg_parser = new stringstream(argv[2]);
	*arg_parser >> NUM_MESSAGES;
	delete arg_parser;

	arg_parser = new stringstream(argv[3]);
	unsigned int port_num;
	*arg_parser >> port_num;
	delete arg_parser;

	FILE *file = fopen(argv[4],"a+");

	if(!file) {
		cerr << "Unable to open file " << argv[4] << endl;
		return 1;
	}

	delays = new double[NUM_MESSAGES];

	ticp::Client<ticp::data_v2_t> ticpClient;

	/* connect to receive TM sync module */
	const string hostname("localhost");
	cout << "Trying to connect to server at " << hostname;
	cout << " on port " << port_num << " ... " << flush;
	ticpClient.ticpConnectBlocking(hostname, port_num, 0);
	//ticpConnect("localhost", 1736);
	cout << "successful" << endl;


	while (msg_count < NUM_MESSAGES) {
		ticp::data_v2_t frame;
		ticpClient.ticpGetFrameBlocking(frame);
		readMsg(frame[0]);
		cout << "." << flush;
	}
	cout << endl;

	// Calculate statistics
	/*
	double average = 0.0;
	for(unsigned int i=0;i<NUM_MESSAGES;i++) {
		average += delays[i];
	}
	average /= NUM_MESSAGES;

	double std_dev = 0.0;
	for(unsigned int i=0;i<NUM_MESSAGES;i++) {
		const double err = delays[i]-average;
		printf("Delay error is %8.6lf ms and sqared it is %8.6lf ms^2\n",err,err*err);
		std_dev += err*err;
	}
	std_dev /= NUM_MESSAGES;
	printf("Variance is %lf ms^2\n", std_dev);
	std_dev = sqrt(std_dev);
	*/
	
	double average = accumulate(delays,delays+NUM_MESSAGES, 0.0) / NUM_MESSAGES;
	double std_dev = sqrt( inner_product(delays,delays+NUM_MESSAGES, delays, 0.0)/(NUM_MESSAGES-1) - (average*average) );

	fprintf(file, "%s, %d, %d, %lf, %lf\n", argv[5], FRAME_LEN, NUM_MESSAGES, average, std_dev);

	fclose(file);

	delete[] delays;

	cout << "Received " << NUM_MESSAGES << " commands with ";
	cout << "average delay of " << average << " ms and a ";
	cout << "standard deviation of " << std_dev << " ms." << endl;
	
	return 0;
}
