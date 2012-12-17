#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_lo_feedback.h"
#include <ccsds_local_oscillator_cc.h>
#include <gr_msg_queue.h>
#include <gr_message.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <math.h>
#include <fftw3.h>

// #define LO_DEBUG
// #define LO_NUM_SAMPS_AND_DONE 500000lu

ccsds_local_oscillator_cc_sptr
ccsds_make_local_oscillator_cc (unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq)
{
    return ccsds_local_oscillator_cc_sptr (new ccsds_local_oscillator_cc (block_length, osf, msgq));
}

ccsds_local_oscillator_cc::ccsds_local_oscillator_cc (unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq)
  : gr_sync_block ("ccsds_local_oscillator_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature (1, 1, sizeof (gr_complex))), d_L(block_length*osf), d_OSF(osf), d_msgq(msgq)
{
	d_phase = 0.0f;
	d_phase_incr = 0.0f;
	d_last_phase_incr = 0.0f;

	if(d_L < 1) {
		fprintf(stderr,"ERROR LO: block length is smaller one\n");
		exit(EXIT_FAILURE);
		return;
	}

	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));


	#ifdef LO_DEBUG
		dbg_count = 0;
		dbg_toggle = false;
		
		dbg_file = fopen("debug_lo.dat","w");
		dbg_file_msg = fopen("debug_lo_msg.dat","w");
		if(dbg_file == NULL || dbg_file_msg == NULL) {
			fprintf(stderr,"ERROR LO: can not open debug file\n");
			exit(EXIT_FAILURE);
			return;
		}
		fprintf(dbg_file, "#k,freq,interp freq,toggle\n");
		fprintf(dbg_file, "#k,freq_incr,freq\n");
	#endif
}

ccsds_local_oscillator_cc::~ccsds_local_oscillator_cc () {

	#ifdef LO_DEBUG
		fflush(dbg_file);
		fflush(dbg_file_msg);

		fclose(dbg_file);
		fclose(dbg_file_msg);
	#endif
}

const double ccsds_local_oscillator_cc::d_TWOPI =2.0f*M_PI;

inline void ccsds_local_oscillator_cc::wrap_phase(void) {
	d_phase = fmod(d_phase+M_PI,d_TWOPI)-M_PI;
	return;
}

void ccsds_local_oscillator_cc::process_messages(void) {
	if(d_msgq->empty_p()) {
		// nothing new, return
		return;
	} else {
		// there are new messages

		// in order to avoid frequency jumps, we take every frequency
		// message for at least one block, even if it might be already 
		// outdated
		gr_message_sptr msg = d_msgq->delete_head_nowait();

		// sanity check 1: there should always be a message
		if(msg == 0) {
			fprintf(stderr,"ERROR LO: invalid message\n");
			exit(EXIT_FAILURE);
			return;
		}

		// sanity check 2: frequency adjustment messages should have the
		// following values (arbitrary chosen)
		// type = (long) MSG_FREQ_TYPE (defined in ccsds.h)
		// arg1 = (double) requested phase_incr
		// arg2 = (double) MSG_FREQ_ARG2 (defined in ccsds.h)
		// length = 0 (we just pass the arguments)
		if(msg->type() != MSG_FREQ_TYPE || msg->arg2() != MSG_FREQ_ARG2 || msg->length() != 0) {
			fprintf(stderr,"ERROR LO: message has wrong type or wrong parameters\n");
			//exit(EXIT_FAILURE);
			return;
		}

		// looks like we are fine, take the message's phase increase
		// value and use it for future rotations
		double correction = msg->arg1();

		// apply a nonlinearity to keep high correction values and
		// attenuate low ones
		correction *= (correction/M_PI < 0.00001) ? 0.1 : 1.0;
		correction *= (correction/M_PI < 0.0001) ? 0.5 : 1.0;
		correction *= (correction/M_PI < 0.001) ? 0.5 : 1.0;

		d_phase_incr += correction / (double)d_OSF;

		// add lo tag to inform pll of new frequency
		add_tag();

		#ifdef LO_DEBUG
			fprintf(dbg_file_msg, "%f,%f,%f\n",(float)this->nitems_written(0)/d_OSF,msg->arg1()/((double)d_OSF*M_PI), d_phase_incr/M_PI);
		#endif
	}
}

