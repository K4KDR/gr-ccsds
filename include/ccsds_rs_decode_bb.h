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

#ifndef INCLUDED_CCSDS_RS_DECODE_BB_H
#define INCLUDED_CCSDS_RS_DECODE_BB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>

extern "C" {
#include <fec.h>
#include <fixed.h>
}

class ccsds_rs_decode_bb;
typedef boost::shared_ptr<ccsds_rs_decode_bb> ccsds_rs_decode_bb_sptr;

CCSDS_API ccsds_rs_decode_bb_sptr ccsds_make_rs_decode_bb (gr_msg_queue_sptr output_queue, int codeblock_bits);

/*! \brief A (255,223) Reed-Solomon decoder for CCSDS TM Frames
 * \ingroup ccsds
 *
 * The forward error correction library by Phil Karn is used.
 *
 * The input is a stream of R-S symbols (8 bit packed bytes) that represent the codeblocks (encoded frames) only,
 * i.e. the ASMs have been removed from the stream, and are synchronised on the first symbol of the block.
 *
 * There is no stream output, instead the packed bytes of the decoded 
 * frame are inserted into a dedicated output queue (minus the R-S parity bits).
 * Frames with uncorrectable errors are not propagated.
 */

class CCSDS_API ccsds_rs_decode_bb : public gr_sync_block
{
	friend CCSDS_API ccsds_rs_decode_bb_sptr ccsds_make_rs_decode_bb (gr_msg_queue_sptr output_queue, int codeblock_bits);

	ccsds_rs_decode_bb (gr_msg_queue_sptr output_queue, int codeblock_bits);
	
 private:
	gr_msg_queue_sptr d_output_queue;
	int d_codeblock_bits;
	int d_codeblock_syms;
	int d_codeword_syms;
	int d_interleave_depth;
	int d_count;
	int d_block_bits;
	int d_parity_syms_per_word;
	int d_frame_bits;
	int d_frame_bytes;
	int d_deinterleaved_frame_pos;
	int d_loop;
	int d_frame_counter;
	int d_lost_frame_counter;
	unsigned char *d_rs_in;

	int decode(unsigned char *deinterleaved);
	
	void forecast (int noutput_items, gr_vector_int &ninput_items_required)
	{
		// make sure we have at least one frame per input buffer
		unsigned ninputs = ninput_items_required.size ();
		for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = d_codeblock_syms;
	}


 public:
	~ccsds_rs_decode_bb ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
};

#endif /* INCLUDED_CCSDS_RS_DECODE_BB_H */

