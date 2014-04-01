#ifndef INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_H
#define INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_H

#include <ccsds/api.h>
#include <gnuradio/sync_interpolator.h>


namespace gr {
  namespace ccsds {

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
    class CCSDS_API mpsk_detector_soft_cf : virtual public gr::sync_interpolator
    {
    public:
    	typedef boost::shared_ptr<mpsk_detector_soft_cf> sptr;
    	static sptr make(const unsigned int M);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_DETECTOR_SOFT_CF_H */
