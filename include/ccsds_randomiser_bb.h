/* -*- c++ -*- */
/*
 * Copyright 2012
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
#ifndef INCLUDED_ccsds_randomiser_BB_H
#define INCLUDED_ccsds_randomiser_BB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include "gri_lfsr.h"
#include <gr_msg_queue.h>

class ccsds_randomiser_bb;
typedef boost::shared_ptr<ccsds_randomiser_bb> ccsds_randomiser_bb_sptr;

CCSDS_API ccsds_randomiser_bb_sptr ccsds_make_randomiser_bb(gr_msg_queue_sptr output_queue, int frame_bits, int mask, int seed, int len);

/*! \brief De/randomise a TM Frame using an LFSR.  
 * \ingroup ccsds
 *
 * \param output_queue	The message queue where to output individual messages (frames) after processing
 * \param frame_bits	The size of the frame in bits
 * \param mask			Polynomial mask for LFSR
 * \param seed			Initial shift register contents
 * \param len			Shift register length
 *
 * The input is a stream of unpacked bits that represent the encoded frames only,
 * i.e. the ASMs have been removed from the stream, and are synchronised on the first bit of the frame.
 *
 * There is no stream output, instead the bits of the decoded frame are packed 
 * and inserted into a dedicated output queue.
 *
 * The scrambler works by XORing the LSBs of the incoming bit stream by the output of
 * the LFSR.  After 'frame_bits' bits have been processed, the shift
 * register is reset to the seed value.  This allows processing fixed length
 * frames according to CCSDS recommendations.
 *
 * The gnuradio-core gri_lfsr has a bug in that can be worked-around by defining the length of the 
 * shift register (in the 'len' parameter) to be one less than its actual length
 *
 * \ingroup telemetry
 */
class CCSDS_API ccsds_randomiser_bb : public gr_sync_block
{
	friend CCSDS_API ccsds_randomiser_bb_sptr 
		ccsds_make_randomiser_bb(gr_msg_queue_sptr output_queue, int frame_bits, int mask, int seed, int len);

	gr_msg_queue_sptr d_output_queue;
	gri_lfsr d_lfsr;
	int d_bits;
	int d_count;
	int d_byte;
	int d_frame_bits;
	int d_frame_bytes;
	int d_frame_counter;
	unsigned char *d_packed_frame;

	ccsds_randomiser_bb(gr_msg_queue_sptr output_queue, int frame_bits, int mask, int seed, int len);

	void forecast (int noutput_items, gr_vector_int &ninput_items_required)
	{
		const unsigned int num_frames = std::max(noutput_items/d_frame_bits, 1);

		// make sure that we get at least one frame per input buffer
		const unsigned int ninputs = ninput_items_required.size ();
		for (unsigned i = 0; i < ninputs; i++)
		ninput_items_required[i] = d_frame_bits*num_frames;
	}

public:
	int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_ccsds_randomiser_BB_H */
