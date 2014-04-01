#ifndef INCLUDED_CCSDS_MPSK_MOD_BC_IMPL_H
#define INCLUDED_CCSDS_MPSK_MOD_BC_IMPL_H

#include <ccsds/mpsk_mod_bc.h>
#include "mpsk_constellation.h"
#include <complex.h>
#include <stdio.h>
#include <math.h>

namespace gr {
  namespace ccsds {
    
    class mpsk_mod_bc_impl : public mpsk_mod_bc
    {
    private:
    
    
    	/*! \brief Storage for the modulation order. */
    	unsigned int d_M;
    
    	/*! \brief Storage for precomputed constellation. */
    	gr_complex* d_constellation;
    
    public:
    	/*! \brief Public constructor for the M-PSK modulator.
    	 *
    	 *  \param M Modulation order.
    	 */
    	mpsk_mod_bc_impl (unsigned int M);   // public constructor
    
    	/*! \brief Public deconstructor. */
    	~mpsk_mod_bc_impl ();  // public destructor
    
    	int work (  int                         noutput_items,
                            gr_vector_const_void_star   &input_items,
                            gr_vector_void_star         &output_items);
    };

  } //namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_MOD_BC_IMPL_H */
