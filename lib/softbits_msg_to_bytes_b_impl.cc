#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "softbits_msg_to_bytes_b_impl.h"

#include <ccsds/softbits.h>

#include <gnuradio/io_signature.h>


namespace gr {
  namespace ccsds {

    softbits_msg_to_bytes_b::sptr 
    softbits_msg_to_bytes_b::make() {
        return gnuradio::get_initial_sptr 
	  (new softbits_msg_to_bytes_b_impl() );
    }
    
    softbits_msg_to_bytes_b_impl::softbits_msg_to_bytes_b_impl ()
      : gr::sync_block ("softbits_msg_to_bytes_b",
    	gr::io_signature::make (0, 0, sizeof(unsigned char)),
    	gr::io_signature::make (1, 1, sizeof(unsigned char)))
    {
    	d_stop = false;
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&softbits_msg_to_bytes_b_impl::process_message, this, _1));
    }
    
    softbits_msg_to_bytes_b_impl::~softbits_msg_to_bytes_b_impl () {
    	// nothing to do
    }
    
    void softbits_msg_to_bytes_b_impl::process_message(pmt::pmt_t msg_in) {
    
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		d_stop = true;
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING SOFTBIT MSG TO BYTES: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING SOFTBIT MSG TO BYTES: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a float vector
    	if(!pmt::is_f32vector(msg)) {
    		fprintf(stderr,"WARNING SOFTBIT MSG TO BYTES: expecting message data of type f32vector, skipping.\n");
    		return;
    	}
    
    	const unsigned int num_bits = pmt::length(msg);
    
    	if(num_bits % 8 != 0) {
    		fprintf(stderr,"WARNING SOFTBIT MSG TO BYTES: expecting message to contain a integer number of bytes. Actual length %d bits, last byte will be padded with zeros.\n", num_bits);
    	}
    
    	// Message is a f32vector with an integer multiple of 8 bits
    
    	const unsigned int num_bytes = std::ceil(num_bits/8.0);
    
    	// go through all output bytes
    	for(unsigned int i=0;i<num_bytes;i++) {
    
    		uint8_t byte = 0x00;
    
    		// go through all bits of the current byte
    		for(unsigned int j=0;j<8;j++) {
    			// convert soft bit to hard decision
    			const uint8_t bit = (8*i+j < num_bits) ? softbits::hard_decision(pmt::f32vector_ref(msg,i*8+j)) : 0u;
    
    			// add bit to output
    			byte = (byte<<1) | bit;
    		}
    
    		d_queue.push(byte);
    	}
    
    	return;
    }
    
    int softbits_msg_to_bytes_b_impl::work (int                 noutput_items,
                                    gr_vector_const_void_star&   /*input_items*/,
                                    gr_vector_void_star&         output_items)
    {
    
    	if(d_stop && d_queue.size() == 0) {
    		// signal EOF
    		return -1;
    	}
    
    	unsigned char *out = (unsigned char *) output_items[0];
    
    	unsigned int num_out;
    
    	for(num_out = 0;num_out<(unsigned int)noutput_items && d_queue.size()>0;num_out++) {
    		out[num_out] = d_queue.front();
    		d_queue.pop();
    	}
    
    	// Tell runtime system how many output items we produced
    	return num_out;
    }
    
  } // namespace ccsds
} // namespace gr
