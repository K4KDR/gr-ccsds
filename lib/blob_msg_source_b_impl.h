#ifndef INCLUDED_CCSDS_BLOB_MSG_SOURCE_B_IMPL_H
#define INCLUDED_CCSDS_BLOB_MSG_SOURCE_B_IMPL_H

#include <ccsds/blob_msg_source_b.h>
#include <queue>
#include <cstdio>
#include "asynchronous_start.hpp"

namespace gr {
  namespace ccsds {
  
    class blob_msg_source_b_impl : public blob_msg_source_b
    {
    private:
    	/*! \brief Length of BLOBs in bytes */
    	const unsigned int d_BLOB_LEN;
    
    	/*! \brief Queue in which to buffer the message bytes. */
    	std::queue<unsigned char> d_queue;
    
    	/*! \brief Flag to indicate EOF arrival. When true and the buffers are
    	 *	empty, signal EOF.
    	 */
    	bool d_stop;
    
    	/*! \brief workarround to ensure we only start transmitting, when the
    	 *	flowgraph is started.
    	 * \todo Remove this workarround as soon as GNURadio has a method to
    	 *      signal the start of a flowgraph for asynchronous sources.
    	 */
    	asynchronous_start d_astart;
    
    	/*! \brief Store incomming message into buffer. */
    	void process_message(pmt::pmt_t msg_in);
    
    public:
    	/*!
    	 * \brief Public constructor of the TICP frame sink
    	 *
    	 *  \param blob_len Length of a BLOB in bytes.
    	 */
    	blob_msg_source_b_impl(const unsigned int blob_len);

    	/*! \brief Public deconstructor of the ticp frame sink */	
    	~blob_msg_source_b_impl ();  // public destructor
    
    	bool start(void);
    	bool stop(void);
    
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_IMPL_H */
