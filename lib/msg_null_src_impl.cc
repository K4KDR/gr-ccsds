#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "msg_null_src_impl.h"

namespace gr {
  namespace ccsds {
    msg_null_src::sptr msg_null_src::make() {
        return gnuradio::get_initial_sptr (new msg_null_src_impl() );
    }
    
    msg_null_src_impl::msg_null_src_impl() : gr::block ("msg_null_src", gr::io_signature::make (0, 0, 0), gr::io_signature::make (0, 0, 0)) {
    	// register output port
    	message_port_register_out(pmt::mp("out"));
    
    }
   msg_null_src_impl::~msg_null_src_impl()
   {
   // nothing to do
   }

  } // namespace ccsds
} // namespace gr
