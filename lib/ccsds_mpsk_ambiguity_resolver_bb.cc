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



ccsds_mpsk_ambiguity_resolver_bb_sptr ccsds_make_mpsk_ambiguity_resolver_bb(const unsigned int M, std::string ASM, const unsigned int threshold, const unsigned int frame_length) {
    return ccsds_mpsk_ambiguity_resolver_bb_sptr (new ccsds_mpsk_ambiguity_resolver_bb(M, ASM, threshold, frame_length) );
}

ccsds_mpsk_ambiguity_resolver_bb::ccsds_mpsk_ambiguity_resolver_bb (const unsigned int M, std::string ASM, unsigned int threshold, const unsigned int frame_length)
  : gr_block ("ccsds_mpsk_ambiguity_resolver_bb",
	gr_make_io_signature (1, 1, sizeof (unsigned char)),
	gr_make_io_signature (1, 1, sizeof (unsigned char))), d_M(M), d_ldM((unsigned int)(log(M)/log(2))), d_ASM_LEN(std::floor(ASM.length()/2)), d_THRESHOLD(threshold), d_BER_THRESHOLD(0), d_FRAME_LEN(frame_length)
{

	// If no ASM is found, samples will be dropped, so no rate can be
	// specified.
	set_fixed_rate(false);
	
	// Mapping to translate from the input stream to any ambiguity stream
	// (assume the detector saw a rotated constellation)
	d_map_index_to_bits = ccsds_make_mpsk_map(M);
	d_map_bits_to_index = ccsds_make_mpsk_rev_map(M);

	// Copy ASM to private memory
	d_ASM = new unsigned char[d_ASM_LEN];
	ccsds_hexstring_to_binary(&ASM, d_ASM);

	// Initialize variables
	d_state = STATE_SEARCH;
	d_count = 0;
	d_locked_on_stream = 0;
	d_offset_bytes = 0;
	d_offset_bits =0;

	// At the beginning samples and bytes are aligned
	d_samp_skip_bits = 0;
}

ccsds_mpsk_ambiguity_resolver_bb::~ccsds_mpsk_ambiguity_resolver_bb () {
	delete[] d_ASM;
	delete[] d_map_index_to_bits;
	delete[] d_map_bits_to_index;
}

void ccsds_mpsk_ambiguity_resolver_bb::forecast(int noutput_items,gr_vector_int &ninput_items_required){
	// How many frames will we process if we produce the requested number of
	// samples
	unsigned int num_frames = std::floor(noutput_items / (d_ASM_LEN+d_FRAME_LEN) );

	// We want to process at least one frame, in order to do something.
	num_frames = std::max( 1u, num_frames);

	// If we know exactly where to look, we need one ASM plus the following
	// frame data per requested frame.
	const unsigned int in = (d_ASM_LEN + d_FRAME_LEN) * num_frames;

	// If we don't know where to look we need additional d_ASM_LEN-1 samples
	// to guarantee that there is one complete ASM in the first sequence.
	// Afterwards the state should be locked.
	if(d_state == STATE_SEARCH) {
		ninput_items_required[0] = std::ceil((in+d_ASM_LEN)*8.0/(double)d_ldM);
		
	} else { // d_state == STATE_LOCKED
		// No need to search again (hopefully)
		ninput_items_required[0] = std::ceil(in*8.0/(double)d_ldM);
	}

	return;
}

