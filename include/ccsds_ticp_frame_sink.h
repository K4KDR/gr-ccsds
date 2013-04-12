#ifndef INCLUDED_CCSDS_TICP_FRAME_SINK_H
#define INCLUDED_CCSDS_TICP_FRAME_SINK_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include <vector>
#include <map>
#include <ticp/TicpServer.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <queue>

// #define CCSDS_TICP_FRAME_SINK_DEBUG

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
 *  \param data_type Type of the dataset which's data should be used for the
 *	actuall output data.
 *  \param map_names Vector of names from the PDU header that should be
 *	used. Can be empty. Must have the same length as map_types. All
 *	elements in this vector must be unique.
 *  \param map_types Vector of datatypes that should be used when inserting
 *	metadata intp the TICP protocol. Can be empty. Must have the same
 *	length as map_names. All elements in this vector must be unique. *  \return Shared pointer to the created TICP frame sink.
 */
CCSDS_API ccsds_ticp_frame_sink_sptr ccsds_make_ticp_frame_sink(unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types);

/*! \brief Reads frames from message input port "in" and outputs it to a
 *	TicpClient.
 *
 *  \ingroup ticp
 */
class CCSDS_API ccsds_ticp_frame_sink : public gr_block
{
private:
	friend CCSDS_API ccsds_ticp_frame_sink_sptr ccsds_make_ticp_frame_sink(unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types);

	/*!
	 * \brief Private constructor of the TICP frame sink
	 *
	 *  \param port Port of the TicpServer
	 *  \param frame_length Length of a frame in bytes.
	 *  \param data_type Type of the dataset which's data should be used for the
	 *	actuall output data.
	 *  \param map_names Vector of names from the PDU header that should be
	 *	used. Can be empty. Must have the same length as map_types. All
	 *	elements in this vector must be unique.
	 *  \param map_types Vector of datatypes that should be used when inserting
	 *	metadata intp the TICP protocol. Can be empty. Must have the same
	 *	length as map_names. All elements in this vector must be unique.
	 */
	ccsds_ticp_frame_sink(unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types);
	
	/*! \brief Length of a frame in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Type of the dataset's data that should be used as PDU payload. */
	const uint8_t d_DATA_TYPE;

	/*! \brief FIFO queue in which to buffer the frames. */
	std::queue<ticp::data_v2_t> d_queue;

	/*! \brief Mutex to controll access to queue */
	boost::mutex d_mutex;

	/*! \brief Condition variable to notify the consumer when there is new data */
	boost::condition_variable d_condition_var;

	/*! \brief Shared pointer to the TICP Server */
	boost::shared_ptr< ticp::Server<ticp::data_v2_t> > d_ticp_sptr;

	/*! \brief Map to store the metadata mapping in. */
	std::map<std::string, uint8_t> d_metadata_map;

	#ifdef CCSDS_TICP_FRAME_SINK_DEBUG
		/*! \brief File pointer for debugging. */
		FILE *dbg_file;

		/*! \brief Counter for debugging. */
		unsigned int dbg_count;
	#endif

	unsigned long d_frame_count;

	/*! \brief Pops a frame from the queue and send it to the client.
	 *	Blocks until there is a frame in the queue.
	 *
	 *  \return Frame from the queue.
	 */
	const ticp::data_v2_t getFrame(void);

	/*! \brief Store incomming message into buffer. */
	void process_message(pmt::pmt_t msg_in);

public:
	/*! \brief Public deconstructor of the ticp frame sink */	
	~ccsds_ticp_frame_sink ();  // public destructor

};
#endif /* INCLUDED_CCSDS_TICP_FRAME_SINK_H */
