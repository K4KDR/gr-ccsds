#ifndef INCLUDED_CCSDS_TICP_FRAME_SINK_H
#define INCLUDED_CCSDS_TICP_FRAME_SINK_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include <ticp/TicpServer.hpp>
#include <boost/thread.hpp>
#include <gr_msg_queue.h>


class ccsds_ticp_frame_sink_b;

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
typedef boost::shared_ptr<ccsds_ticp_frame_sink_b> ccsds_ticp_frame_sink_b_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_ticp_frame_sink_b
 *
 *  Create an instance of ccsds_ticp_frame_sink_b and return it's shared_ptr.
 *
 *  \param port Port of the TicpServer
 *  \param frame_length Length of a frame in bytes.
 *  \param msgq Message queue containing the frames to be send to the client.
 *	It's messages should be exactly of size \c d_FRAME_LEN in bytes.
 *  \return Shared pointer to the created TICP frame sink.
 */
CCSDS_API ccsds_ticp_frame_sink_b_sptr ccsds_make_ticp_frame_sink_b(unsigned int port, const unsigned int frame_length, gr_msg_queue_sptr msgq);

/*! \brief Reads frames from asynchronous message queue and outputs it to a TicpClient. */
class CCSDS_API ccsds_ticp_frame_sink_b : public gr_block, public TicpServer
{
private:
	friend CCSDS_API ccsds_ticp_frame_sink_b_sptr ccsds_make_ticp_frame_sink_b(unsigned int port, const unsigned int frame_length, gr_msg_queue_sptr msgq);

	/*!
	 * \brief Private constructor of the TICP frame sink
	 *
	 *  \param port Port of the TicpServer
	 *  \param frame_length Length of a frame in bytes.
	 *  \param msgq Message queue containing the frames to be send to the client.
	 *	It's messages should be exactly of size \c d_FRAME_LEN in bytes.
	 */
	ccsds_ticp_frame_sink_b(unsigned int port, const unsigned int frame_length, gr_msg_queue_sptr msgq);
	
	/*! \brief Length of a frame in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Messagequeue from where to get the frames. */
	gr_msg_queue_sptr d_msgq;

	/*! \brief File pointer for debugging. */
	FILE *dbg_file;

	/*! \brief Counter for debugging. */
	unsigned int dbg_count;

	/*! \brief Pops a frame from the message queue and send it to the client.
	 *	Blocks until there is a frame in the message queue.
	 *
	 *  \return Frame from the message queue. Contains exactly d_FRAME_LEN
	 *	elements
	 */
	const std::vector< unsigned char > getFrame(void);

public:
	/*! \brief Public deconstructor of the ticp frame sink */	
	~ccsds_ticp_frame_sink_b ();  // public destructor

	int  general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items);};

#endif /* INCLUDED_CCSDS_TICP_FRAME_SINK_H */
