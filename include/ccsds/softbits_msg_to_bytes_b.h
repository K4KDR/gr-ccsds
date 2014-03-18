#ifndef INCLUDED_CCSDS_SOFTBITS_MSG_TO_BYTES_B_H
#define INCLUDED_CCSDS_SOFTBITS_MSG_TO_BYTES_B_H

#include <ccsds/api.h>
#include <gnuradio/sync_block.h>

/*! \brief Reads frames of softbits from message input port "in" and outputs the
 *	corresponding hard decisions in a packed byte.
 */
namespace gr {
  namespace ccsds {

    class CCSDS_API softbits_msg_to_bytes_b : virtual public gr::sync_block
    {
    public:
    	typedef boost::shared_ptr<softbits_msg_to_bytes_b> sptr;
    	static sptr make();
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_H */
