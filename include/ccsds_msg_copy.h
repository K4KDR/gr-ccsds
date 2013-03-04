#ifndef INCLUDED_CCSDS_MSG_COPY_H
#define INCLUDED_CCSDS_MSG_COPY_H

#include <ccsds_api.h>
#include <gr_block.h>


class ccsds_msg_copy;

typedef boost::shared_ptr<ccsds_msg_copy> ccsds_msg_copy_sptr;

CCSDS_API ccsds_msg_copy_sptr ccsds_make_msg_copy(bool dummy=false);

class CCSDS_API ccsds_msg_copy : public gr_block
{
public:
	friend CCSDS_API ccsds_msg_copy_sptr ccsds_make_msg_copy(bool dummy);

private:
	ccsds_msg_copy(void);

	void process_frame(pmt::pmt_t msg);
};

#endif /* INCLUDED_CCSDS_MSG_COPY_H */
