#ifndef INCLUDED_CCSDS_CONV_DECODE27_IMPL_H
#define INCLUDED_CCSDS_CONV_DECODE27_IMPL_H

#include <ccsds/conv_decode27.h>

#include <fec.h>

// Redefine mapping of decode functions to libfec functions
#define FEC_FUNC_VITERBI27_SET_POLYNOMIAL set_viterbi27_polynomial_port
#define FEC_FUNC_VITERBI27_CREATE create_viterbi27_port
#define FEC_FUNC_VITERBI27_DELETE delete_viterbi27_port
#define FEC_FUNC_VITERBI27_INIT init_viterbi27_port
#define FEC_FUNC_VITERBI27_UPDATE_BLK update_viterbi27_blk_port
#define FEC_FUNC_VITERBI27_CHAINBACK chainback_viterbi27_port

namespace gr {
  namespace ccsds {

    class conv_decode27_impl : public conv_decode27 {

    private:
    	typedef boost::shared_ptr<float[]> softbits_sptr;


    	/*! \brief Type of unpuncturing that has to be performed before the decoding. */
    	const conv_puncturing27::punct_t d_PUNCT_TYPE;

    	/*! \brief Number of bits (more precise: bytes containing a soft bit)
    	 *	that need to be read in to generate \c d_RATE_NUM_OUT bytes.
    	 *
    	 *  \sa d_RATE_NUM_OUT
    	 */
    	const unsigned int d_RATE_NUM_IN;

    	/*! \brief pointer to the viterbi decoder structure. */
    	void *d_viterbi;

    	/*! \brief Number of bytes that are generated when \c d_RATE_NUM_IN bits
    	 *	have been read.
    	 *
    	 *  \sa d_RATE_NUM_IN
    	 */
    	const unsigned int d_RATE_NUM_OUT;
    
    	/*! \brief Number of soft bits that come in with one message */
    	const unsigned int d_BLOCK_NUM_BITS_IN;
    
    	/*! \brief Number of soft bits that come in one unpunctured message */
    	const unsigned int d_BLOCK_NUM_BITS_IN_UNPUNC;
    
    	/*! \brief Number of data bits that go out of this block. */
    	const unsigned int d_BLOCK_NUM_BITS_OUT;
    
    	/*! \brief Last 6 bits of the ASM that are used to generate the first
    	 *	bits of the frame data.
    	 */
    	const int d_START_STATE;
    
    	/*! \brief First 6 bits of the ASM which should be in the registers,
    	 *	when the decoder has processed the frame and the tail symbols.
    	 */
    	const unsigned int d_TERM_STATE;
    
    	/*! \brief buffer for storing unpunctured soft bits. */
    	unsigned char *d_buffer;
    
    	boost::shared_ptr<bool[]> d_punct_pattern;
    
    	const unsigned int d_PATTERN_LEN;
    
    	/*! \brief Performs the unpuncturing by taking in the punctured data and
    	 *	insert an erasure bit at every position where puncturing occured.
    	 *
    	 *  \param out Array of soft bits that contain the full unpunctured
    	 *	symbol sequence. Memory for at least \c d_RATE_NUM_OUT * 16
    	 *	elements must be allocated.
    	 *  \param in Array of punctured soft bits in float format. Must contain
    	 *	at least \c d_RATE_NUM_IN elements.
    	 */
    	void unpuncture_and_convert(unsigned char *out, const float *in);
    
    	/*! \brief Convert polynom from our own encoder's format to the one used
    	 *	by Phil Karn's FEC library.
    	 *
    	 *  The polynom information is encoded in the 7 least significant bits,
    	 *  but in reverse direction. The inversion is indiceted by a sign flip
    	 *  in Phil Karn's format.
    	 */
    	static int convert_poly(const uint8_t in);
    
    	void process_message(pmt::pmt_t msg_in);
    
    	static unsigned int get_rate_in(conv_puncturing27::punct_t puncturing_type);
    	static unsigned int get_rate_out(conv_puncturing27::punct_t puncturing_type);
    
    	static unsigned int get_num_inputs_req(conv_puncturing27::punct_t puncturing_type, const unsigned int num_out);
    
    	/*! \brief Extract the last 6 bits from the ASM as a right aligned integer */
    	static int get_start_state(std::string ASM);
    
    	/*! \brief Extract the first 6 bits from the ASM as a right aligned integer  */
    	static unsigned int get_term_state(std::string ASM);
    
    
    public:
    	/*! \brief Public constructor for the convolutional decoder block.
    	 *
    	 *  \param gen_poly_c1 Compact generator polynom for c1 stream. Default is 0x79.
    	 *  \param gen_poly_c2 Compact generator polynom for c2 stream. Default is 0xDB.
    	 *  \param puncturing_type Symbol for type of puncturing. Default is NONE.
    	 *  \param block_len Number of output data bits per block as BLOB. (BLOB len =
    	 *	\c block_len / 8).
    	 *  \param ASM Unencoded attached sync marker that is used to determine the
    	 *	start and termination state of the decoder.
    	 *  \return Shared pointer of the Convolutional encoder block.
    	 *
    	 *  The compact generator polynom contains two informations. Encoded in the MSB
    	 *  is the information whether the stream should be inverted (MSB set) or not
    	 *  (MSB unset). Encoded in the 7 less significant bits is the information about
    	 *  the connections between the shift registers and the output. The MSB-1
    	 *  represents the influence of the current information bit i(t) while the LSB
    	 *  represents the influence of the information bit i(t-6). A set bit means
    	 *  there is a connection, while an unset bit means there is no.
    	 *
    	 *  The polynomials for the convolutional code defined in ECSS can be generated
    	 *  with 0x79 for c1 and 0x5B for c2. Together with the MSB for the inversion of
    	 *  c2 the compact polynomials are 0x79 and 0xDB.
    	 */
    	conv_decode27_impl(const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, conv_puncturing27::punct_t puncturing_type, const unsigned int block_len, std::string ASM);
    
    	/*! \brief Public deconstructor. */
    	~conv_decode27_impl(void);
    
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_CONV_DECODE27_IMPL_H */
