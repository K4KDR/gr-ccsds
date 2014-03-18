#ifndef INCLUDED_CCSDS_SOFTBITS_MSG_TO_BYTES_B_IMPL_H
#define INCLUDED_CCSDS_SOFTBITS_MSG_TO_BYTES_B_IMPL_H
#include <ccsds/softbits_msg_to_bytes_b.h>
#include <queue>
#include <cstdio>

namespace gr {
  namespace ccsds {

    class CCSDS_API softbits_msg_to_bytes_b_impl : public softbits_msg_to_bytes_b
    {
    private:
    	
    	/*! \brief Queue in which to buffer the pointer to the blobs. */
    	std::queue<unsigned char> d_queue;
    
    	/*! \brief Flag to indicate EOF arrival. When true and the buffers are
    	 *	empty, signal EOF.
    	 */
    	bool d_stop;
    
    	/*! \brief Store incomming message into buffer. */
    
    public:
    	/*! \brief Public constructor of the TICP frame sink */
    	softbits_msg_to_bytes_b_impl();
    	/*! \brief Public deconstructor of the ticp frame sink */	
    	~softbits_msg_to_bytes_b_impl ();  // public destructor
    
    	void process_message(pmt::pmt_t msg_in);
    	int work (  int                         noutput_items,
                            gr_vector_const_void_star   &input_items,
                            gr_vector_void_star         &output_items);
    };
  
  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_IMPL_H */
