#ifndef INCLUDED_CCSDS_RS_ENCODE_IMPL_H
#define INCLUDED_CCSDS_RS_ENCODE_IMPL_H

#include <gnuradio/ccsds/rs_encode.h>
#include <string> // required by fec

#include <fec.h>

namespace gr {
  namespace ccsds {
    
    class rs_encode_impl : public rs_encode
    {
    private:
    
    	
    	/*! \brief Depth of interleaving */
    	const unsigned int d_I;
    
    	/*! \brief Number of information symbols per codeword. Value=223 */
    	const unsigned int d_k;
    
    	/*! \brief Number of symbols per codeword. Value=255 */
    	const unsigned int d_n;
    
    	/*! \brief Number of parity check symbols per codeword. Value=32 */
    	const unsigned int d_2E;
    
    	/*! \brief Number of data bytes per codeblock. Value=k*I */
    	const unsigned int d_DATA_LEN;
    
    	/*! \brief Number of parity bytes per codeblock. Value=2E*I */
    	const unsigned int d_PARITY_LEN;
    
    	/*! \brief Number of bytes per codeblock. Value=(k+2E)*I */
    	const unsigned int d_OUT_LEN;
    
    	/*  Storage for the first interleaving stage */
    	unsigned char *d_buf_data;
    
    	/*  Storage for the parity check symbols */
    	unsigned char *d_buf_parity;
    
    	/*  Storage for the parity check symbols after 2nd interleaving step */
    	unsigned char *d_buf_parity2;
    
    	/*  Storage for the parity check symbols */
    	unsigned char *d_buf_out;
    
    
    public:
    	/*!
    	 *  \brief Public constructor of the RS Encode block.
    	 *
    	 *  \param I Depth of interleaving
    	 */
    	rs_encode_impl(const unsigned int I);
    
    	/*! \brief Public deconstructor of the RS Encode block */	
    	~rs_encode_impl ();  // public destructor
    
    	/*! \brief Asynchronous work function which is processing the queues. */
    	void process_frame(pmt::pmt_t msg);
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_RS_ENCODE_IMPL_H */
