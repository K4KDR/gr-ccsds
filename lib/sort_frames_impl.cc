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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_utils.h"

#include <gnuradio/io_signature.h>
#include "sort_frames_impl.h"

#include <cassert>

namespace gr {
  namespace ccsds {

    sort_frames::sptr
    sort_frames::make(size_t window_size, bool allow_duplicates, std::string sort_field, std::string score_field, bool pick_highest_score, sort_frame_verbosity_t verbosity)
    {
      return gnuradio::get_initial_sptr
      (new sort_frames_impl(window_size, allow_duplicates, sort_field, score_field, pick_highest_score, verbosity));
    }

    /*
     * The private constructor
     */
    sort_frames_impl::sort_frames_impl(size_t window_size, bool allow_duplicates, std::string sort_field, std::string score_field, bool pick_highest_score, sort_frame_verbosity_t verbosity)
      : gr::sync_block("sort_frames",
        gr::io_signature::make(0, 0, 0),
        gr::io_signature::make(0, 0, 0)),
        d_WINDOW(window_size),
        d_ALLOW_DUPLICATES(allow_duplicates),
        d_SORT_FIELD(pmt::intern(sort_field)),
        d_HAS_SCORE_FIELD(!score_field.empty()),
        d_SCORE_FIELD(d_HAS_SCORE_FIELD ? pmt::intern(score_field) : pmt::PMT_NIL),
        d_SORT_ASCENDING(pick_highest_score),
        d_INTPUT_PORT(pmt::intern("in")),
        d_OUTPUT_PORT(pmt::intern("out")),
        d_VERBOSITY(verbosity),
        d_ORDERING_EXPL(
            d_HAS_SCORE_FIELD
            ? (pick_highest_score ? "keeping the highest score" : "keeping the lowest score")
            : (pick_highest_score ? "keeping the most recent frame" : "keeping the first frame")
        ),
        d_frame_storage(window_size)
    {
        d_current_frame = 0lu;
        d_storage_offset = d_WINDOW-1lu;
        d_stopped = false;
        d_flushed = false;
        
        message_port_register_in(d_INTPUT_PORT);
        set_msg_handler(d_INTPUT_PORT, boost::bind(&sort_frames_impl::store_frame, this, _1));
        
        message_port_register_out(d_OUTPUT_PORT);
    }

    /*
     * Our virtual destructor.
     */
    sort_frames_impl::~sort_frames_impl()
    {
    }
    
    bool sort_frames_impl::stop() {
        d_stopped = true;
        return true;
    }
    
    sort_frames_impl::frame_duplicates_t& sort_frames_impl::get_duplicates(uint64_t frame_number) {
        assert( frame_number <= d_current_frame );
        assert( frame_number + d_WINDOW >= d_current_frame );
        
        const size_t diff = utils::minus_cap_zero(d_current_frame, frame_number);
        assert(diff <= d_WINDOW);
        
        const size_t index = utils::minus_cap_zero(d_WINDOW+d_storage_offset, diff) % d_WINDOW;
        
        return d_frame_storage[index];
    }
    
    boost::optional<uint64_t> sort_frames_impl::get_frame_number(const pmt::pmt_t& header) const {
        const pmt::pmt_t sort_value = pmt::dict_ref(header, d_SORT_FIELD, pmt::PMT_NIL);
        return pmt::is_null(sort_value) ? boost::none : boost::optional<uint64_t>{pmt::to_uint64(sort_value)};
    }
    
    boost::optional<double> sort_frames_impl::get_frame_score(const pmt::pmt_t& header) const {
        if (d_HAS_SCORE_FIELD) {
            const pmt::pmt_t value = pmt::dict_ref(header, d_SCORE_FIELD, pmt::PMT_NIL);
            return pmt::is_null(value) ? boost::none : boost::optional<double>{pmt::to_double(value)};
        } else {
            return boost::none;
        }
    }
    
