#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mpsk_mod_bc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace ccsds {

    mpsk_mod_bc::sptr
    mpsk_mod_bc::make (unsigned int M)
    {
        return gnuradio::get_initial_sptr (new mpsk_mod_bc_impl (M));
    }
    
    static const int MIN_IN = 1;    // mininum number of input streams
    static const int MAX_IN = 1;    // maximum number of input streams
    static const int MIN_OUT = 1;   // minimum number of output streams
    static const int MAX_OUT = 1;   // maximum number of output streams
    
    mpsk_mod_bc_impl::mpsk_mod_bc_impl (unsigned int M)
      : gr::sync_block ("mpsk_mod_bc",
    	gr::io_signature::make (MIN_IN, MAX_IN, sizeof (char)),
    	gr::io_signature::make (MIN_OUT, MAX_OUT, sizeof (gr_complex))), d_M(M)
    {
    	d_constellation = make_mpsk_const(M);
    }
    
    mpsk_mod_bc_impl::~mpsk_mod_bc_impl ()
    {
    	// free memory allocated for the constellation
    	delete[] d_constellation;
    }
    
    int  mpsk_mod_bc_impl::work (int                     noutput_items,
                             gr_vector_const_void_star   &input_items,
                             gr_vector_void_star         &output_items)
    {
    	const char *in = (const char *) input_items[0];
    	gr_complex *out = (gr_complex *) output_items[0];
    
    	const unsigned int num = noutput_items;
    
    	for(unsigned int i=0;i<num;i++) {
    		out[i] = d_constellation[(in[i]) % d_M];
    	}
    
    	//consume_each(i);
    
    	// Tell runtime system how many output items we produced.
    	return num;
    }

  } // namespace ccsds
} // namespace gr
