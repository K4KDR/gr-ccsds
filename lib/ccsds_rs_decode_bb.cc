/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <ccsds_rs_decode_bb.h>
#include <iostream>
#include <cstdio>

#define VERBOSE 0
#define ERRCNT 0

ccsds_rs_decode_bb_sptr
ccsds_make_rs_decode_bb (gr_msg_queue_sptr output_queue, int codeblock_bits)
{
	return ccsds_rs_decode_bb_sptr (new ccsds_rs_decode_bb (output_queue, codeblock_bits));
}


ccsds_rs_decode_bb::ccsds_rs_decode_bb (gr_msg_queue_sptr output_queue, int codeblock_bits)
	: gr_sync_block ("rs_decode_bb",
		gr_make_io_signature(1, 1, sizeof(char)),
		gr_make_io_signature(0, 0, sizeof(char))),
	d_output_queue(output_queue),
	d_codeblock_bits(codeblock_bits), // 10200
	d_codeblock_syms(codeblock_bits>>3), // 1275
	d_codeword_syms(255),
	d_parity_syms_per_word(32),
	d_interleave_depth(d_codeblock_syms/d_codeword_syms), // 5
	d_frame_bits(codeblock_bits-(d_interleave_depth*d_parity_syms_per_word*8)), // 10200 - (5*32*8) = 8920
	d_frame_bytes(d_frame_bits>>3), // 1115
	d_deinterleaved_frame_pos(0),
	d_loop(0),
	d_count(0),
	d_frame_counter(0),
	d_lost_frame_counter(0)
{
	d_rs_in = new unsigned char[d_codeblock_syms];
#if VERBOSE
	printf("\nRS decoder: %d %d %d %d %d\n", d_codeblock_bits,d_codeblock_syms,d_interleave_depth,d_frame_bits,d_frame_bytes);
#endif

}


ccsds_rs_decode_bb::~ccsds_rs_decode_bb ()
{
}

bool ccsds_rs_decode_bb::stop(void) {
	d_output_queue->insert_tail(gr_make_message(1, 0, 0, 0)); // EOF message
	return true;
}

int 
ccsds_rs_decode_bb::decode(unsigned char *deinterleaved) {
	int derrlocs[NROOTS];
	memset(derrlocs,0,sizeof(derrlocs));
	int erasures = 0;
	int pad = 0;
	int derrors = decode_rs_ccsds(deinterleaved,derrlocs,erasures,pad);

#if VERBOSE
	printf("\nerasures = %d\n", erasures);
	printf("derrors = %d\n", derrors);
#endif
	return derrors;
}

int
ccsds_rs_decode_bb::work (int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star& /*output_items*/)
{
	const char *in = (const char *) input_items[0];

	unsigned int ninput_items = 0;
	#if VERBOSE
		printf("\nde-interleaving %d items\n",noutput_items);
	#endif

	// De-interleave
	while( ninput_items < (unsigned int)noutput_items ) {
		while( ninput_items < (unsigned int)noutput_items && d_deinterleaved_frame_pos < d_codeblock_syms ) {
			d_rs_in[d_deinterleaved_frame_pos] = in[ninput_items++];
			d_deinterleaved_frame_pos += d_codeword_syms;
			d_count++;
		}
	
		if (d_count == d_codeblock_syms) {
			#if VERBOSE
				printf("\ndecoding and interleaving\n");
			#endif

			// Decode and interleave
			unsigned char message[d_codeword_syms];
			int errors = 0;
			int total_errors = 0;
			gr_message_sptr msg = gr_make_message(0, 0, 0, d_frame_bytes);
			unsigned char *frame = msg->msg();

			for(unsigned int loop = 0; loop < d_interleave_depth; loop++)
			{
				memcpy(message, &d_rs_in[loop * d_codeword_syms], d_codeword_syms * sizeof(unsigned char));
				errors = decode(message);
				if (errors < 0) { // -1 means uncorrectable errors in the codeword, entire frame lost
					total_errors = errors; 
					break;
				} else { // correctable errors, interleave the codeword into the output frame, minus the RS parity bits
					total_errors += errors;
					int message_pos = 0;
					for(unsigned int frame_pos = loop; frame_pos < d_frame_bytes; frame_pos += d_interleave_depth)
						frame[frame_pos] = message[message_pos++];
				}
			}

			if (total_errors >= 0) {		
				d_output_queue->insert_tail(msg);	// send it
				# if VERBOSE
					std::cout << "\nRS:msg made\n" << msg->to_string() << "\n\n";
				# endif
				# if ERRCNT
					printf("RS:sent frame %d with %d errors corrected\n",++d_frame_counter, errors);
				# endif
			} else {
				# if ERRCNT
					printf("RS:lost frame due to uncorrectable errors, total lost so far %d\n",++d_lost_frame_counter);
				# endif
			}
			msg.reset();  				// free it up

			d_count = 0;
			d_loop = 0;
			d_deinterleaved_frame_pos = 0;
		}
		else d_deinterleaved_frame_pos = ++d_loop;
	}

	// Tell runtime system how many input items we've consumed.
	return noutput_items;
}