    void sort_frames_impl::store_frame(pmt::pmt_t frame) {
        // check for EOF
        if (pmt::is_eof_object(frame)) {
            flush_frames();
            
            message_port_pub(d_OUTPUT_PORT, pmt::PMT_EOF);
            
            d_stopped = true;
            return;
        }
        
        // check that input is a pair value
        if(!pmt::is_pair(frame)) {
            fprintf(stderr,"WARNING sort_frame: expecting message of type pair, skipping.\n");
            return;
        }
        
        const pmt::pmt_t header = pmt::car(frame);
        
        // check that input header is a dictionary
        if(!pmt::is_dict(header)) {
            fprintf(stderr,"WARNING sort_frame: expecting message header of type dict, skipping.\n");
            return;
        }
        
        const boost::optional<uint64_t> frame_number = get_frame_number(header);
        const boost::optional<double> frame_score = get_frame_score(header);
        
        
        if (!frame_number) {
            fprintf(stderr,"WARNING sort_frame: Frame does not have a header field '%s' to sort. Dropping frame.\n", pmt::write_string(d_SORT_FIELD).c_str());
            return;
        }
        if (!frame_score && d_HAS_SCORE_FIELD)  {
            fprintf(stderr,"WARNING sort_frame: Frame does not have a field '%s' to score. Dropping frame.\n", pmt::write_string(d_SCORE_FIELD).c_str());
            return;
        }
        
        if (*frame_number+d_WINDOW <= d_current_frame) {
            fprintf(stderr,"WARNING sort_frame: Frame number %lu is outside the current window [%lu, %lu].\n", *frame_number, utils::minus_cap_zero(d_current_frame+1lu, d_WINDOW), d_current_frame);
            return;
        }
        
        // If necessarry, update and flush older frames
        while(d_current_frame < *frame_number) {
            if (d_current_frame >= d_WINDOW) {
                send_frame(d_current_frame-d_WINDOW);
            }
            advance_storage();
            d_current_frame++;
        }
        
        frame_duplicates_t& dups = get_duplicates(*frame_number);
        
        // If there are no duplicates for this frame yet, save it right away.
        if (dups.frames.empty()) {
            dups.frames.push_back(frame);
            dups.best_value = frame_score.value_or(0.0);
            dups.best_index = 0lu;
            
            if (d_VERBOSITY >= SORT_FRAME_OUTPUT_ALL) {
                if (frame_score) {
                    printf("Frame %lu with score %lf is seen for the first time and will be kept.\n", *frame_number, *frame_score);
                } else {
                    printf("Frame %lu is seen for the first time and will be kept.\n", *frame_number);
                }
            }
            
            return;
        }
        
        const bool append = d_ALLOW_DUPLICATES;
        bool better = false;
        
        if (d_HAS_SCORE_FIELD) {
            better = (d_SORT_ASCENDING) ? (frame_score.value_or(0.0) > dups.best_value) : (frame_score.value_or(0.0) < dups.best_value);
        } else {
            better = d_SORT_ASCENDING; // if true, store last, otherwise keep first frame
        }
        
        if (append) {
            if (d_VERBOSITY >= SORT_FRAME_OUTPUT_DUPLICATE) {
                if (frame_score) {
                    printf("Frame %lu with score %lf is a duplicate. The best score so far is %lf, which means this frame is %s (%s). Since duplicates are allowed, we will keep this frame.\n", *frame_number, *frame_score, dups.best_value, better ? "BETTER" : "NOT better", d_ORDERING_EXPL);
                } else {
                    printf("Frame %lu is a duplicate. Since duplicates are allowed, we will keep this frame.\n", *frame_number);
                }
            }
            
            dups.frames.push_back(frame);
            utils::update_nobranch(dups.best_value, better, frame_score.value_or(0.0));
            utils::update_nobranch(dups.best_index, better, dups.frames.size()-1lu);
        } else if (better) {
            if (d_VERBOSITY >= SORT_FRAME_OUTPUT_DUPLICATE) {
                if (frame_score) {
                    printf("Frame %lu with score %lf is a duplicate. This is better than the best score so far, which was %lf (%s). Keeping this frame as best candidate.\n", *frame_number, *frame_score, dups.best_value, d_ORDERING_EXPL);
                } else {
                    printf("Frame %lu is a duplicate. Keeping this frame as best candidate (%s).\n", *frame_number, d_ORDERING_EXPL);
                }
            }
            
            dups.frames[0] = frame;
            dups.best_value = frame_score.value_or(0.0);
            dups.best_index = 0lu;
        } else if (d_VERBOSITY >= SORT_FRAME_OUTPUT_DROPPED) {
            if (frame_score) {
                printf("Frame %lu with score %lf is a duplicate that is not better than the current best score %lf (%s). It will be dropped.\n", *frame_number, *frame_score, dups.best_value, d_ORDERING_EXPL);
            } else {
                printf("Frame %lu is a duplicate. It will be dropped (%s).\n", *frame_number, d_ORDERING_EXPL);
            }
        }
        return;
    }
    
    void sort_frames_impl::send_frame(uint64_t frame_number) {
        frame_duplicates_t& dups = get_duplicates(frame_number);
        const size_t num_frames = dups.frames.size();
        
        assert(d_ALLOW_DUPLICATES ? true : (num_frames <= 1lu));
        
        for (size_t i=0lu; i<num_frames; i++) {
            message_port_pub(d_OUTPUT_PORT, dups.frames[i]);
        }
        dups.frames.clear();
    }
    
    void sort_frames_impl::advance_storage() {
        const size_t new_offset = (d_storage_offset +1lu) % d_WINDOW;
        
        // reset duplicate entry
        d_frame_storage[new_offset] = frame_duplicates_t{};
        d_storage_offset = new_offset;
    }
    
    void sort_frames_impl::flush_frames() {
        for (uint64_t i=utils::minus_cap_zero(d_current_frame+1lu, d_WINDOW); i<=d_current_frame; i++) {
            send_frame(i);
        }
        d_flushed = true;
    }
    
    int
    sort_frames_impl::work(int /*noutput_items*/,
        gr_vector_const_void_star &/*input_items*/,
        gr_vector_void_star &/*output_items*/)
    {
        // When the stopped signal is received
        if (d_stopped && !d_flushed) {
            flush_frames();
        }
      // Tell runtime system how many output items we produced.
      return d_stopped ? WORK_DONE : 0;
    }

  } /* namespace ccsds */
} /* namespace gr */

