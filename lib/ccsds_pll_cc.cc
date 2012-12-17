#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_lo_feedback.h"

#include <ccsds_pll_cc.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <stdio.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>
#include <gr_msg_queue.h>
#include <gr_message.h>

// #define PLL_DEBUG

ccsds_pll_cc_sptr ccsds_make_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq) {
    return ccsds_pll_cc_sptr (new ccsds_pll_cc(m, loop_bandwidth, msgq) );
}

ccsds_pll_cc::ccsds_pll_cc (unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq)
  : gr_block ("ccsds_pll_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	//gr_make_io_signature3 (1, 3, sizeof (gr_complex), sizeof (float), sizeof (float)))
	gr_make_io_signature (1, 1, sizeof (gr_complex))), d_M(m), d_msgq(msgq)
{
	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));

	d_filter = ccsds_make_lpf2(loop_bandwidth, 0.5, 1.0);


	d_lo_msg_tag = false;
	d_lo_msg_offset = 0;

	send_freq_estimate(0.0);

	d_phi_hat = 0.0f;

	#ifdef PLL_DEBUG
		dbg_count    = 0;
		dbg_count_lo = 0;

		dbg_file = fopen("debug_pll.dat","w");
		dbg_file_lo = fopen("debug_pll_lo.dat","w");
		if(dbg_file == NULL || dbg_file_lo == NULL) {
			fprintf(stderr,"ERROR PLL: can not open debug file\n");
			exit(EXIT_FAILURE);
			return;
		}
		fprintf(dbg_file, "#k,phi_raw,phi_hat,arg(in),arg(in^M),abs(in),real(in),imag(in),freq_est\n");
		fprintf(dbg_file_lo, "#k,freq_raw, freq_filtered\n");
	#endif
}

ccsds_pll_cc::~ccsds_pll_cc ()
{
	delete d_filter;

	#ifdef DLL_DEBUG
		fflush(dbg_file);

		fclose(dbg_file);
	#endif
}

const double ccsds_pll_cc::D_TWOPI=2.0f*M_PI;

void ccsds_pll_cc::rotate_constellation(gr_complex *out, const gr_complex *in, const float angle, const unsigned int num) {
	gr_complex rot = std::polar(1.0f, angle);

	if(is_unaligned()) {
		volk_32fc_s32fc_multiply_32fc_u(out, in, rot, num);
	} else {
		volk_32fc_s32fc_multiply_32fc_a(out, in, rot, num);
	}
}

void ccsds_pll_cc::remove_modulation(gr_complex *out, const gr_complex *in, const unsigned int num) {
	// FIXME volk power block seems to calculate wrong results 
	// (e.g. 1 squared is -1)	
	if(true || is_unaligned()) {
		for(unsigned int i=0;i<num;i++) {
			out[i] = std::pow(in[i],d_M);
		}
	} else {
		volk_32fc_s32f_power_32fc_a(out,in,(float)d_M,num);
	}
	return;
}

void ccsds_pll_cc::calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num) {
	// tmp_c and tmp_f are guaranteed to be aligned
	volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, (float)d_M, num);
	//volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, 1.0f, num);

	/* without volk
	for(unsigned int i=0;i<num;i++) {
			tmp_f[i] = std::arg(tmp_c[i]);
	}
	//*/
	return;
}

void ccsds_pll_cc::check_lo_tags(const uint64_t from, const unsigned int num) {
	if(d_lo_msg_tag) {
		// we already have received a tag and are waiting
		return;
	}

	std::vector<gr_tag_t> tags;

	// default tag is out of range
	uint64_t offset = from+num;

	//read all tags associated with port 0 in given range
	this->get_tags_in_range(tags, 0, from, from+num, LO_TAG_KEY);

	// did we receive new tags?
	if(tags.size() == 0) {
		return;
	} else {

		// go through all tags and find the earliest one
		for(unsigned int i=0;i<tags.size();i++) {
			offset = (tags[i].offset < offset) ? tags[i].offset : offset;
		}

		d_lo_msg_tag = true;
		d_lo_msg_offset = (unsigned int) offset-from + PLL_FREQ_UPDATE;

		return;
	}
}

void ccsds_pll_cc::send_freq_estimate(double est) {
	// frequency message with the following values (arbitrary chosen)
	// type = (long) MSG_FREQ_TYPE (defined in ccsds.h)
	// arg1 = (float) requested phase_incr per symbol
	// arg2 = (float) MSG_FREQ_ARG2 (defined in ccsds.h)
	// length = 0 (we just pass the arguments)

	// make sure there is space in the queue
	if(d_msgq->full_p()) {
		// just delete one, so in case of race conditions there are
		// still some older messages arround (but any message in the
		// queue is still newer than the information the LO has.
		d_msgq->delete_head_nowait();
		//FIXME add increase to current estimate
	}

	// now put the new message in the queue
	d_msgq->handle(gr_make_message(MSG_FREQ_TYPE, est, MSG_FREQ_ARG2, 0));

	return;
}

