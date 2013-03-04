#ifndef INCLUDED_CCSDS_conv_encode27_bb_H
#define INCLUDED_CCSDS_conv_encode27_bb_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <gr_complex.h>
#include <ccsds_conv_encode27_aux.h>

class ccsds_conv_encode27_bb;

typedef boost::shared_ptr<ccsds_conv_encode27_bb> ccsds_conv_encode27_bb_sptr;

/*! \brief Create a Convolutional encoder block and return it's shared pointer.
 *
 *  \param gen_poly_c1 Compact generator polynom for c1 stream. Default is 0x79.
 *  \param gen_poly_c2 Compact generator polynom for c2 stream. Default is 0xDB.
 *  \param puncturing_type Symbol for type of puncturing. Default is NONE.
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
CCSDS_API ccsds_conv_encode27_bb_sptr ccsds_make_conv_encode27_bb (const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, ccsds_conv_encode27_punct::punct_t puncturing_type);

/*! \brief Perform convloutional encoding and puncturing on the input stream.
 *  \ingroup channelcoding
 *
 *  \param gen_poly_c1 Compact generator polynom for c1 stream. Default is 0x79.
 *  \param gen_poly_c2 Compact generator polynom for c2 stream. Default is 0xDB.
 *  \param puncturing_type Symbol for type of puncturing. Default is NONE.
 */
class ccsds_conv_encode27_bb : public gr_block, protected ccsds_conv_encode27_punct_aux {

private:
	friend CCSDS_API ccsds_conv_encode27_bb_sptr ccsds_make_conv_encode27_bb(const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, ccsds_conv_encode27_punct::punct_t puncturing_type);

	/*! \brief Private constructor for the convolutional encoder block.
	 *
	 *  \param gen_poly_c1 Compact generator polynom for c1 stream. Default is 0x79.
	 *  \param gen_poly_c2 Compact generator polynom for c2 stream. Default is 0xDB.
	 *  \param puncturing_type Symbol for type of puncturing. Default is NONE.
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
	ccsds_conv_encode27_bb (const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, ccsds_conv_encode27_punct::punct_t puncturing_type);   // private constructor

	/*! \brief Return relative rate in/out for the given puncturing. */
	static float get_rate(ccsds_conv_encode27_punct::punct_t puncturing_type);
	
	/*! \brief in/out rate for the selected puncturing. */
	const float d_RATE;

	/*! \brief Buffer for encoded bits that have not been send out so far. */
	uint16_t d_buffer;

	/*! \brief Counter of elements in \c d_buffer.
	 *
	 *  \sa d_buffer
	 */
	uint8_t  d_buffer_count;
public:
	/*! \brief Public deconstructor. */
	~ccsds_conv_encode27_bb ();  // public destructor

	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work (int                     noutput_items,
			 gr_vector_int 		     &ninput_items,
                         gr_vector_const_void_star   &input_items,
                         gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_conv_encode27_bb_H */
