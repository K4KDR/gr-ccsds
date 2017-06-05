#ifndef INCLUDED_CCSDS_SOFTBIT_MSG_SOURCE_F_H
#define INCLUDED_CCSDS_SOFTBIT_MSG_SOURCE_F_H

#include <ccsds/api.h>
#include <gnuradio/sync_block.h>

/*! \brief Reads frames from message input port "in" and outputs it to a
 *	stream.
 */
namespace gr {
  namespace ccsds {

    class CCSDS_API softbit_msg_source_f : virtual public gr::sync_block
    {
    public:
    	typedef boost::shared_ptr<softbit_msg_source_f> sptr;
    	static sptr make(const unsigned int frame_len);
    };

  } // namepsace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SOFTBIT_MSG_SOURCE_F_H */
