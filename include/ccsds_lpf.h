#ifndef INCLUDED_LPF_H
#define INCLUDED_LPF_H

#define LPF_STATE_TYPE double
#define LPF_STATE_CAST (double)

#include <stdio.h>

class lpf;

lpf * ccsds_make_lpf(float loop_bw);

class lpf
{
private:
	friend lpf * ccsds_make_lpf(float alpha);

	const LPF_STATE_TYPE ALPHA, ALPHAB;
	LPF_STATE_TYPE d_state;
	bool d_state_init;

	FILE *debugFile;

	LPF_STATE_TYPE filter_step(LPF_STATE_TYPE);
	lpf(float alpha);

public:
	~lpf();  // public destructor

	void filter(float *values, const unsigned int n);
};

#endif /* INCLUDED_LPF_H */
