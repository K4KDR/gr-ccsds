#ifndef INCLUDED_CCSDS_PLL_CC_H
#define INCLUDED_CCSDS_PLL_CC_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ccsds {
    
    /*! \brief Phase locked loop that sends frequency correction messages.
     *  \ingroup synchronization
     *
     *  Phase locked loop using the Viterbi&Viterbi method to estimate the sample's
     *  phase offset. Estimates are filtered with a linear second order low pass
     *  filter.
     */
    
    class CCSDS_API pll_cc : virtual public gr::block
    {
    public:
    	typedef std::shared_ptr<pll_cc> sptr;
    	static sptr make(unsigned int m, float loop_bandwidth);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_PLL_CC_H */
