#ifndef INCLUDED_CCSDS_ADD_ASM_IMPL_H
#define INCLUDED_CCSDS_ADD_ASM_IMPL_H

#include "ccsds/add_asm.h"


namespace gr {
  namespace ccsds {

    class CCSDS_API add_asm_impl : public add_asm
    {
    private:
    	
    	/*! \brief Length of ASM in bytes 
    	 *
    	 *  \sa d_ASM
    	 */
    	const unsigned int d_ASM_LEN;
    
    	/*! \brief Length of a frame (without ASM) in bytes. */
    	const unsigned int d_FRAME_LEN;
    
    	/*  Storage for the attached sync marker
    	 *
    	 *  \sa d_ASM_LEN
    	 */
    	unsigned char *d_ASM;
    
    	bool d_stop;
    
    	/*! \brief Asynchronous work function which is processing the queues. */
    	void process_frame(pmt::pmt_t msg);
    
    public:
    	add_asm_impl(std::string ASM, const unsigned int frame_len);
    	~add_asm_impl ();  
    
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_ADD_ASM_IMPL_H */
