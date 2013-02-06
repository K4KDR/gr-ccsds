#ifndef INCLUDED_CCSDS_TICP_FRAME_SINK_H
#define INCLUDED_CCSDS_TICP_FRAME_SINK_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include <ticp/TicpServer.hpp>
#include <boost/thread.hpp>
#include <gr_msg_queue.h>

#define CCSDS_TICP_FRAME_SINK_DEBUG

class ccsds_ticp_frame_sink;

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
typedef boost::shared_ptr<ccsds_ticp_frame_sink> ccsds_ticp_frame_sink_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_ticp_frame_sink
 *
 *  Create an instance of ccsds_ticp_frame_sink and return it's shared_ptr.
 *
 *  \param port Port of the TicpServer
 *  \param frame_length Length of a frame in bytes.
 *  \return Shared pointer to the created TICP frame sink.
 */
CCSDS_API ccsds_ticp_frame_sink_sptr ccsds_make_ticp_frame_sink(unsigned int port, const unsigned int frame_length);

/*! \brief Reads frames from message input port "in" and outputs it to a
 *	TicpClient.
 *
 *  \ingroup ticp
 */
class CCSDS_API ccsds_ticp_frame_sink : public gr_block, public TicpServer
{
private:
	friend CCSDS_API ccsds_ticp_frame_sink_sptr ccsds_make_ticp_frame_sink(unsigned int port, const unsigned int frame_length);

	/*!
	 * \brief Private constructor of the TICP frame sink
	 *
	 *  \param port Port of the TicpServer
	 *  \param frame_length Length of a frame in bytes.
	 */
	ccsds_ticp_frame_sink(unsigned int port, const unsigned int frame_length);
	
	/*! \brief Length of a frame in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Messagequeue in which to buffer the frames. */
	gr_msg_queue_sptr d_msgq;

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		/*! \brief File pointer for debugging. */
		FILE *dbg_file;

		/*! \brief Counter for debugging. */
		unsigned int dbg_count;
	#endif

	/*! \brief Pops a frame from the message queue and send it to the client.
	 *	Blocks until there is a frame in the message queue.
	 *
	 *  \return Frame from the message queue. Contains exactly d_FRAME_LEN
	 *	elements
	 */
	const std::vector< unsigned char > getFrame(void);

	/*! \brief Store incomming message into buffer. */
	void process_message(pmt::pmt_t msg_in);

public:
	/*! \brief Public deconstructor of the ticp frame sink */	
	~ccsds_ticp_frame_sink ();  // public destructor

};
#endif /* INCLUDED_CCSDS_TICP_FRAME_SINK_H */
