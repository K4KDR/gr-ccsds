#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "randomiser_softbits_impl.h"
#include <stdio.h>

namespace gr {
  namespace ccsds {
    randomiser_softbits::sptr randomiser_softbits::make(const uint8_t polynomial, const uint8_t seed) {
        return gnuradio::get_initial_sptr (new randomiser_softbits_impl(polynomial, seed) );
    }
    
    randomiser_softbits_impl::randomiser_softbits_impl(const uint8_t polynomial, const uint8_t seed)
      : gr::sync_block ("randomiser_softbits",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), d_POLY(polynomial), d_SEED(seed)
    {
    	d_stop = false;
    
    	d_state = d_SEED;
    
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&randomiser_softbits_impl::process_frame, this, _1));
    }
    
    randomiser_softbits_impl::~randomiser_softbits_impl () {
    }
    
    void randomiser_softbits_impl::reset(void) {
    	d_state = d_SEED;
    }
    
    uint8_t randomiser_softbits_impl::get_next_byte(void) {
    	
    	uint8_t out = 0x00;
    
    	// loop 8 times to collect a new byte
    	for(unsigned int i=0;i<8;i++) {
    		// Bits from all registers that generate the next input bits
    		uint8_t input_bits = d_state & d_POLY;
    
    		// Buffer to store the XORed input bit in the LSB position
    		uint8_t input_bit = 0x00;
    
    		// go through all positions in input_bits and summ them up
    		for(unsigned int j=0;j<8;j++) {
    			input_bit ^= (input_bits>>j) & 0x01;
    		}
    
    		// collect the MSB as output before it is shifted out of scope
    		out = (out<<1) | ((d_state>>7) & 0x01);
    
    		// perform the shift and add new bit
    		d_state = (d_state<<1) | input_bit;
    	}
    
    	// The state has been updated 8 times now, output the collected bits
    	return out;
    }
    
    void randomiser_softbits_impl::process_frame(pmt::pmt_t msg_in) {
    
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		d_stop = true;
    
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING RANDOMISER: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING RANDOMISER: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a float vector
    	if(!pmt::is_f32vector(msg)) {
    		fprintf(stderr,"WARNING RANDOMISER: expecting message data of type f32 vector, skipping.\n");
    		return;
    	}
    
    	// Message is f32_vec
    	const unsigned int NUM_BITS = pmt::length(msg);
    	size_t len;
    	// Assign input and output pointer
    	const float *data_in = (const float *) pmt::f32vector_elements(msg, len);
    	boost::shared_ptr<float[]> data_out = boost::shared_ptr<float[]>(new float[NUM_BITS]);
    
    	// Reset state before new block
    	reset();
    
    	// Go through all bytes of the block
		uint8_t prn_byte;
		float prn_bit;
    	for(unsigned int i=0;i<NUM_BITS; i+=8) {
			prn_byte = get_next_byte();
			
			for(unsigned int j=0;j<8;j++) {
				prn_bit = ((prn_byte >> (7-j)) & 0x01) ? -1.0f : 1.0f;
				data_out[i+j] = (data_in[i+j] * prn_bit);
			}
    	}
    
    	// Generate output message data
    	pmt::pmt_t msg_out_data = pmt::init_f32vector(NUM_BITS, data_out.get());
    
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	// Post message
    	message_port_pub( pmt::mp("out"), msg_out );
    }
    
    int randomiser_softbits_impl::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
    	return (d_stop ? -1 : 0);
    }

  } // namespace ccsds
}  // namespace gr
