#ifndef INCLUDED_CCSDS_SYM_TO_SOFTBITS_BB_H
#define INCLUDED_CCSDS_SYM_TO_SOFTBITS_BB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>

class ccsds_sym_to_softbits_bb;

typedef boost::shared_ptr<ccsds_sym_to_softbits_bb> ccsds_sym_to_softbits_bb_sptr;

/*! \brief Create a symbol to soft bits encoder block and return it's shared
 *	pointer.
 *
 *  \param info_bits Number of information bits in each incomming byte. Also
 *	determins the input/output ration, as for each of the \c num_bits a byte
 *	containing the soft bit is generated.
 *  \param num_quant_bits Number of quantization bits used to encode the
 *	confidentiality value.
 *  \return Shared pointer of the ymbol to soft bits encoder block.
 *
 *  Takes two input sequences with the same rate. The first stream contains
 *  decoded symbols (sequences of \c num_bits data bits) and the second contains
 *  a \c num_quant_bits value which represents the confidence in the data bits,
 *  where a lower value means less confidence.
 *
 *  For each information bit in the input streams a byte is put to the output
 *  streams containing a 7 bit quantized soft bit as expected by  Phil Karn's
 *  viterbi decoder. (0: strongest zero bit, 128: erasure, 255: strongest one
 *  bit).
 */
CCSDS_API ccsds_sym_to_softbits_bb_sptr ccsds_make_sym_to_softbits_bb (unsigned int info_bits, unsigned char num_quant_bits);

/*! \brief Convert data symbols into soft bits for convolutional coding.
 *  \ingroup channelcoding
 *
 *  Takes two input sequences with the same rate. The first stream contains
 *  decoded symbols (sequences of \c num_bits data bits) and the second contains
 *  a \c num_quant_bits value which represents the confidence in the data bits,
 *  where a lower value means less confidence.
 *
 *  For each information bit in the input streams a byte is put to the output
 *  streams containing a 7 bit quantized soft bit as expected by  Phil Karn's
 *  viterbi decoder. (0: strongest zero bit, 128: erasure, 255: strongest one
 *  bit).
 */
class ccsds_sym_to_softbits_bb : public gr_sync_block {

private:
	friend CCSDS_API ccsds_sym_to_softbits_bb_sptr ccsds_make_sym_to_softbits_bb(unsigned int info_bits, unsigned char num_quant_bits);

	/*! \brief Private constructor for the symbol to soft bits encoder block.
	 *
	 *  \param info_bits Number of information bits in each incomming byte. Also
	 *	determins the input/output ration, as for each of the \c num_bits a byte
	 *	containing the soft bit is generated.
	 *  \param num_quant_bits Number of quantization bits used to encode the
	 *	confidentiality value.
	 */
	ccsds_sym_to_softbits_bb (unsigned int info_bits, unsigned char num_quant_bits);   // private constructor

	/*! \brief Number of data bits per incomming symbol. */
	const uint8_t d_NUM_BITS;

	/*! \brief Maximum number the confidence value can have. */
	const uint8_t d_QUANT_MAX;

	/*! \brief Transforms a data bit and it's confidence into a soft bit.
	 *
	 *  \param data_bit Input data bit at the LSB. True if one, false if zero.
	 *  \param confidence Confidence in the decision.
	 */
	uint8_t bit_to_softbit(bool data_bit, uint8_t confidence);

public:
	/*! \brief Public deconstructor. */
	~ccsds_sym_to_softbits_bb ();  // public destructor

	int work (int noutput_items,
		      gr_vector_const_void_star   &input_items,
                      gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_SYM_TO_SOFTBITS_BB_H */
