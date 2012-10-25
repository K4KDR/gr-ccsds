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

#ifndef INCLUDED_CCSDS_MSG_SOURCE_H
#define INCLUDED_CCSDS_MSG_SOURCE_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>

class ccsds_msg_source;
typedef boost::shared_ptr<ccsds_msg_source> ccsds_msg_source_sptr;

CCSDS_API ccsds_msg_source_sptr ccsds_make_msg_source (size_t itemsize, int msgq_limit=0);
CCSDS_API ccsds_msg_source_sptr ccsds_make_msg_source (size_t itemsize, gr_msg_queue_sptr msgq);

/*!
 * \brief Turn received messages into a stream
 * \ingroup source_blk
 */
class CCSDS_API ccsds_msg_source : public gr_sync_block
{
 private:
  size_t	 	d_itemsize;
  gr_msg_queue_sptr	d_msgq;
  gr_message_sptr	d_msg;
  unsigned		d_msg_offset;
  bool			d_eof;

  friend CCSDS_API ccsds_msg_source_sptr
  ccsds_make_msg_source(size_t itemsize, int msgq_limit);
  friend CCSDS_API ccsds_msg_source_sptr
  ccsds_make_msg_source(size_t itemsize, gr_msg_queue_sptr msgq);

 protected:
  ccsds_msg_source (size_t itemsize, int msgq_limit);
  ccsds_msg_source (size_t itemsize, gr_msg_queue_sptr msgq);

 public:
  ~ccsds_msg_source ();

  gr_msg_queue_sptr	msgq() const { return d_msgq; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_ccsds_msg_SOURCE_H */
