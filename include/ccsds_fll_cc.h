#ifndef INCLUDED_CCSDS_FLL_CC_H
#define INCLUDED_CCSDS_FLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <ccsds_lpf.h>
#include <math.h>
#include <gr_msg_queue.h>

class ccsds_fll_cc;

typedef boost::shared_ptr<ccsds_fll_cc> ccsds_fll_cc_sptr;

/*!
 *  \brief Create a frequency locked loop and return it's shared pointer.
 *
 *  \param obsv_length Number of samples that should be accumulated for a
 *	frequency estimate. Higher values are less sensitive to noise, but take
 *	more time during which the frequency could change.
 *  \param loop_bw Loop bandwidth of the loop filter.
 *  \param power Take the power of the incomming samples for frequency
 *	estimation instead of the incomming samples directly to cancel out
 *	modulations. For M-PSK modulated signal the M-th power is recommended,
 *	while power=1 means take the incomming samples directly (modulations is
 *	still canceled out by averaging over \c obsv_length samples).
 *  \param msgq Message queue to store new frequency corrections. See \ref
 *	page_lo_feedback for more details.
 *  \return Shared pointer to the created FLL block.
 *  \note Might not work out of the box with the ccsds_local_oscillator_cc as it
 *	is matched to ccsds_dll_cc right now.
 *  \todo Match frquency corrections to new LO interface
 */
CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc (unsigned int obsv_length, float  loop_bw, unsigned int power, gr_msg_queue_sptr msgq);

/*!
 *  \brief Frequency locked loop
 *  \ingroup synchronization
 *
 *  Frequency estimation based on feedforward delay and multiply scheme. Sends
 *  frequency estimate to local_oscillator.
 *
 *  See also: \ref page_lo_feedback
 *
 *  \todo Make sure outer loop is only updated when frequency tags arrive and
 *	drop the substraction of the frequecy tag.
 */
