#ifndef INCLUDED_CCSDS_MPSK_PREAMBLE_CC_H
#define INCLUDED_CCSDS_MPSK_PREAMBLE_CC_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/block.h>


/*! \brief Append a M-PSK preamble before the input stream. 
 *  \ingroup sender
 *
 *  \param num Number of preamble symbols.
 *
 *  Outputs \c num preamble symbols, before the input stream is copied to
 *  the output stream. This can be used to let the receiver synchronize before
 *  sending frames. This way even the first transmitted frame can be received
 *  and decoded.
 *
 *  The preable consists of alternating BPSK symbols (starting with 1+0j). BPSK
 *  symbols are also part of any other M-PSK constellation where M is a power of
 *  two and thus works for all M-PSK receivers.
 */
namespace gr {
  namespace ccsds {
    
    class CCSDS_API mpsk_preamble_cc : virtual public gr::block
    {
    public:
    	typedef std::shared_ptr<mpsk_preamble_cc> sptr;
    	static sptr make (const unsigned int num_symbols);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_PREAMBLE_CC_H */