unsigned char * ccsds_mpsk_ambiguity_resolver_bb::get_packed_bytes(const unsigned char *in_unpacked, const unsigned int bytes_offset,
											const unsigned int bytes_req, unsigned int ambiguity) {

	if(bytes_req == 0) {
		return new unsigned char[0];
	}

	// Allocate space for output buffer
	unsigned char * out = new unsigned char[bytes_req];

	// Index of the sample that contains the first bit that belongs to the requested byte
	const unsigned int samp_offset = std::floor((d_samp_skip_bits+8*bytes_offset)/(double)d_ldM);

	// Index of last sample that contains bits that belong to the requested bytes
	const unsigned int samp_offset_to = std::ceil((d_samp_skip_bits+8*(bytes_offset+bytes_req))/(double)d_ldM -1);

	// Number of bits starting from the MSB that should be ignored, because
	// they still belong to the previous bit
	const unsigned int bit_offset = (d_samp_skip_bits+8*bytes_offset) % d_ldM;

	// Bit position (starting from the MSB) of the previous bit where the
	// fist bits from the package should be written to (if non zero this is
	// the position in the -1 byte that will be skipped anyway)
	unsigned int out_bit  = (8-bit_offset) % 8;

	// If we do not start at the MSB, the first bit(s) still belongs to the
	// last byte
	int out_byte = (out_bit == 0) ? 0 : -1;

	// We create at least one byte (otherwise we would have returned earlier)
	// Initialize first bit
	if(out_byte >= 0) {	
		out[out_byte] = 0u;
	}

	// Go through all packets in range
	for(unsigned int i=samp_offset;i<=samp_offset_to;i++) {

		// convert stream
		unsigned char amb_bits;

		// do we need to change the ambiguity?
		if(ambiguity == 0) {
			// no just pack the stream
			amb_bits = in_unpacked[i];
		} else {
			// yes, convert bits now
			unsigned int index = d_map_bits_to_index[in_unpacked[i]];
			amb_bits = d_map_index_to_bits[(d_M+index-ambiguity)%d_M];
		}

		// pack stream
		if(8-out_bit >= d_ldM) {
			// Enough space in the current byte to store all bits

			// sanity check, this case should never occur, because
			// it means that samp_offset os wrong
			if(out_byte < 0) {
				fprintf(stderr,"ERROR AR: invalid indices.\n");
			}

			// Assign bits to current byte
			out[out_byte] = out[out_byte] | (amb_bits << (8-out_bit-d_ldM));
			
			// Increase counters
			out_bit += d_ldM;
			if(out_bit >= 8) {
				// Current byte is now full, go to next one
				out_byte++;
				out_bit = 0;

				if(out_byte >= bytes_req) {
					// next loop we would write out of the
					// requested range, so we are finished now
					return out;
				}

				// we will continue to write into out. Make sure
				// new byte is initialized to zero
				out[out_byte] = 0u;
			}
		} else {
			// Bits need to be split into two bytes
			
			// how many bits will go into the second byte?
			const unsigned int split = d_ldM-8+out_bit;

			// first byte, if in range
			if(out_byte >= 0) {
				out[out_byte] = out[out_byte] | (amb_bits >> split);
			}

			// increase counters (already assume 2nd portion written)
			out_byte++;
			out_bit = split;

			if(out_byte >= bytes_req) {
				// we would write out of the requested range, so
				// we are finished now
				return out;
			}

			// second byte (start with a fresh byte)
			out[out_byte] = (amb_bits << (8-split));
		}
	}

	return out;
}


/*! \brief Checks if stream matches an ASM at given bit offset.
 *
 *  \param stream Array of bytes to check ASM against. If \c offset_bits is zero
 *	\c stream must have at least \c d_ASM_LEN elements, otherwise one element
 *	more.
 *  \param offset_bits Number of bits to ignore in the first byte of the stream.
 *  \return \a true if stream is matching the ASM, \a false if not.
 *
 *  \sa ::d_BER_THRESHOLD
 *
 *  The stream is considered to match if ::d_BER_THRESHOLD or less bits differ.
 */
