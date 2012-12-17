#ifndef INCLUDED_CCSDS_LO_CC_H
#define INCLUDED_CCSDS_LO_CC_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

class ccsds_local_oscillator_cc;

typedef boost::shared_ptr<ccsds_local_oscillator_cc> ccsds_local_oscillator_cc_sptr;

CCSDS_API ccsds_local_oscillator_cc_sptr ccsds_make_local_oscillator_cc (unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq);

class CCSDS_API ccsds_local_oscillator_cc : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_local_oscillator_cc_sptr ccsds_make_local_oscillator_cc(unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq);

	ccsds_local_oscillator_cc(unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq);   // private constructor
	
	inline void wrap_phase(void);
	void process_messages(void);
	void add_tag(void);
	void calc_rotation(gr_complex *out, const gr_complex *in, const double phase_incr, const double phase_incr_slope, const unsigned int num);

	static const double d_TWOPI;
	const unsigned int d_L;
	const unsigned int d_OSF;
	double d_phase;
	double d_phase_incr, d_last_phase_incr;

	gr_msg_queue_sptr d_msgq;

	// temp variable for sine and cosine part of rotator
	float d_tmp_sin, d_tmp_cos;

	FILE *dbg_file;
	FILE *dbg_file_msg;
	unsigned int dbg_count;
	bool dbg_toggle;

public:
	~ccsds_local_oscillator_cc ();  // public destructor
	int work (  int                         noutput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_LO_CC_H */
