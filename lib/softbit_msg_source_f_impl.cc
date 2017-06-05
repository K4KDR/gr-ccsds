#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "softbit_msg_source_f_impl.h"
//#include <gr_io_signature.h>
//#include <gr_msg_queue.h>
//#include <gr_message.h>

namespace gr {
  namespace ccsds {
    
    softbit_msg_source_f::sptr 
    softbit_msg_source_f::make(const unsigned int frame_len) {
        return gnuradio::get_initial_sptr (new softbit_msg_source_f_impl(frame_len) );
    }
    
    softbit_msg_source_f_impl::softbit_msg_source_f_impl (const unsigned int frame_len)
      : gr::sync_block ("softbit_msg_source_f",
    	gr::io_signature::make (0, 0, sizeof(float)),
    	gr::io_signature::make (1, 1, sizeof(float))), d_FRAME_LEN(frame_len)
    {
    	d_stop = false;
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&softbit_msg_source_f_impl::process_message, this, _1));
    }
    
    softbit_msg_source_f_impl::~softbit_msg_source_f_impl () {
    	// nothing to do
    }
    
    bool softbit_msg_source_f_impl::start(void) {
    	d_astart.start();
    	return true;
    }
    
    bool softbit_msg_source_f_impl::stop(void) {
    	d_astart.stop();
    	return true;
    }
    
    void softbit_msg_source_f_impl::process_message(pmt::pmt_t msg_in) {
    
    	if(d_stop) {
    		// Do not accept new messages after EOF
    		return;
    	}
    
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		d_stop = true;
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING MSG SOFTBIT SOURCE: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING MSG SOFTBIT SOURCE: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a float vector
    	if(!pmt::is_f32vector(msg)) {
    		fprintf(stderr,"WARNING MSG SOFTBIT SOURCE: expecting message data of type f32vector, skipping.\n");
    		return;
    	}
    
    	// check if message has right length
    	if(pmt::length(msg) != d_FRAME_LEN) {
    		fprintf(stderr,"WARNING MSG SOFTBIT SOURCE: message of length %lu does not match the expected length of %u, skipping.\n",(long unsigned int)pmt::length(msg), d_FRAME_LEN);
    		return;
    	}
    
    	// Message is f32vector and has the right length
    	for(unsigned int i=0;i<d_FRAME_LEN;i++) {
    		d_queue.push(pmt::f32vector_ref(msg, i));
    	}
    
    	return;
    }
    
    int softbit_msg_source_f_impl::work (int		               noutput_items,
    				gr_vector_const_void_star&     /*input_items*/,
                                    gr_vector_void_star&           output_items)
    {
    
    	if(d_stop && d_queue.size() == 0) {
    		// signal EOF
    		return -1;
    	}
    
    	float *out = (float *) output_items[0];
    
    	const unsigned int num_out = std::min(noutput_items,(int)d_queue.size());
    
    	for(unsigned int i=0;i<num_out;i++) {
    		// Pull element from queue
    		out[i] = d_queue.front();
    
    		// Delete element from queue
    		d_queue.pop();
    	}
    
    	return num_out;
    }

  } // namespace ccsds 
} // namespace gr
