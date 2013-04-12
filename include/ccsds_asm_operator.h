#ifndef INCLUDED_CCSDS_ASM_OPS_H
#define INCLUDED_CCSDS_ASM_OPS_H

#include <cstring>
#include <cstdio>
#include <cmath>

/*! \brief Outputs debug messages to stdout if defined */
// #define CCSDS_ASM_DEBUG

/*! \brief Helper class for unpacked bit wise ASM search functions
 *
 *  \ingroup receiver
 *  \ingroup synchronization
 *
 *  \sa ccsds_mpsk_ambiguity_resolver_bb
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

	/*! \brief Length of ASM in symbols (rounded up). */
	const unsigned int d_ASM_SYM_LEN;

	/*! \brief Maximum number of bit errors that may occur between a
	 *	sequence and the ASM to still consider the sequence as an ASM.
	 *	If set to zero, sequence must match the ASM exactly.
	 */
	const unsigned int d_BER_THRESHOLD;

	/*! \brief Number of information bits per symbol. */
	const unsigned int d_INFO_BITS_PER_SYMBOL;
public:

	/*! \brief Constructor of ASM operator
	 *  
	 *  \param ASM Attached sync marker to look for
	 *  \param ASM_LEN Length of the ASM in bytes.
	 *  \param BER_THRESHOLD Maximum number of bit errors that may occur
	 *	between a sequence and the ASM to still consider the sequence as
	 *	an ASM. If set to zero, sequence must match the ASM exactly.
	 *  \param INFO_BITS_PER_SYMBOL Number of information bits per byte.
	 *	Default is 8 (packed bytes).
	 */
	ccsds_asm_operator(const unsigned char *ASM, const unsigned int ASM_LEN, const unsigned int BER_THRESHOLD, const unsigned int INFO_BITS_PER_SYMBOL = 8) :
		d_ASM_LEN(ASM_LEN), d_ASM_SYM_LEN(std::ceil((unsigned int) (d_ASM_LEN*8.0f/INFO_BITS_PER_SYMBOL))), d_BER_THRESHOLD(BER_THRESHOLD), d_INFO_BITS_PER_SYMBOL(INFO_BITS_PER_SYMBOL) {


		d_ASM = new unsigned char[ASM_LEN];
		memcpy(d_ASM, ASM, ASM_LEN);

	}

	/*! \brief Deconstructor of ASM operator */
	~ccsds_asm_operator() {
		delete[] d_ASM;
	}

	/*! \return The number of unpacked symbols necesarry to hold the ASM. */
	unsigned int get_asm_sym_len(void) {
		return d_ASM_SYM_LEN;
	}

	/*! \brief Checks if stream matches an ASM at given bit offset and
	 *	return the number of bit errors found.
	 *
	 *  \param symbols Array of unpacked bytes to check ASM against. Must
	 *	contain at least d_ASM_SYM_LEN elements.
	 *  \param offset_bits Number of bits to ignore in the first symbol.
	 *  \return Hamming distance (number of bit positions that differ)
	 *	between the symbols and the ASM.
	 */
	unsigned int check_for_asm_hamming(const unsigned char* symbols, unsigned int offset_bits) {
		#ifdef CCSDS_ASM_DEBUG
			printf("++ check_for_asm in: ");
			for(unsigned int i=0;i<std::ceil(d_ASM_LEN*8.0f/d_INFO_BITS_PER_SYMBOL);i++) {
				printf("%02X ",symbols[i]);
			}
			printf("  with bit_offset=%u++\n",offset_bits);
		#endif

		if(offset_bits >= d_INFO_BITS_PER_SYMBOL) {
			fprintf(stderr,"WARNING ASM OPERATOR: invalid offset_bits %u.\n", offset_bits);
			return false;
		}
		

		// Bitmasks to select a single bit position
		unsigned char mask[8];
		for(unsigned int i=0;i<8;i++) {
			mask[i] = 0x1 << (7-i);
		}

		// counter how many bits are still to be compared
		unsigned int count = d_ASM_LEN*8u;

		// byte index in the stream where the current bit is.
		unsigned int s_byte = 0;
		// bit index in the current stream 0 means MSB
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
			const unsigned char bit_mask = (1 << (d_INFO_BITS_PER_SYMBOL-1)) >> s_bit;

			// current bit value in the stream moved to the
			// LSB position
			const unsigned char val_s_bit = (symbols[s_byte] & bit_mask) >> (d_INFO_BITS_PER_SYMBOL-1-s_bit);
			// current bit value in the ASM moved to the LSB
			// position
			const unsigned char val_a_bit = ( d_ASM[a_byte] & mask[a_bit]) >> (7-a_bit);

			#ifdef CCSDS_ASM_DEBUG
				printf("  bit %u %sequal, %u comparisons remaining\n",val_s_bit,((val_s_bit != val_a_bit) ? "not " : ""), count);
			#endif

			// if bits are not equal, there is a ber
			if(val_s_bit != val_a_bit) {
				err_count++;
				#ifdef CCSDS_ASM_DEBUG
					printf("    bit error %u/%u\n",err_count, d_BER_THRESHOLD);
				#endif
			}
		
			s_bit++;
			a_bit++;
			count--;
			if(s_bit >= d_INFO_BITS_PER_SYMBOL) {
				s_bit = 0;
				s_byte++;
			}
			if(a_bit >= 8) {
				a_bit = 0;
				a_byte++;
			}
		}

		#ifdef CCSDS_ASM_DEBUG
			printf("  Check complete with %u bit errors\n", err_count);
			printf("+++++++++++++++++++\n");
		#endif

		return err_count;
	}

	/*! \brief Checks if stream matches an ASM at given bit offset.
	 *
	 *  \param symbols Array of unpacked bytes to check ASM against. Must
	 *	contain at least d_ASM_SYM_LEN elements.
	 *  \param offset_bits Number of bits to ignore in the first symbol.
	 *  \return \a true if stream is matching the ASM, \a false if not.
	 *
	 *  \sa d_BER_THRESHOLD
	 *
	 *  The stream is considered to match if \c d_BER_THRESHOLD or less bits
	 *  differ.
	 */
	bool check_for_asm(const unsigned char* symbols, unsigned int offset_bits) {
		return (check_for_asm_hamming(symbols, offset_bits) <= d_BER_THRESHOLD);
	}

	/*! \brief Searches for ASM in the given stream.
	 *
	 *  \param stream Array of bytes in which to search for the ASM.
	 *  \param stream_len Length of the stream. The last \c d_ASM_SYM_LEN bytes of this
	 *	stream are not searched (as the ASM would only fit in partially).
	 *  \param *offset_bytes Pointer to an unsigned int where the offset bytes between
	 *	start of stream and the found ASM should be stored, if a match is found.
	 *  \param *offset_bits Pointer to an unsigned int where the offset bits between
	 *	start of stream and the found ASM should be stored, if a match is found.
	 *  \return \a true if ASM is found, \a false otherwise.
	 *
	 *  The match with the minimum Hamming distance is searched. If this
	 *  minimum Hamming distance is less or equal the \c d_BER_THRESHOLD a
	 *  match is found, otherwise no match is found and \c d_offset_bytes
	 *  and \c d_offset_bits are set to zero.
	 *
	 *  In case there are two matches with the same Hamming distance the one
	 *  with the lower byte offset or lower bit offset (in this priority) is
	 *  chosen.
	 */
	bool search_asm(const unsigned char* stream, const unsigned int stream_len, unsigned int *offset_bytes, unsigned int *offset_bits) {
		*offset_bytes = 0;
		*offset_bits  = 0;
		unsigned int min_hamming = d_BER_THRESHOLD+1; // To many bit errors
		// check all possible byte offsets
		for(unsigned int byte=0;byte<stream_len-d_ASM_SYM_LEN;byte++) {
			// for a given byte offset, check all possible bit offsets
			for(unsigned int bit=0;bit<d_INFO_BITS_PER_SYMBOL;bit++) {

				const unsigned int hamming_dist = check_for_asm_hamming(&stream[byte], bit);
				// Does the stream at this position match the ASM?			
				if(hamming_dist < min_hamming && hamming_dist <= d_BER_THRESHOLD) {

					// Better match found, store position
					*offset_bytes = byte;
					*offset_bits  = bit;

					min_hamming = hamming_dist;
				}
			}
		}

		return (min_hamming <= d_BER_THRESHOLD);
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
