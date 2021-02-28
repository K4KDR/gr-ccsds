/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf.
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

#ifndef INCLUDED_CCSDS_FRAME_SYNC_IMPL_H
#define INCLUDED_CCSDS_FRAME_SYNC_IMPL_H

#include <ccsds/frame_sync.h>

#include <ccsds/asm_header.h>
#include <ccsds/asm_position.h>
#include <ccsds/snr_est.h>
#include "frame_sync_buffer.h"

#include <boost/optional.hpp>

#include <vector>
#include <list>


namespace gr {
  namespace ccsds {

    struct frame_copy_task_t {
      const size_t num_symbols_total;
      const gr_complex amb_correction;
      
      asm_position asm_pos;
      uint64_t copy_from;
      bool primary;
      pmt::pmt_t header;
      pmt::pmt_t msg_buffer;
      size_t num_symbols_copied;
      bool header_computed;
      bool message_send_out;
      
      bool data_complete() const;
      size_t num_remaining() const;
      
      bool is_send() const;

      pmt::pmt_t generate_pmt_to_send();
      
      frame_copy_task_t(asm_position asm_pos, uint64_t frame_start, gr_complex amb_correction, bool primary, size_t num_symbols_total, size_t num_bits_per_symbol);
      frame_copy_task_t() = delete;
    };

    class frame_sync_impl : public frame_sync {
     private:
      const frame_sync_config::sptr d_FRAME_SYNC_CONFIG;
      const frame_generation_config::sptr d_FRAME_GEN_CONFIG;
      const size_t d_NUM_BITS_PER_SYMBOL;
      const size_t d_BLOCK_LEN_SYMBOLS;
      const size_t d_PAYLOAD_LEN_SYMBOLS;
      const size_t d_ASM_LEN_SYMBOLS;
      const size_t d_TOTAL_SYMBOLS_TO_COPY;
      const pmt::pmt_t d_PORT_OUT;
      const int d_SOFT_DECISION_PRECISION;
      
      gr::digital::constellation_sptr d_CONSTELLATION;
      aux::frame_sync_buffer d_buffer;
      snr_est d_snr_est;
      std::list<frame_copy_task_t> d_frame_queue;
      uint64_t d_sequence_number;
      
      pmt::pmt_t d_extra_tags;
      
      asm_position findMaximumInBuffer(size_t last_indx);
      asm_header computeSingleScore(asm_position asm_pos, bool primary);
      void processBuffer(size_t last_indx);

      void queueFrameOutput(asm_position frame, bool primary);
      void setTaskHeader(frame_copy_task_t &task, asm_header header);

      void processFrameQueue();
      

     public:
      frame_sync_impl(
        boost::shared_ptr<frame_sync_config> sync_conf,  // Note: use explicit boost::shared_ptr instead of constellation_sptr typedef because the latter confuses SWIG
        boost::shared_ptr<frame_generation_config> gen_conf,
        asm_position_list::sptr additional_positions
      );
      ~frame_sync_impl() = default;

      // Where all the action really happens
      int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
      ) override;
      
      bool stop() override;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_SYNC_IMPL_H */

