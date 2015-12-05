#ifndef INCLUDED_CCSDS_conv_encode27_bb_IMPL_H
#define INCLUDED_CCSDS_conv_encode27_bb_IMPL_H

#include <ccsds/conv_encode27_bb.h>
#include <complex.h>
#include "conv_encode27_aux.h"

namespace gr {
  namespace ccsds {

    class conv_encode27_bb_impl : public conv_encode27_bb, protected conv_encode27_aux {
    
    private:
    	/*! \brief Return relative rate in/out for the given puncturing. */
    	static float get_rate(conv_puncturing27::punct_t puncturing_type);
    	
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
    	/*! \brief Public constructor for the convolutional encoder block.
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
    	conv_encode27_bb_impl(const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, conv_puncturing27::punct_t puncturing_type);

    	/*! \brief Public deconstructor. */
    	~conv_encode27_bb_impl();
    
    	void forecast(int noutput_items, gr_vector_int &ninput_items_required);
    
    	int general_work (int                     noutput_items,
    			 gr_vector_int 		     &ninput_items,
                             gr_vector_const_void_star   &input_items,
                             gr_vector_void_star         &output_items);
    };
  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_conv_encode27_bb_IMPL_H */
