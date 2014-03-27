#ifndef INCLUDED_CCSDS_TICP_FRAME_SINK_IMPL_H
#define INCLUDED_CCSDS_TICP_FRAME_SINK_IMPL_H

#include <ccsds/ticp_frame_sink.h>
#include <string>
#include <vector>
#include <map>
#include <ticp/TicpServer.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <queue>

// #define CCSDS_TICP_FRAME_SINK_DEBUG

/*! Print out warnings about missing metadata header mappings, otherwise be silent */
// #define CCSDS_TICP_FRAME_SINK_WARN_HEADER

namespace gr {
  namespace ccsds {

    class ticp_frame_sink_impl : public ticp_frame_sink
    {
    private:
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
    
    public:
    	/*!
    	 * \brief Public constructor of the TICP frame sink
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
    	ticp_frame_sink_impl(unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types);
    	/*! \brief Public deconstructor of the ticp frame sink */	
    	~ticp_frame_sink_impl ();  // public destructor
    
    	/*! \brief Store incomming message into buffer. */
    	void process_message(pmt::pmt_t msg_in);
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_TICP_FRAME_SINK_IMPL_H */
