#ifndef INCLUDED_CCSDS_ADD_ASM_H
#define INCLUDED_CCSDS_ADD_ASM_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>
#include <string>


namespace gr {
  namespace ccsds {

/*!
 *  \brief Takes a frame and adds a sync marker in front of it.
 *
 *  \ingroup telemetry
 *
 *  The block has an input message port named "in" at which new frames can
 *  arrive. On arrival a new message is created consisting of the ASM sequence
 *  followed by the data of the incomming message. This new message is then
 *  queued in the output message port named "out".
 */

    class CCSDS_API add_asm : virtual public gr::sync_block
    {
    public:
    	typedef std::shared_ptr<add_asm> sptr;
    
    	static sptr make(std::string ASM, const unsigned int frame_len);
    	
    };

  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_ADD_ASM_H */
