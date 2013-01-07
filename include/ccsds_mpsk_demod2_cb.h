#ifndef INCLUDED_CCSDS_MPSK_DEMOD2_CB_H
#define INCLUDED_CCSDS_MPSK_DEMOD2_CB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_complex.h>

class ccsds_mpsk_demod2_cb;

typedef boost::shared_ptr<ccsds_mpsk_demod2_cb> ccsds_mpsk_demod2_cb_sptr;

/*! \brief Create an M-PSK demodulator and return it's shared pointer.
 *
 *  \param M Modulation order of the M-PSK signal.
 *  \return Shared pointer of the created demodulator.
 */
CCSDS_API ccsds_mpsk_demod2_cb_sptr ccsds_make_mpsk_demod2_cb (unsigned int M);

/*! \brief M-PSK demodulator. 
 *  \ingroup demodulation
 *
 *  M-PSK demodulation based on the phase of the incomming signal.
 *
 *  The incomming phase is converted to a conteallation point (counting
 *  counterclockwise starting from zero at symbol 1+0j) with the non linear
 *  function:
 *
 *  \code const_point = floor( 0.5*M*fmod( angle/pi + 2, 2) + 0.5 ) \endcode
 *
 *  And then the constellation point is mapped to the actual symbol bit sequence
 *  by a binary to gray code mapping [http://en.wikipedia.org/wiki/Grey_code#Converting_to_and_from_Gray_code]:
 *
 *  \code data = (const_point >> 1) ^ const_point \endcode
 */
class CCSDS_API ccsds_mpsk_demod2_cb : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_mpsk_demod2_cb_sptr ccsds_make_mpsk_demod2_cb (unsigned int M);

	/*! \brief Public constructor of the M-PSK demodulator
	 *
	 *  \param M Modulation order of the M-PSK signal.
	 *  \sa ccsds_mpsk_demod_cb
	 *
	 *  Demodulate M-PSK modulated symbols back to data bits. This
	 *  demodulator	detects the data bits by a non linear transformation of
	 *  the sample's phase. See \ref ccsds_mpsk_demod_cb for a demodulator
	 *  based on the nearest neighbour principle.
	 */
	ccsds_mpsk_demod2_cb (const unsigned int M);   // private constructor

	/*! \brief Modulation order. */
	const unsigned int d_M;

	/*! \brief Precomputed constant for 2pi with float precision. */
	const float d_TWOPI;

	/*! \brief Array to map binary constellation points to gray coded data
	 *	bits.
	 */
	char *map;
	
public:

	/*! \brief Public deconstructor */
	~ccsds_mpsk_demod2_cb ();  // public destructor

	int work ( int                         noutput_items,
		gr_vector_const_void_star   &input_items,
		gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_DEMOD2_CB_H */
