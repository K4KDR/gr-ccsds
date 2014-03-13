#ifndef INCLUDED_CCSDS_TRUNK_TAIL_H
#define INCLUDED_CCSDS_TRUNK_TAIL_H

#include <ccsds/api.h>
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
     */
    class CCSDS_API trunk_tail : virtual public gr::sync_block
    {
    public:
    	typedef boost::shared_ptr<trunk_tail> sptr;
    	static sptr make(const unsigned int trunk_len, const unsigned int frame_len);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_TRUNK_TAIL_H */