bool ccsds_mpsk_ambiguity_resolver_bb::check_for_asm(const unsigned char* stream, unsigned int offset_bits) {

	#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
		printf("++ check_for_asm ++\n");
		printf("  compare: %u, offset=%u\n",stream[0],offset_bits);
		printf("     with: %u\n",d_ASM[0]);
	#endif

	// Bitmasks to select a single bit position
	unsigned char* mask = new unsigned char[8];
	for(unsigned int i=0;i<8;i++) {
		mask[i] = 0x1 << (7-i);
	}
	
	// counter which bit we will compare next
	unsigned int count = d_ASM_LEN*8u;

	// byte index in the stream where the current bit is.
	unsigned int s_byte = 0;
	// bit index in the current stream byte 0 means MSB
	unsigned int s_bit  = offset_bits;

	// byte index in the ASM that we are currently comparing
	unsigned int a_byte = 0;
	// bit index in the current ASM byte. 0 means MSB
	unsigned int a_bit  = 0;

	// Number of bit errors that we can still tolerate to
	// consider sequence as ASM
	unsigned int err_count = 0;

	// loop through all ASM bits
	while(count > 0) {
		// current bit value in the stream moved to the
		// LSB position
		unsigned char val_s_bit = (stream[s_byte] & mask[s_bit]) >> (7-s_bit);
		// current bit value in the ASM moved to the LSB
		// position
		unsigned char val_a_bit = ( d_ASM[a_byte] & mask[a_bit]) >> (7-a_bit);

		// if bits are not equal, there is a ber
		if(val_s_bit != val_a_bit) {
			err_count++;
		}
		
		// To much errors, this sequence is not the ASM
		if(err_count > d_BER_THRESHOLD) {
			delete[] mask;

			#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
				printf("\n abort with %u comparisons remaining\n",count);
				printf("+++++++++++++++++++\n");
			#endif

			return false;
		}
		
		#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
			printf("  bit %1u equal, %u comparisons remaining\n",val_s_bit,count);
		#endif

		s_bit++;
		a_bit++;
		count--;
		if(s_bit >= 8) {
			s_bit = 0;
			s_byte++;
		}
		if(a_bit >= 8) {
			a_bit = 0;
			a_byte++;
		}
	}
	
	delete[] mask;

	#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_DEBUG
		printf("+++++++++++++++++++\n");
	#endif

	//exit(0);
	// we survived, so we must have received an ASM
	return true;
}

/*! \brief Searches for ASM in the given stream.
 *
 *  \param stream Array of bytes in which to search for the ASM.
 *  \param stream_len Length of the stream. The last \c d_ASM_LEN bytes of this
 *	stream are not searched (as the ASM would only fit in partially).
 *  \param *offset_bytes Pointer to an unsigned int where the offset bytes between
 *	start of stream and the found ASM should be stored, if a match is found.
 *  \param *offset_bits Pointer to an unsigned int where the offset bits between
 *	start of stream and the found ASM should be stored, if a match is found.
 *  \return \a true if ASM is found, \a false otherwise.
 */
bool ccsds_mpsk_ambiguity_resolver_bb::search_asm(const unsigned char* stream, const unsigned int stream_len, unsigned int *offset_bytes, unsigned int *offset_bits) {

	// check all possible byte offsets
	for(unsigned int byte=0;byte<stream_len-d_ASM_LEN;byte++) {
		// for a given byte offset, check all possible bit offsets
		for(unsigned int bit=0;bit<8;bit++) {

			// Does the stream at this position match the ASM?			
			if(check_for_asm(&stream[byte], bit)) {

				// Match found, store position
				*offset_bytes = byte;
				*offset_bits  = bit;

				// ASM found, return
				return true;
			}
		}
	}

	// No match found so far, return.
	return false;
}

void ccsds_mpsk_ambiguity_resolver_bb::copy_stream(const unsigned char * stream_in, unsigned char * stream_out, const unsigned int len, const unsigned int offset_bits) {

	if(offset_bits == 0u) {
		// Input stream is aligned, use memcpy instead of doing
		// everything by hand
		memcpy(stream_out, stream_in, len*sizeof(unsigned char));
	} else {
		// Input stream has a bit offset, so we need to do the work

		// Go through all output bytes
		for(unsigned int i=0;i<len;i++) {
			// Shift the bits from the first byte to the right position
			unsigned char tmp_l = stream_in[i]   << offset_bits;
			// Shift the bits from the second byte to the right position
			unsigned char tmp_h = stream_in[i+1] >> (8-offset_bits);

			// Both bytes should now have all zeros on the positions
			// where they don't carry information, we join them by
			// bit-wise OR operations and write them out.
			stream_out[i] = tmp_l | tmp_h;
		}
	}
	
	return;
}

