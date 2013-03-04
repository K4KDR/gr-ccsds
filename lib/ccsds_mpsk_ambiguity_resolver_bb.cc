#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_mpsk_ambiguity_resolver_bb.h>
#include "ccsds_hexstring_to_binary.h"
#include "ccsds_mpsk_constellation.h"
#include <gr_io_signature.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>
#include <cstdio>



ccsds_mpsk_ambiguity_resolver_bb_sptr ccsds_make_mpsk_ambiguity_resolver_bb(const unsigned int M, std::string ASM, const unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length) {
    return ccsds_mpsk_ambiguity_resolver_bb_sptr (new ccsds_mpsk_ambiguity_resolver_bb(M, ASM, threshold, ber_threshold, frame_length) );
}

ccsds_mpsk_ambiguity_resolver_bb::ccsds_mpsk_ambiguity_resolver_bb (const unsigned int M, std::string ASM, unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length)
  : gr_sync_block ("ccsds_mpsk_ambiguity_resolver_bb",
	gr_make_io_signature (1, 1, sizeof (unsigned char)),
	gr_make_io_signature (1, 1, sizeof (unsigned char))),

	d_M(M),
	d_ldM((unsigned int)(log(M)/log(2))),
	d_THRESHOLD(threshold),
	d_FRAME_LEN(frame_length),
	d_FRAME_LEN_BITS(d_FRAME_LEN*8),
	d_ASM_LEN(std::floor(ASM.length()/2.0f)),
	d_ASM_LEN_BITS(d_ASM_LEN*8),
	d_FULL_FRAME_REMAIN_BITS( (d_ASM_LEN_BITS+d_FRAME_LEN_BITS) % d_ldM )
{
	
	// Mapping to translate from the input stream to any ambiguity stream
	// (assume the detector saw a rotated constellation)
	d_map_index_to_bits = ccsds_make_mpsk_map(M);
	d_map_bits_to_index = ccsds_make_mpsk_rev_map(M);

	// Copy ASM to private memory
	unsigned char *tmp = new unsigned char[d_ASM_LEN];
	ccsds_hexstring_to_binary(&ASM, tmp);
	d_asm_operator = new ccsds_asm_operator(tmp, d_ASM_LEN, ber_threshold, d_ldM);

	// Initialize variables
	d_state = STATE_SEARCH;
	d_count = 0;
	d_locked_on_stream = 0;
	d_offset_bytes = 0;
	d_offset_bits =0;

	// start with search len
	set_output_multiple(std::ceil(from_packed_to_unpacked(d_FRAME_LEN + 2u * d_ASM_LEN)));

	#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
		dbg_file_in = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_debug_in.dat","w");
		dbg_file_out = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_debug_out.dat","w");
		dbg_count = 0;
	#endif
}

ccsds_mpsk_ambiguity_resolver_bb::~ccsds_mpsk_ambiguity_resolver_bb () {
	//delete[] d_ASM;
	delete[] d_map_index_to_bits;
	delete[] d_map_bits_to_index;
	delete d_asm_operator;

	#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
		fflush(dbg_file_in);
		fflush(dbg_file_out);

		fclose(dbg_file_in);
		fclose(dbg_file_out);
	#endif
}

float ccsds_mpsk_ambiguity_resolver_bb::from_packed_to_unpacked(unsigned int packed) {
	return (packed * 8.0f / d_ldM);
}

void ccsds_mpsk_ambiguity_resolver_bb::confine_offsets(void) {
	while(d_offset_bits >= d_ldM) {
		d_offset_bytes++;
		d_offset_bits -= d_ldM;
	}
	return;
}

void ccsds_mpsk_ambiguity_resolver_bb::convert_ambiguity(unsigned char *out, const unsigned char *in, const unsigned int num, unsigned int ambiguity) {

	// shortcut if there is no ambiguity
	if(ambiguity == 0) {
		memcpy(out, in, num);
		return;
	}

	// there is a non zero ambiguity, so do the conversion
	for(unsigned int i=0;i<num;i++) {
		// convert bits now
		const unsigned int index = d_map_bits_to_index[in[i]];
		out[i] = d_map_index_to_bits[(d_M+index-ambiguity)%d_M];
	}
	return;
}

