#ifndef INCLUDED_CCSDS_MSG_NULL_SRC_H
#define INCLUDED_CCSDS_MSG_NULL_SRC_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>


namespace gr {
  namespace ccsds {

    
    
    /*! \brief Defines a message out port that is never used. This block can be
     *	used to connect asynchronous blocks to the flowgraph and then pass in
     *	messages to them directly from the python code.
     *  This block should only be used in QA code.
     *	\ingroup ccsds
     */
    class CCSDS_API msg_null_src : virtual public gr::sync_block
    {
    public:
      typedef std::shared_ptr<msg_null_src> sptr;
      static sptr make(); 
    };

  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_MSG_NULL_SRC_H */
