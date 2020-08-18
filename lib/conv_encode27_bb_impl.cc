#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "conv_encode27_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {

    conv_encode27_bb::sptr
    conv_encode27_bb::make (const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, conv_puncturing27::punct_t puncturing_type) {
        return gnuradio::get_initial_sptr (new conv_encode27_bb_impl (gen_poly_c1, gen_poly_c2, puncturing_type));
    }
    
    float conv_encode27_bb_impl::get_rate(conv_puncturing27::punct_t puncturing_type) {
    	float rate;
    	switch(puncturing_type) {
    		case conv_puncturing27::NONE:	 	rate = 1.0f/2.0f; break;
    		case conv_puncturing27::ECSS_23:	rate = 2.0f/3.0f; break;
    		case conv_puncturing27::ECSS_34:	rate = 3.0f/4.0f; break;
    		case conv_puncturing27::ECSS_56:	rate = 5.0f/6.0f; break;
    		case conv_puncturing27::ECSS_78:	rate = 7.0f/8.0f; break;
    		default:			rate = 0.0f;    fprintf(stderr,"ERROR CONV ENCODE: unknown puncturing type.\n");
    	}
    	return rate;
    }
    
    conv_encode27_bb_impl::conv_encode27_bb_impl(const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, conv_puncturing27::punct_t puncturing_type)
      : gr::block ("ccsds_conv_encode27_bb",
    	gr::io_signature::make (1, 1, sizeof(unsigned char)),
    	gr::io_signature::make (1, 1, sizeof(unsigned char))),
        conv_encode27_aux(gen_poly_c1, gen_poly_c2, puncturing_type),
        d_RATE(get_rate(puncturing_type))
    {
    	d_buffer_count = 0;
    	d_buffer = 0x0000;
    }
    
    conv_encode27_bb_impl::~conv_encode27_bb_impl ()
    {
    	// nothing to do
    }
    
    void conv_encode27_bb_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
    	const size_t num_bytes_req = static_cast<size_t>(noutput_items)*8lu-d_buffer_count;
		const float num_input_bytes = static_cast<float>(num_bytes_req) * d_RATE/8.0f;
		ninput_items_required[0] = static_cast<int>(std::ceil(num_input_bytes));
    	//printf("CONVE ENCODE forecast: need %d inputs for %d output. Still have %u bits in buffer\n",ninput_items_required[0], noutput_items, d_buffer_count);
    	return;
    }
    
    int  conv_encode27_bb_impl::general_work (int       noutput_items,
    			 gr_vector_int 		     &ninput_items,
                             gr_vector_const_void_star   &input_items,
                             gr_vector_void_star         &output_items)
    {
    	const unsigned char *in = (const unsigned char *) input_items[0];
    	unsigned char *out = (unsigned char *) output_items[0];
    
    	//printf("CONV ENCODE27: %d bytes requested, %d bytes given, %d bits in buffer\n",noutput_items, ninput_items[0], d_buffer_count);
    
    	unsigned int num_in = 0;
    	unsigned int num_out = 0;
    
    	// is there still a full byte stored from last time?	
    	if(d_buffer_count >= 8u) {
    		// do we have memory to output?
    		if(noutput_items > 0) {
    			// output buffer
    			out[0] = uint8_t ((d_buffer >> 8) & 0x00FF);
    			// increase counter
    			num_out++;
    			// shift still unused bits at MSB position in buffer
    			d_buffer = static_cast<uint16_t>(d_buffer << 8);
    			// update buffer counter
    			d_buffer_count -= 8u;
    		} else {
    			// no output memory
    			fprintf(stderr,"ERROR CONV ENCODE: called general_work() but no output memory allocated .\n");
    			return 0;
    		}
    	}
    
    	// At this point d_buffer contains at most 7 bits.
    
    	while((unsigned int)ninput_items[0] > num_in && (unsigned int)noutput_items > num_out) {
    		// There is at least one output byte allocated and at least one
    		// input byte available. Encode can create at most 16 new bits
    		// so we will create one or two new output bits.
    
    		uint16_t bits_enc;
    		unsigned int num_bits_out;
    
    		// read in new byte and encode it
    		encode_punct(bits_enc, num_bits_out, in[num_in]);
    		num_in++;
    
    		// are there enough bits left for a full byte and is there still memory to output it?
    		// this loop runs at least once since encode will create at least 8 bits.
    		while(d_buffer_count + num_bits_out >= 8 && (unsigned int)noutput_items > num_out) {
    			// yes, shift new bits to make place for the
    			// d_buffer_count old bytes at the front
    
    			// first make sure that the bits are left aligned
    			uint16_t new_bits = static_cast<uint16_t>(bits_enc << (16u-num_bits_out));
    			// then shift back to the right to make space for the
    			// data and make sure the left 8 bits are empty as we
    			// want to cast to byte.
    			new_bits = static_cast<uint16_t>(new_bits >> (d_buffer_count + 8u));
    
    			// add bits from buffer
    			new_bits |= static_cast<uint16_t>(d_buffer >> 8);
    
    			// update bit count
    			num_bits_out -= (8u-d_buffer_count);
    
    			// empty buffer
    			d_buffer = 0;
    			d_buffer_count = 0;
    
    			// output new byte
    			out[num_out] = (uint8_t) (new_bits & 0x00FF);
    			num_out++;
    		}
    
    		// At this point d_buffer will be empty, because the above loop
    		// will run at least once, which will empty the buffer.
    
    		// are there a few bits left that did not fit into a full byte?
    		if(num_bits_out > 0) {
    			// align them to start at the MSB of a byte.
    			d_buffer = static_cast<uint16_t>(bits_enc << (16u-num_bits_out));
    			d_buffer_count = num_bits_out;
    		}
    	}
    
    	//printf("    %u bytes read, %u bytes written, %d bits in buffer\n",num_in,num_out,d_buffer_count);
    
    	// consume input stream
    	consume_each(static_cast<int>(num_in));
    
    	// Tell runtime system how many output items we produced.
    	return static_cast<int>(num_out);
    }

  } // namespace ccsds
}  // namespace gr
