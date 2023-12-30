#ifndef INCLUDED_CCSDS_TRUNK_TAIL_IMPL_H
#define INCLUDED_CCSDS_TRUNK_TAIL_IMPL_H

#include <gnuradio/ccsds/trunk_tail.h>
#include <cstdio>

namespace gr {
  namespace ccsds {

    class trunk_tail_impl : public trunk_tail
    {
    private:
    	/*! \brief Number of softbits to trunkate
    	 *
    	 *  \sa d_TRUNK_LEN
    	 */
    	const unsigned int d_TRUNK_LEN;
    
    	/*! \brief Length of a frame (without ASM) in bytes. */
    	const unsigned int d_FRAME_LEN;
   	
	/*! \brief number of bits that are uncertain at the end */
	const unsigned int d_BLANC_BITS;

    	bool d_stop;
    
    public:
    	/*!
    	 *  \brief Public constructor of the Trunk Tail block.
    	 *
     	 *  \param trunk_len Number of softbits to trunkate
    	 *
    	 *  \param frame_len Length of a frame (without ASM) in bytes.
	 *
	 *  \param blanc_bits bit at the end that are uncertain
    	 */
    	trunk_tail_impl(const unsigned int trunk_len, const unsigned int frame_len, const unsigned int blanc_bits);

    	/*! \brief Public deconstructor of the AR */	
    	~trunk_tail_impl ();  // public destructor
    
    
    	/*! \brief Asynchronous work function which is processing the queues. */
    	void process_frame(pmt::pmt_t msg);
    
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_TRUNK_TAIL_IMPL_H */
