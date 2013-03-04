#ifndef INCLUDED_CCSDS_MSG_NULL_SRC_H
#define INCLUDED_CCSDS_MSG_NULL_SRC_H

#include <ccsds_api.h>
#include <gr_block.h>


class ccsds_msg_null_src;

typedef boost::shared_ptr<ccsds_msg_null_src> ccsds_msg_null_src_sptr;

CCSDS_API ccsds_msg_null_src_sptr ccsds_make_msg_null_src(bool dummy=false);

/*! \brief Defines a message out port that is never used. This block can be
 *	used to connect asynchronous blocks to the flowgraph and then pass in
 *	messages to them directly from the python code.
 *
 *  This block should only be used in QA code.
 */
class CCSDS_API ccsds_msg_null_src : public gr_block
{
public:
	friend CCSDS_API ccsds_msg_null_src_sptr ccsds_make_msg_null_src(bool dummy);

private:
	/*! \brief Private constructor. Define a outgoing port witht the name
	 *	\c out. Otherwise do nothing.
	 */
	ccsds_msg_null_src(bool dummy=false);
};

#endif /* INCLUDED_CCSDS_MSG_NULL_SRC_H */
