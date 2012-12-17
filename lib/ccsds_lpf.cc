#include <ccsds_lpf.h>
#include <stdio.h>
#include <cmath>

#define LPF_DEBUG

lpf * ccsds_make_lpf(float loop_bw) {
	return new lpf(4.0f*loop_bw);
}

lpf::lpf(float alpha) : ALPHA(alpha), ALPHAB(1.0 - ALPHA) {
	d_state=0.0;
	d_state_init = false;

	#ifdef LPF_DEBUG
		debugFile = fopen("debug_lpf.csv","w");
		fprintf(debugFile,"#in,state[PHI],state[XI],state[EPHI]\n");
	#endif
}


lpf::~lpf() {
	#ifdef LPF_DEBUG
		fclose(debugFile);
	#endif
}

double lpf::filter_step(double in) {
	if(d_state_init) {
		d_state = ALPHAB * d_state + ALPHA * in;
	} else {
		d_state = in;
		d_state_init = true;
	}
	#ifdef LPF_DEBUG
		fprintf(debugFile,"%.10lf,%.10lf,%.10f,%.10f\n",in,d_state,0.0f,0.0f);
	#endif
	return d_state;
}

void lpf::filter(double *out, float *in, const unsigned int n) {
	for(unsigned int i=0;i<n;i++) {
		out[i] = filter_step(in[i]);
	}
}

void lpf::filter(float *out, float *in, const unsigned int n) {
	for(unsigned int i=0;i<n;i++) {
		out[i] = (float) filter_step(in[i]);
	}
}
