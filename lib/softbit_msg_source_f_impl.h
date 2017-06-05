#ifndef INCLUDED_CCSDS_SOFTBIT_MSG_SOURCE_F_IMPL_H
#define INCLUDED_CCSDS_SOFTBIT_MSG_SOURCE_F_IMPL_H

#include <ccsds/softbit_msg_source_f.h>
#include <queue>
#include <cstdio>
#include "asynchronous_start.hpp"

namespace gr {
  namespace ccsds {
  
    class softbit_msg_source_f_impl : public softbit_msg_source_f
    {
    private:
    	/*! \brief Length of FRAME in softbits */
    	const unsigned int d_FRAME_LEN;
    
    	/*! \brief Queue in which to buffer the message bytes. */
    	std::queue<float> d_queue;
    
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
    	 *  \param frame_len Length of a frame in softbits.
    	 */
    	softbit_msg_source_f_impl(const unsigned int frame_len);

    	/*! \brief Public deconstructor of the ticp frame sink */	
    	~softbit_msg_source_f_impl ();  // public destructor
    
    	bool start(void);
    	bool stop(void);
    
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SOFTBIT_MSG_SOURCE_F_IMPL_H */
