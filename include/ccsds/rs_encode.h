#ifndef INCLUDED_CCSDS_RS_ENCODE_H
#define INCLUDED_CCSDS_RS_ENCODE_H

#include <ccsds/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ccsds {

    /*!
     *  \brief Takes I frames and generates the transmitted codeblock out of it.
     *
     *  \ingroup channelcoding
     *
     *  The block has an input message port named "in" at which frames of length
     *  \c 223*I bytes are received. On reception the frames are interleaved and
     *  encoded. The generated codeblock consisting of \c 255*I bytes is then
     *  queued in the output message port named "out".
     */
    class CCSDS_API rs_encode : virtual public gr::block
    {
    public:
    	typedef boost::shared_ptr<rs_encode> sptr;
    	static sptr make(const unsigned int I);
    };

  } // namespace ccsds 
} // namespace gr

#endif /* INCLUDED_CCSDS_RS_ENCODE_H */
