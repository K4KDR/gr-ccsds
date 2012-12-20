#ifndef INCLUDED_CCSDS_LO_CC_H
#define INCLUDED_CCSDS_LO_CC_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

class ccsds_local_oscillator_cc;

typedef boost::shared_ptr<ccsds_local_oscillator_cc> ccsds_local_oscillator_cc_sptr;

/* \brief Create a local oscillator and return a shared pointer to it.
 *
 *  \param block_length Unused in current implementation
 *  \param osf Oversampling factor, used to scale down symbolrate frequency
 *	estimates
 *  \param msgq Message queue from which to obtain frequency corrections
 *  \return Return shared pointer to local oscillator
 *  \todo Remove block_length from the code
 *  \todo Improve adaptive filtering of frequency estimates
 */
CCSDS_API ccsds_local_oscillator_cc_sptr ccsds_make_local_oscillator_cc (unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq);

/*! \brief Local oscillator controlled by asynchronous frequency corrections
 *  \ingroup synchronization
 *
 *  Oscillates with an internal frequency (initially zero) that can be tuned
 *  by asynchronous frequency correction messages. Every time a new frequency is
 *  used a stream tag with the new frequency is attatched to the sample stream.
 */
class CCSDS_API ccsds_local_oscillator_cc : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_local_oscillator_cc_sptr ccsds_make_local_oscillator_cc(unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq);

	/*! \brief 2pi with double precision */
	static const double d_TWOPI;

	/* \brief Block length */
	const unsigned int d_L;

	/* \brief Oversampling factor as defined in the constructor */
	const unsigned int d_OSF;

	/*! \brief Current phase of the local oscillator */
	double d_phase;

	/*! \brief Current estimated phase increase per sample (i.e. the current
	 *	frequency estimate)
	 */
	double d_phase_incr;

	/*! \brief Last estimated phase increase per sample (i.e. the last
	 *	frequency estimate)
	 *
	 *  Used for linear interpolation between two frequency estimated to
	 *  avoid frequency jumps
	 */
	double d_last_phase_incr;

	/*! \brief Messagequeue to obtain frequency corrections from. */
	gr_msg_queue_sptr d_msgq;

	/*! \brief temporary storage for cosine calculation in the rotator
	 *
	 *  \sa calc_rotation()
	 */
	float d_tmp_cos;

	/*! \brief temporary storage for sine calculation in the rotator
	 *
	 *  \sa calc_rotation()
	 */
	float d_tmp_sin;

	/*! \brief File pointer for debugging */
	FILE *dbg_file;

	/*! \brief File pointer for debugging */
	FILE *dbg_file_msg;

	/*! \brief Counter for debugging */
	unsigned int dbg_count;

	/*! \brief Block toggle for debugging */
	bool dbg_toggle;


	/* \brief Private constructor of a local oscillator
	 *
	 *  \param block_length Unused in current implementation
	 *  \param osf Oversampling factor, used to scale down symbolrate frequency
	 *	estimates
	 *  \param msgq Message queue from which to obtain frequency corrections
	 *  \return Return shared pointer to local oscillator
	 */
	ccsds_local_oscillator_cc(unsigned int block_length, unsigned int osf, gr_msg_queue_sptr msgq);   // private constructor
	
	/*! \brief Keep d_phase in range [-pi,pi] */
	inline void wrap_phase(void);

	/*! \brief Look for new frequency corrections and update the per sample
	 *	phase increase.
	 *
	 *  Get new message from d_msgq and check for valid frequency update
	 *  messages.
	 *
	 *  \sa ccsds_lo_feedback.h
	 */
	void process_messages(void);

	/*! \brief Attach a stream tag with the current per symbol frequency
	 *	estimate to the first sample in the current input block.
	 */
	void add_tag(void);

	/*! \brief Rotate the sample vector with current frequency estimate
	 *
	 *  Rotate the complex input vector with the local phase d_phase, and
	 *  a linear changing phase increase with constant slope and offset
	 *  interpolated between d_phase_incr and d_last_phase_incr.
	 *
	 *  The phase increase from sample i to i+1 is
	 *  phase[i+1] = phase[i] + phase_incr + i*phase_incr_slope
	 *
	 *  \param out Complex array to which the rotated samples should be
	 *	written. Memory for at least num elements must be allocated.
	 *  \param in Complex array of samples that should be rotated. Must
	 *	contain at least num elements.
	 *  \param phase_incr Constant per sample phase increase, i.e. frequency
	 *	estimate.
	 *  \param phase_incr_slope Slope with which the per sample phase
	 *	increase is changed during this block.
	 *  \param num Number of samples to process.
	 */
	void calc_rotation(gr_complex *out, const gr_complex *in, const double phase_incr, const double phase_incr_slope, const unsigned int num);


public:
	~ccsds_local_oscillator_cc ();  // public destructor
	int work (  int                         noutput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_LO_CC_H */
