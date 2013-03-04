#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_mpsk_ambiguity_resolver_f.h>
#include "ccsds_hexstring_to_binary.h"
#include "ccsds_mpsk_constellation.h"
#include <gr_io_signature.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>
#include <cstdio>
#include <gruel/pmt.h>
#include <boost/scoped_array.hpp>


ccsds_mpsk_ambiguity_resolver_f_sptr ccsds_make_mpsk_ambiguity_resolver_f(const unsigned int M, std::string ASM, const unsigned int asm_len, const unsigned int threshold, const float ber_threshold, const unsigned int frame_length, const unsigned int num_tail_syms) {
    return ccsds_mpsk_ambiguity_resolver_f_sptr (new ccsds_mpsk_ambiguity_resolver_f(M, ASM,asm_len, threshold, ber_threshold, frame_length, num_tail_syms) );
}

ccsds_mpsk_ambiguity_resolver_f::ccsds_mpsk_ambiguity_resolver_f (const unsigned int M, std::string ASM, const unsigned int asm_len, const unsigned int threshold, const float ber_threshold, const unsigned int frame_length, const unsigned int num_tail_syms)
  : gr_block ("ccsds_mpsk_ambiguity_resolver_f",
	gr_make_io_signature (1, 1, sizeof (float)),
	gr_make_io_signature (0, 0, sizeof (float))),

	d_M(M),
	d_ldM((unsigned int)round(log(M)/log(2.0f))),
	d_THRESHOLD(threshold),
	d_BER_THRESHOLD(ber_threshold),
	d_FRAME_LEN_BITS(frame_length*8),
	d_ASM_LEN_BITS(asm_len),
	d_NUM_TAIL_SYMS(num_tail_syms),
	d_SEARCH_LEN_MIN(get_upper_mul(d_ASM_LEN_BITS + d_ldM)),
	d_SEARCH_LEN_MAX(get_upper_mul(d_FRAME_LEN_BITS + d_NUM_TAIL_SYMS + 2u * d_ASM_LEN_BITS - 1u))
{
	
	// Mapping to translate from the input stream to any ambiguity stream
	// (assume the detector saw a rotated constellation)
	d_map_index_to_bits = ccsds_make_mpsk_map(M);
	d_map_bits_to_index = ccsds_make_mpsk_rev_map(M);

	// Copy ASM to private memory
	const unsigned int asm_len_byte = std::ceil((d_ASM_LEN_BITS)/8.0f);
	boost::scoped_array<unsigned char> tmp(new unsigned char[asm_len_byte]);
	ccsds_hexstring_to_binary(&ASM, tmp.get());

	d_ASM = new float[d_ASM_LEN_BITS];
	for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
		const uint8_t byte = tmp[i/8];
		
		const bool bit = ((byte >> (7-(i%8))) & 0x01) == 0x01;
		d_ASM[i] = (bit ? 1.0f : -1.0f);
	}

	// Initialize variables
	d_state = STATE_SEARCH;
	d_count = 0;
	d_locked_on_stream = 0;
	d_offset_bits =0;

	// register output
	message_port_register_out(pmt::mp("out"));

	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
		dbg_file_in = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_debug_in.dat","w");
		dbg_file_out = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_debug_out.dat","w");
	#endif
	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
		dbg_count = 0;

		printf("==== Ambiguity resolution and Frame sync ====\n");
		printf("  Sym len:   %u bit\n",d_ldM);
		printf("  ASM len:   %u bit\n",d_ASM_LEN_BITS);
		printf("  Frame len: %u bit\n",d_FRAME_LEN_BITS);
		printf("  Tail len:  %u bit\n",d_NUM_TAIL_SYMS);
		printf("  Min search len: %u\n",d_SEARCH_LEN_MIN);
		printf("  Max search len: %u\n",d_SEARCH_LEN_MAX);
		printf("=============================================\n");
	#endif
}

ccsds_mpsk_ambiguity_resolver_f::~ccsds_mpsk_ambiguity_resolver_f () {
	delete[] d_map_index_to_bits;
	delete[] d_map_bits_to_index;
	delete[] d_ASM;

	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
		fflush(dbg_file_in);
		fflush(dbg_file_out);

		fclose(dbg_file_in);
		fclose(dbg_file_out);
	#endif
}

bool ccsds_mpsk_ambiguity_resolver_f::stop(void) {
	// Signal EOF to next block
	message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
	return true;
}

