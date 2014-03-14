#ifndef INCLUDED_CCSDS_PLL_CC_IMPL_H
#define INCLUDED_CCSDS_PLL_CC_IMPL_H

#include <ccsds/pll_cc.h>
#include "lpf2.h"
#include <math.h>

/*! \brief Number os samples the PLL has to wait after eception of an
 *	frequency_offset tag before another frequency correction is send out.
 */
#define PLL_FREQ_UPDATE 5000

namespace gr {
  namespace ccsds {

    class pll_cc_impl : public pll_cc
    {
    private:
    	/*! \brief Constant buffer of two pi with float precision. */
    	static const double D_TWOPI;
    
    	/*! \brief Storage for modulation order. */
    	const unsigned int d_M;
    
    
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
    	//bool d_lo_msg_tag;
    
    	/*! \brief Number of samples that need to be processed by the loop
    	 *	before a new frequency correction message can be send out.
    	 *
    	 *  \sa d_lo_msg_tag
    	 *
    	 *  ccsds_pll_cc::lo_msg_tag indicates whether this value is valid, or
    	 *  not.
    	 */
    	//unsigned int d_lo_msg_offset;
    
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
    	//void check_lo_tags(const uint64_t from, const unsigned int num);
    
    	/*! \brief Send a frequency correction message
    	 *
    	 *  \param est Per symbol phase increase (i.e. frequency) estimate in
    	 *	radians are send out on port "freq" as double.
    	 *  \sa \ref page_lo_feedback
    	 *  \sa ccsds_lo_feedback.h
    	 *
    	 *  Sends given estimate \c est to output port "freq" according to
    	 *  \ref page_lo_feedback_msg
    	 *  \todo document new message passing type of frequency correction
    	 */
    	//void send_freq_estimate(double est);
    
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
    
    	/*! \brief Public constructor.
    	 *
    	 *  \param m Modulation order.
    	 *  \param loop_bandwidth Bandwidth of the loop filter.
    	 */
    	pll_cc_impl(unsigned int m, float loop_bandwidth);   // public constructor
    	
    	/*! \brief Public deconstructor. */
    	~pll_cc_impl ();  // public destructor
    
    	int general_work (  int                         noutput_items,
                            gr_vector_int               &ninput_items,
                            gr_vector_const_void_star   &input_items,
                            gr_vector_void_star         &output_items);
    };

  } // namespace ccsds
} // namespace gr
    
#endif /* INCLUDED_CCSDS_PLL_CC_IMPL_H */
