/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_randomiser_bb.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <iostream>

#define VERBOSE 0

ccsds_randomiser_bb_sptr
ccsds_make_randomiser_bb(gr_msg_queue_sptr output_queue, int frame_bits, int mask, int seed, int len)
{
  return gnuradio::get_initial_sptr(new ccsds_randomiser_bb(output_queue, frame_bits, mask, seed, len));
}

ccsds_randomiser_bb::ccsds_randomiser_bb(gr_msg_queue_sptr output_queue, int frame_bits, int mask, int seed, int len)
	: gr_sync_block("ccsds_randomiser_bb",
		  gr_make_io_signature (1, 1, sizeof (unsigned char)),
		  gr_make_io_signature (0, 0, sizeof (unsigned char))),
	d_output_queue(output_queue),
	d_lfsr(mask, seed, len),
	d_bits(0),
	d_count(0),
	d_byte(0),
	d_frame_bits(frame_bits),
	d_frame_bytes(frame_bits>>3),
	d_frame_counter(0)
{
	d_packed_frame = new unsigned char[d_frame_bytes];
}

int
ccsds_randomiser_bb::work(int noutput_items,
				gr_vector_const_void_star& input_items,
				gr_vector_void_star& /*output_items*/)
{
	const unsigned char *in = (const unsigned char *) input_items[0];

	int count = 0;

	while (count < noutput_items){
		d_packed_frame[d_count++>>3] = d_byte = (d_byte<<1) | ((in[count++]^d_lfsr.next_bit()) & 1);
	
		if (d_count == d_frame_bits) {
			// make a new message with the frame as payload
			gr_message_sptr msg = gr_make_message(0, 0, 0, d_frame_bytes);
			memcpy(msg->msg(), d_packed_frame, d_frame_bytes);
			d_output_queue->insert_tail(msg);	// send it
			# if VERBOSE
				std::cout << "\nPDR:msg made\n" << msg->to_string() << "\n\n";
				printf("\nPDR:sent frame %d\n",++d_frame_counter);
			# endif
			msg.reset();  				// free it up
			d_lfsr.reset(); // reset for next frame - returns the shift register to the original seed
			d_count = 0;
		}
	}

	return noutput_items;
}
