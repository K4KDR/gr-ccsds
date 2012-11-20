#include <ccsds_lpf2.h>
#include <stdio.h>
#include <cmath>

#define LPF2_DEBUG

/** 	help function for newton algorithm to determine the loop parameters.

	returns the value of the polynom f(x)=x^3+ax^2+bx+c
*/
double rho_newton_f(const double x, const double a, const double b, const double c) {
	double ret = std::pow(x,3);
	ret += a * std::pow(x,2);
	ret += b * x;
	ret += c;
	return ret;
}

/** 	help function for newton algorithm to determine the loop parameters.

	returns the value of the polynom f'(x)=3x^2+2ax+b
*/
double rho_newton_df(double x, const double a, const double b) {
	double ret = 3.0 * std::pow(x,2);
	ret += 2.0 * a * x;
	ret += b;
	return ret;
}

lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope) {
	// calculate constants
	const double a = 4.0*loop_bw*(damping_factor_squared-1.0) + 2.0*damping_factor_squared + 2.0;
	const double b = 8.0*loop_bw*(damping_factor_squared-1.0) + 4.0*damping_factor_squared + 1.0;
	const double c = 4.0*loop_bw;

	double rho = 0.0;
	// run newton for 10 times
	for(unsigned int i=0;i<10;i++) {
		rho -= rho_newton_f(rho,a,b,c) / rho_newton_df(rho,a,b);
	}

	double gamma = (4.0*damping_factor_squared*rho) / (s_slope * (1.0+rho));

	return new lpf2(LPF2_STATE_CAST gamma, LPF2_STATE_CAST rho);
}

lpf2 * ccsds_make_lpf2(double loop_bw) {
	return ccsds_make_lpf2(loop_bw, 0.5, 1.0);
}

lpf2::lpf2(LPF2_STATE_TYPE gamma, LPF2_STATE_TYPE rho) : RHO(rho), RHOB(1.0 + RHO), GAMMA(gamma) {
	
	d_phi =0.0;
	d_xi  =0.0;
	d_ephi=0.0;
	
	#ifdef LPF2_DEBUG
		debugFile = fopen("debug_lpf2.csv","w");
		fprintf(debugFile,"#in,PHI,XI,EPHI\n");
	#endif
}

lpf2::~lpf2() {
	#ifdef LPF2_DEBUG
		fclose(debugFile);
	#endif
}

LPF2_STATE_TYPE lpf2::filter_step(LPF2_STATE_TYPE in) {
	d_xi  += GAMMA * (RHOB*(in-d_phi) - d_ephi);
	d_phi += d_xi;
	d_ephi = in - d_phi;

	/*	
	state[XI]  += GAMMA * (RHOB*(in-state[PHI]) - state[EPHI]);
	state[PHI] += state[XI];
	state[EPHI] = in-state[PHI];
	//*/

	#ifdef LPF2_DEBUG
		fprintf(debugFile,"%.10f,%.10f,%.10f,%.10f\n",in,d_phi,d_xi,d_ephi);
	#endif
	
	return d_phi;
}


void lpf2::filter(float *values, const unsigned int n) {
	for(unsigned int i=0;i<n;i++) {
		values[i] = filter_step(LPF2_STATE_CAST values[i]);
	}
}

