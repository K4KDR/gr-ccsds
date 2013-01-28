#ifndef INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_BB_H
#define INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_BB_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include "ccsds_asm_operator.h"

/*! \brief Verbosity level: Do not output anything */
#define CCSDS_AR_OUTPUT_NONE 0

/*! \brief Verbosity level: Do output state information */
#define CCSDS_AR_OUTPUT_STATE 1

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_AR_OUTPUT_DEBUG 2

/*! \brief Level of verbosity of this block.
 *
 *  \sa #CCSDS_AR_OUTPUT_NONE
 *  \sa #CCSDS_AR_OUTPUT_STATE
 *  \sa #CCSDS_AR_OUTPUT_DEBUG
 */
#define CCSDS_AR_VERBOSITY_LEVEL CCSDS_AR_OUTPUT_NONE

class ccsds_mpsk_ambiguity_resolver_bb;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<ccsds_mpsk_ambiguity_resolver_bb> ccsds_mpsk_ambiguity_resolver_bb_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_mpsk_ambiguity_resolver_bb
 *
 *  Create an instance of ccsds_mpsk_ambiguity_resolver_bb and return it's shared_ptr.
 *
 *  \param M Modulation order. Determins how many parallel streams to watch.
 *  \param ASM String containing the attached sync marker to look for in
 *	hexadecimal representation. Must be of even length.
 *  \param threshold Number of ASM losses to enter full search again. If
 *	set to one, a full search is performed every time an ASM is lost.
 *  \param frame_length Length of a frame (without ASM) in bytes.
 *  \return Shared pointer to the created AR block
 */
CCSDS_API ccsds_mpsk_ambiguity_resolver_bb_sptr ccsds_make_mpsk_ambiguity_resolver_bb(const unsigned int M, std::string ASM, unsigned int threshold, const unsigned int frame_length);

/*!
 * \brief M-PSK Ambiguity resolution.
 *
 * \ingroup synchronization
 *
 * The input to this block is a stream of unpacked bytes, each containing \c ld(M)
 * used information bits. The output of this block is a stream of packed bytes.
 *
 * The block will start in searchmode, looking for every possible bit and byte
 * offset in all possible \c M ambiguities. Once an ASM is found it will enter
 * locked state where it will only check for the ASM at the expected position
 * and ambiguity. If the ASM is found a counter is increased up to
 * \c d_THRESHOLD. If an exprected ASM is not found the counter is decreased.
 * If the counter reaches zero the block goes into search mode again.

 * The stream with the last known ambiguity and bit offset is send to the output.
 * In locked state this will result in the correct byte stream beeing output,
 * while in searchmode the output might be nonsense (since there is no ASM in it)
 * or might still be correct if the block has lost it's lock to early.
 */
class CCSDS_API ccsds_mpsk_ambiguity_resolver_bb : public gr_block
{
private:
	friend CCSDS_API ccsds_mpsk_ambiguity_resolver_bb_sptr ccsds_make_mpsk_ambiguity_resolver_bb(const unsigned int M, std::string ASM, unsigned int threshold, const unsigned int frame_length);

	/*!
	 * \brief Private constructor of the AR
	 *  \param M Modulation order. Determins how many parallel streams to
	 *	watch.
	 *  \param ASM String containing the attached sync marker to look for in
	 *	hexadecimal representation. Must be of even length.
	 *  \param threshold Number of ASM losses to enter full search again. If
	 *	set to one, a full search is performed every time an ASM is lost.
	 *  \param frame_length Length of a frame (without ASM) in bytes.
	 *
	 *  Constructs a AR block that searches for the ASM in the \c M input
	 *  streams and outputs the one that contains the ASM. If no ASM is
	 *  found, no output is given. This block preserves the ASM, as it is
	 *  still needed for frame synchronization after decoding.
	 */
	ccsds_mpsk_ambiguity_resolver_bb(const unsigned int M, std::string ASM, unsigned int threshold, const unsigned int frame_length);
	
