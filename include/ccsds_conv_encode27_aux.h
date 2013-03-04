#ifndef CCSDS_CONV_ENCODE27_AUX_H
#define CCSDS_CONV_ENCODE27_AUX_H

#include <stdint.h>
#include <ccsds_conv_encode27_punct.h>

class ccsds_conv_encode27_punct_aux {

protected:
	/*!
	 *  \brief Whether or not the c1 stream should be inverted.
	 */
	const bool d_INVERT_C1;

	/*!
	 *  \brief Whether or not the c2 stream should be inverted.
	 */
	const bool d_INVERT_C2;

	/*!
	 *  \brief Generator polynomial for stream c1.
	 *
	 *  LSB corresponds to \c i(t-6), MSB-1 corresponds to \c i(t).
	 *
	 *  Default value according to ECSS is 0x79 or 171
	 */
	const unsigned char d_POLY_C1;

	/*!
	 *  \brief Generator polynomial for stream c2.
	 *
	 *  LSB corresponds to \c i(t-6), MSB-1 corresponds to \c i(t).
	 *
	 *  Default value according to ECSS is 0x5B or 133
	 */
	const unsigned char d_POLY_C2;

	/*! \brief Which punctured version of the code to use. */
	const ccsds_conv_encode27_punct::punct_t d_PUNCT_TYPE;

	/*! \brief Number of bits after which the puncturing pattern repeats. */
	const unsigned int d_PUNCT_LEN;

private:

	/*!
	 *  \brief State information vector
	 *
	 * Counting from 0, starting at the LSB this state contains:
	 *	bit0  - bit6  : Next input byte (that is not processed yet)
	 *	bit7          : Current information bit i(t)
	 *      bit8          : i(t-1)
	 *      bit9          : i(t-2)
	 *      bit10         : i(t-3)
	 *      bit11         : i(t-4)
	 *      bit12         : i(t-5)
	 *      bit13         : i(t-6)
	 *      bit14         : i(t-7)
	 *	bit15         : Allways zero to esnure no overfloat when
	 *				shifting the state left.
	 */	 
	uint16_t d_state;

	/*! \brief Array of booleans when to keep (true) and when to drop bits. */
	boost::shared_ptr<bool[]> d_punct_pattern;

	/*!
	 *  \brief Number of bits we already processed in the current puncturing
	 *	pattern.
	 */
	unsigned int d_punct_count;

protected:

	/*!
	 *  \brief Encode input byte without puncturing. The output are 2 bytes
	 *
	 *  \param output_bits Encoded bitstream (16bits).
	 *  \param input_byte Unencoded bitstream (8bits).
	 */
	void encode(uint16_t& output_bits, unsigned char input_byte);

	/*!
	 *  \brief Perfrom puncturing and report how many bits of the output
	 *	contain valid information bits.
	 *
	 *  \param output_bits Encoded and punctured information bits.
	 *	(\c num_out_bits bits).
	 *  \param num_out_bits Number of bits that contain valid information
	 *	bits in \c output_bits.
	 *  \param input_bits Encoded unpunctured input bitstream (16bits).
	 */
	void puncture(uint16_t& output_bits, uint8_t& num_out_bits, uint16_t input_bits);

public:
	/*!
	 *  \brief Constructor for the encoder helper class.
	 *
	 *  \param gen_poly_c1 Generator polynomial for stream c1 (see
	 *	description below). Default is 0x79 (0x79 and no inversion as
	 *	defined in ECSS)
	 *  \param gen_poly_c2 Generator polynomial for stream c1 (see
	 *	description below). Default is 0x5B|0x80 (0x5B and inversion as
	 *	defined in ECSS)
	 *  \param puncturing Chosen puncturing scheme. Defaults to NONE, which
	 *	means no puncturing is done.
	 *
	 *  \sa d_POLY_C1
	 *  \sa d_POLY_C2
	 *
	 *  The generator polynomial is a condensed information about how to
	 *  combine the shift register outputs and whether or not to invert the
	 *  output stream.
	 *  
	 *  If the MSB is set the output stream will be inverted.
	 *
	 *  The lower 7 bits encode the actual polynomials. The generator
	 *  polynomial is masked with 0x7F (set MSB to zero) and the copied into
	 *  the \c d_poly_x variables.
	 */
	ccsds_conv_encode27_punct_aux(uint8_t gen_poly_c1=0x79, uint8_t gen_poly_c2=0xDB, ccsds_conv_encode27_punct::punct_t puncturing=ccsds_conv_encode27_punct::NONE);

	/*! \brief Destructor */
	~ccsds_conv_encode27_punct_aux();

	/*!
	 *  \brief Encode information byte including puncturing
	 *
	 *  \param output_bits Up to 16 bits which are the output of the
	 *	encoding and puncturing. The exact number of bits that contain
	 *	information is written into \c num_output_bits
	 *  \param num_out_bits Number of bits in \c output_bits that contain
	 *	information based on chosen puncturing.
	 *  \param input_byte 8 input bits to encode and puncture.
	 */
	void encode_punct(uint16_t& output_bits, uint8_t& num_out_bits, unsigned char input_byte);
};

#endif /* CCSDS_CONV_ENCODE27_AUX_H */
