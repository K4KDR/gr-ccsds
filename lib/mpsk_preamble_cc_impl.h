#ifndef INCLUDED_CCSDS_MPSK_PREAMBLE_CC_IMPL_H
#define INCLUDED_CCSDS_MPSK_PREAMBLE_CC_IMPL_H

#include <ccsds/mpsk_preamble_cc.h>
#include "mpsk_constellation.h"
#include <stdio.h>
#include <math.h>
#include <complex.h>

namespace gr {
  namespace ccsds {

    class mpsk_preamble_cc_impl : public mpsk_preamble_cc
    {
    private:
    
    	/*! \brief Storage for the number of preable symbols to generate. */
    	const unsigned int d_NUM_SYMBOLS;
    
    	/*! \brief Counter for the number of symbols already generated. */
    	unsigned int d_preamble_count;
    
    	/*! \brief Buffer for the two preamble symbols. */
    	gr_complex d_preamble_syms[2];

    public:
    	/*! \brief Public constructor for the M-PSK preamble block.
    	 *
    	 *  \param num_symbols Number of preamble symbols.
    	 */
    	mpsk_preamble_cc_impl (const unsigned int num_symbols);   // public constructor
    
    	/*! \brief Public deconstructor. */
    	~mpsk_preamble_cc_impl ();  // public destructor
    
    	void forecast(int noutput_items, gr_vector_int &ninput_items_required);
    
    	int general_work (int                     noutput_items,
    			 gr_vector_int 		     &ninput_items,
                             gr_vector_const_void_star   &input_items,
                             gr_vector_void_star         &output_items);
    };
    
  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_MPSK_PREAMBLE_CC_IMPL_H */
