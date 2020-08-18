#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "softbit_msg_sink_f_impl.h"
#include <gnuradio/io_signature.h>
#include "ccsds_utils.h"


namespace gr {
  namespace ccsds {

    softbit_msg_sink_f::sptr 
    softbit_msg_sink_f::make(const unsigned int frame_len) {
        return gnuradio::get_initial_sptr (new softbit_msg_sink_f_impl(frame_len) );
    }
    
    softbit_msg_sink_f_impl::softbit_msg_sink_f_impl (const unsigned int frame_len)
      : gr::sync_block ("softbit_msg_sink_f",
    	gr::io_signature::make (1, 1, sizeof(float)),
    	gr::io_signature::make (0, 0, sizeof(float))), d_FRAME_LEN(frame_len)
    {
    	// register input type
    	message_port_register_out(pmt::mp("out"));
    
    	d_buffer = new float[d_FRAME_LEN];
    	d_buffer_count = 0;
    }
    
    softbit_msg_sink_f_impl::~softbit_msg_sink_f_impl () {
    	delete[] d_buffer;
    }
    
    bool softbit_msg_sink_f_impl::stop(void) {
    	// Signal EOF
    	message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    	return true;
    }
    
    int softbit_msg_sink_f_impl::work (int                 noutput_items,
                                    gr_vector_const_void_star&   input_items,
                                    gr_vector_void_star&         /*output_items*/)
    {
    	const float *in = (const float *) input_items[0];
    
    	unsigned int to_process = static_cast<unsigned int>(noutput_items);
    
    	while(to_process > 0) {
    
    		// fill buffer
    
    		// how many bytes to copy before either the buffer is full, or
    		// we processed all input samples?
    		const unsigned int num_copy = std::min(to_process, d_FRAME_LEN-d_buffer_count);
    
    		// index of the input stream from where to start copying.
    		const unsigned int num_processed = utils::minus_cap_zero(static_cast<unsigned int>(noutput_items), to_process);
    
    		// fill buffer
    		memcpy(&d_buffer[d_buffer_count], &in[num_processed], num_copy*sizeof(float));
    
    		// updated counters
    		d_buffer_count += num_copy;
    		to_process -= num_copy;
    
    
    		// empty buffer, if full
    		if(d_buffer_count >= d_FRAME_LEN) {
    			// buffer is full, empty it
    			
    			// create message data
    			pmt::pmt_t msg_out_data = pmt::init_f32vector(d_FRAME_LEN, d_buffer);
    
    			// Construct the new message using the received header
    			pmt::pmt_t msg_out = pmt::cons(pmt::make_dict(), msg_out_data);
    
    			// send message out
    			message_port_pub( pmt::mp("out"), msg_out );
    
    			// reset counter
    			d_buffer_count -= d_FRAME_LEN;
    		}
    	}
    	
    	// We are a sink, so no data generated, but we are a sync block, so tell
    	// runtime how many inputs we consumed.
    	return noutput_items;
    }

  } // namespace ccsds
} // namespace gr
