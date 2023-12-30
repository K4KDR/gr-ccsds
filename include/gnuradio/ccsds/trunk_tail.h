#ifndef INCLUDED_CCSDS_TRUNK_TAIL_H
#define INCLUDED_CCSDS_TRUNK_TAIL_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ccsds {

    /*!
     *  \brief Takes a frame and trunkates a number of bits from the end.
     *
     *  \ingroup telemetry
     *
     *  The block has an input message port named "in" at which new frames can
     *  arrive. The message is trunkated then queued in the output message port named "out".
     *  
     *  
     */
    class CCSDS_API trunk_tail : virtual public gr::sync_block
    {
    public:
    	typedef std::shared_ptr<trunk_tail> sptr;
	/*!
	* \param trunc_len bits that are trucated
	* \param frame_len the total frame length including trunc_len
	* \param blanc_bits bit at the end that are uncertain
	*/
    	static sptr make(const unsigned int trunc_len, const unsigned int frame_len, const unsigned int blanc_bits);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_TRUNK_TAIL_H */
