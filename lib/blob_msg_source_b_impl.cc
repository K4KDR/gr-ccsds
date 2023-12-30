#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "blob_msg_source_b_impl.h"
#include "ccsds_utils.h"

namespace gr {
  namespace ccsds {
    
    blob_msg_source_b::sptr 
    blob_msg_source_b::make(const unsigned int blob_len, const std::string length_tag_name) {
        return gnuradio::get_initial_sptr (new blob_msg_source_b_impl(blob_len, length_tag_name) );
    }
    
    blob_msg_source_b_impl::blob_msg_source_b_impl (const unsigned int blob_len, const std::string length_tag_name)
      : gr::sync_block ("blob_msg_source_b",
    	gr::io_signature::make (0, 0, sizeof(unsigned char)),
    	gr::io_signature::make (1, 1, sizeof(unsigned char))),
    	d_BLOB_LEN(blob_len),
    	d_LENGTH_TAG_KEY(pmt::string_to_symbol(length_tag_name)),
    	d_LENGTH_TAG_VALUE(pmt::from_uint64(blob_len))
    {
    	d_stop = false;
        d_num_bytes_copied = 0lu;
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->process_message(msg); });
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
    		fprintf(stderr,"WARNING MSG BLOB SOURCE: message of length %lu does not match the expected length of %u, skipping.\n", pmt::length(msg), d_BLOB_LEN);
    		return;
    	}
    
    	// Message is BLOB and has the right length
    
    	// pointer to frame
    	const unsigned char *data_in = (const unsigned char *) pmt::blob_data(msg);
    
    	for(unsigned int i=0;i<d_BLOB_LEN;i++) {
    		d_queue.push(data_in[i]);
    	}
    	
    	d_metadata_queue.push(hdr);
        
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
    
    	const size_t num_out = utils::pick_smaller(static_cast<size_t>(noutput_items),d_queue.size());
        
        for(size_t i=0;i<num_out;i++) {
            if ((d_num_bytes_copied % static_cast<uint64_t>(d_BLOB_LEN)) == 0lu) {
                add_item_tag(0, d_num_bytes_copied, d_LENGTH_TAG_KEY, d_LENGTH_TAG_VALUE);

                pmt::pmt_t hdr_items = pmt::dict_items(d_metadata_queue.front());
                const size_t len = pmt::length(hdr_items);
                for (size_t i=0lu; i<len; i++) {
                    pmt::pmt_t kvpair = pmt::nth(i, hdr_items);
                    
                    add_item_tag(0, d_num_bytes_copied, pmt::car(kvpair), pmt::cdr(kvpair));
                }
                d_metadata_queue.pop();
            }
            
    		// Pull element from queue
    		out[i] = d_queue.front();
            d_num_bytes_copied++;
            
    		// Delete element from queue
    		d_queue.pop();
    	}
    
    	return static_cast<int>(num_out);
    }

  } // namespace ccsds 
} // namespace gr