int  ccsds_mpsk_ambiguity_resolver_bb::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	// pointer to pointer to a const gr_complex
	const unsigned char *in_unpacked = (const unsigned char *) input_items[0];
	unsigned char *out = (unsigned char *) output_items[0];
	
	// how many samples can we still process, without violating the array
	// bounds of the output or one of the input buffers?
	unsigned int bytes_remain = std::min(noutput_items, (int)std::floor(ninput_items[0]*d_ldM/8.0));
	
	// If a state detects that it needs more samples to continue, it can
	// signal an abort with this flag. A new try is done once the work
	// function is called again by the scheduler
	bool abort = false;
	
	int num_out = 0;
	unsigned int bytes_consumed = 0;

	// In order to catch at least one complete ASM (if there is one) we need
	// this many bytes.
	// Worst case: The ASM started one bit before our current observation
	// interval. In this case the next ASM starts in the last bit of the 
	// d_ASM_LEN+d_FRAME_LEN byte and we need another d_ASM_LEN byte to
	// catch the complete ASM (and the first byte of the following frame)
	const unsigned int search_len = d_FRAME_LEN + 2u * d_ASM_LEN;

	// How many samples do we need if locked? If ASM is not aligned we need
	// another byte for the last bit(s)
	const unsigned int bytes_req = d_ASM_LEN + d_FRAME_LEN + ( (d_offset_bits) ? 1u : 0u );

	// If we copy to the output stream, how many bytes do we copy?
	const unsigned int copy = d_ASM_LEN + d_FRAME_LEN;

	while(!abort) {
		switch(d_state) {
			case STATE_SEARCH:
				// We want to search, make sure we have enough
				// samples
				if(bytes_remain < search_len) {
					abort = true;
					break;
				}

				#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
					printf("AR: state=SEARCH\n");
				#endif

				// We have enough samples, let's search in each
				// stream
				for(unsigned int i=0;i<d_M;i++) {
					// we need search_len
					const unsigned char *in_bytes = get_packed_bytes(in_unpacked, bytes_consumed, search_len, i);

					if(search_asm(in_bytes, search_len, &d_offset_bytes, &d_offset_bits)) {
						// ASM found, enter LOCK
						d_state = STATE_LOCK;
						d_locked_on_stream = i;

						// Do not output the first one,
						// just consume the samples and
						// shift the indices.
						bytes_consumed += d_offset_bytes;
						bytes_remain -= d_offset_bytes;
						d_offset_bytes = 0;

						// Clean up
						delete[] in_bytes;

						// Do not search the other streams
						break;
					}
					delete[] in_bytes;
				}

				// Consume samples if we did not find anything
				if(d_state != STATE_LOCK) {
					bytes_consumed += search_len - d_ASM_LEN;
					bytes_remain -= search_len - d_ASM_LEN;
				}
				break; // End state SEARCH

			case STATE_LOCK:
				// Make sure we have enough samples
				if(bytes_remain < bytes_req) {
					abort = true;
					break;
				} else {
					// We have enough samples, continue in
					// else clause to have an own local 
					// context for this state.

					#if CCSDS_AR_VERBOSITY_LEVEL >= CCSDS_AR_OUTPUT_STATE
								printf("AR: state=LOCK, stream=%u, bit_offset=%u, byte_offset=%u, confidence=%u/%u\n",
									d_locked_on_stream, d_offset_bits, d_offset_bytes, d_count, d_THRESHOLD);
					#endif

					const unsigned char *in_bytes = get_packed_bytes(in_unpacked, bytes_consumed, bytes_req, d_locked_on_stream);

					// We have enough samples, lets check for the ASM
					if(check_for_asm(&in_bytes[d_offset_bytes], d_offset_bits)) {
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

						// Reset variables that are undefined in
						// search state
						d_offset_bytes = 0;
						d_offset_bits  = 0;
						d_locked_on_stream = 0;
					
						// Do not consume any samples here. Our
						// lock on this stream seems to be wrong
						// so start new search with the same
						// samples again
					
						// clear buffer
						delete[] in_bytes;

						break;
					}

					// We are still locked

					// copy to output
					copy_stream(&in_bytes[d_offset_bytes], &out[num_out], copy, d_offset_bits);
				
					// consume samples
					num_out	     += copy;
					bytes_consumed += copy;
					bytes_remain -= copy;

					delete[] in_bytes;
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

	// We know we don't use the next bytes_consumed bytes anymore, so which
	// samples can we throw away?
	unsigned int bits_consumed = bytes_consumed*8 + d_samp_skip_bits;
	unsigned int samps_consumed = std::floor(bits_consumed/d_ldM);
	d_samp_skip_bits = (bytes_consumed*8) - ((samps_consumed*d_ldM)-d_samp_skip_bits);

	// Tell runtime system, how many input samples per stream are not
	// obsolete
	consume_each(samps_consumed);

	// Tell runtime system how many output items we produced
	return num_out;
}