float ccsds_mpsk_ambiguity_resolver_f::check_for_ASM(const float *in, const unsigned int offset) {
	float correlation = 0.0f;

	for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
		//TODO use volk for multiplication
		correlation += d_ASM[i] * in[i+offset];
	}

	// normalization
	correlation /= d_ASM_LEN_BITS;


	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
		/*
		printf("Check if ASM equals with offset %u:\n  in : ",offset);
		for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
			printf("%1u ", (in[i+offset]>0.0) ? 1 : 0);
		}
		printf("\n  asm: ");
		for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
			printf("%1u ", (d_ASM[i]>0.0) ? 1 : 0);
		}
		printf("\n  correlation: %lf\n",correlation);
		//*/
	#endif

	return correlation;
}
float ccsds_mpsk_ambiguity_resolver_f::search_for_ASM(const float *in, const unsigned int num_in, unsigned int *offset) {

	const unsigned int search_len = num_in-d_ASM_LEN_BITS;

	float max = -1.0f;
	*offset = 0;

	for(unsigned int i=0;i<=search_len;i++) {
		float corr = check_for_ASM(in,i);
		if(corr > max) {
			max = corr;
			*offset = i;

			if(max >= 1.0f) {
				// Maximum found, stop searching
				break;
			}
		}
	}

	return max;
}

unsigned char ccsds_mpsk_ambiguity_resolver_f::convert_to_char(const float *in) {
	uint8_t out = 0x00;

	for(unsigned int i=0;i<d_ldM;i++) {
		out = (out<<1) | ((in[i] > 0) ? 0x01 : 0x00);
	}

	return out;
}

void ccsds_mpsk_ambiguity_resolver_f::convert_ambiguity(float *out, const float *in, const unsigned int num_bits, unsigned int ambiguity) {

	// Get the number of symbols we should convert. If num_bits is not a
	// multiple of d_ldM, ignore the last few bits. (Doku explicitly says
	// it must be a multiple).
	const unsigned int num_syms = get_lower_mul(num_bits) / d_ldM;

	// shortcut if there is no ambiguity
	if(ambiguity == 0) {
		memcpy(out, in, num_bits*sizeof(float));
		return;
	}

	// there is a non zero ambiguity, so do the conversion
	for(unsigned int i=0;i<num_syms;i++) {
		// convert bits now
		
		// index of in shoul always be  smaller than num_bits, but as a
		// saftey measure, wrap it arround if some input parameters are
		// not well defined
		const unsigned char bits_in = convert_to_char(&in[(d_ldM*i) % num_bits]);
		const unsigned int index = d_map_bits_to_index[bits_in];
		const uint8_t bits_out = d_map_index_to_bits[(d_M+index-ambiguity)%d_M];

		// find out where positions differ, to assign the right value
		uint8_t diff = bits_in ^ bits_out;
		for(unsigned int j=0;j<d_ldM;j++) {
			// wrap output around num_bits as a saftey measure if 
			// d_ldM and num_bits do not match together.
			const unsigned int indx = (i*d_ldM+j) % num_bits;
			out[indx] = (diff & 0x01 << (d_ldM-1-j)) ? -in[indx] : in[indx];
		}
		
	}
	return;
}

void ccsds_mpsk_ambiguity_resolver_f::forecast(int /*noutput_items*/,gr_vector_int &ninput_items_required){
	// In Search mode we only search for a ASM, we don't need the Frame yet
	// to make sure we do not request more than necessary and GNURadio drops
	// everything

	switch(d_state) {
		case STATE_SEARCH:	ninput_items_required[0] = d_SEARCH_LEN_MIN;
					break;
		case STATE_LOCK:	ninput_items_required[0] = get_upper_mul(d_NUM_TAIL_SYMS + d_ASM_LEN_BITS + d_FRAME_LEN_BITS + d_offset_bits);
					break;
	}
	//printf("AR: need at least %d input items in this state\n",ninput_items_required[0]);
	return;
}

unsigned int ccsds_mpsk_ambiguity_resolver_f::get_lower_mul(const unsigned int n) {
	return (unsigned int)std::floor((float)n/d_ldM)*d_ldM;
}

unsigned int ccsds_mpsk_ambiguity_resolver_f::get_upper_mul(const unsigned int n) {
	return (unsigned int)std::ceil((float)n/d_ldM)*d_ldM;
}

