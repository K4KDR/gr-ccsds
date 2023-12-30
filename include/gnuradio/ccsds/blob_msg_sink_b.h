#ifndef INCLUDED_CCSDS_BLOB_MSG_SINK_B_H
#define INCLUDED_CCSDS_BLOB_MSG_SINK_B_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ccsds {

    /*! \brief Reads frames from message input port "in" and outputs it to a
     *	TicpClient.
     *
     *  \ingroup ticp
     */
    class CCSDS_API blob_msg_sink_b : virtual public gr::sync_block
    {
    public:
    	typedef std::shared_ptr<blob_msg_sink_b> sptr;
    	static sptr make(const unsigned int blob_len);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MSG_BLOB_SINK_B_H */
