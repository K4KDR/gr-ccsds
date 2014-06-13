#ifndef INCLUDED_CCSDS_RS_DECODE_H
#define INCLUDED_CCSDS_RS_DECODE_H

#include <ccsds/api.h>
#include <gnuradio/block.h>
namespace gr {
  namespace ccsds {
    /*!
     *  \brief Takes the transmitted codeblock and generates the frame out of it, or
     *	drops it, if recovery is not possible.
     *
     *  \ingroup channelcoding
     *
     *  The block has an input message port named "in" at which codeblocks of length
     *  \c 255*I bytes are received. On reception the data codeblock is interleaved
     *  and decoded. The generated frame consisting of \c 223*I bytes is then
     *  queued in the output message port named "out" or dropped if it can't be
     *  decoded.
     */
    class CCSDS_API rs_decode : virtual public gr::block
    {
    public:
    	typedef boost::shared_ptr<rs_decode> sptr;

	enum repr_t {BERLEKAMP = 0, CONVENTIONAL = 1};

	/*!
	 *  \brief Create RS decoder
	 *
	 *  \param I interleaving length
	 *  \param representation Select the representation to use. 0 for dual basis
	 *  (Berlekamp) representation and 1 for conventional representation
	 */
    	static sptr make(const unsigned int I, const repr_t representation);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_RS_DECODE_H */
