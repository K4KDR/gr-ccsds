#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "trunk_tail_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace ccsds {

    trunk_tail::sptr 
    trunk_tail::make (const unsigned int trunk_len, const unsigned int frame_len, const unsigned int blanc_bits) {
        return gnuradio::get_initial_sptr (new trunk_tail_impl(trunk_len, frame_len, blanc_bits) );
    }
    
    trunk_tail_impl::trunk_tail_impl(const unsigned int trunk_len, const unsigned int frame_len, const unsigned int blanc_bits)
      : gr::sync_block ("trunk_tail",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), d_TRUNK_LEN(trunk_len), d_FRAME_LEN(frame_len), d_BLANC_BITS(blanc_bits)
    {
    
    	d_stop = false;
    
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->process_frame(msg); });
    }
    
    trunk_tail_impl::~trunk_tail_impl () {
    	return;
    }
    
    
    
    void trunk_tail_impl::process_frame(pmt::pmt_t msg_in) {
    
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		d_stop = true;
    
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING TRUNK TAIL: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING TRUNK TAIL: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a blob
    	if(!pmt::is_f32vector(msg)) {
    		fprintf(stderr,"WARNING TRUNK TAIL: expecting message data of type f32vector, skipping.\n");
    		return;
    	}
    	
    	pmt::pmt_t msg_out_data = pmt::make_f32vector(d_FRAME_LEN-d_TRUNK_LEN, 0.0f);
    	
    	for(size_t i=0;i<d_FRAME_LEN - d_TRUNK_LEN - d_BLANC_BITS;i++) { // don't copy last 12 bits (d_BLANC_BITS) so they stay 0.0 and thus have no weight for the viterbi decoder
    		pmt::f32vector_set(msg_out_data, i, pmt::f32vector_ref(msg, i));
    	}
    
    	// TODO Check frame length
    	
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	message_port_pub( pmt::mp("out"), msg_out );
    
    }
    
    int trunk_tail_impl::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
    	return (d_stop ? -1 : 0);
    }

  } // namespace ccsds
} // namespace gr
