#ifndef INCLUDED_CCSDS_RS_DECODE_IMPL_H
#define INCLUDED_CCSDS_RS_DECODE_IMPL_H

#include <ccsds/rs_decode.h>
#include <cstdio>
#include <string> // required by fec

#include <fec.h>

/*! \brief Verbosity level: Do not output */
#define CCSDS_RS_DECODE_OUTPUT_NONE 0

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_RS_DECODE_OUTPUT_FRAMEERR 1

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_RS_DECODE_OUTPUT_FRAMEINFO 2

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_RS_DECODE_OUTPUT_DEBUG 3

/*! \brief Level of verbosity of this block.
 *
 *  \sa #CCSDS_RS_DECODE_OUTPUT_NONE
 *  \sa #CCSDS_RS_DECODE_OUTPUT_DEBUG
 */
#define CCSDS_RS_DECODE_VERBOSITY_LEVEL CCSDS_RS_DECODE_OUTPUT_FRAMEERR

namespace gr {
  namespace ccsds {

    class rs_decode_impl : public rs_decode
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
    
    	/*! \brief Number of bytes per codeblock. Value=(k+2E)*I */
    	const unsigned int d_IN_LEN;
    
    	/*!  \brief Storage for the first interleaving stage */
    	unsigned char *d_buf_in;
    
    	/*! \brief Storage for the second interleaving stage */
    	unsigned char *d_buf_data;
    
    	/*! \brief Storage for the output frames symbols */
    	unsigned char *d_buf_out;
    
	/*! \brief Storage for representation type */
	const repr_t d_representation;

    	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_DEBUG
    		/*! \brief File pointer for debugging. */
    		FILE *dbg_file_in;
    
    		/*! \brief File pointer for debugging. */
    		FILE *dbg_file_in_deinterleaved;
    
    		/*! \brief File pointer for debugging. */
    		FILE *dbg_file_in_valid;
    
    		/*! \brief File pointer for debugging. */
    		FILE *dbg_file_out;
    	#endif
    
    
    public:
    	/*!
    	 *  \brief Public constructor of the RS_Decode block.
    	 *
    	 *  \param I Depth of interleaving
	 *  \param representation Data representation to use. 0 For Berlekamp
	 *  (dual basis) representation , 1 for conventional representation.
    	 */
    	rs_decode_impl(const unsigned int I, const repr_t representation);

    	/*! \brief Public deconstructor of the AR */	
    	~rs_decode_impl ();  // public destructor
    
    	/*! \brief Asynchronous work function which is processing the queues. */
    	void process_frame(pmt::pmt_t msg);
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_RS_DECODE_IMPL_H */
