#include "lpf2.h"
#include <stdio.h>
#include <cmath>

//#define LPF2_DEBUG
namespace gr {
  namespace ccsds {
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
    	const double c = -4.0*loop_bw;
    
    	double rho = 0.0;
    	// run newton for 10 times
    	for(unsigned int i=0;i<10;i++) {
    		rho -= rho_newton_f(rho,a,b,c) / rho_newton_df(rho,a,b);
    	}
    
    	double gamma = (4.0*damping_factor_squared*rho) / (s_slope * (1.0+rho)*(1.0+rho));
    
    	/*
    	printf("Create 2nd order low pass filter with loop bandwidth %f, damping factor of %f and a S-curve slope of %f at the origin\n",loop_bw,std::sqrt(damping_factor_squared),s_slope);
    	printf("	with a=%f, b=%f, c=%f\n",a,b,c);
    	printf("	this will result in the following loop gains: gamma=%f, rho=%f\n",gamma, rho);
    	//*/
    
    	return new lpf2(gamma, rho);
    }
    
    lpf2 * ccsds_make_lpf2(double loop_bw) {
    	return ccsds_make_lpf2(loop_bw, 0.5, 1.0);
    }
    
    lpf2::lpf2(double  gamma, double  rho) : RHO(rho), GAMMA(gamma), RHOB(1.0 + rho) {
    	
    	d_phi =0.0;
    	d_xi  =0.0;
    	d_ephi=0.0;
    
    	#ifdef LPF2_DEBUG
    		debugFile = fopen("/tmp/ccsds_lpf2_debug.dat","w");
    		debug_count = 0;
    		fprintf(debugFile,"#n,in,PHI,XI,EPHI,wrap_max\n");
    	#endif
    }
    
    lpf2::~lpf2() {
    	#ifdef LPF2_DEBUG
    		fclose(debugFile);
    	#endif
    }
    
    double lpf2::filter_step(float in) {
    	//*
    	double  new_ephi = (double)in-d_phi;
    	d_xi  += GAMMA * (RHOB*new_ephi - d_ephi);
    	d_phi += d_xi;
    	d_ephi = new_ephi;
    	//*/
    
    	/*
    	d_xi  = d_xi + GAMMA*RHOB*in - GAMMA*d_ephi;
    	d_phi = d_phi + d_xi;
    	d_ephi = in;
    	//*/
    
    	#ifdef LPF2_DEBUG
    		debug_count++;
    		fprintf(debugFile,"%u,%2.10f,%2.10f,%2.10f,%2.10f,0.0\n",debug_count,in,d_phi,d_xi,new_ephi);
    	#endif
    	
    	/*	
    	state[XI]  += GAMMA * (RHOB*(in-state[PHI]) - state[EPHI]);
    	state[PHI] += state[XI];
    	state[EPHI] = in-state[PHI];
    	//*/
    
    	
    	return d_phi;
    }
    
    inline double lpf2::wrap(double in, double max) {
    	// if nothing to wrap, return immediatley
    	if(in <= max && in > -max) {
    		return in;
    	}
    
    	while(in > max) {
    		in -= 2.0f*max;
    	}
    	while(in <= -max) {
    		in += 2.0f*max;
    	}
    
    	return in;
    }
    
    double lpf2::filter_step_wrapped(float in, const float wrap_max) {
    	
    	double ephi = wrap(in-d_phi,wrap_max);
    
    	d_xi += GAMMA * wrap(RHOB*ephi - d_ephi, wrap_max);
    	d_xi  = wrap(d_xi, wrap_max);
    	double ret = d_phi;
    	d_phi = wrap(d_phi + d_xi, M_PI);
    
    	d_ephi = ephi;
    
    	#ifdef LPF2_DEBUG
    		debug_count++;
    		fprintf(debugFile,"%u,%2.10f,%2.10f,%2.10f,%2.10f,%2.10f\n",debug_count,in/M_PI,ret/M_PI,d_xi/M_PI,ephi/M_PI,wrap_max/M_PI);
    	#endif
    	
    	return ret;
    }
    
    
    void lpf2::filter(float *values, const unsigned int n) {
    	filter(values, values, n);
    }
    
    void lpf2::filter(float *out, const float *in, const unsigned int n) {
    	for(unsigned int i=0;i<n;i++) {
    		out[i] = (float) filter_step(in[i]);
    	}
    }
    
    void lpf2::filter(double *out, const float *in, const unsigned int n) {
    	for(unsigned int i=0;i<n;i++) {
    		out[i] = filter_step(in[i]);
    	}
    }
    
    
    void lpf2::filter_wrapped(float *values, float *freq_est, const float wrap_max, const unsigned int n) {
    	for(unsigned int i=0;i<n;i++) {
    		values[i] = static_cast<float>(filter_step_wrapped(values[i], wrap_max));
    		freq_est[i] = static_cast<float>(get_freq_estimate());
    	}
    }
    
    
    double lpf2::get_freq_estimate(void) {
    	return d_xi;
    }

  } // namespace ccsds
} // namespace gr
