#ifndef INCLUDED_CCSDS_MPSK_MOD_BC_H
#define INCLUDED_CCSDS_MPSK_MOD_BC_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

/*! \brief M-PSK modulator. 
 *  \ingroup sender
 *
 *  On construction a constellation array is generated. This constellation is
 *  later used to map the individual data bits to symbols.
 */
namespace gr {
  namespace ccsds {

    class CCSDS_API mpsk_mod_bc :  virtual public gr::sync_block
    {
    public:
    	typedef std::shared_ptr<mpsk_mod_bc> sptr;
    	static sptr make (unsigned int M);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_MOD_BC_H */
