#ifndef INCLUDED_CCSDS_PLL_CC_IMPL_H
#define INCLUDED_CCSDS_PLL_CC_IMPL_H

#include <gnuradio/ccsds/pll_cc.h>
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