void ccsds_local_oscillator_cc::add_tag() {
	pmt::pmt_t value = pmt::mp(d_phase_incr);

	//write at tag to output port 0 with given absolute item offset
	this->add_item_tag(0, this->nitems_written(0), LO_TAG_KEY, value);

	return;
 }

void ccsds_local_oscillator_cc::calc_rotation(gr_complex *out, const gr_complex *in, const double phase_incr, const double phase_incr_slope, const unsigned int num) {

	//* with volk	
	gr_complex *rot;
	rot = (gr_complex*)fftwf_malloc(num*sizeof(gr_complex));
	if(rot == 0) {
		fprintf(stderr,"ERROR: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return;
	}

	// create the inidividual rotors
	for(unsigned int i=0;i<num;i++) {
		d_phase -= (phase_incr + i*phase_incr_slope);
		
		// calculate sine and cosine values for this phase. joint cal-
		// culation for the same angle is faster, than two individual
		// calls to sin and cos.
		sincosf(d_phase,&d_tmp_sin, &d_tmp_cos);

		// assemble the rotator and store it in an array
		rot[i] = gr_complex(d_tmp_cos,d_tmp_sin);

		#ifdef LO_DEBUG
			fprintf(dbg_file, "%f,%f,%f,%f,%d,%f,%f\n", (float)(dbg_count++)/(float)d_OSF, d_OSF*(phase_incr + i*phase_incr_slope), d_OSF*phase_incr, d_phase/M_PI,
							(int)dbg_toggle, d_tmp_cos, d_tmp_sin);
		#endif
	}

	if(!is_unaligned()) {
		volk_32fc_x2_multiply_32fc_a(out, in, rot, num);
	} else {
		volk_32fc_x2_multiply_32fc_u(out, in, rot, num);
	}

	fftw_free(rot);
	//*/

	#ifdef LO_DEBUG
		dbg_toggle = !dbg_toggle;
	#endif

	/* without volk
	gr_complex rotator;
	for(unsigned int i=0;i<d_L;i++) {
		d_phase -= phase_incr;
		
		rotator = std::polar(1.0f,(float)d_phase);
		out[i] = in[i] * rotator;
	}
	//*/

	// confine phase between 0 and 2 PI, to ensure that we do not
	// encounter float over or underruns when iterating to long
	// wraping is done once per loop as no over/underrun is expected
	// during one block.
	
	//wrap_phase();

	return;
}

int  ccsds_local_oscillator_cc::work (int                     noutput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	// how many sample blocks can we process?
	const unsigned int num = noutput_items;

	if(num == 0) {
		consume_each(0);
		return 0;
	}

	#ifdef LO_NUM_SAMPS_AND_DONE
		uint64_t nread = this->nitems_read(0);
		if(nread > LO_NUM_SAMPS_AND_DONE) {
			printf("\nLO: %lu samples processed, exit.\n", LO_NUM_SAMPS_AND_DONE);
			exit(EXIT_FAILURE);
			return 0;
		}
		printf("\nLO: %lu/%lu samples processed.\n", nread,LO_NUM_SAMPS_AND_DONE);
	#endif

	process_messages();

	double phase_incr_slope = (d_phase_incr-d_last_phase_incr)/(double)num;
	
	calc_rotation(out, in, d_last_phase_incr, phase_incr_slope, num);

	d_last_phase_incr = d_phase_incr;

	/*
	for(unsigned int i=0;i<num_ops;i++) {
		// check for new frequency estimates
		process_messages();

	
		// rotate the samples according to the signalled frequency
		// copy block to aligned memory		
		memcpy(d_tmp_c, &in[i*d_L], d_L*sizeof(gr_complex));
		// process block
		calc_rotation(d_tmp_c);
		// copy results to output
		memcpy(&out[i*d_L],d_tmp_c, d_L*sizeof(gr_complex));

		// tag samples with frequency tag, so the FLL knows on which
		// frequency estimate these samples are based on
		//add_tag(i*d_L);

		#ifdef LO_DEBUG
			fprintf(dbg_file, "%f,%f\n",(float)(d_L/d_OSF)*(dbg_count++), d_phase_incr*(float)d_OSF);
		#endif
	}
	*/

	// Tell runtime how many input samples we used
	//consume_each(num);

	// Tell runtime system how many output items we produced
	return num;
}
