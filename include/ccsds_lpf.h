#ifndef INCLUDED_LPF_H
#define INCLUDED_LPF_H

#include <stdio.h>

class lpf;

lpf * ccsds_make_lpf(float loop_bw);

class lpf
{
private:
	friend lpf * ccsds_make_lpf(float alpha);

	const double ALPHA, ALPHAB;
	double d_state;
	bool d_state_init;

	FILE *debugFile;

	double filter_step(double in);
	lpf(float alpha);

public:
	~lpf();  // public destructor

	void filter(double *out, float *in, const unsigned int n);
	void filter(float *out, float *in, const unsigned int n);
};

#endif /* INCLUDED_LPF_H */