int  ccsds_mpsk_ambiguity_resolver_bb::work (int	noutput_items,
							gr_vector_const_void_star   &input_items,
							gr_vector_void_star         &output_items)
{
	// pointer to pointer to a const gr_complex
	const unsigned char *in = (const unsigned char *) input_items[0];
	unsigned char *out = (unsigned char *) output_items[0];
	
	// If a state detects that it needs more samples to continue, it can
	// signal an abort with this flag. A new try is done once the work
	// function is called again by the scheduler
	bool abort = false;
	
	unsigned int num_out = 0;
	
	// In order to catch at least one complete ASM (if there is one) we need
	// this many bytes.
	// Worst case: The ASM started one bit before our current observation
	// interval. In this case the next ASM starts in the last bit of the 
	// d_ASM_LEN+d_FRAME_LEN byte and we need another d_ASM_LEN byte to
	// catch the complete ASM (and the first byte of the following frame)
	const unsigned int search_len = std::ceil(from_packed_to_unpacked(d_FRAME_LEN + 2u * d_ASM_LEN));

	// How many samples do we need if locked? If ASM is not aligned we need
	// another byte for the last bit(s)
	unsigned int bytes_req = std::ceil((d_ASM_LEN_BITS + d_FRAME_LEN_BITS + d_offset_bits) / (float)d_ldM);

	// If we copy to the output stream, how many bytes do we copy?
	const unsigned int copy = std::floor(from_packed_to_unpacked(d_ASM_LEN + d_FRAME_LEN));

	// Check if these samples are the last ones before EOF
	if(output_multiple_set() && noutput_items < output_multiple()) {
		printf("requested only %d samples, although multiple is set to %d\n",noutput_items,output_multiple());
	} else {
		//printf("requested %d samples, multiple is set to %d\n",noutput_items,output_multiple());
	}

	while(!abort) {

		// Are there any offset symbols? If so, remove them before
		// searching/checking as they are not needed anymore
		if(d_offset_bytes > 0) {

			// do we have enougth output items
			if(noutput_items-num_out < d_offset_bytes) {

				#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
					printf("AR: skip in order to consume the first %u offset symbols while there are only %d output slots left\n",d_offset_bytes,noutput_items-num_out);
				#endif

				// no, break and do this at the start of next round
				break;
			}

			unsigned char *in_syms = new unsigned char[d_offset_bytes];
					
			convert_ambiguity(in_syms, &in[num_out], d_offset_bytes, d_locked_on_stream);

			memcpy(&out[num_out],in_syms,d_offset_bytes);

			#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
				for(unsigned int j=0;j<d_offset_bytes;j++) {
					fprintf(dbg_file_out,"%02X ",out[num_out+j]);
				}
				fprintf(dbg_file_out,"\n");
			#endif

			num_out += d_offset_bytes;
			d_offset_bytes = 0;
		}

		switch(d_state) {
			case STATE_SEARCH:
				// We want to search, make sure we have enough
				// samples
				if(noutput_items-num_out < search_len) {
					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
						printf("AR: skip because we need %u symbols for the next search, while there are only %d left\n",search_len,noutput_items-num_out);
					#endif

					abort = true;
					break;
				}

				#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
					printf("AR: state=SEARCH at symbol %lu\n",dbg_count+num_out);
				#endif

				// We have enough samples, let's search in each
				// stream
				for(unsigned int i=0;i<d_M;i++) {
					// we need search_len
					unsigned char *in_syms = new unsigned char[search_len];
					
					convert_ambiguity(in_syms, &in[num_out], search_len, i);

					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
						fprintf(dbg_file_in,"AR: ambiguity=%u in=",i);
						for(unsigned int j=0;j<search_len;j++) {
							fprintf(dbg_file_in,"%02X ",in_syms[j]);
						}
						fprintf(dbg_file_in,"   ");
					#endif
					if(d_asm_operator->search_asm(in_syms, search_len, &d_offset_bytes, &d_offset_bits)) {
						#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
							fprintf(dbg_file_in,"match at symbol %u and bit %u\n",d_offset_bytes,d_offset_bits);
						#endif

						// ASM found, enter LOCK
						d_state = STATE_LOCK;
						d_count = 0;
						d_locked_on_stream = i;

						// now we need bytes_req per input
						set_output_multiple(bytes_req);

						// output the first symbols with the found ambiguity.
						// If we found the right ambiguity here then the previous
						// output also has to have this abiguity.
						if(d_offset_bytes > 0) {
							memcpy(&out[num_out],in_syms, d_offset_bytes);
							num_out += d_offset_bytes;
							d_offset_bytes = 0;
						}

						// Clean up
						delete[] in_syms;

						// Do not search the other streams
						break;
					} else {
						#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
							fprintf(dbg_file_in,"no match\n");
						#endif
						delete[] in_syms;
					}
				}

				// Consume samples if we did not find anything
				if(d_state != STATE_LOCK) {
					unsigned char *in_syms = new unsigned char[search_len];
					
					convert_ambiguity(in_syms, &in[num_out], search_len, d_locked_on_stream);

					memcpy(&out[num_out], in_syms, copy);

					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
						for(unsigned int j=0;j<copy;j++) {
							fprintf(dbg_file_out,"%2X ",out[num_out+j]);
						}
						fprintf(dbg_file_out,"\n");
					#endif

					// consume samples
					num_out	     += copy;

					delete[] in_syms;
				}
				break; // End state SEARCH

			case STATE_LOCK:
				// How many samples do we need if locked? If ASM is not aligned we need
				// another byte for the last bit(s)
				bytes_req = std::ceil((d_ASM_LEN_BITS + d_FRAME_LEN_BITS + d_offset_bits) / (float)d_ldM);

				// check if we have to update the output multiple
				if(output_multiple() != (int)bytes_req) {
					set_output_multiple(bytes_req);
				}

				// Make sure we have enough samples
				if(noutput_items-num_out < bytes_req) {
					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
						printf("AR: skip because we need %u symbols for the next lock, while there are only %d left\n",bytes_req,noutput_items-num_out);
					#endif

					abort = true;
					break;
				} else {
					// We have enough samples, continue in
					// else clause to have an own local 
					// context for this state.

					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
								printf("AR: state=LOCK, stream=%u, bit_offset=%u, sym_offset=%u, confidence=%u/%u\n",
									d_locked_on_stream, d_offset_bits, d_offset_bytes, d_count, d_THRESHOLD);
					#endif

					unsigned char *in_syms = new unsigned char[bytes_req];
					convert_ambiguity(in_syms, &in[num_out], bytes_req, d_locked_on_stream);

					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
						fprintf(dbg_file_in,"AR: ambiguity=%u in=",d_locked_on_stream);
						for(unsigned int j=0;j<search_len;j++) {
							fprintf(dbg_file_in,"%02X ",in_syms[j]);
						}
						fprintf(dbg_file_in," expected match at sym %u and bit %u\n",d_offset_bytes,d_offset_bits);
					#endif

					// We have enough samples, lets check for the ASM
					if(d_asm_operator->check_for_asm(&in_syms[d_offset_bytes], d_offset_bits)) {
						// ASM found
						d_count = std::min(d_count+1, d_THRESHOLD);
					} else if(d_count > 1) {
						// No match, but we still have
						// confidence in out locking
						d_count--;
					} else {
						// Lost lock, enter search
						d_state = STATE_SEARCH;
						d_count = 0;
					
						// now we need search_len symbols per input
						set_output_multiple(search_len);

						// Do not consume any samples here. Our
						// lock on this stream seems to be wrong
						// so start new search with the same
						// samples again
					
						// clear buffer
						delete[] in_syms;

						break;
					}

					// We are still locked

					// copy to output
					memcpy(&out[num_out],in_syms, copy);

					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
						for(unsigned int j=0;j<copy;j++) {
							fprintf(dbg_file_out,"%02X ",out[num_out+j]);
						}
							fprintf(dbg_file_out,"\n");
					#endif

					// consume samples
					num_out	+= copy;

					// We might not have copied every bit, so adjust the
					// offsets to find the next ASM at the expected position
					d_offset_bits += d_FULL_FRAME_REMAIN_BITS;
					confine_offsets();

					delete[] in_syms;
				}
				break; // End state LOCK
			default:
				fprintf(stderr,"ERROR AR: undefined state, entering search\n");
				d_state = STATE_SEARCH;
				d_count = 0;

				// now we need search_len symbols per input
				set_output_multiple(search_len);

				break;
		}
	}

	//
	//// Cleanup
	//

	#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
		dbg_count += num_out;
	#endif

	// Tell runtime system how many output items we produced
	return num_out;
}
