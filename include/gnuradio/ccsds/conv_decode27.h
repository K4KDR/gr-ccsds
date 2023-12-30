#ifndef INCLUDED_CCSDS_CONV_DECODE27_H
#define INCLUDED_CCSDS_CONV_DECODE27_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>
#include "gnuradio/ccsds/conv_puncturing27.h"

namespace gr {
  namespace ccsds {

    /*! \brief Perform unpuncturing and convloutional decoding on the input stream.
     *  \ingroup channelcoding
     *
     *  This block takes an asynchronous message of punctured convolutional encoded
     *  softbits. The input softbits are frame synchronized, which means that the
     *  leading and following ASM sequences have been stripped off and only contains
     *  the encoded symbols from the actual frame data.
     *
     *  It performs unpuncturing and viterbi decoding of the message and outputs the
     *  decoded data stream as a BLOB message of packed bytes.
     *
     *  Since before and after each message a known ASM is send, the knowledge of
     *  this ASM is used to determine the start and and state of the viterbi decoder
     *  as well as the tail sequence of the block, which is the start of the new ASM
     *  sequence that still depends on the data bits of the current block due to the
     *  code's constraint length.
     *
     *  \todo Synchronize unpuncturing with the puncturer at the sender side. Right
     *	now unpuncturing only works for rate 1/2 and 2/3 as they repeat every
     *	byte.
     */
    class CCSDS_API conv_decode27 : virtual public gr::sync_block {
    public:
    	typedef std::shared_ptr<conv_decode27> sptr;
    	static sptr make (const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, conv_puncturing27::punct_t puncturing_type, const unsigned int block_len, std::string ASM);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_CONV_DECODE27_H */
