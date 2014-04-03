#ifndef INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_IMPL_H
#define INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_IMPL_H

#include <ccsds/mpsk_detector_soft_cf.h>
#include "mpsk_constellation.h"
#include <stdio.h>
#include <math.h>
#include <volk/volk.h>
#include <complex.h>

namespace gr {
  namespace ccsds {

    class mpsk_detector_soft_cf_impl : public mpsk_detector_soft_cf
    {
    private:
    
    
    	/*! \brief Modulation order. */
    	const unsigned int d_M;
    
    	/*! \brief Number of bits per Symbol. */
    	const unsigned int d_ldM;
    
    	/*! \brief Precomputed constant for 2pi with float precision. */
    	const float d_TWOPI;
    
    	/*! \brief Array to map binary constellation points to gray coded data
    	 *	bits.
    	 */
    	const unsigned char *map;
    
    	/*! \brief Calculated the logariothmus dualis of M (log of base two) */
    	static unsigned int get_ldM(const unsigned int M);
    public:
    	/*! \brief Public constructor of the M-PSK soft bit detector
    	 *
    	 *  \param M Modulation order of the M-PSK signal.
    	 *
    	 *  Detects data bits from M-PSK modulated symbols. This block detects
    	 *  the data bits by a non linear transformation of the sample's phase.
    	 */
    	mpsk_detector_soft_cf_impl (const unsigned int M);   // public constructor
    
    	~mpsk_detector_soft_cf_impl ();
    
    	int work (int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_IMPL_H */
