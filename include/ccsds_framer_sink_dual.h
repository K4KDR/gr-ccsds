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

#ifndef INCLUDED_CCSDS_FRAMER_SINK_DUAL_H
#define INCLUDED_CCSDS_FRAMER_SINK_DUAL_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

class ccsds_framer_sink_dual;
typedef boost::shared_ptr<ccsds_framer_sink_dual> ccsds_framer_sink_dual_sptr;

CCSDS_API ccsds_framer_sink_dual_sptr
ccsds_make_framer_sink_dual (gr_msg_queue_sptr target_queue, int framesyms, int sync_marker_syms);

/*!
 * \brief Given 2 parallel streams of bits and access_code flags, assemble TM Frames.
 * \ingroup receiver
 * \ingroup synchronization
 *
 * It is a fact of non-differential BPSK demodulation that an inverted bit stream may be produced. 
 * Two parallel gr_correlate_access_code_bb blocks have been setup, one looking for the non-inverted ASM,
 * and the other for the inverted ASM. If the framer sink detects a frame marker on the inverted stream,
 * it inverts the input.
 *
 * There is no stream output, instead the MSB-first packed bytes of the synchronised 
 * frame are inserted into a dedicated output queue.
 *
 * The input data consists of bytes that have two bits used.
 * Bit 0, the LSB, contains the data bit.
 * Bit 1 if set, indicates that the corresponding bit is the
 * the first bit of the packet.  That is, this bit is the first
 * one after the access code.
 */
class CCSDS_API ccsds_framer_sink_dual : public gr_sync_block
{
  friend CCSDS_API ccsds_framer_sink_dual_sptr
  ccsds_make_framer_sink_dual (gr_msg_queue_sptr target_queue, int framesyms, int sync_marker_syms);

 private:
  enum state_t {STATE_SYNC_SEARCH, STATE_LOCKING, STATE_HAVE_SYNC};
  
  // static const int MAX_PKT_LEN    = 4096;
  // static const int HEADERBITLEN   = 32;

  gr_msg_queue_sptr  d_target_queue;		// where to send the packet when received
  const int d_frame_byte_length;
  const int d_frame_bit_length;
  const int d_asm_byte_length;
  const int d_asm_bit_length;
  const int d_sync_lock_diff;
  int d_count;
  unsigned char d_byte;
  int d_frame_counter;
    


  state_t            d_state;

  bool 	b_standard;			// flag to identify if the standard stream is being used. if false, the inverted stream is used instead
  bool		b_buffering_standard;		// Buffer the standard input until sync marker is confirmed
  bool		b_buffering_inverse;		// Buffer the standard input until sync marker is confirmed

  bool		b_strip_sync_marker;		// If set to true, the synchronization marker is not streamed

  int			d_payload_size;			// The payload size
  int			d_packet_size;			// The frame size, which is the sum of the payload plus the frame sync marker

  int d_message_bits;
  // unsigned char	*d_packet; //[MAX_PKT_LEN];	// assembled payload
  unsigned char	*d_packed_frame; //[MAX_PKT_LEN];
  // unsigned char	*d_buffer_inverse; //[MAX_PKT_LEN];
  const unsigned char *d_in;
  

  int			d_packetlen;		// length of packet
  int			d_packet_whitener_offset;  // offset into whitener string to use
  int			d_packetlen_cnt;		// how many so far
/*
  struct input_stream {
	state_t state;
	bool buffering;
	int buffer_byte_count;
	int sync_lock_diff;
	unsigned char buffer_byte;
	unsigned int buffer_byte_index;
	// unsigned char *packet; // assembled payload
	unsigned char *buffer; 
	unsigned char *in;
	
	void reset() {
		buffering = false;
		buffer_byte_count = 0;
		sync_lock_diff = 0;
		buffer_byte = 0;
		buffer_byte_index = 0;
		memset(buffer,0,d_packet_size)
	}
  };
  struct input_stream d_standard;
  struct input_stream d_inverse;
*/
  inline static int bit (int input) {
	return input & 0x1;
  }
  inline static int inverse_bit (int input) {
	return (input ^ 0x1) & 0x1;
  }
  // static int (*d_bit)(int);
  inline int d_bit (int input) {
	if (b_buffering_inverse) return inverse_bit(input);
	else return bit(input);
  }
  inline bool check_flag (unsigned char byte) {
	if (byte & 0x2) return true;
	else return false;
  }
  void forecast (int /*noutput_items*/, gr_vector_int &ninput_items_required)
  {
  // make sure all inputs have noutput_items available
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
	// we want at least one frame per input buffer
    ninput_items_required[i] = 2 * d_frame_bit_length + d_asm_bit_length;
  }
  
 protected:
  ccsds_framer_sink_dual(gr_msg_queue_sptr target_queue, int framebytes, int sync_marker_bytes);

  void enter_search();
  void enter_locking(bool standard);
  void enter_have_sync();

 public:
  ~ccsds_framer_sink_dual();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

};

#endif /* INCLUDED_CCSDS_FRAMER_SINK_DUAL_H */
