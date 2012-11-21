#include <ccsds_lpf.h>
#include <stdio.h>
#include <cmath>

// #define LPF_DEBUG

lpf * ccsds_make_lpf(float alpha) {
	return new lpf(alpha);
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

LPF_STATE_TYPE lpf::filter_step(LPF_STATE_TYPE in) {
	if(d_state_init) {
		d_state = ALPHAB * d_state + ALPHA * in;
	} else {
		d_state = in;
		d_state_init = true;
	}
	#ifdef LPF_DEBUG
		fprintf(debugFile,"%.10f,%.10f,%.10f,%.10f\n",in,d_state,0.0f,0.0f);
	#endif
	return d_state;
}

void lpf::filter(float *values, const unsigned int n) {
	for(unsigned int i=0;i<n;i++) {
		values[i] = filter_step(LPF_STATE_CAST values[i]);
	}
}
