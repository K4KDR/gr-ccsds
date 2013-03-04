#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_msg_copy.h>

CCSDS_API ccsds_msg_copy_sptr ccsds_make_msg_copy(bool /*dummy*/) {
    return ccsds_msg_copy_sptr (new ccsds_msg_copy() );
}

void ccsds_msg_copy::process_frame(pmt::pmt_t msg) {

	message_port_pub( pmt::mp("out"), msg);

	return;
}

ccsds_msg_copy::ccsds_msg_copy(void) : gr_block ("ccsds_msg_copy", gr_make_io_signature (0, 0, 0), gr_make_io_signature (0, 0, 0)) {
	// register output port
	message_port_register_out(pmt::mp("out"));

	// register input type
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&ccsds_msg_copy::process_frame, this, _1));
}