class CCSDS_API ccsds_fll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc(unsigned int obsv_length, float  loop_bw, unsigned int power, gr_msg_queue_sptr msgq);

	/*! \brief Private constructor for frequency locked loop.
	 *
	 *  \param obsv_length Number of samples that should be accumulated for a
	 *	frequency estimate. Higher values are less sensitive to noise, but take
	 *	more time during which the frequency could change.
	 *  \param loop_bw Loop bandwidth of the loop filter.
	 *  \param power Take the power of the incomming samples for frequency
	 *	estimation instead of the incomming samples directly to cancel out
	 *	modulations. For M-PSK modulated signal the M-th power is recommended,
	 *	while power=1 means take the incomming samples directly (modulations is
	 *	still canceled out by averaging over \c obsv_length samples).
	 *  \param msgq Message queue to store new frequency corrections. See \ref
	 *	page_lo_feedback for more details.
	 *
	 *  \sa ::ccsds_make_fll_cc()
	 */
	ccsds_fll_cc(unsigned int obsv_length, float  loop_bw, unsigned int power, gr_msg_queue_sptr msgq);   // private constructor
	
	/*! \brief take an array of complex samples and compute their power.
	 *
	 *  \param out Complex array to store the computed powers. Memory for at
	 *	least \c num elements must be allocated.
	 *  \param in Complex array of samples to compute the power from. Must
	 *	contain at least \c num elements.
	 *  \param num Number of samples to process.
	 *
	 *  Take the complex input array \c in, calculate the samplewise power
	 *  ccsds_fll_cc::d_POWER and store them in \c out.
	 *
	 *  \todo Check if volk kernel of new GNU radio version computes the
	 *	right powers and switch back to volk if true.
	 */
	void calc_power(gr_complex *out, const gr_complex *in, const unsigned int num);

	/*! \brief Calculate the samplewise phase difference as a rotator.
	 *
	 *  \param tmp_c Aligned complex array in which the differences should be
	 *	stored. Memory for at least \c num elements must be allocated.
	 *  \param in Complex array of samples which's differences should be
	 *	computed. Must contain at least \c num elements.
	 *  \param num Number of samples to process.
	 *
	 *  Implements the delay and multiply frequency error detector
	 *  tmp_c[k] = in[k] * conj(in[k-1])
	 *  where conj( . ) computes the complex conjugate.
	 *
	 *  The algorithm assumes a complex sample 1+0j at the very beginning
	 *  and stores the last sample per block for the next iteration after
	 *  the first run. This way \c num input samples generate \c num output
	 *  samples without going out of array bounds.
	 *
	 *  This block is using volk, \c tmp_c \b must be aligned, \c in \b 
	 *  should be aligned for better performance.
	 */
	void calc_diffs(gr_complex *tmp_c, const gr_complex *in, const unsigned int num);

	/*! \brief Accumulates blocks of samples from a complex array.
	 *
	 *  \param phasors Complex vector of samples that should be accumulated.
	 *	Must contain at least \c num elements. The accumulated results
	 *	will be written back into this vector into the first \c num/
	 *	\c ::D_L positions.
	 *  \param num Number of samples to process, must be a multiple of ::D_L.
	 *
	 *  Accumulates blocks of ::D_L samples and stores them at the begining
	 *  of \c phasors. Used to filter out modulations and smooth the
	 *  frequency estimates.
	 */
	void calc_summs(gr_complex *phasors, unsigned int num);

	/*! \brief Calculates the phases of an array of complex numbers.
	 *
	 *  \param tmp_f \b Aligned Array in which the phases should be stored in. Memory
	 *	must be allocated for at least \c num elements.
	 *  \param tmp_c \b Aligned array of complex samples which's phases should be
	 *	calculated.
	 *  \param num Number of samples to process.
	 */
	void calc_phases(float  *tmp_f, const gr_complex *tmp_c, const unsigned int num);

	/*! \brief Divides array samplewise by ccsds_fll_cc::d_POWER
	 *
	 *  \param phases Array to process. Must contain at least \c num
	 *	elements. Results are written back into this array.
	 *  \param num Number of samples to process.
	 *
	 *  By taking the M-th power of the samples the frequency offset between
	 *  the samples is amplified by a factor of M. In this block this effect
	 *  is reversed by dividing by ccsds_fll_cc::d_POWER.
	 */
	void adjust_phases(float  *phases, unsigned int num);

	/*! \brief Looks for new lo_frequency tags in blocks and returns their
	 *	frequency value.
	 *
	 *  \param lo_freqs Array in which the estimates should be written.
	 *	Memory for at least \c num elements must be allocated.
	 *  \param num Number of samples to process.
	 *
	 *  /note The current version of ccsds_fll_cc does not conform to the
	 *	new lo frequency feedback scheme and needs to be updated.
	 *  \todo Update to new lo frequency feedback scheme.
	 */
	void get_lo_tags(float  *lo_freqs, const unsigned int num);	

	/*! \brief Sums up \c tmp_fs and \c tmp_lo elementwise and copies the
	 *	sum into \c tmp_f ::D_L times each.
	 *
	 *  \param tmp_f Array in which to store the sums. Memory must be
	 *	allocated for at least \c num elements.
	 *  \param tmp_fs Array with summands. Must contain at least \c num_out/
	 *	\c ::D_L elements.
	 *  \param tmp_lo Array with summands. Must contain at least \c num_out/
	 *	\c ::D_L elements.
	 *  \param num Number of samples to output.
	 *
	 *  By summing over ::D_L samples each we decimated by a rate of ::D_L.
	 *  While summing up tmp_fs and tmp_lo we repeat them ::D_L times each
	 *  to return to the original input rate.
	 *
	 *  \todo continue documentation of ccsds_fll_cc
	 */
	void fill_freqs(float  *tmp_f, float  *tmp_fs, float  *tmp_lo, const unsigned int num_out);
	void send_freq_estimate(double  est);
	void substract_lo_freq(double *tmp_f, float  *tmp_lo, const unsigned int num);
	void calc_rotation(gr_complex *out, const gr_complex *in, const double  *tmp_f, const unsigned int num);

	const unsigned int D_L;
	const unsigned int d_POWER;

	gr_msg_queue_sptr d_msgq;

	gr_complex d_last_sample;
	float d_lo_freq;
	float d_phase;
	static const float D_TWOPI;

	FILE *dbg_file;
	FILE *dbg_file_pow;
	FILE *dbg_file_dif;
	FILE *dbg_file_sum;
	FILE *dbg_file_tag;
	unsigned int dbg_count;
	unsigned int dbg_count_pow;
	unsigned int dbg_count_dif;
	unsigned int dbg_count_sum;
	unsigned int dbg_count_tag;
	unsigned int dbg_input_toggle;
	float dbg_last_msg;

	lpf *d_filter;

public:
    ~ccsds_fll_cc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_FLL_CC_H */
