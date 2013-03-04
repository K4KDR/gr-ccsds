#ifndef INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_H
#define INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_H

#include <ccsds_api.h>
#include <gr_sync_interpolator.h>
#include <gr_complex.h>

class ccsds_mpsk_detector_soft_cf;

typedef boost::shared_ptr<ccsds_mpsk_detector_soft_cf> ccsds_mpsk_detector_soft_cf_sptr;

/*! \brief Create an M-PSK softbit detector and return it's shared pointer.
 *
 *  \param M Modulation order of the M-PSK signal.
 *  \return Shared pointer of the created detector.
 */
CCSDS_API ccsds_mpsk_detector_soft_cf_sptr ccsds_make_mpsk_detector_soft_cf(const unsigned int M);

/*! \brief M-PSK soft bit detector. 
 *  \ingroup receiver
 *
 *  M-PSK soft bit detector.
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
 *
 *  For each incomming symbol \c ld(M) output symbols are generated containing
 *  the detected soft bit as float values, where -1.0f represents the most
 *  confident 0 bit, 0.0f is an erasure (in this case, the received symbol has
 *  no energy at all) and 1.0f represents the most confident 1 bit.
 */
class CCSDS_API ccsds_mpsk_detector_soft_cf : public gr_sync_interpolator
{
private:
	friend CCSDS_API ccsds_mpsk_detector_soft_cf_sptr ccsds_make_mpsk_detector_soft_cf(const unsigned int M);

	/*! \brief Public constructor of the M-PSK soft bit detector
	 *
	 *  \param M Modulation order of the M-PSK signal.
	 *
	 *  Detects data bits from M-PSK modulated symbols. This block detects
	 *  the data bits by a non linear transformation of the sample's phase.
	 */
	ccsds_mpsk_detector_soft_cf (const unsigned int M);   // private constructor

	/*! \brief Modulation order. */
	const unsigned int d_M;

	/*! \brief Number of bits per Symbol. */
	const unsigned int d_ldM;

	/*! \brief Precomputed constant for 2pi with float precision. */
	const float d_TWOPI;

	/*! \brief Array to map binary constellation points to gray coded data
	 *	bits.
	 */
	const unsigned char *map;

	/*! \brief Calculated the logariothmus dualis of M (log of base two) */
	static unsigned int get_ldM(const unsigned int M);
public:

	~ccsds_mpsk_detector_soft_cf ();
	//void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int work (int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_H */
