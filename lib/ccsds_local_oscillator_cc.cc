#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds.h"
#include <ccsds_local_oscillator_cc.h>
#include <gr_msg_queue.h>
#include <gr_message.h>
#include <gr_io_signature.h>
#include <volk/volk.h>
#include <math.h>
#include <fftw3.h>

#define LO_DEBUG

ccsds_local_oscillator_cc_sptr
ccsds_make_local_oscillator_cc (unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq)
{
    return ccsds_local_oscillator_cc_sptr (new ccsds_local_oscillator_cc (block_length, osf, msgq));
}

ccsds_local_oscillator_cc::ccsds_local_oscillator_cc (unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq)
  : gr_block ("ccsds_local_oscillator_cc",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature (1, 1, sizeof (gr_complex))), d_L(block_length), d_OSF(osf), d_msgq(gr_make_msg_queue(0))
{
	d_phase = 0.0f;
	d_phase_incr = 0.0f;

	if(d_L < 1) {
		fprintf(stderr,"ERROR LO: block length is smaller one\n");
		exit(EXIT_FAILURE);
		return;
	}

	// we want to process blocks of d_L samples, or multiples of it, so we
	// will also output in multiples of it 
	set_output_multiple(d_L);

	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));

	d_rot = (gr_complex*)fftwf_malloc(d_L*sizeof(gr_complex));
	if(d_rot == 0) {
		fprintf(stderr,"ERROR: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return;
	}
}

ccsds_local_oscillator_cc::~ccsds_local_oscillator_cc () {
	// nothing to do
	fftw_free(d_rot);
}

const double ccsds_local_oscillator_cc::d_TWOPI =2.0f*M_PI;

void ccsds_local_oscillator_cc::wrap_phase(void) {
	while(d_phase > M_PI) {
		d_phase -= M_PI;
	}
	while(d_phase < M_PI) {
		d_phase += M_PI;
	}
	return;
}

void ccsds_local_oscillator_cc::process_messages(void) {
	if(d_msgq->empty_p()) {
		// nothing new, return
		return;
	} else {
		// there are new messages
		gr_message_sptr msg;

		for(unsigned int j=0;j<d_msgq->count();j++) {
			// we only want the last (most recent) message
			msg = d_msgq->delete_head_nowait();
		}

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
			exit(EXIT_FAILURE);
			return;
		}

		// looks like we are fine, take the message's phase increase
		// value and use it for future rotations
		d_phase_incr = msg->arg1() / (double)d_OSF;
	}
}

void ccsds_local_oscillator_cc::calc_rotation(gr_complex *out, const gr_complex *in) {
	
	//* with volk	
	// create the inidividual rotos
	for(unsigned int i=0;i<d_L;i++) {
		d_phase -= d_phase_incr;
		
		// confine phase between 0 and 2 PI, to ensure that we do not
		// encounter float over or underruns when iterating to long
		wrap_phase();

		// calculate sine and cosine values for this phase. joint cal-
		// culation for the same angle is faster, than two individual
		// calls to sin and cos.
		sincosf(d_phase,&d_tmp_sin, &d_tmp_cos);

		// assemble the rotator and store it in an array
		d_rot[i] = gr_complex(d_tmp_cos,d_tmp_sin);
	}

	if(is_unaligned()) {
		volk_32fc_x2_multiply_32fc_u(out, in, d_rot, d_L);
	} else {
		volk_32fc_x2_multiply_32fc_a(out, in, d_rot, d_L);
	}
	//*/

	
	/* without volk
	gr_complex rotator;
	for(unsigned int i=0;i<d_L;i++) {
		d_phase -= phase_incr;
		wrap_phase();
		
		rotator = std::polar(1.0f,(float)d_phase);
		out[i] = in[i] * rotator;
	}
	//*/
	return;
}

void ccsds_local_oscillator_cc::add_tag(unsigned int local_offset) {
	const uint64_t offset = this->nitems_written(0) + local_offset;

	pmt::pmt_t key   = pmt::mp("freq_offset");
	pmt::pmt_t value = pmt::mp(d_phase_incr);

	//write at tag to output port 0 with given absolute item offset
	this->add_item_tag(0, offset, key, value);
}

int  ccsds_local_oscillator_cc::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];

	// how many sample blocks can we process?
	const unsigned int num_ops = (unsigned int) ( ((noutput_items > ninput_items[0]) ? ninput_items[0] : noutput_items) / d_L );
	const unsigned int num = num_ops * d_L;

	if(num == 0 || num_ops == 0) {
		consume_each(0);
		return 0;
	}

	for(unsigned int i=0;i<num_ops;i++) {

		// check for new frequency estimates
		process_messages();

		// rotate the samples according to the signalled frequency
		calc_rotation(out, in);

		// tag samples with frequency tag, so the FLL knows on which
		// frequency estimate these samples are based on
		add_tag(i*d_L);
	}
	
	// Tell runtime how many input samples we used
	consume_each(num);

	// Tell runtime system how many output items we produced
	return num;
}
