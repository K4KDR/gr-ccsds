#ifndef INCLUDED_CCSDS_PLL_CC_H
#define INCLUDED_CCSDS_PLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <ccsds_lpf2.h>
#include <math.h>
#include <gr_msg_queue.h>

/*! \brief Number os samples the PLL has to wait after eception of an
 *	frequency_offset tag before another frequency correction is send out.
 */
#define PLL_FREQ_UPDATE 5000

class ccsds_pll_cc;

typedef boost::shared_ptr<ccsds_pll_cc> ccsds_pll_cc_sptr;

/*! \brief Generate a Phase locked loop and return it's shared pointer.
 *
 *  \param m Modulation order of the signal.
 *  \param loop_bandwidth Bandwidth of the loop filter.
 *  \param msgq Mesage queue to which frequency correction messages should be
 *	send.
 */
CCSDS_API ccsds_pll_cc_sptr ccsds_make_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq);

/*! \brief Phase locked loop that sends frequency correction messages.
 *  \ingroup synchronization
 *
 *  \sa \ref page_lo_feedback
 *  \sa ccsds_lo_feedback.h
 *
 *  Phase locked loop using the Viterbi&Viterbi method to estimate the sample's
 *  phase offset. Estimates are filtered with a linear second order low pass
 *  filter.
 *
 *  After reception of a frequency_offset tag the loop waits for another
 *  #PLL_FREQ_UPDATE samples before a new frequency correction message is send
 *  out to close the frequency feedback loop.
 */

class CCSDS_API ccsds_pll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_pll_cc_sptr ccsds_make_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq);

	/*! \brief Constant buffer of two pi with float precision. */
	static const double D_TWOPI;

	/*! \brief Storage for modulation order. */
	const unsigned int d_M;
	
	/*! \brief Shared pointer to frequency correction message queue.
	 *
	 *  \sa \ref page_lo_feedback
	 *  \sa ccsds_lo_feedback.h
	 *
	 *  Shared pointer to the message queue where frequency corrections
	 *  should be send to.
	 */
	gr_msg_queue_sptr d_msgq;

	/*! \brief Indicator whether the loop has recently received a
	 *	frequency_offset tag or not.
	 *
	 *  \sa d_lo_msg_offset
	 *
	 *  If \a true, the loop has recently received a frequency_offset tag
	 *  and the counter in ccsds_pll_cc::d_lo_msg_offset tells how many more
	 *  saples the loop has to process before it should send the next
	 *  frequency correction message.
	 *
	 *  If \a flase, the loop should look for new frequency_offset tags.
	 */
	bool d_lo_msg_tag;

	/*! \brief Number of samples that need to be processed by the loop
	 *	before a new frequency correction message can be send out.
	 *
	 *  \sa d_lo_msg_tag
	 *
	 *  ccsds_pll_cc::lo_msg_tag indicates whether this value is valid, or
	 *  not.
	 */
	unsigned int d_lo_msg_offset;

	/*! \brief Filter used for smoothing the frequency estimates. */
	lpf2* d_filter;

	/*! \brief Storage for current phase estimate. */
	float d_phi_hat;

	/*! \brief File pointer for debugging. */
	FILE *dbg_file;

	/*! \brief File pointer for debugging. */
	FILE *dbg_file_lo;

	/*! \brief Counter for debugging. */
	unsigned int dbg_count;

	/*! \brief Counter for debugging. */
	unsigned int dbg_count_lo;




	/*! \brief Private constructor.
	 *
	 *  \param m Modulation order.
	 *  \param loop_bandwidth Bandwidth of the loop filter.
	 *  \param msgq Mesage queue to which frequency correction messages should be
	 *	send.
	 */
	ccsds_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq);   // private constructor
	
	/*! \brief Remove phase modulation by taking the M-th power of the
	 *	samples.
	 *
	 *  \param tmp_c Complex array in which the powered samples should be
	 *	stored. Memory for at least \c num elements must be allocated.
	 *  \param in Complex array of samples which should be powered. Must
	 *	contain at least \c num elements.
	 *  \param num Number of samples to process.
	 *  \todo Check volk power impementation of new GNU Radio version and
	 *	switch back to volk if it works. Change documentation to require
	 *	aligned arrays.
	 *
	 *  Remove modulation by following the M-power principle. The
	 *  ccsds_pll_cc::d_M th power of the input samples is calculated and
	 *  written to the complex array \c tmp_c.
	 */
	void remove_modulation(gr_complex *tmp_c, const gr_complex *in, const unsigned int num);

	/*! \brief Calculate the phases of an array of complex numbers.
	 *
	 *  \param tmp_f \b Aligned array in which the calculated phases should
	 *	be stored. \b Aligned memory for at least \c num elements must
	 *	be allocated.
	 *  \param tmp_c \b Aligned complex array of samples which's phases
	 *	should be calculated. Must contain at least \c num elements.
	 *	Memory must be \b aligned.
	 *  \param num Number of samples to process.
	 */
	void calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num);

	/*! \brief Checks input stream for possible frequency_offset tags.
	 *
	 *  \param from Absolute counter of processed samples from which the
	 *	tags should be searched. Any tags before this counter are
	 *	ignored.
	 *  \param num Number os samples that should be searched starting from
	 *	\c from.
	 *  \sa \ref page_lo_feedback
	 *  \sa ccsds_lo_feedback.h
	 *
	 *  If ccsds_pll_cc::d_lo_msg_tag is \a true the function will return
	 *  immediatley. This means that the loop has allready eceived a
	 *  frequency_offset tag within the last #PLL_FREQ_UPDATE samples and is
	 *  now waiting for the counter to hit zero so a new frequency
	 *  correction message can be send.
	 *
	 *  If ccsds_pll_cc::d_lo_msg_tag is \a false, the funktion will look
	 *  for frequency_offset tags in range [from,from+num) and if
	 *  any is found the earliest's relative offset to \c from plus
	 *  #PLL_FREQ_UPDATE is stored in ccsds_pll_cc::d_lo_msg_offset and
	 *  ccsds_pll_cc::d_lo_msg_tag is set to \a true. This will cause the
	 *  loop to process another #PLL_FREQ_UPDATE samples before new
	 *  frequency correction messages are send.
	 */
	void check_lo_tags(const uint64_t from, const unsigned int num);

	/*! \brief Send a frequency correction message to LO
	 *
	 *  \param est Per symbol phase increase (i.e. frequency) estimate in
	 *	radians, that should be send to ccsds_pll_cc::d_msgq.
	 *  \sa \ref page_lo_feedback
	 *  \sa ccsds_lo_feedback.h
	 *
	 *  Sends given estimate \c est to mesage queue according to
	 *  \ref page_lo_feedback_msg
	 */
	void send_freq_estimate(double est);

	/*! \brief Rotates complex array according to phase estimates.
	 *
	 *  \param out Complex array to store the rotated samples in. Memory for
	 *	at least \c num elements must be allocated.
	 *  \param in Complex array of samples to be rotated. Must contain at
	 *	least \c num elements.
	 *  \param tmp_f Sample's phase estimate in radians. Must contain at
	 *	least \c num elements.
	 *  \param num Number of samples to process.
	 *
	 *  Rotates samplewise by estimated phase.
	 */
	void calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num);

public:

	/*! \brief Public deconstructor. */
	~ccsds_pll_cc ();  // public destructor

	int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_PLL_CC_H */
