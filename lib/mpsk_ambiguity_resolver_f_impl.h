#ifndef INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_F_IMPL_H
#define INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_F_IMPL_H


#include <ccsds/mpsk_ambiguity_resolver_f.h>
#include "hexstring_to_binary.h"
#include "mpsk_constellation.h"
#include <gnuradio/tags.h>
#include <string>
#include <map>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <cstdio>
#include <boost/scoped_array.hpp>
#include <volk/volk.h>

namespace gr {
  namespace ccsds {

    class mpsk_ambiguity_resolver_f_impl : public mpsk_ambiguity_resolver_f
    {
    private:
    	
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
    
    	/*! \brief Minimum correlation that a sequence and the ASM must have to
    	 *	have a match.
    	 */
    	const float d_CORRELATION_THRESHOLD;
    
    	/*! \brief Length of a frame (without ASM) in bytes. */
    	//const unsigned int d_FRAME_LEN;
    
    	/*! \brief Length of a frame (without ASM) in bits. */
    	const unsigned int d_FRAME_LEN_BITS;
    
    	/*! \brief Storage for soft bit ASM sequence */
    	float *d_ASM;
    
    	/*! \brief Length of an ASM in bits. */
    	const unsigned int d_ASM_LEN_BITS;
    
    	/*! \brief Number of symbols/bits that should be copied as well after a frame */
    	const unsigned int d_NUM_TAIL_SYMS;
    
    	/*! \brief Minimum search length */
    	const unsigned int d_SEARCH_LEN_MIN;
    
    	/*! \brief Maximum search length
    	 *
    	 *  Within this length there must be a full ASM if there is one.
    	 */
    	const unsigned int d_SEARCH_LEN_MAX;
    
	/*! \brief Block verbosity level */
	const ambiguity_verbosity_t d_VERBOSITY;

    	/*! \brief Counter variable on how many ASMs have been observed */
    	unsigned int d_count;
    
    	/*! \brief buffer to strore message in */
    	pmt::pmt_t d_msg_buffer;
    
    	/*! \brief Number of elements that are already in buffer */
    	unsigned int d_msg_buffer_count;
    
    	/*! \brief Flag whether the buffer is beeing filled right now, or not.
    	 *	If not and we are in state LOCK, check for the existence of an
    	 *	ASM at the current position.
    	 */
    	bool d_msg_buffer_fill;
    
    	/*! \brief Counter for the outgoing frames. */
    	uint64_t d_frame_count;
    
    	/*! \brief Absolute sample counter when the tags have been updated the last time */
    	uint64_t d_tag_lastupdate;
    
    	/*! \brief Absolute sample counter when the rx_time was seen last time */
    	uint64_t d_tag_last_rx_time;
    
    	/*! \brief Buffer to store received stream tags that are copied into the frame */
    	std::map<std::string,pmt::pmt_t> d_tag_buffer;
    
    	/*! \brief Aligned buffer of d_ASM_LEN_BITS floats for ASM checking. */
    	float *d_tmp_fv;
    
    	/*! \brief Aligned buffer of one float for ASM checking. */
    	float *d_tmp_f;
    
	/*! \brief File pointer for debugging. */
	FILE *dbg_file_in;

	/*! \brief File pointer for debugging. */
	FILE *dbg_file_in_hard;

	/*! \brief File pointer for debugging. */
	FILE *dbg_file_out;

	/*! \brief File pointer for debugging. */
	FILE *dbg_file_asms;
	
	/*! \brief Counter for debugging. */
	unsigned long dbg_count;
    	
	/* ControlPort test
	 * export internal variable correlation
	 */
	float d_correlation;
    
    	/*! \brief Index of the input stream that the AR is locked in. If in
    	 *	search state, this variable may contain anything.
    	 *
    	 *  \sa d_state
    	 */
    	unsigned int d_locked_on_stream;
    	
    
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
        
        /*! \brief Remember if the stop function has been called, in which
         * case the block will stop processing and  tell GNURadio that it has
         * no more output available.
         */
        bool d_stopped;
        
    	/*!
    	 *  \brief Returns the number of unpacked symbols (as float) that relate
    	 *	to the given number of packed bytes.
    	 *
    	 *  \param packed Number of packed bytes.
    	 *  \return Number of unpacked bytes (symbols) that relate to \c packed.
    	 */
    	float from_packed_to_unpacked(unsigned int packed);
    
    	/*!
    	 *  \brief Ensures that \c d_offset_bits is confined within [0,d_ldM).
    	 *	If not \c d_offset_bytes is increased accordingly.
    	 *
    	 *  \sa d_offset_bytes
    	 *  \sa d_offset_bits
    	 */
    	void confine_offsets(void);
    
    	/*!
    	 *  \brief Change ambiguity of incomming symbol stream.
    	 *
    	 *  \param out Symbol array where to put the new ambiguity in. Memory
    	 *	for at least \c num elements must be allocated.
    	 *  \param in Symbol array which's abmiguity is to be changed. Must
    	 *	contain at least \c num elements.
    	 *  \param num_bits Number of elements (single bits) that should be converted.
    	 *	Must be a multiple of \c d_ldM, otherwise the last bits will be
    	 *	ignored.
    	 *  \param ambiguity Ambiguity that should be calculated. Must be in the
    	 *	range of [0,d_M), where 0 will just copy \c num elements from
    	 *	\c in to \c out.
    	 */
    	void convert_ambiguity(float *out, const float *in, const unsigned int num_bits, unsigned int ambiguity);
    