int  ccsds_mpsk_ambiguity_resolver_f::general_work (int 		    /*noutput_items*/,
					        gr_vector_int&              ninput_items,
					        gr_vector_const_void_star&  input_items,
					        gr_vector_void_star&        /*output_items*/)
{
	const float *in = (const float *) input_items[0];

	unsigned int num_consumed = 0;

	// If a state detects that it needs more samples to continue, it can
	// signal an abort with this flag. A new try is done once the work
	// function is called again by the scheduler
	bool abort = false;
	

	// How many samples are necessary to perform a check for an expected
	// ASM. Can vary depending on d_offset_bits and is updated by the
	// statemachine.
	unsigned int check_len;

	while(!abort) {

		switch(d_state) {
			case STATE_SEARCH:
				// We want to search, make sure we have enough
				// samples
				if(ninput_items[0]-num_consumed < d_SEARCH_LEN_MIN) {
					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
						printf("AR: skip because we need %u symbols for the next search, while there are only %d left\n",d_SEARCH_LEN_MIN,ninput_items[0]-num_consumed);
					#endif

					abort = true;
					break;
				} else {

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
						printf("AR: state=SEARCH at symbol %lu\n",dbg_count+num_consumed);
					#endif

					const unsigned int search_len = get_lower_mul(std::min(ninput_items[0]-num_consumed, d_SEARCH_LEN_MAX));

					// We have enough samples, let's search in each
					// stream
					float max_corr = -1.0f;
					unsigned int max_amb = 0;

					for(unsigned int i=0;i<d_M;i++) {
						boost::scoped_array<float> in_syms(new float[search_len]);
					
						convert_ambiguity(in_syms.get(), &in[num_consumed], search_len, i);

						unsigned int offset;
						const float correlation = search_for_ASM(in_syms.get(), search_len, &offset);

						// Do we have a better match than the previous?
						if(correlation > max_corr) {
							max_corr = correlation;
							max_amb  = i;
							d_offset_bits = offset;

							if(max_corr >= 1.0f) {
								// Maximum found, stop searching
								break;
							}
						}

						// continue search for other ambiguities
					} // END search all ambiguity streams

					// Did we find something?
					if(max_corr > d_BER_THRESHOLD) {
						// ASM found, enter LOCK
						d_state = STATE_LOCK;
						d_count = 0;
						d_locked_on_stream = max_amb;

						// drop the first symbols that do not match the ASM
						const unsigned int num_syms = d_offset_bits / d_ldM;
						if(num_syms > 0) {
							num_consumed += num_syms*d_ldM;
							d_offset_bits -= num_syms*d_ldM;
						}

					} else { // We found nothing
						// Consume samples if we did not find anything
					
						// the last d_ASM_BITS-1 have not been searched yet, everything
						// else can be consumed as long as throw away multiples of the
						// symbol bits. Otherwise we will loose track of the symbol periods
						num_consumed += get_lower_mul(search_len-(d_ASM_LEN_BITS-1));

						#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
							printf("AR: nothing found, consume %u samples.\n",get_lower_mul(search_len-(d_ASM_LEN_BITS-1)));
						#endif
					}

				} // END num_in >= search_len_min
				break; // End state SEARCH

			case STATE_LOCK:
				// How many samples do we need if locked? If ASM is not aligned we need
				// d_offset_bits additional bits.
				check_len = get_upper_mul(d_NUM_TAIL_SYMS + d_ASM_LEN_BITS + d_FRAME_LEN_BITS + d_offset_bits);

				// Make sure we have enough samples
				if(ninput_items[0]-num_consumed < check_len) {
					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
						printf("AR: skip because we need %u symbols for the next lock, while there are only %d left\n",check_len,ninput_items[0]-num_consumed);
					#endif

					abort = true;
					break;
				} else {
					// We have enough samples, continue in
					// else clause to have an own local 
					// context for this state.

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
								printf("AR: state=LOCK, stream=%u, bit_offset=%u, confidence=%u/%u\n",
									d_locked_on_stream, d_offset_bits, d_count, d_THRESHOLD);
					#endif

					boost::scoped_array<float> in_syms(new float[check_len]);
					convert_ambiguity(in_syms.get(), &in[num_consumed], check_len, d_locked_on_stream);

					const float correlation = check_for_ASM(in_syms.get(), d_offset_bits);
					
					// We have enough samples, lets check for the ASM
					if(correlation > d_BER_THRESHOLD) {				
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
						d_offset_bits = 0;

						// Do not consume any samples here. Our
						// lock on this stream seems to be wrong
						// so start new search with the same
						// samples again

						break;
					}

					// We are still locked

					// copy to output
					pmt::pmt_t msg = pmt::pmt_make_f32vector(d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS, 0.0f);
					for(unsigned int i=0;i<d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS;i++) {
						pmt::pmt_f32vector_set(msg, i, in_syms[i+d_offset_bits+d_ASM_LEN_BITS]);
					}
					message_port_pub(pmt::mp("out"), msg);

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
						//TODO bring this up to date again
						for(unsigned int j=0;j<copy;j++) {
							//fprintf(dbg_file_out,"%02X ",out[num_out+j]);
						}
							//fprintf(dbg_file_out,"\n");
					#endif

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
						printf("AR: %u bits copied (offset=%u), %u samples consumed.\n",d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS,
								d_offset_bits,
								get_lower_mul(d_NUM_TAIL_SYMS+d_ASM_LEN_BITS+d_FRAME_LEN_BITS+d_offset_bits));
					#endif

					// consume samples
					num_consumed	+= get_lower_mul(d_NUM_TAIL_SYMS+d_ASM_LEN_BITS+d_FRAME_LEN_BITS+d_offset_bits);

					// We might not have copied every bit, so adjust the
					// offsets to find the next ASM at the expected position
					d_offset_bits = (d_NUM_TAIL_SYMS+d_ASM_LEN_BITS+d_FRAME_LEN_BITS+d_offset_bits) % d_ldM;
				}
				break; // End state LOCK
			default:
				fprintf(stderr,"ERROR AR: undefined state, entering search\n");
				d_state = STATE_SEARCH;
				d_count = 0;

				break;
		}
	}

	//
	//// Cleanup
	//

	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
		dbg_count += num_consumed;
	#endif

	consume_each(num_consumed);

	// Stream sink, so no output
	return 0;
}
