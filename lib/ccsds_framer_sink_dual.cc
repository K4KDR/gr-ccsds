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

#include <ccsds_framer_sink_dual.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <stdexcept>
#include <string.h>
#include <iostream>

#define VERBOSE 0	

inline void
ccsds_framer_sink_dual::enter_search()
{
	// if(b_buffering_standard || b_buffering_inverse ) {
		// if (VERBOSE) {
			// printf( "@ enter_search: buffering\n");
		// }
		// return;
	// }

	# if VERBOSE
		printf( "@ enter_search\n");
	# endif
	
	d_state = STATE_SYNC_SEARCH;
	b_buffering_standard = b_buffering_inverse = false;
}

inline void
ccsds_framer_sink_dual::enter_locking(const unsigned char *stream, bool standard)
{
	# if VERBOSE
		printf( "@ enter_locking\n");
	# endif

	d_state = STATE_LOCKING;
	if(standard) {
		b_buffering_standard = true;
		// d_bit = &bit;
		std::cout << "lock on standard\n";
	}
	else {
		b_buffering_inverse = true;
		// d_bit = &inverse_bit;
		std::cout << "lock on inverse\n";
	}
	d_count = 0;
	// d_in = stream;
	// d_inverse_buffer_byte_count = 0;
}

ccsds_framer_sink_dual_sptr
ccsds_make_framer_sink_dual(gr_msg_queue_sptr target_queue, int framesyms, int sync_marker_syms)
{
	return gnuradio::get_initial_sptr(new ccsds_framer_sink_dual(target_queue, framesyms, sync_marker_syms));
}


ccsds_framer_sink_dual::ccsds_framer_sink_dual(gr_msg_queue_sptr target_queue, int framesyms, int sync_marker_syms)
: gr_sync_block ("framer_sink_dual",
		gr_make_io_signature (2, 2, sizeof(unsigned char)),
		gr_make_io_signature (0, 0, 0)),
		d_target_queue(target_queue),
		d_frame_byte_length(framesyms>>3),
		d_frame_bit_length(framesyms),
		d_asm_byte_length(sync_marker_syms>>3),
		d_asm_bit_length(sync_marker_syms),
		d_sync_lock_diff(0),
		d_count(0),
		d_byte(0),
		d_frame_counter(0)
{
	// Toggle me: this is only meant for the case when example output is required of the frame and its 
	// preceding ASM. It doesn't grab every frame (i.e. when the ASM crosses an input buffer boundary), 
	// so improve the implementation if you want it to grab every frame.
	b_strip_sync_marker = true; 
	
	d_packet_size = d_frame_byte_length;
	if (!b_strip_sync_marker) d_packet_size += d_asm_byte_length;
	d_message_bits = d_packet_size << 3;
	d_packed_frame = new unsigned char[d_packet_size];

	b_buffering_standard = false;
	b_buffering_inverse = false;

	# if VERBOSE
		printf( "@ Initialization\n");
		printf( "Frame size %d, ASM size = %d\n", d_packet_size,d_asm_byte_length);
	# endif
	
	enter_search();
}
/*
ccsds_framer_sink_dual::process_stream(input_stream &stream, int &count, input_stream &alt, int noutput_items){
	stream.buffer_byte = (stream.buffer_byte << 1) | (stream.in[count] & 0x1);
	if (stream.buffer_byte_index++ == 7) {	  	// byte is full so move to next byte
		stream.buffer[stream.buffer_byte_count++] = stream.buffer_byte;
		stream.buffer_byte_index = 0;
		stream.buffer_byte = 0;
		// packet is filled
		if (stream.buffer_byte_count == d_packet_size) {
			if (VERBOSE) {
				printf( "Buffer full\n");
				printf( "Frame size %d, ASM size = %d\n", d_packet_size,d_asm_byte_length);
			}
			// build a message
			// FIXME: pass timestamp as arg1 
			gr_message_sptr msg = gr_make_message(0, 0, 0, d_packet_size);
			memcpy(msg->msg(), strea.buffer, d_packet_size);
			d_target_queue->insert_tail(msg);	// send it
			msg.reset();  				// free it up

		
			// confirm lock on this stream
			if(count+d_asm_bit_length >= noutput_items) {
				stream.sync_lock_diff = noutput_items - count;
			}
			else if ( stream.in[count + d_asm_bit_length + 1] & 0x2 ) {	// Found it on the next sync marker, lock sync
				count += (d_asm_byte_length * 8) + 1;
				enter_have_sync(stream);
				alt.reset();
			}
			else {
				if (VERBOSE) {
					printf( "Did not lock the standard stream\n");
				}
				b_buffering_standard=false;
				if(!b_buffering_inverse) {
					enter_search();
				}

			}
			d_standard_buffer_byte = 0;
			d_standard_buffer_byte_count = 0;
			memset(d_packed_frame,0,d_packet_size);
		}
	}
}
*/
ccsds_framer_sink_dual::~ccsds_framer_sink_dual ()
{
}

