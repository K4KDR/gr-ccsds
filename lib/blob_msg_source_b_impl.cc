#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "blob_msg_source_b_impl.h"
//#include <gr_io_signature.h>
//#include <gr_msg_queue.h>
//#include <gr_message.h>

namespace gr {
  namespace ccsds {
    
    blob_msg_source_b::sptr 
    blob_msg_source_b::make(const unsigned int blob_len) {
        return gnuradio::get_initial_sptr (new blob_msg_source_b_impl(blob_len) );
    }
    
    blob_msg_source_b_impl::blob_msg_source_b_impl (const unsigned int blob_len)
      : gr::sync_block ("blob_msg_source_b",
    	gr::io_signature::make (0, 0, sizeof(unsigned char)),
    	gr::io_signature::make (1, 1, sizeof(unsigned char))), d_BLOB_LEN(blob_len)
    {
    	d_stop = false;
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&blob_msg_source_b_impl::process_message, this, _1));
    }
    
    blob_msg_source_b_impl::~blob_msg_source_b_impl () {
    	// nothing to do
    }
    
    bool blob_msg_source_b_impl::start(void) {
    	d_astart.start();
    	return true;
    }
    
    bool blob_msg_source_b_impl::stop(void) {
    	d_astart.stop();
    	return true;
    }
    
    void blob_msg_source_b_impl::process_message(pmt::pmt_t msg_in) {
    
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
    		fprintf(stderr,"WARNING MSG BLOB SOURCE: expecting message of type pair, skipping.\n");
    		return;
    	}
    
    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);
    
    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING MSG BLOB SOURCE: expecting message header of type dict, skipping.\n");
    		return;
    	}
    
    	// check that input data is a float vector
    	if(!pmt::is_blob(msg)) {
    		fprintf(stderr,"WARNING MSG BLOB SOURCE: expecting message data of type blob, skipping.\n");
    		return;
    	}
    
    	// check if message has right length
    	if(pmt::length(msg) != d_BLOB_LEN) {
    		fprintf(stderr,"WARNING MSG BLOB SOURCE: message of length %u does not match the expected length of %u, skipping.\n",pmt::length(msg), d_BLOB_LEN);
    		return;
    	}
    
    	// Message is BLOB and has the right length
    
    	// pointer to frame
    	const unsigned char *data_in = (const unsigned char *) pmt::blob_data(msg);
    
    	for(unsigned int i=0;i<d_BLOB_LEN;i++) {
    		d_queue.push(data_in[i]);
    	}
    
    	return;
    }
    
    int blob_msg_source_b_impl::work (int		               noutput_items,
    				gr_vector_const_void_star&     /*input_items*/,
                                    gr_vector_void_star&           output_items)
    {
    
    	if(d_stop && d_queue.size() == 0) {
    		// signal EOF
    		return -1;
    	}
    
    	unsigned char *out = (unsigned char *) output_items[0];
    
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
