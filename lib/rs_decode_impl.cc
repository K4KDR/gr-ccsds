#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rs_decode_impl.h"
#include <interleaving_aux.h>
#include <gnuradio/io_signature.h>


namespace gr {
  namespace ccsds {
    
    rs_decode::sptr
    rs_decode::make(const unsigned int I, const repr_t representation, decoder_verbosity_t verbosity) {
        return gnuradio::get_initial_sptr (new rs_decode_impl(I,representation, verbosity) );
    }
    
    rs_decode_impl::rs_decode_impl(const unsigned int I, const repr_t representation, decoder_verbosity_t verbosity)
      : gr::block ("ccsds_rs_decode",
    	gr::io_signature::make (0, 0, 0),
    	gr::io_signature::make (0, 0, 0)), d_I(I), d_k(223), d_n(255), d_2E(32), d_DATA_LEN(d_k*d_I), d_IN_LEN(d_DATA_LEN+d_I*d_2E),
        d_VERBOSITY(verbosity),
	d_representation(representation)
    {
    	// create buffers
    	d_buf_in   = new unsigned char[ d_IN_LEN ];
    	d_buf_data = new unsigned char[ d_DATA_LEN ];
    	d_buf_out  = new unsigned char[ d_DATA_LEN ];
    
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    	// register input type
    	message_port_register_in(pmt::mp("in"));
    	set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->process_frame(msg); });
    
    	if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
            dbg_file_in 	   = fopen("/tmp/rs_decode_impl_debug_in.dat","w");
            dbg_file_in_deinterleaved = fopen("/tmp/rs_decode_impl_debug_in_deinterleaved.dat","w");
            dbg_file_in_valid  = fopen("/tmp/rs_decode_impl_debug_in_valid.dat","w");
            dbg_file_out	   = fopen("/tmp/rs_decode_impl_debug_out.dat","w");

