#ifndef INCLUDED_MOVING_AVERAGE_H
#define INCLUDED_MOVING_AVERAGE_H

class moving_average
{
private:
	unsigned int D_NUM_RESET;
	unsigned int D_NUM_COLLECT;
	double freq[2];
	unsigned int freq_count[2];

	double* act_freq;
	double* inact_freq;
	unsigned int* act_count;
	unsigned int* inact_count;

public:
	moving_average();   // public constructor
	~moving_average();  // public destructor

	float filter(float input);
	float getState();
};

#endif /* INCLUDED_MOVING_AVERAGE_H */
