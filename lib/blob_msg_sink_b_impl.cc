#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "blob_msg_sink_b_impl.h"
#include "ccsds_utils.h"
#include <gnuradio/io_signature.h>



namespace gr {
  namespace ccsds {

    blob_msg_sink_b::sptr 
    blob_msg_sink_b::make(const unsigned int blob_len) {
        return gnuradio::get_initial_sptr (new blob_msg_sink_b_impl(blob_len) );
    }
    
    blob_msg_sink_b_impl::blob_msg_sink_b_impl (const unsigned int blob_len)
      : gr::sync_block ("blob_msg_sink_b",
    	gr::io_signature::make (1, 1, sizeof(unsigned char)),
    	gr::io_signature::make (0, 0, sizeof(unsigned char))), d_BLOB_LEN(blob_len)
    {
    	// register input type
    	message_port_register_out(pmt::mp("out"));
    
    	d_buffer = new unsigned char[d_BLOB_LEN];
    	d_buffer_count = 0;
    }
    
    blob_msg_sink_b_impl::~blob_msg_sink_b_impl () {
    	delete[] d_buffer;
    }
    
    bool blob_msg_sink_b_impl::stop(void) {
    	// Signal EOF
    	message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    	return true;
    }
    
    int blob_msg_sink_b_impl::work (int                 noutput_items,
                                    gr_vector_const_void_star&   input_items,
                                    gr_vector_void_star&         /*output_items*/)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
    
        const unsigned int num_out = static_cast<unsigned int>(noutput_items);
        
        unsigned int num_processed = 0u;
        while(utils::minus_cap_zero(num_out, num_processed) > 0u) {
    
    		// fill buffer
    
    		// how many bytes to copy before either the buffer is full, or
    		// we processed all input samples?
    		const unsigned int num_copy = utils::pick_smaller(
				utils::minus_cap_zero(num_out, num_processed),
				utils::minus_cap_zero(d_BLOB_LEN, d_buffer_count)
			);
			
    		// fill buffer
    		memcpy(&d_buffer[d_buffer_count], &in[num_processed], num_copy);
    
    		// updated counters
    		d_buffer_count += num_copy;
			num_processed += num_copy;
    
    
    		// empty buffer, if full
    		if(d_buffer_count >= d_BLOB_LEN) {
    			// buffer is full, empty it
    			
    			// create message data
    			pmt::pmt_t msg_out_data = pmt::make_blob(d_buffer, d_BLOB_LEN);
    
    			// Construct the new message using the received header
    			pmt::pmt_t msg_out = pmt::cons(pmt::make_dict(), msg_out_data);
    
    			// send message out
    			message_port_pub( pmt::mp("out"), msg_out );
    
    			// reset counter
    			d_buffer_count = 0;
    		}
    	}
    	
    	// We are a sink, so no data generated, but we are a sync block, so tell
    	// runtime how many inputs we consumed.
    	return noutput_items;
    }

  } // namespace ccsds
} // namespace gr
