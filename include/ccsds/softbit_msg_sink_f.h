#ifndef INCLUDED_CCSDS_SOFTBIT_MSG_SINK_F_H
#define INCLUDED_CCSDS_SOFTBIT_MSG_SINK_F_H

#include <ccsds/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ccsds {

    /*! \brief Reads frames from input stream and sends them out as messages
     */
    class CCSDS_API softbit_msg_sink_f : virtual public gr::sync_block
    {
    public:
    	typedef boost::shared_ptr<softbit_msg_sink_f> sptr;
    	static sptr make(const unsigned int frame_len);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SOFTBIT_MSG_SINK_F_H */
