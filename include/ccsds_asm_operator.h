#ifndef INCLUDED_CCSDS_ASM_OPS_H
#define INCLUDED_CCSDS_ASM_OPS_H

#include <cstring>
#include <cstdio>

/*! \brief Outputs debug messages to stdout if defined */
// #define CCSDS_ASM_DEBUG

/*! \brief Helper class for bit wise ASM search functions
 *
 *  \ingroup receiver
 *  \ingroup synchronization
 *
 *  \sa ccsds_mpsk_ambiguity_resolver_bb
 *  \sa ccsds_frame_sync_bb
 *
 *  \todo Continue searching if ASM matches with a few tolerable bit errors (but there might also be a better match at later ambiguities)
 *  \todo Document search algorithm
 */
class ccsds_asm_operator {

private:

	/*! \brief Storage for attached snc marker
	 *
	 *  \sa d_ASM_LEN
	 */
	unsigned char *d_ASM;

	/*! \brief Length of ASM in bytes 
	 *
	 *  \sa d_ASM
	 */
	const unsigned int d_ASM_LEN;

	/*! \brief Maximum number of bit errors that may occur between a
	 *	sequence and the ASM to still consider the sequence as an ASM.
	 *	If set to zero, sequence must match the ASM exactly.
	 */
	const unsigned int d_BER_THRESHOLD;

public:

	/*! \brief Constructor of ASM operator
	 *  
	 *  \param ASM Attached sync marker to look for
	 *  \param ASM_LEN Length of the ASM in bytes.
	 *  \param BER_THRESHOLD Maximum number of bit errors that may occur
	 *	between a sequence and the ASM to still consider the sequence as
	 *	an ASM. If set to zero, sequence must match the ASM exactly.
	 */
	ccsds_asm_operator(const unsigned char *ASM, const unsigned int ASM_LEN, const unsigned int BER_THRESHOLD) :
		d_ASM_LEN(ASM_LEN), d_BER_THRESHOLD(BER_THRESHOLD) {


		d_ASM = new unsigned char[ASM_LEN];
		memcpy(d_ASM, ASM, ASM_LEN);

	}

	/*! \brief Deconstructor of ASM operator */
	~ccsds_asm_operator() {
		delete[] d_ASM;
	}

	/*! \brief Checks if stream matches an ASM at given bit offset.
	 *
	 *  \param stream Array of bytes to check ASM against. If \c offset_bits is zero
	 *	\c stream must have at least \c d_ASM_LEN elements, otherwise one element
	 *	more.
	 *  \param offset_bits Number of bits to ignore in the first byte of the stream.
	 *  \return \a true if stream is matching the ASM, \a false if not.
	 *
	 *  \sa d_BER_THRESHOLD
	 *
	 *  The stream is considered to match if \c d_BER_THRESHOLD or less bits
	 *  differ.
	 */
	bool check_for_asm(const unsigned char* stream, unsigned int offset_bits) {
		#ifdef CCSDS_ASM_DEBUG
			printf("++ check_for_asm ++\n");
			printf("  compare: %u, offset=%u\n",stream[0],offset_bits);
			printf("     with: %u\n",d_ASM[0]);
		#endif

		// Bitmasks to select a single bit position
		unsigned char mask[8];
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
				#ifdef CCSDS_ASM_DEBUG
					printf("\n abort with %u comparisons remaining\n",count);
					printf("+++++++++++++++++++\n");
				#endif

				return false;
			}
		
			#ifdef CCSDS_ASM_DEBUG
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

		#ifdef CCSDS_ASM_DEBUG
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
	bool search_asm(const unsigned char* stream, const unsigned int stream_len, unsigned int *offset_bytes, unsigned int *offset_bits) {

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


	/*! \brief Copy bitstream from a byte array with a bit offset into an
	 *	aligned byte array.
	 *
	 *  \param stream_in Array of bytes to copy. First bit to copy is
	 *	located in the first element of this array. If \c offset_bit is
	 *	zero this array must contain at least \c len elements, if
	 *	\c offset_bit is greater thatn zero, this array must contain
	 *	at least \c len+1 elements.
	 *  \param stream_out Array in which the resulting bytes should be
	 *	copied. Memory for at least \c len elements must be allocated.
	 *  \param len Number of bytes to copy.
	 *  \param offset_bits Number of bits to skip in the first byte, starting
	 *	with the MSB. If offset_bit is zero this function copies the
	 *	first \c len bytes from \c stream_in to \c stream_out by using
	 *	\c memcpy. Otherwise the copy operation is done manually using
	 *	bitwise operations.
	 *
	 *  
	 *  The first \c offset_bits starting with the MSB from the first
	 *  element of \c stream_in are skipped, afterwards all bits from
	 *  \c stream_in are copied to \c stream_out until \c len bytes have
	 *  been copied to \c stream_out.
	 */
	void copy_stream(unsigned char * stream_out, const unsigned char * stream_in, const unsigned int len, const unsigned int offset_bits) {

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

};
#endif /* INCLUDED_CCSDS_ASM_OPS_H */
