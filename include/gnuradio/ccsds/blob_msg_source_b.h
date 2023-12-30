#ifndef INCLUDED_CCSDS_BLOB_MSG_SOURCE_B_H
#define INCLUDED_CCSDS_BLOB_MSG_SOURCE_B_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

/*! \brief Reads frames from message input port "in" and outputs it to a
 *	TicpClient.
 *
 *  \ingroup ticp
 */
namespace gr {
  namespace ccsds {

    class CCSDS_API blob_msg_source_b : virtual public gr::sync_block
    {
    public:
    	typedef std::shared_ptr<blob_msg_source_b> sptr;
    	static sptr make(const unsigned int blob_len, const std::string length_tag_name);
    };

  } // namepsace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_H */
