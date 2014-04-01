#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rs_encode_impl.h"
#include <interleaving_aux.h>
#include <gnuradio/io_signature.h>
#include <cstdio>


namespace gr {
  namespace ccsds {
    
    rs_encode::sptr 
    rs_encode::make(const unsigned int I) {
        return gnuradio::get_initial_sptr (new rs_encode_impl(I) );
    }
    
    rs_encode_impl::rs_encode_impl(const unsigned int I)
      : gr::block ("rs_encode",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), 
    	d_I(I), 
    	d_k(223), 
    	d_n(255), 
    	d_2E(32), 
    	d_DATA_LEN(d_k*d_I), 
    	d_PARITY_LEN(d_2E*d_I), 
    	d_OUT_LEN(d_DATA_LEN+d_PARITY_LEN)
    {
    	// create buffers
    	d_buf_data    = new unsigned char[ d_DATA_LEN ];
    	d_buf_parity  = new unsigned char[ d_PARITY_LEN ];
    	d_buf_parity2 = new unsigned char[ d_PARITY_LEN ];
    	d_buf_out     = new unsigned char[ d_OUT_LEN ];
    
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&rs_encode_impl::process_frame, this, _1));
    }
    
    rs_encode_impl::~rs_encode_impl () {
    	delete[] d_buf_data;
    	delete[] d_buf_parity;
    	delete[] d_buf_parity2;
    	delete[] d_buf_out;
    }
    
    void rs_encode_impl::process_frame(pmt::pmt_t msg_in) {
    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		return;
    	}
    
    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING RS DECODE: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING RS DECODE: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a blob
    	if(!pmt::is_blob(msg)) {
    		fprintf(stderr,"WARNING RS DECODE: expecting message data of type blob, skipping.\n");
    		return;
    	}
    
    	const unsigned int blob_len = (unsigned int) pmt::blob_length(msg);
    
    	if(blob_len < d_DATA_LEN) {
    		fprintf(stderr,"ERROR RS ENCODE: blob message length of %u bytes is smaller than the expected length of %u bytes.\n", blob_len, d_DATA_LEN);
    		return;
    	} else if(blob_len != d_DATA_LEN) {
    		fprintf(stderr,"WARNING RS ENCODE: blob message length of %u bytes does not match the expected length of %u bytes.\n", blob_len, d_DATA_LEN);
    	}
    
    
    	// Message is BLOB
    	const unsigned char *data_in = (const unsigned char *) pmt::blob_data(msg);
    
    	// instead of performing the second step of the interleaving where we
    	// would mainly copy data arround to get it back into the order it came
    	// in, we copy the input data to the output buffer and just perform the 
    	// second interleaving step for the parity bits to get them into the
    	// right order.
    
    	// start interleaving (1st step)
    	for(unsigned int i=0;i<d_DATA_LEN;i++) {
    		d_buf_data[i] = data_in[ interl_indx(i, d_I, d_DATA_LEN) ];
    	}
    
    	// encode
    	for(unsigned int i=0;i<d_I;i++) {
    		encode_rs_ccsds(&d_buf_data[i*d_k], &d_buf_parity[i*d_2E], 0); // use no padding
    	}
    
    	// perform interleaving (2nd step) only for parity
    	for(unsigned int i=0;i<d_PARITY_LEN;i++) {
    		d_buf_parity2[interl_indx(i, d_I, d_PARITY_LEN)] = d_buf_parity[i];
    	}
    
    	// copy blocks
    	memcpy(d_buf_out, data_in, d_DATA_LEN);
    	memcpy(&d_buf_out[d_DATA_LEN], d_buf_parity2, d_PARITY_LEN);
    
    	// create output message
    	pmt::pmt_t msg_out_data = pmt::make_blob(d_buf_out, d_OUT_LEN);
    
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	// send generated codeblock
    	message_port_pub( pmt::mp("out"), msg_out );
    }

  } // namespace ccsds
} // namespace gr
