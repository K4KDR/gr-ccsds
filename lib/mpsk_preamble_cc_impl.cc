#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mpsk_preamble_cc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace ccsds {
    
    mpsk_preamble_cc::sptr
    mpsk_preamble_cc::make (const unsigned int num_symbols) {
        return gnuradio::get_initial_sptr (new mpsk_preamble_cc_impl (num_symbols));
    }
    
    mpsk_preamble_cc_impl::mpsk_preamble_cc_impl(const unsigned int num_symbols)
      : gr::block ("mpsk_preamble_cc",
    	gr::io_signature::make (1, 1, sizeof (gr_complex)),
    	gr::io_signature::make (1, 1, sizeof (gr_complex))), d_NUM_SYMBOLS(num_symbols)
    {
    	d_preamble_count = 0;
    
    	d_preamble_syms[0] = gr_complex( 1.0f,0.0f);
    	d_preamble_syms[1] = gr_complex(-1.0f,0.0f);
    }
    
    mpsk_preamble_cc_impl::~mpsk_preamble_cc_impl ()
    {
    	// nothing to do
    }
    
    void mpsk_preamble_cc_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
    	// are we still generating, or already copying?
    	if(d_preamble_count >= d_NUM_SYMBOLS) {
    		// copying
    		ninput_items_required[0] = noutput_items;
    	} else {
    		// generating
    		ninput_items_required[0] = 0;
    	}
    	return;
    }
    
    int  mpsk_preamble_cc_impl::general_work (int                     noutput_items,
    			 gr_vector_int 		     &ninput_items,
                             gr_vector_const_void_star   &input_items,
                             gr_vector_void_star         &output_items)
    {
    	const gr_complex *in = (const gr_complex *) input_items[0];
    	gr_complex *out = (gr_complex *) output_items[0];
    
    
    	// do we still have to generate the preamble?
    	if(d_preamble_count < d_NUM_SYMBOLS) {
    
    		// yes, generate preamble
    		const unsigned int num = std::min(d_NUM_SYMBOLS-d_preamble_count, (unsigned int)noutput_items);
    		for(unsigned int i=0;i<num;i++) {
    			out[i] = d_preamble_syms[d_preamble_count%2];
    			d_preamble_count++;
    		}
    
    		// return here to don't handle overlapping cases in the later
    		// copy code
    		consume_each(0);
    		return num;
    	} else {
    		// no, we can copy the input stream
    
    		// how many samples can we process
    		const unsigned int num = std::min(noutput_items, ninput_items[0]);
    
    		// copy stream
    		memcpy(out, in, num*sizeof(gr_complex));
    
    		// consume input stream
    		consume_each(num);
    
    		// Tell runtime system how many output items we produced.
    		return num;
    	}
    }
  } // namespace ccsds
} // namepsace gr
