#ifndef INCLUDED_CCSDS_SOFTBIT_MSG_SINK_F_IMPL_H
#define INCLUDED_CCSDS_SOFTBIT_MSG_SINK_F_IMPL_H

#include <ccsds/softbit_msg_sink_f.h>

namespace gr {
  namespace ccsds {

    class softbit_msg_sink_f_impl : public softbit_msg_sink_f
    {
    private:
    	/*!
    	 * \brief Private constructor of the Softbit frame sink
    	 *
    	 *  \param frame_len Length of a Frame in softbits.
    	 */
    	
    	/*! \brief Length of frame in softbits */
    	const unsigned int d_FRAME_LEN;
    
    	/*!
    	 *  \brief Buffer for \c d_FRAME_LEN incomming samples
    	 *
    	 *  \sa d_FRAME_LEN
    	 */
    	float *d_buffer;
    
    	/*!
    	 *  \brief Counter for the number of elements in \c d_buffer
    	 *
    	 *  \sa d_buffer
    	 */
    	unsigned int d_buffer_count;
    
    public:
    	softbit_msg_sink_f_impl(const unsigned int frame_len);
    	/*! \brief Public deconstructor of the ticp frame sink */	
    	~softbit_msg_sink_f_impl ();  // public destructor
    
    	bool stop(void);
    
    	int work (  int                         noutput_items,
                            gr_vector_const_void_star   &input_items,
                            gr_vector_void_star         &output_items);
    };

  } // namepsace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SOFTBIT_MSG_SINK_F_IMPL_H */
