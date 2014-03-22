#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "reversebits_impl.h"
#include <stdio.h>

namespace gr {
  namespace ccsds {
    reversebits::sptr reversebits::make(const size_t msg_length, const std::vector<size_t> reverse_indices) {
        return gnuradio::get_initial_sptr (new reversebits_impl(msg_length, reverse_indices) );
    }

    reversebits_impl::reversebits_impl(const size_t msg_length, const std::vector<size_t> reverse_indices)
      : gr::sync_block ("reversebits",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), d_MSG_LEN(msg_length), d_POSITIONS(reverse_indices)
    {
    	d_stop = false;

    	// register output port
    	message_port_register_out(pmt::mp("out"));

    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), boost::bind(&reversebits_impl::process_frame, this, _1));
    }

    reversebits_impl::~reversebits_impl () {
    }

    uint8_t reversebits_impl::reverse_bits(const uint8_t in) {
	// output
	uint8_t out = 0x00;

	// iterate from (input) LSB (i=0) to MSB (i=7)
	for(uint8_t i=0;i<8;i++) {
		out = (out << 1) | ( (in >> i) & 0x01 );
	}
	return out;
    }

    void reversebits_impl::process_frame(pmt::pmt_t msg_in) {

    	// check for EOF
    	if(pmt::is_eof_object(msg_in)) {
    		message_port_pub( pmt::mp("out"), pmt::PMT_EOF );
    		d_stop = true;

    		return;
    	}

    	// check that input is a pair value
    	if(!pmt::is_pair(msg_in)) {
    		fprintf(stderr,"WARNING REVERSEBITS: expecting message of type pair, skipping.\n");
    		return;
    	}

    	const pmt::pmt_t hdr = pmt::car(msg_in);
    	const pmt::pmt_t msg = pmt::cdr(msg_in);

    	// check that input header is a dictionary
    	if(!pmt::is_dict(hdr)) {
    		fprintf(stderr,"WARNING REVERSEBITS: expecting message header of type dict, skipping.\n");
    		return;
    	}

    	// check that input data is blob
    	if(!pmt::is_blob(msg)) {
    		fprintf(stderr,"WARNING REVERSEBITS: expecting message data of type blob, skipping.\n");
    		return;
    	}

	// check message length
    	const unsigned int BLOB_LEN = pmt::length(msg);
	if(BLOB_LEN != d_MSG_LEN) {
		fprintf(stderr,"WARNING REVERSEBITS: message of length %lu does not match expected length of %lu, skipping.\n", (long unsigned int)BLOB_LEN, (long unsigned int)d_MSG_LEN);
		return;
	}

    	// Message is BLOB with correct length

    	// Assign input and output pointer
    	const uint8_t *data_in = (const uint8_t *) pmt::blob_data(msg);
	uint8_t data_out[d_MSG_LEN];

	// Copy message, reverse bits afterwards
	memcpy(data_out, data_in, d_MSG_LEN);

	// Iterate over all bytes to reverse
	for(std::vector<size_t>::const_iterator it = d_POSITIONS.begin(); it != d_POSITIONS.end(); ++it) {
		// Check if index is within our message
		if(*it >= d_MSG_LEN || *it < 0) {
			fprintf(stderr, "WARNING REVERSEBITS: attempting to reverse bits of byte at position %lu, which is outside the message, skipping.\n", (long unsigned int)*it);
			continue;
		}

		// reverse bits
		data_out[*it] = reverse_bits(data_in[*it]);
	}

    	// Generate output message data
    	pmt::pmt_t msg_out_data = pmt::make_blob(data_out, BLOB_LEN);

    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);

    	// Post message
    	message_port_pub( pmt::mp("out"), msg_out );
    }

    int reversebits_impl::work(int /*noutput_items*/, gr_vector_const_void_star& /*input_items*/, gr_vector_void_star& /*output_items*/) {
    	return (d_stop ? -1 : 0);
    }

  } // namespace ccsds
}  // namespace gr
