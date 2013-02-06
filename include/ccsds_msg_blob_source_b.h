#ifndef INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_H
#define INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <queue>
#include <asynchronous_start.hpp>

class ccsds_msg_blob_source_b;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<ccsds_msg_blob_source_b> ccsds_msg_blob_source_b_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_msg_blob_source_b
 *
 *  Create an instance of ccsds_msg_blob_source_b and return it's shared_ptr.
 *
 *  \param blob_len Length of a BLOB in bytes.
 *  \return Shared pointer to the created TICP frame sink.
 */
CCSDS_API ccsds_msg_blob_source_b_sptr ccsds_make_msg_blob_source_b(const unsigned int blob_len);

/*! \brief Reads frames from message input port "in" and outputs it to a
 *	TicpClient.
 *
 *  \ingroup ticp
 */
class CCSDS_API ccsds_msg_blob_source_b : public gr_block
{
private:
	friend CCSDS_API ccsds_msg_blob_source_b_sptr ccsds_make_msg_blob_source_b(const unsigned int blob_len);

	/*!
	 * \brief Private constructor of the TICP frame sink
	 *
	 *  \param blob_len Length of a BLOB in bytes.
	 */
	ccsds_msg_blob_source_b(const unsigned int blob_len);
	
	/*! \brief Length of BLOBs in bytes */
	const unsigned int d_BLOB_LEN;

	/*! \brief Queue in which to buffer the pointer to the blobs. */
	std::queue<unsigned char *> d_queue;

	/*! \brief Counter to indicate exit.
	 *
	 *  Set to -1 by default.
	 *
	 *  On arrival of an EOF message this value is set to the number of
	 *  messages that are in the queue right now.
	 *
	 *  If greater than zero this counter is decreased everytime a message
	 *  is copied to the output stream.
	 *
	 *  If this counter is exactly zero, signal EOF
	 */
	int d_stop;

	/*! \brief workarround to ensure we only start transmitting, when the
	 *	flowgraph is started.
	 * \todo reove workarround
	 */
	asynchronous_start d_astart;

	/*! \brief Store incomming message into buffer. */
	void process_message(pmt::pmt_t msg_in);

public:
	/*! \brief Public deconstructor of the ticp frame sink */	
	~ccsds_msg_blob_source_b ();  // public destructor

	bool start(void);
	bool stop(void);

	int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};
#endif /* INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_H */