void ccsds_pll_cc::calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num) {
	//* with volk	
	gr_complex *rot = (gr_complex*)fftwf_malloc(sizeof(gr_complex)*num);

	// temp variable for sine and cosine part of rotator
	float tmp_s, tmp_c;
	float phase_offset = 0.0f;

	if(d_M == 4) { // QPSK constellation rotated by 45 degree
		phase_offset = M_PI/4.0f;
	}

	// create the inidividual rotos
	for(unsigned int i=0;i<num;i++) {
		// calculate sine and cosine values for this phase. joint cal-
		// culation for the same angle is faster, than two individual
		// calls to sin and cos.
		sincosf(-(tmp_f[i]+phase_offset),&tmp_s, &tmp_c);

		// assemble the rotator and store it in an array
		rot[i] = gr_complex(tmp_c,tmp_s);
	}

	if(is_unaligned()) {
		volk_32fc_x2_multiply_32fc_u(out, in, rot, num);
	} else {
		volk_32fc_x2_multiply_32fc_a(out, in, rot, num);
	}

	fftw_free(rot);
	// */

	/* without volk
	gr_complex rotator;
	for(unsigned int i=0;i<num;i++) {
		// do the phase integration in double precision
		d_phase = std::fmod(d_phase-(double)tmp_f[i], D_TWOPI);
		// the rotation itself can be single precision
		rotator = std::polar(1.0f,(float)d_phase);
		out[i] = in[i] * rotator;
	}
	//*/
	return;
}

int  ccsds_pll_cc::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	
	// how many samples can we process?
	unsigned int num = (noutput_items > ninput_items[0]) ? ninput_items[0] : noutput_items;
	const uint64_t nread = this->nitems_read(0); //number of items read on port 0

	// auxilliary variables
	gr_complex *tmp_c;
	float *tmp_f, *tmp_freq;

	// allocate temporary memory
	tmp_c = (gr_complex *) fftw_malloc(num * sizeof(gr_complex));
	tmp_f = (float *)      fftw_malloc(num * sizeof(float));
	tmp_freq = (float *)      fftw_malloc(num * sizeof(float));
	if (tmp_c == 0 || tmp_f == 0 || tmp_freq == 0) {
		fprintf(stderr,"ERROR: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return 0;
	}

	//
	// do the synchronization
	//
	
	if(d_M == 4) { // QPSK constellation rotated by 45 degree, fix it
		rotate_constellation(tmp_c, in, -M_PI/4.0f, num);
	} else { // no fix necessarry, just move the samples to the next buffer
		memcpy(tmp_c,in,num*sizeof(gr_complex));
	}

	// remove the modulation
	remove_modulation(tmp_c,tmp_c,num);

	#ifdef PLL_DEBUG
		gr_complex *mod_removed;
		mod_removed = new gr_complex[num];
		memcpy(mod_removed, tmp_c, num*sizeof(gr_complex));
	#endif

	// take the calculated phasors and calculate the phase
	calc_phases(tmp_f, tmp_c, num);
	
	#ifdef PLL_DEBUG
		float *phase_unfiltered;
		phase_unfiltered = new float[num];
		memcpy(phase_unfiltered, tmp_f, num*sizeof(float));
	#endif

	// Put these calculations into the filter
	d_filter->filter_wrapped(tmp_f, tmp_freq, M_PI/(float)d_M,num);
	
	// check if new lo frequency tag arrived
	check_lo_tags(nread, num);

	// did we receive a tag?
	if(d_lo_msg_tag) {
		// check if new frequency update is due
		if(d_lo_msg_offset >= num) {
			// not yet, just update counter
			d_lo_msg_offset -= num;
		} else {
			send_freq_estimate(tmp_freq[d_lo_msg_offset]);

			#ifdef PLL_DEBUG
				fprintf(dbg_file_lo, "%lu,%f\n",nread+d_lo_msg_offset,tmp_freq[d_lo_msg_offset]);
			#endif

			// we "consumed" the tag, wait for the next one
			d_lo_msg_tag = false;
		}
	}

	/*
	// send frequency update every PLL_FREQ_UPDATE symbols
	double freq_est = 0.0;
	for(unsigned int i=0;i<num;i++) {

		// wait five update blocks to let the other loops settle
		if(nread+i >= PLL_FREQ_SKIP_FIRST && (nread+i)%PLL_FREQ_UPDATE == 0) {
			d_freq_filter->filter(&freq_est, &tmp_freq[i], 1);
			send_freq_estimate(freq_est);

			#ifdef PLL_DEBUG
				fprintf(dbg_file_lo, "%lu,%f,%f\n",nread+i,tmp_freq[i],freq_est);
			#endif
		}
	
		
	}
	*/
	
	// rotate the samples according to the filtered phase
	calc_rotation(out, in, tmp_f, num);

	#ifdef PLL_DEBUG
		for(unsigned int i=0;i<num;i++) {
			fprintf(dbg_file, "%d,%f,%f,%f,%f,%f,%f,%f,%f\n",dbg_count++,phase_unfiltered[i]/M_PI,tmp_f[i]/M_PI,std::arg(in[i])/M_PI,std::arg(mod_removed[i])/M_PI,
					std::abs(in[i]),std::real(in[i]),std::imag(in[i]),tmp_freq[i]);
		}
		delete[] phase_unfiltered;
		delete[] mod_removed;
	#endif

	// free resources
	fftw_free(tmp_f);
	fftw_free(tmp_freq);
	fftw_free(tmp_c);

	// Tell runtime how many input samples we used
	consume_each(num);

	// Tell runtime system how many output items we produced
	return num;
}
