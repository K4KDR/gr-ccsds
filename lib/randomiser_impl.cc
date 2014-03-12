#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "randomiser_impl.h"
#include <stdio.h>

namespace gr {
  namespace ccsds {
    randomiser::sptr randomiser::make(const uint8_t polynomial, const uint8_t seed) {
        return gnuradio::get_initial_sptr (new randomiser_impl(polynomial, seed) );
    }
    
    randomiser_impl::randomiser_impl(const uint8_t polynomial, const uint8_t seed)
      : gr::sync_block ("randomiser",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), d_POLY(polynomial), d_SEED(seed)
    {
    	d_stop = false;
    
    	d_state = d_SEED;
    
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&randomiser_impl::process_frame, this, _1));
    }
    
    randomiser_impl::~randomiser_impl () {
    }
    
    void randomiser_impl::reset(void) {
    	d_state = d_SEED;
    }
    
    uint8_t randomiser_impl::get_next_byte(void) {
    	
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
    
    void randomiser_impl::process_frame(pmt::pmt_t msg_in) {
    
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
    	if(!pmt::is_blob(msg)) {
    		fprintf(stderr,"WARNING RANDOMISER: expecting message data of type blob, skipping.\n");
    		return;
    	}
    
    	// Message is BLOB
    	const unsigned int BLOB_LEN = pmt::length(msg);
    
    	// Assign input and output pointer
    	const uint8_t *data_in = (const uint8_t *) pmt::blob_data(msg);
    	boost::shared_ptr<uint8_t[]> data_out = boost::shared_ptr<uint8_t[]>(new uint8_t[BLOB_LEN]);
    
    	// Reset state before new block
    	reset();
    
    	// Go through all bytes of the block
    	for(unsigned int i=0;i<BLOB_LEN;i++) {
    		data_out[i] = (data_in[i] ^ get_next_byte());
    	}
    
    	// Generate output message data
    	pmt::pmt_t msg_out_data = pmt::make_blob(data_out.get(), BLOB_LEN);
    
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	// Post message
    	message_port_pub( pmt::mp("out"), msg_out );
    }
    
    int randomiser_impl::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
    	return (d_stop ? -1 : 0);
    }

  } // namespace ccsds
}  // namespace gr
