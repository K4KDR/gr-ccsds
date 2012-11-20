#ifndef INCLUDED_LPF2_H
#define INCLUDED_LPF2_H

#define LPF2_STATE_TYPE double
#define LPF2_STATE_CAST (double)

#include <stdio.h>

class lpf2;

lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope);
lpf2 * ccsds_make_lpf2(double loop_bw);

class lpf2
{
private:
	friend lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope);

	const LPF2_STATE_TYPE RHO, GAMMA, RHOB;
	LPF2_STATE_TYPE d_phi, d_xi, d_ephi;
	FILE *debugFile;

	lpf2(LPF2_STATE_TYPE gamma, LPF2_STATE_TYPE rho);   // private constructor
	LPF2_STATE_TYPE filter_step(LPF2_STATE_TYPE);
	

public:
	~lpf2();  // public destructor

	void filter(float *values, const unsigned int n);
};

#endif /* INCLUDED_LPF2_H */
