/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <ccsds_viterbi27_bb.h>
#include "ccsds_conv_encode.h"
#include <gr_io_signature.h>
#include <stdio.h>
#include <iostream>

#define VERBOSE 0

ccsds_viterbi27_bb_sptr
ccsds_make_viterbi27_bb(gr_msg_queue_sptr output_queue, int framesyms, unsigned long int sync_marker, int asm_length)
{
  return gnuradio::get_initial_sptr(new ccsds_viterbi27_bb(output_queue, framesyms, sync_marker, asm_length));
}

ccsds_viterbi27_bb::ccsds_viterbi27_bb(
		gr_msg_queue_sptr output_queue, int framesyms, unsigned long int sync_marker, int asm_length)
  : gr_sync_block("viterbi27_bb",
		      gr_make_io_signature(1, 1, sizeof(char)),
		      gr_make_io_signature(0, 0, sizeof(char))),
		      // 1),  // Rate 1/2 code
	
	d_output_queue(output_queue),
	d_count(0),
	d_frame_counter(0),
	d_framesyms(framesyms),
	d_framebytes(framesyms>>4),
	d_framebits(framesyms>>1),
	d_sync_marker(sync_marker),
	d_asm_length(asm_length)
{
	int polys[2] = { V27POLYB, -V27POLYA }; // the polynomials as recommended by the CCSDS
	set_viterbi27_polynomial(polys);
	// int dec_length = 128; // length of the decision array in bits for streaming decode, or
	// int framebits = 10200; // length of RS encoded frame for frame decode
	if((d_vp = create_viterbi27(d_framebits)) == NULL){
		printf("create_viterbi27 failed\n");
		exit(1);
	}
	d_viterbi_in = new unsigned char[framesyms+6];
	# if VERBOSE
		std::cout << "ASM=" << d_sync_marker << ", length=" << d_asm_length << "\n";
		std::cout << "framebits=" << d_framebits << ", framebytes=" << d_framebytes << "\n";
	# endif
}

ccsds_viterbi27_bb::~ccsds_viterbi27_bb()
{
}

int
ccsds_viterbi27_bb::work(int noutput_items,
			    gr_vector_const_void_star& input_items,
			    gr_vector_void_star& /*output_items*/)
{
  const char *in = (const char *)input_items[0];

  int Gain = 32;
  int ninput_items = 0;

  while (ninput_items < noutput_items) {
	// Initialize Viterbi decoder 
    d_viterbi_in[d_count++] = in[ninput_items] ? -Gain : Gain; // inverting the symbols, I don't get it either...
    if (d_count == d_framesyms) {
		# if VERBOSE
			printf("\nviterbi decoding, channel symbols = %d\n",d_count);
		# endif
		for(int j = 1; j <= 6; j++) 
			// the 6 bits at the head of the next ASM that 'flush out' from the register the 
			// final channel symbols that are affected by the frame bits
			d_viterbi_in[d_count++] = ((d_sync_marker >> (d_asm_length-j)) & 1) ? -Gain : Gain;
		init_viterbi27(d_vp,d_sync_marker&0x3f); // start state is tail of ASM

		// Decode block (decode frame)
		update_viterbi27_blk(d_vp,d_viterbi_in,d_framebits+6);

		// Do Viterbi chainback and load the decoded frame directly into the message
		gr_message_sptr msg = gr_make_message(0, 0, 0, d_framebytes);
		chainback_viterbi27(d_vp, msg->msg(), d_framebits, 
					((d_sync_marker>>(d_asm_length-6))&0x3f)); // end state is head of ASM
		d_output_queue->insert_tail(msg);	// send it
		# if VERBOSE
			std::cout << "\nV:msg made\n" << msg->to_string() << "\n\n";
			printf("V:sent frame %d\n",++d_frame_counter);
		# endif
		msg.reset();  				// free it up

		d_count = 0;
	}
	ninput_items++;
  }
  /* WIP stream decoder
  for (int i = 0; i < noutput_items*2; i++) {
	// Initialize Viterbi decoder 
    d_viterbi_in[d_count++] = in[i] ? -Gain : Gain;
    if (d_count % 2 == 0) {
		unsigned char data[d_framebits/8];
		printf("%d\n",d_count);
		for(int j = 6; j > 0; j--)
			d_viterbi_in[d_count++] = ((0xd6 >> j) & 1) ? -Gain : Gain;
		init_viterbi27(d_vp,0x16); // FIXME: hardcoded ASM tail

		// Decode block 
		update_viterbi27_blk(d_vp,d_viterbi_in,d_framebits+6);

		// Do Viterbi chainback 
		chainback_viterbi27(d_vp,out,d_framebits,0x1a); // FIXME: hardcoded ASM head
		d_count = 0;
		// break;
	}
  }
  */
  # if VERBOSE
	printf("V:finished %d\n",ninput_items);
  # endif
  return ninput_items;
}
