#include <ccsds_moving_average.h>
#include <stdio.h>

moving_average::moving_average() {
	moving_average::D_NUM_RESET = 10000;
	moving_average::D_NUM_COLLECT=200;

	freq[0]=0.0;
	freq[1]=0.0;

	freq_count[0]=0;
	freq_count[1]=0;

	act_freq   = &freq[0];
	inact_freq = &freq[1];

	act_count   = &freq_count[0];
	inact_count = &freq_count[1];
}

moving_average::~moving_average() {
	// nothing to do
}

/*
	For frequency filtering we use two parallel moving average filter
	which are reseted after D_NUM_RESET iterations to avoid numeric
	instabilities. D_NUM_COLLECT iterations before one filter is resetted
	the second filter is started in parallel to initialize with D_NUM_COLLECT
	samples when the first filter is reseted and the output of the second
	filter is used as frequency estimate until this filter reaches D_NUM_RESET
	iterations, then the two filters switch their roles again.
	d_freq_sel points to the index of the currently active filter (0 or 1)
*/

float moving_average::filter(float input) {
	// check if filter switches are necessarry
	if(*act_count >= D_NUM_RESET) { // switch filters
		double* tmp_v = act_freq;
		act_freq = inact_freq;
		inact_freq = tmp_v;
		unsigned int* tmp_c = act_count;
		act_count = inact_count;
		inact_count = tmp_c;

		*inact_count=0;
		*inact_freq=0.0;
	}

	// put the result into the active filter
	if(*act_count > 0) {
		double old = ((double)*act_count/((*act_count)+1)) * (*act_freq);
		(*act_count)++;
		double nw = (double)input / *act_count;
		*act_freq = old+nw;
		
	} else {
		*act_freq = input;
		(*act_count)++;
	}
	
	if(*act_count == D_NUM_RESET-D_NUM_COLLECT+1) { // start populating the second filter
		*inact_freq = input;
		*inact_count = 1;
	} else if(*act_count > D_NUM_RESET-D_NUM_COLLECT+1) { // populate second filter
		// assume *inact_count >= 1 because of if clauses above, so dividing by it is okay
		double old = ((double)*inact_count/((*inact_count)+1)) * (*inact_freq);
		(*inact_count)++;
		double nw = (double)input / (*inact_count);
		*inact_freq = old+nw;
	}

	//printf("MA state: freq[0]=%1.5f, freq[0]=%1.5f, count[0]=%2d, count[1]=%2d \t\t"
	//		,(float)freq[0],(float)freq[1],freq_count[0],freq_count[1]);

	return (float) *act_freq;

}

float moving_average::getState() {
	return (float) *act_freq;
}