int
ccsds_framer_sink_dual::work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
		{
	const unsigned char *in_standard = (const unsigned char *) input_items[0];
	const unsigned char *in_inverse = (const unsigned char *) input_items[1];

	//printf("noutput_items=%d\n",noutput_items);
	int count=0;

	// if we're starting a new input stream in the middle of a previous lock
	// we move directly to where we expect the first bit of the next frame to be
	// if (d_sync_lock_diff) {
		// if (VERBOSE)
			// printf("d_sync_lock_diff = %d\n", d_sync_lock_diff);
		// count += d_sync_lock_diff; 
		// d_sync_lock_diff = 0;
		// d_in = b_buffering_standard ? in_standard : in_inverse;
		// printf("%d%d%d\n", d_in[count-1]&0x2,d_in[count]&0x2,d_in[count+1]&0x2);
		// if ( !check_flag(d_in[count]) ){	// not found it where we expected it to be
			// enter_search();
		// }
	// }
	# if VERBOSE
		printf(">>> Entering state machine\n");
	# endif

	while (count < noutput_items){
		switch(d_state) {

		case STATE_SYNC_SEARCH:    // Look for flag indicating beginning of pkt
			# if VERBOSE
				printf("SYNC Search, noutput=%d, bit=%d\n", noutput_items, in_standard[count]);
			# endif

			while (count < noutput_items) {
				# if VERBOSE
					printf("%d", in_standard[count]&0x2);
				# endif
				// printf("%d", in_standard[count]);
				if ( check_flag(in_standard[count]) ){	// Found it on the standard stream
					if (!b_strip_sync_marker) {
						if (count >= d_asm_bit_length) {
							count -= d_asm_bit_length;
							# if VERBOSE
								printf("keeping ASM, count=%d\n", count);
							# endif
						}
						else { // we want the full ASM and part of it was in the last input buffer, so...
							count++;
							continue; 
						}
					}
					enter_locking(in_standard,true);
					break;
				}
				else if ( check_flag(in_inverse[count]) ){	// Found it on the inverted stream
					if (!b_strip_sync_marker) {
						if (count >= d_asm_bit_length) count -= d_asm_bit_length;
						else {
							count++;
							continue;
						}
					}
					enter_locking(in_inverse,false);
					break;
				}
				else 
					count++;
			}
			break;

		case STATE_LOCKING:   	// Look for flag indicating beginning of the next packet, confirming that the correct sync marker was capture
			# if VERBOSE
				printf("SYNC Lock\n");
			# endif
			d_in = b_buffering_standard ? in_standard : in_inverse;
			while (count < noutput_items) {   
				// shift bits into bytes of buffer one at a time
				d_packed_frame[d_count++>>3] = d_byte = (d_byte<<1) | (d_bit(d_in[count++]) & 0x1);
				if (d_count == d_message_bits) {
				// printf("%d", d_in[count]);
					# if VERBOSE
						printf( "Frame is complete\n");
					# endif

					// build a message
					// FIXME: add timestamp as arg1
					gr_message_sptr msg = gr_make_message(0, 0, 0, d_packet_size);
					memcpy(msg->msg(), d_packed_frame, d_packet_size);
					d_target_queue->insert_tail(msg);	// send it
					# if VERBOSE
						// std::cout << "\nmsg made\n" << msg->to_string() << "\n\n";
						printf("sent frame %d\n",++d_frame_counter);
					# endif
					msg.reset();  				// free it up
					
					memset(d_packed_frame,0,d_packet_size);

					// confirm lock on this stream
					int next_flag_pos = count + d_asm_bit_length;
					if (next_flag_pos >= noutput_items) {
						// d_sync_lock_diff = noutput_items - count; // set the offset for the next input buffer
						count = noutput_items; // jump to the end of this input buffer
						enter_search(); // and trigger the code that looks for the new frame flag 
										// at the beginning of the next input buffer
						break;
					}
					else if ( check_flag(d_in[next_flag_pos]) ) {	
						// Found it on the next sync marker, lock sync
						if (b_strip_sync_marker) count = next_flag_pos; 
						# if VERBOSE
							printf( "Lock confirmed, continuing to stream this stream\n");
						# endif
					}
					else {
						# if VERBOSE
							printf( "Lost lock\n");
						# endif
						enter_search();
						break;
					}
					d_count = 0;
				}
			} // while
			# if VERBOSE
				printf("d_packed_frame byte_count: %d\n", d_count>>3);
				printf( "Frame is over, continuing with the next block\n");
			# endif
			break;
		default:
			assert(0);

		} // switch

	}   // while
	return noutput_items;
}