            fprintf(dbg_file_in, "# received byte stream, splitted into data frames and combined redundancy part\n");
            fprintf(dbg_file_in_deinterleaved, "# deinterleaved byte stream as put into the RS decoder, redundancy part separated by whitespaces\n");
    	}
    }
    
    rs_decode_impl::~rs_decode_impl () {
    	delete[] d_buf_in;
    	delete[] d_buf_data;
    	delete[] d_buf_out;
    
    	if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
    		fflush(dbg_file_in);
    		fflush(dbg_file_in_deinterleaved);
    		fflush(dbg_file_in_valid);
    		fflush(dbg_file_out);
    
    		fclose(dbg_file_in);
    		fclose(dbg_file_in_deinterleaved);
    		fclose(dbg_file_in_valid);
    		fclose(dbg_file_out);
    	}
    }
    
    void rs_decode_impl::process_frame(pmt::pmt_t msg_in) {
    
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
    
    	const uint64_t frame_number = pmt::to_uint64(pmt::dict_ref(hdr, pmt::mp("frame_number"), pmt::from_uint64(0)));
    	
    	const unsigned int blob_len = (unsigned int) pmt::blob_length(msg);
    
    	if(blob_len < d_IN_LEN) {
    		fprintf(stderr,"ERROR RS DECODE: blob message length of %u bytes is smaller than the expected length of %u bytes.\n", blob_len, d_IN_LEN);
    		return;
    	} else if(blob_len != d_IN_LEN) {
    		fprintf(stderr,"WARNING RS DECODE: blob message length of %u bytes does not match the expected length of %u bytes.\n", blob_len, d_IN_LEN);
    	}
    
    	// Message is BLOB
    	const unsigned char *data_in = (const unsigned char *) pmt::blob_data(msg);
    
    	if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
           // Write input to debug file

		    // data part
            for(unsigned int i=0;i<d_I;i++) {
                for(unsigned int k=0;k<d_k;k++) {
                    fprintf(dbg_file_in, "%02X ",data_in[i*d_n+k]);
			    }
			    fprintf(dbg_file_in, "\n");
            }
            for(unsigned int i=d_I*d_k;i<d_I*d_n;i++) {
                fprintf(dbg_file_in, "%02X ",data_in[i]);
            }
            fprintf(dbg_file_in, "\n\n");
        }
    
    	// start interleaving (1st step)
    	for(unsigned int i=0;i<d_IN_LEN;i++) {
    		d_buf_in[i] = data_in[interl_indx(i, d_I, d_IN_LEN)];
    	}
    
    	// decode
    	for(unsigned int i=0;i<d_I;i++) {
            if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
                // Write deinterleaved input to debug file

                // data part
                for(unsigned int k=0;k<d_k;k++) {
                    fprintf(dbg_file_in_deinterleaved, "%02X ",d_buf_in[i*(d_k+d_2E)+k]);
                }
                fprintf(dbg_file_in_deinterleaved, "    ");
                // redundancy part
                for(unsigned int k=d_k;k<d_n;k++) {
                    fprintf(dbg_file_in_deinterleaved, "%02X ",d_buf_in[i*(d_k+d_2E)+k]);
                }
                fprintf(dbg_file_in_deinterleaved, "\n");
    		}

    		// no hints about erasures, use no padding
		int ret;
    		if(d_representation == BERLEKAMP) {
			ret = decode_rs_ccsds(&d_buf_in[i*(d_k+d_2E)], NULL, 0, 0);
		} else if(d_representation == CONVENTIONAL) {
			ret = decode_rs_8(&d_buf_in[i*(d_k+d_2E)], NULL, 0, 0);
		} else {
			ret = -1;
	    		fprintf(stderr,"WARNING RS DECODE: invalid representation type %d.\n", d_representation);
		}
    		if(ret < 0) {
    			// unable to decode, drop this codeblock
    			if (d_VERBOSITY >= DECODER_VERBOSITY_DROPPED) {
    				printf("Invalid RS codeblock, dropping frame number %lu\n",frame_number);
    			}
    			return;
    		} else {
                if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
                    printf("RS codeblock for frame number %lu is valid after %d bytes have been corrected.\n", frame_number, ret);
                } else if (d_VERBOSITY >= DECODER_VERBOSITY_ALL) {
                    printf("RS codeblock for frame number %lu is valid.\n", frame_number);
                }
    
    			// copy to data buffer (without parity) for an easier
    			// 2nd interleaving step
    			memcpy(&d_buf_data[i*d_k], &d_buf_in[i*(d_k+d_2E)], d_k);
    		}
    	}
    
        if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
            // Write newline after frame to debug file
            fprintf(dbg_file_in_deinterleaved, "\n");
        }


    	// perform interleaving (2nd step)
    	for(unsigned int i=0;i<d_DATA_LEN;i++) {
    		d_buf_out[interl_indx(i, d_I, d_DATA_LEN)] = d_buf_data[i];
    	}
    
        if (d_VERBOSITY >= DECODER_VERBOSITY_DEBUG) {
            // Write input to debug file (at this point the frame is valid)
            for(unsigned int i=0;i<blob_len;i++) {
                fprintf(dbg_file_in_valid, "%02X ",data_in[i]);
            }
            fprintf(dbg_file_in_valid, "\n");

            // Write output to debug file
            for(unsigned int i=0;i<d_DATA_LEN;i++) {
                fprintf(dbg_file_out, "%02X ",d_buf_out[i]);
            }
            fprintf(dbg_file_out, "\n");
        }
    
    	// create output message data
    	pmt::pmt_t msg_out_data = pmt::make_blob(d_buf_out, d_DATA_LEN);
    
    	// Construct the new message using the received header
    	pmt::pmt_t msg_out = pmt::cons(hdr, msg_out_data);
    
    	// send generated codeblock
    	message_port_pub( pmt::mp("out"), msg_out );
    }

  } // namespace ccsds
} // namespace gr
