#ifndef INCLUDED_CCSDS_BLOB_MSG_SINK_B_IMPL_H
#define INCLUDED_CCSDS_BLOB_MSG_SINK_B_IMPL_H

#include <gnuradio/ccsds/blob_msg_sink_b.h>

namespace gr {
  namespace ccsds {

    class blob_msg_sink_b_impl : public blob_msg_sink_b
    {
    private:
    	/*!
    	 * \brief Private constructor of the TICP frame sink
    	 *
    	 *  \param blob_len Length of a BLOB in bytes.
    	 */
    	
    	/*! \brief Length of BLOBs in bytes */
    	const unsigned int d_BLOB_LEN;
    
    	/*!
    	 *  \brief Buffer for \c d_BLOB_LEN incomming samples
    	 *
    	 *  \sa d_BLOB_LEN
    	 */
    	unsigned char *d_buffer;
    
    	/*!
    	 *  \brief Counter for the number of elements in \c d_buffer
    	 *
    	 *  \sa d_buffer
    	 */
    	unsigned int d_buffer_count;
    
    public:
    	blob_msg_sink_b_impl(const unsigned int blob_len);
    	/*! \brief Public deconstructor of the ticp frame sink */	
    	~blob_msg_sink_b_impl ();  // public destructor
    
    	bool stop(void);
    
    	int work (  int                         noutput_items,
                            gr_vector_const_void_star   &input_items,
                            gr_vector_void_star         &output_items);
    };

  } // namepsace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MSG_BLOB_SINK_B_IMPL_H */
