#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "add_asm_impl.h"
#include "hexstring_to_binary.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace ccsds {


    add_asm::sptr 
    add_asm::make(std::string ASM, const unsigned int frame_len)
    {
        return gnuradio::get_initial_sptr 
	  (new add_asm_impl(ASM, frame_len) );
    }
    
    add_asm_impl::add_asm_impl(std::string ASM, const unsigned int frame_len)
      : gr::sync_block ("add_asm",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), d_ASM_LEN(std::floor(ASM.length()/2)), d_FRAME_LEN(frame_len)
    {
    	// Transfer ASM from Hex to bytes
    	d_ASM = new unsigned char[d_ASM_LEN];
    	hexstring_to_binary(&ASM, d_ASM);
    
    	d_stop = false;
    
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&add_asm_impl::process_frame, this, _1));
    }
    
    add_asm_impl::~add_asm_impl () {
    	delete[] d_ASM;
    }
    
    void add_asm_impl::process_frame(pmt::pmt_t msg_in) {
    
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		d_stop = true;
    
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING ADD ASM: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING ADD ASM: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a blob
    	if(!pmt::is_blob(msg)) {
    		fprintf(stderr,"WARNING ADD ASM: expecting message data of type blob, skipping.\n");
    		return;
    	}
    
    	// Message is BLOB
    	const unsigned char *data_in = (const unsigned char *) pmt::blob_data(msg);
    
    	unsigned char *data_out = new unsigned char[d_ASM_LEN+d_FRAME_LEN];
    
    	memcpy(&data_out[0]        , d_ASM  , d_ASM_LEN  *sizeof(unsigned char));
    	memcpy(&data_out[d_ASM_LEN], data_in, d_FRAME_LEN*sizeof(unsigned char));
    
    	pmt::pmt_t msg_out_data = pmt::make_blob(data_out, d_ASM_LEN+d_FRAME_LEN);
    
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	message_port_pub( pmt::mp("out"), msg_out );
    
    	delete[] data_out;
    }
    
    int add_asm_impl::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
    	return (d_stop ? -1 : 0);
    }
  } // namespace ccsds
} // namespace gr

