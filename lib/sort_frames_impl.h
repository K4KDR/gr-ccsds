/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf <mail@mluelf.de>.
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

#ifndef INCLUDED_CCSDS_SORT_FRAMES_IMPL_H
#define INCLUDED_CCSDS_SORT_FRAMES_IMPL_H

#include <ccsds/sort_frames.h>

#include <pmt/pmt.h>

#include <boost/optional.hpp>

#include <vector>

namespace gr {
  namespace ccsds {

    class sort_frames_impl : public sort_frames
    {
     private:
        const size_t d_WINDOW;
        const bool d_ALLOW_DUPLICATES;
        const pmt::pmt_t d_SORT_FIELD;
        const bool d_HAS_SCORE_FIELD;
        const pmt::pmt_t d_SCORE_FIELD;
        const bool d_SORT_ASCENDING;
        const pmt::pmt_t d_INTPUT_PORT;
        const pmt::pmt_t d_OUTPUT_PORT;
        const sort_frame_verbosity_t d_VERBOSITY;
        const char* d_ORDERING_EXPL;
        
        struct frame_duplicates_t {
            std::vector<pmt::pmt_t> frames;
            size_t best_index;
            double best_value;
        };
        
        uint64_t d_current_frame;
        std::vector<frame_duplicates_t> d_frame_storage;
        
        /**
         * \brief Offset in the storage vector where the current frame is stored
         * 
         * This value is always within [0, d_WINDOW)
         */
        size_t d_storage_offset;
        
        bool d_stopped;
        bool d_flushed;
        
        frame_duplicates_t& get_duplicates(uint64_t frame_number);
        
        boost::optional<uint64_t> get_frame_number(const pmt::pmt_t& header) const;
        boost::optional<double> get_frame_score(const pmt::pmt_t& header) const;
        
        void store_frame(pmt::pmt_t frame);
        void send_frame(uint64_t frame_number);
        void advance_storage();
        
        void flush_frames();
        
     public:
         sort_frames_impl(size_t window_size, bool allow_duplicates, std::string sort_field, std::string score_field, bool pick_highest_score, sort_frame_verbosity_t verbosity);
      ~sort_frames_impl();
        
      bool stop();
      
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SORT_FRAMES_IMPL_H */

