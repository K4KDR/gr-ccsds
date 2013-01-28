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
#ifndef INCLUDED_CCSDS_VITERBI27_BB_H
#define INCLUDED_CCSDS_VITERBI27_BB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

extern "C" {
#include <fec.h>
}

class ccsds_viterbi27_bb;

typedef boost::shared_ptr<ccsds_viterbi27_bb> ccsds_viterbi27_bb_sptr;

CCSDS_API ccsds_viterbi27_bb_sptr ccsds_make_viterbi27_bb(
				gr_msg_queue_sptr output_queue, int framebits, unsigned long int sync_marker, int asm_length);

/*! \brief A rate 1/2, k=7 convolutional frame decoder for the CCSDS standard
 * \ingroup channelcoding
 *
 * This block performs hard-decision convolutional decoding using the Viterbi
 * algorithm. The fec library by Phil Karn is used.
 *
 * The input is a stream of unpacked bits (hard channel symbols) that represent the encoded frames only,
 * i.e. the ASMs have been removed from the stream, and are synchronised on the first bit of the frame.
 *
 * There is no stream output, instead the MSB-first packed bytes of the decoded 
 * frame are inserted into a dedicated output queue.
 *
 * This block is designed for framed data, not continuous data streaming.
 */

class CCSDS_API ccsds_viterbi27_bb : public gr_sync_block
{
private:
  friend CCSDS_API ccsds_viterbi27_bb_sptr ccsds_make_viterbi27_bb(
				gr_msg_queue_sptr output_queue, int framesyms, unsigned long int sync_marker, int asm_length);

  ccsds_viterbi27_bb(gr_msg_queue_sptr output_queue, int framesyms, unsigned long int sync_marker, int asm_length);

  unsigned char *d_viterbi_in;
  void *d_vp;
  gr_msg_queue_sptr  d_output_queue;		// where to send the decoded frame

  int d_count;
  int d_frame_counter;
  int d_framesyms;
  int d_framebytes;
  int d_framebits;
  unsigned long int d_sync_marker;
  int d_asm_length;
  
  void forecast (int noutput_items, gr_vector_int &ninput_items_required)
  {
	  // make sure that we get at least one frame per input buffer
	  unsigned ninputs = ninput_items_required.size ();
	  for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = d_framesyms;
  }


public:
  ~ccsds_viterbi27_bb();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_CCSDS_VITERBI27_BB_H */
