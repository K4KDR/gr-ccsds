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


#ifndef INCLUDED_CCSDS_SORT_FRAMES_H
#define INCLUDED_CCSDS_SORT_FRAMES_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ccsds {
    
    /*! \brief Enum of ambiguity resolver verbosity levels */
    enum sort_frame_verbosity_t {
        SORT_FRAME_OUTPUT_QUIET=0,    ///< No output
        SORT_FRAME_OUTPUT_DROPPED,    ///< Only print output, if a duplicate frame is dropped
        SORT_FRAME_OUTPUT_DUPLICATE,  ///< Only print output, if a duplicate frame is found (dropped, or kept)
        SORT_FRAME_OUTPUT_ALL         ///< Print output for every frame
    };
      
    /*!
     * \brief Sort incomming frames according to one header field.
     * 
     * The block works on a window of size W, assuming that if frame number N
     * is received we have received all possible frames for frame number N-W.
     * I.e. the smallest possible frame number we can receive after receiving
     * frame number N is N-W+1.
     * 
     * If a frame number is received multiple times it can either forward all
     * of them in the order it received them, or it can pick the one with the
     * best score function (picked from a selectable header field) and discard
     * all others. The best score function could be the largest, or smallest
     * value. If no field for the score function is selected, but duplicates
     * should be eliminated the first received block is selected.
     * 
     * \ingroup ccsds
     *
     */
    class CCSDS_API sort_frames : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<sort_frames> sptr;
        
      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::sort_frames.
       *
       * To avoid accidental use of raw pointers, ccsds::sort_frames's
       * constructor is in a private implementation
       * class. ccsds::sort_frames::make is the public interface for
       * creating new instances.
       * 
       * \param window_size The size of the search window. If a frame with number
       *    N is received it is assumed that the lowest possible frame number that
       *    can be received next is N-window_size+1, i.e. all frames with number
       *    N-window_size and smaller are final and can be send out.
       * \param allow_duplicates Whether or not duplicate frames should all be
       *    send out, or just one.
       * \param sort_field The header field that should be used for sorting. This
       *    field needs to be a uint64 field, or pmt needs to be able to convert it
       *    into a uint64. The sort is performed in ascending order.
       * \param score_field In case duplicates should be eliminated only pick the
       *    best duplicate according to this header field. The selected field needs
       *    to be comparable by pmt. An empty string here picks the duplicate that
       *    was received first, or last depending on whether pick_highest_score
       *    is set to true, or false, respectively.
       * \param pick_highest_score Whether the duplicate with the highest, or lowest
       *    score function should be picked. If the score field is empty, true means
       *    the duplicate that was received last will be selected.
       * \param verbosity Select when a frame output should be printed. \sa sort_frame_verbosity_t
       */
      static sptr make(size_t window_size, bool allow_duplicates, std::string sort_field, std::string score_field, bool pick_highest_score, sort_frame_verbosity_t verbosity);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SORT_FRAMES_H */

