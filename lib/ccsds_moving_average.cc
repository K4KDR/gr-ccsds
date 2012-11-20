#include <ccsds_moving_average.h>
#include <stdio.h>
#include <cmath>

moving_average::moving_average() {
	moving_average::D_NUM_RESET = 100000000;
	moving_average::D_NUM_COLLECT=1000000;

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
	the second filter is started in parallel to populate with D_NUM_COLLECT
	samples when the first filter is reseted and the output of the second
	filter is used as frequency estimate until this filter reaches D_NUM_RESET
	iterations, then the two filters switch their roles again.
	d_freq_sel points to the index of the currently active filter (0 or 1)
*/

//* with optimization
void moving_average::populate_filter(const float *values, unsigned int n) {
	// if we start with a reseted filter, initialize first
	if(*inact_count <= 0) {
		// initialize
		*inact_freq = values[0];
		*inact_count = 1;

		// shift the pointer by one to no longer process the first sample
		values++;
		// pointer is shifted, one sample less to compute
		n--;
		// output the first sample (which is identical to the input, so no need to really process this line)
		// values[0] = *curr_values; // = values[0]

		// make shure there is more to do
		if(n<=0) {
			return;
		}
	}

	//
	//	Now there is at least one value into our buffer and at least one more value to filter.
	//
	//	To add n values to the existing average of m samples
	//	x_bar_n+m = ( m * x_bar_m + sum_m^m+n-1 x_i )
	//		      -----------   ---------------
	//		      const: tmp    add x_i to sum on every step
	//

	// calculate this product only once
	const double tmp = *inact_freq * (double)*inact_count;

	// don't sum over all previous elements at every iteration step, but save the previous sums here
	double sum = 0.0;

	// go through all samples
	for(unsigned int i=0;i<n;i++) {
		sum += values[i];
	}	
	
	*inact_count += n;
	*inact_freq   = (float) ((sum + tmp) / ((double) *inact_count));	
	
	return;
}

void moving_average::apply_filter(float *values, unsigned int n) {
	// if we start with a reseted filter, initialize first
	if(*act_count <= 0) {
		// initialize
		*act_freq = values[0];
		*act_count = 1;

		// shift the pointer by one to no longer process the first sample
		values++;
		// pointer is shifted, one sample less to compute
		n--;
		// output the first sample (which is identical to the input, so no need to really process this line)
		// values[0] = *curr_values; // = values[0]

		// make shure there is more to do
		if(n<=0) {
			//printf("FLL filter: single value\n");
			return;
		}
	}

	//
	//	Now there is at least one value into our buffer and at least one more value to filter.
	//
	//	To add n values to the existing average of m samples
	//	x_bar_n+m = ( m * x_bar_m + sum_m^m+n-1 x_i )
	//		      -----------   ---------------
	//		      const: tmp    add x_i to sum on every step
	//

	// calculate this product only once
	const double tmp = *act_freq * (double)*act_count;

	// don't sum over all previous elements at every iteration step, but save the previous sums here
	double sum = 0.0;

	// go through all samples
	for(unsigned int i=0;i<n;i++) {
		sum += values[i];
		float tmp2 = values[i];
		values[i] = (float) ((sum + tmp) / ( *act_count + i + 1));
	}
	
	*act_count += n;
	*act_freq  = values[n-1];

	return;
}

void moving_average::filter(float *values, const unsigned int n) {
	// divide the buffer into blocks in which each sample
	// in a block is associated with the same filters.

	// process the buffer in blocks until all blocks are processed
	for(unsigned int still_to_process=n;still_to_process>0;) {
		// how many samples still need to be processed until the next RESET? (always >= 0)
		unsigned int samps_to_reset = D_NUM_RESET - *act_count;
		// how many samples are need to process until the inactive filter needs to be populated?
		// if this number is negative, the inactive filter is already populated
		int samps_to_populate = D_NUM_RESET - D_NUM_COLLECT - *act_count;

		if(samps_to_reset > still_to_process) {
			if(samps_to_populate > still_to_process) { // all samples only need to be filtered
				// active filter
				apply_filter(values, still_to_process);

				still_to_process = 0;
			} else if(samps_to_populate <= 0) { // all samples need to be filtered and populated
				// inactive filter
				populate_filter(values, still_to_process);
				// active filter
				apply_filter(values, still_to_process);
				
				still_to_process = 0;
			} else { // filter all samples untill population
				// active filter
				apply_filter(values, samps_to_populate);

				still_to_process -= samps_to_populate;
				values = &values[(unsigned int)samps_to_populate];
			}
		} else { // reset upcomming within still_to_process samples
			if(samps_to_populate <= 0) { // filter and populate all samples until reset
				// inactive filter
				populate_filter(values, samps_to_reset);
				// active filter
				apply_filter(values, samps_to_reset);

				still_to_process -= samps_to_reset;
				values = &values[(unsigned int)samps_to_reset];
			} else { // filter samples until population
				// active filter
				apply_filter(values, samps_to_populate);

				still_to_process -= samps_to_populate;
				values = &values[(unsigned int)samps_to_populate];
			}
		}

		if(*act_count >= D_NUM_RESET) { // switch filters
			// temporary variables
			double       *tmp_d = inact_freq;
			unsigned int *tmp_i = inact_count;

			// switch pointers
			inact_freq  = act_freq;
			inact_count = act_count;
			act_freq    = tmp_d;
			act_count   = tmp_i;

			// reset now inactive filter
			*inact_freq = 0.0;
			*inact_count = 0;
		}
	}
	return;
}
//*/

/* without optimization
void moving_average::filter(float *values, const unsigned int n) {

	for(unsigned int i=0;i<n;i++) {
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
			double nw = (double)values[i] / *act_count;
			*act_freq = old+nw;

		} else {
			*act_freq = values[i];
			(*act_count)++;
		}

		if(*act_count == D_NUM_RESET-D_NUM_COLLECT+1) { // start populating the second filter
			*inact_freq = values[i];
			*inact_count = 1;
		} else if(*act_count > D_NUM_RESET-D_NUM_COLLECT+1) { // populate second filter
			// assume *inact_count >= 1 because of if clauses above, so dividing by it is okay
			double old = ((double)*inact_count/((*inact_count)+1)) * (*inact_freq);
			(*inact_count)++;
			double nw = (double)values[i] / (*inact_count);
			*inact_freq = old+nw;
		}

		//printf("MA state: freq[0]=%1.5f, freq[0]=%1.5f, count[0]=%2d, count[1]=%2d \t\t"
		//		,(float)freq[0],(float)freq[1],freq_count[0],freq_count[1]);

		values[i] = *act_freq;
	}
}
//*/

float moving_average::getState() {
	return (float) *act_freq;
}