    	/*! \brief Return the correlation between the input sequence and the ASM
    	 *
    	 *  \param in Array of input softbits. Must contain at least \c offset+
    	 *	d_ASM_LEN_BITS elements
    	 *  \param offset Number of elements in \c in to skip.
    	 *  \return Correlation
    	 */
    	float check_for_ASM(const float *in, const unsigned int offset);
    
    	/*! \brief Search for ASM in input sequence and return maximum found
    	 *	correlation.
    	 *
    	 *  The search checks all possible offsets in the range of
    	 *  [0, num_in-d_ASM_LEN_BITS] starting from zero and abborts if a
    	 *  correlation of 1.0f is reached, or all offsets have been tried.
    	 *
    	 *  \param in Array of \c num_in input softbits. Must contain at least
    	 *	\c offset+d_ASM_LEN_BITS elements.
    	 *  \param num_in Number of elements in \c in. Must be at lease
    	 *	\c d_ASM_LEN_BITS
    	 *  \param offset Pointer to an unsigned int where the offset of the
    	 *	found maximum correlation is stored.
    	 *  \return The maximum found correlation.
    	 */
    	float search_for_ASM(const float *in, const unsigned int num_in, unsigned int *offset);
    
    	/*! \brief Converts an array of \c d_ldM softbits into a byte of
    	 *	\c d_ldM hardbits.
    	 *
    	 *  \param in Array of floats. Must contain at least \c d_ldM elements.
    	 *  \return Byte containing the converted hardbits in the \c d_ldM least
    	 *	significant bit positions.
    	 */
    	unsigned char convert_to_char(const float *in);
    
    	/*! \brief Get the highest multiple of \c d_ldM lower or equal than \c n.
    	 *
    	 *  \param n Number to process
    	 *  \return floor(\c n / \c d_ldM)
    	 */
    	unsigned int get_lower_mul(const unsigned int n);
    
    	/*! \brief Get the lowest multiple of \c d_ldM higher or equal than \c n.
    	 *
    	 *  \param n Number to process
    	 *  \return ceil(\c n / \c d_ldM)
    	 */
    	unsigned int get_upper_mul(const unsigned int n);
    
    	/*! \brief Scan all stream tags in scope and copy them into \c d_tag_buffer
    	 *
    	 *  Scan for all tags after max(\c d_tag_lastupdated, \c this->nitems_read(0))
    	 *  and before \c until.
    	 *
    	 *  \param until Absolute sample count where to stop searching. This
    	 *	value is not included into the search anymore.
    	 */
    	void updateTags(const uint64_t until);
    
    	/*! \brief Reads all tags from the buffer and creates a frame metadata
    	 *	header out of it.
    	 *
    	 *  Calculates the reception time of the sample with the absolute count
    	 *  \c from based on the tag rx_time. Otherwise ignore the rx_time tag
    	 *  and label the calculated reception time rx_time in the created
    	 *  metadata.
    	 *
    	 *  Copies all other tags into the header.
    	 *
    	 *  \param from Absolute sample count for which the reception time
    	 *	should be calculated.
    	 *  \param len Length of the frame. Consider all tags up to \c from +
    	 *	\c len.
    	 *  \return PMT dictionary containing the extracted metadata. Keys are
    	 *	PMT symbols (strings), values are of generic type PMT and are
    	 *	copied from the Tags.
    	 */
    	pmt::pmt_t extractTags(const uint64_t from, const unsigned int len);
	
	public:
    	/*!
    	 * \brief Public constructor of the AR
    	 *  \param M Modulation order. Determins how many parallel streams to
    	 *	watch.
    	 *  \param ASM String containing the attached sync marker to look for in
    	 *	hexadecimal representation. Must be of even length.
    	 *  \param asm_len Length of the ASM in bits
    	 *  \param threshold Number of ASM losses to enter full search again. If
    	 *	set to one, a full search is performed every time an ASM is lost.
    	 *  \param correlation_threshold Minimum normalized correlation between an incomming
    	 *	bit sequence and the ASM to consider them matching.
    	 *  \param num_tail_syms Number of bits after the Frame that should be copied as
    	 *	well. Default is zero, so only the frame data is copied.
    	 *  \param frame_length Length of a frame (without ASM) in bytes.
		 *  \param verbosity Verbosity level of block output.
    	 *
    	 *  Constructs a AR block that searches for the ASM in the \c M input
    	 *  streams and outputs the one that contains the ASM. If no ASM is
    	 *  found, no output is given. This block preserves the ASM, as it is
    	 *  still needed for frame synchronization after decoding.
    	 */
    	mpsk_ambiguity_resolver_f_impl(const unsigned int M, std::string ASM, const unsigned int asm_len, const unsigned int threshold,
	                                const float correlation_threshold, const unsigned int frame_length, const unsigned int num_tail_syms, ambiguity_verbosity_t verbosity);
    	/*! \brief Public deconstructor of the AR */	
    	~mpsk_ambiguity_resolver_f_impl ();  // public destructor
    
    	void forecast(int noutput_items,gr_vector_int &ninput_items_required);
    
    	bool stop(void);
    	
	// get correlation is needed for ControlPorts
	virtual float get_correlation();
	
	// the function that gives us the Controlport
	void setup_rpc();
	
    	int  general_work (int  noutput_items,
    			        gr_vector_int               &ninput_items,
    			        gr_vector_const_void_star   &input_items,
    			        gr_vector_void_star         &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_F_IMPL_H */
