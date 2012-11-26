#ifndef INCLUDED_LPF2_H
#define INCLUDED_LPF2_H

#include <stdio.h>

class lpf2;

lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope);
lpf2 * ccsds_make_lpf2(double loop_bw);

class lpf2
{
private:
	friend lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope);

	const double  RHO, GAMMA, RHOB;
	double  d_phi, d_xi, d_ephi;
	FILE *debugFile;
	unsigned int debug_count;

	lpf2(double  gamma, double  rho);   // private constructor
	float filter_step(float in);
	

public:
	~lpf2();  // public destructor

	void filter(float *values, const unsigned int n);
};

#endif /* INCLUDED_LPF2_H */
