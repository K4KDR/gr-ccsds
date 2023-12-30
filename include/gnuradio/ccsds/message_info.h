/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_CCSDS_MESSAGE_INFO_H
#define INCLUDED_CCSDS_MESSAGE_INFO_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ccsds {

    enum mode_t {
      MESSAGE_INFO_SILENT = 0,
      MESSAGE_INFO_COMPACT = 10,
      MESSAGE_INFO_HEADER_ONLY = 15,
      MESSAGE_INFO_FULL = 20}
    ;
    
    /*!
     * \brief Block that prints out PDUs
     * \ingroup ccsds
     *
     */
    class CCSDS_API message_info : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<message_info> sptr;
      
      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::message_info.
       *
       * To avoid accidental use of raw pointers, ccsds::message_info's
       * constructor is in a private implementation
       * class. ccsds::message_info::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string prefix, mode_t mode);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MESSAGE_INFO_H */

