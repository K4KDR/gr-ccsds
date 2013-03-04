#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_msg_null_src.h>

CCSDS_API ccsds_msg_null_src_sptr ccsds_make_msg_null_src(bool dummy) {
    return ccsds_msg_null_src_sptr (new ccsds_msg_null_src(dummy) );
}

ccsds_msg_null_src::ccsds_msg_null_src(bool /*dummy*/) : gr_block ("ccsds_msg_null_src", gr_make_io_signature (0, 0, 0), gr_make_io_signature (0, 0, 0)) {
	// register output port
	message_port_register_out(pmt::mp("out"));

}
