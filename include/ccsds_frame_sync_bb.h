#ifndef INCLUDED_CCSDS_FRAME_SYNC_BB_H
#define INCLUDED_CCSDS_FRAME_SYNC_BB_H

#include <ccsds_api.h>
#include <ccsds_asm_operator.h>
#include <gr_block.h>
#include <string>
#include <gr_msg_queue.h>


/*! \brief Verbosity level: Do not output anything */
#define CCSDS_FS_OUTPUT_NONE 0

/*! \brief Verbosity level: Do output state information */
#define CCSDS_FS_OUTPUT_STATE 1

/*! \brief Verbosity level: Do output into debug file */
#define CCSDS_FS_OUTPUT_FILE 2

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_FS_OUTPUT_DEBUG 3


/*! \brief Level of verbosity of this block.
 *
 *  \sa #CCSDS_FS_OUTPUT_NONE
 *  \sa #CCSDS_FS_OUTPUT_STATE
 *  \sa #CCSDS_FS_OUTPUT_DEBUG
 */
#define CCSDS_FS_VERBOSITY_LEVEL CCSDS_FS_OUTPUT_NONE

class ccsds_frame_sync_bb;

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
typedef boost::shared_ptr<ccsds_frame_sync_bb> ccsds_frame_sync_bb_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_frame_sync_bb
 *
 *  Create an instance of ccsds_frame_sync_bb and return it's shared_ptr.
 *
 *  \param ASM String containing the attached sync marker to look for in
 *	hexadecimal representation. Must be of even length.
 *  \param threshold Number of ASM losses to enter full search again. If
 *	set to one, a full search is performed every time an ASM is lost.
 *  \param ber_threshold Maximum number of bit errors between a sequence and the
 *	ASM to consider the sequence matching. If threshold equals zero the
 *	sequence must be an exact copy of the ASM.
 *  \param frame_length Length of a frame (without ASM) in bytes.
 *  \param msgq Shared pointer to an asynchronous message queue to which the
 *	frame data will be copied.
 *  \return Shared pointer to the created AR block
 */
CCSDS_API ccsds_frame_sync_bb_sptr ccsds_make_frame_sync_bb(std::string ASM, unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length, gr_msg_queue_sptr msgq);

/*!
 * \brief Take the input streams and look for an ASM. If the block finds an ASM
 * it will create an asynchronous message containing the \c frame_length bytes
 * after the ASM. The input stream is copied the output stream in any case.
 * \ingroup synchronization
 *
 * \todo Document final search and lock algorithms.
 */
class CCSDS_API ccsds_frame_sync_bb : public gr_block
{
private:
	friend CCSDS_API ccsds_frame_sync_bb_sptr ccsds_make_frame_sync_bb(std::string ASM, unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length, gr_msg_queue_sptr msgq);

	/*!
	 * \brief Private constructor of the frame synchronizer
	 *  \param ASM String containing the attached sync marker to look for in
	 *	hexadecimal representation. Must be of even length.
	 *  \param threshold Number of ASM losses to enter full search again. If
	 *	set to one, a full search is performed every time an ASM is lost.
	 *  \param ber_threshold Maximum number of bit errors between a sequence
	 *	and the ASM to consider the sequence matching. If threshold
	 *	equals zero the sequence must be an exact copy of the ASM.
	 *  \param frame_length Length of a frame (without ASM) in bytes.
	 *  \param msgq Shared pointer to an asynchronous message queue to which
	 *	the frame data will be copied.
	 *
	 *  Constructs a AR block that searches for the ASM in the \c M input
	 *  streams and outputs the one that contains the ASM. If no ASM is
	 *  found, no output is given. This block preserves the ASM, as it is
	 *  still needed for frame synchronization after decoding.
	 */
	ccsds_frame_sync_bb(std::string ASM, unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length, gr_msg_queue_sptr msgq);
	
	/*! \brief Enumerator for the two different states. */
	enum d_STATE {
		STATE_SEARCH=0,
		STATE_LOCK};

	/*! \brief State variable */
	d_STATE d_state;

	/*! \brief Helper class to operate on the ASM stream */
	ccsds_asm_operator *d_asm_operator;

	/*! \brief Length of ASM in bytes 
	 *
	 *  \sa ccsds_asm_operator
	 */
	const unsigned int d_ASM_LEN;

	/*! \brief Number of ASM losses to enter full search again. If
	 *	set to one, a full search is performed every time an ASM is lost.
	 */
	const unsigned int d_THRESHOLD;

	/*! \brief Maximum number of bit errors that may occur between a
	 *	sequence and the ASM to still consider the sequence as an ASM.
	 *	If set to zero, sequence must match the ASM exactly.
	 */
	const unsigned int d_BER_THRESHOLD;

	/*! \brief Length of a frame (without ASM) in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Number of bytes needed for a search
	 *
	 *  In order to catch at least one complete ASM (if there is one) we
	 *  need this many bytes.
	 *  Worst case: The ASM started one bit before our current observation
	 *  interval. In this case the next ASM starts in the last bit of the 
	 *  d_ASM_LEN+d_FRAME_LEN byte and we need another d_ASM_LEN byte to
	 *  catch the complete ASM (and the first byte of the following frame)
	 */
	const unsigned int d_SEARCH_LEN;

	/*! \brief Number of bytes that are copied to output stream */
	const unsigned int d_COPY_LEN;

	/*! \brief Counter variable on how many ASMs have been observed */
	unsigned int d_count;

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

	/*! \brief Shared pointer to message queue to which the frame data
	 *	should be copied.
	 */
	gr_msg_queue_sptr d_msgq;

	#if CCSDS_FS_VERBOSITY_LEVEL >= CCSDS_FS_OUTPUT_FILE
		/*! \brief File pointer for debugging. */
		FILE *dbg_file_in;
		FILE *dbg_file_instream;
		FILE *dbg_file_out;

		/*! \brief Counter for debugging. */
		unsigned int dbg_count;
	#endif

	/*! \brief Calculates how many input bytes are needed to check for a
	 *	known ASM position.
	 *
	 *  \return \c d_ASM_LEN+d_FRAME_LEN if \c d_offset_bits is zero,
	 *	otherwise \c d_ASM_LEN+d_FRAME_LEN+1
	 */
	const unsigned int get_bytes_required(void);


public:
	/*! \brief Public deconstructor of the AR */	
	~ccsds_frame_sync_bb ();  // public destructor

	void forecast(int noutput_items,gr_vector_int &ninput_items_required);

	int  general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items);};

#endif /* INCLUDED_CCSDS_FRAME_SYNC_BB_H */
