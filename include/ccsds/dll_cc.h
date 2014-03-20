#ifndef INCLUDED_CCSDS_DLL_CC_H
#define INCLUDED_CCSDS_DLL_CC_H

#include <ccsds/api.h>
#include <gnuradio/block.h>


namespace gr {
  namespace ccsds {
    
    /*!
     * \brief Delay locked loop decimating from sample- to symbolrate
     * \ingroup synchronization
     *
     * Delay estimation based on the Gardner passband timing error detector. The
     * synchronous samples are obtained by cubic interpolation.
     */
    class CCSDS_API dll_cc : virtual public gr::block
    {
    public:
    	typedef boost::shared_ptr<dll_cc> sptr;
    	static sptr make(unsigned int osf, float gamma);
    };
    
  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_DLL_CC_H */