	/*! \brief Enumerator for the two different states. */
	enum d_STATE {
		STATE_SEARCH=0,
		STATE_LOCK};

	/*! \brief State variable */
	d_STATE d_state;

	/*! \brief Modulation order */
	const unsigned int d_M;

	/*! \brief Mapping to translate from the gray coded bits to the binary
	 *	coded constellation index.
	 */
	const unsigned char *d_map_bits_to_index;

	/*! \brief Mapping to translate from the binary coded constellation
	 *	index to the gray coded bits.
	 */
	const unsigned char *d_map_index_to_bits;

	/*! \brief number of bits in an unpacked input bytecalculated as ld(d_M). */
	const unsigned int d_ldM;

	/*! \brief Number of ASM losses to enter full search again. If
	 *	set to one, a full search is performed every time an ASM is lost.
	 */
	const unsigned int d_THRESHOLD;

	/*! \brief Length of a frame (without ASM) in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Length of an ASM in bytes. */
	const unsigned int d_ASM_LEN;

	/*! \brief Counter variable on how many ASMs have been observed */
	unsigned int d_count;

	/*! \brief Counter variable on how bytes have been observed in total */
	unsigned long dbg_count;

	/*! \brief Index of the input stream that the AR is locked in. If in
	 *	search state, this variable may contain anything.
	 *
	 *  \sa d_state
	 */
	unsigned int d_locked_on_stream;
	

	/*! \brief Byte index of the next expected ASM if locked. If in search
	 *	state this variable may contain anything.
	 *
	 *  \sa d_state
	 *  \sa d_offset_bits
	 */
	unsigned int d_offset_bytes;

	/*! \brief Bit index of the next expected ASM in the byte indicated by
	 *	d_offset_byte if in lock state. If in search state this
	 *	variable may contain anything.
	 *
	 *  \sa d_state
	 *  \sa d_offset_bytes
	 */
	unsigned int d_offset_bits;

	/*! \brief Number of bits the first sample that belong to a old byte
	 *	which's samples have already been consumed.
	 */
	unsigned int d_samp_skip_bits;

	/*! \brief Pointer to helper class \ref ccsds_asm_operator */
	ccsds_asm_operator *d_asm_operator;

	/*! \brief Extract an array of packed bytes from the samples of unpacked
	 *	bits and change it's change ambiguity.
	 *
	 *  \param in_unpacked Array of samples (unpacked bits) to process.
	 *  \param bytes_offset Index of the first requested byte if the
	 *	complete packed stream was available in a single array.
	 *  \param bytes_req Number of bytes that should be produced.
	 *  \param ambiguity The number of symbols by which the received
	 *	constellation was rotated before going into the detector.
	 *
	 *  \sa d_offset_bits
	 *
	 *  Assuming the input samples (unpacked bits) would be converted into
	 *  one huge array of packed bytes. This function gives access to the
	 *  \c bytes_req elements of this array starting at index \c bytes_offset,
	 *  given the complete original input sequence \c in_unpacked.
	 *  Additionally the byte and sample stream can be shifted by a few bits
	 *  is the last run of \ref general_work did not end on a boundary of a
	 *  sample but only on a byte boundary. This shift is stored in
	 *  \c d_offset_bits.
	 *
	 *  The input bits are converted from gray code to binary code to see
	 *  which constellation point was received, increase it's index by
	 *  \c ambiguity_offset and convert the binary index back to gray coded
	 *  bits.
	 */
	unsigned char * get_packed_bytes(const unsigned char *in_unpacked, const unsigned int bytes_offset, const unsigned int bytes_req, unsigned int ambiguity);

	
public:
	/*! \brief Public deconstructor of the AR */	
	~ccsds_mpsk_ambiguity_resolver_bb ();  // public destructor

	void forecast(int noutput_items,gr_vector_int &ninput_items_required);

	int  general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items);};

#endif /* INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_BB_H */
