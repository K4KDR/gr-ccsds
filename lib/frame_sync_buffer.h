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

#ifndef INCLUDED_CCSDS_AUX_frame_sync_BUFFER_H
#define INCLUDED_CCSDS_AUX_frame_sync_BUFFER_H

#include <ccsds/snr.h>
#include <ccsds/snr_interpolation.h>

#include <gnuradio/gr_complex.h>
#include <volk/volk.h>

#if __has_include(<volk/volk_alloc.hh>)
#include <volk/volk_alloc.hh>
#else
// TODO remove this once gr-ccsds is ported to GNURadio 3.8 (that has a recent version of VOLK with volk_alloc.hh). Also remove the copy shipped with gr-ccsds once ported.
#include <ccsds/volk_alloc.hh>
#endif


#include <vector>
#include <cstdint>

namespace gr {
    namespace ccsds {
        namespace aux {

            /**
             * \brief Implements a shifting buffer for the frame syncrhonization
             * 
             * The buffer is structured into four consecutive areas
             *  1 head symbols
             *  2 previous blocks
             *  3. current block
             *  4. overlap
             * 
             * Area 1 and 2 are also called the history. The history allows to recall a certain
             * number of previous blocks including additional head symbols. Each area of the
             * buffer can be accessed by an absolute offset from the very first symbol that was
             * every inserted into the buffer.
             * 
             * Area 3 contains the current block, which can be also be recalled using a local index
             * (starting at the first symbol of the current block by using the functions with the
             * _window prefix).
             * 
             * The overlap contains any trailing symbols that are required to finish all necesarry
             * computations on the current block.
             * 
             * 
             * The buffer holds four different types of information
             *  1. the actual signal
             *  2. the correlation of the actual signal with the provided sync seuence
             *  3. the energy correction of the signal x (N0/sqrt(E)*ln(cosh(sqrt(E)/N0*|x|)))
             *  4. the SNR of the input signal (separated into E and N0)
             * 
             * The signal and enery corrections are available for the entire buffer. The correlation
             * is not available for the overlap.
             * 
             * The SNR is is only available for the first symbol of each block in the buffer and the
             * first symbol in the overlap. The SNR between these points is determined by linear
             * interpolation.
             * 
             * \ingroup ccsds
             */
            class frame_sync_buffer {
            public:
                frame_sync_buffer(size_t block_len, const volk::vector<gr_complex> &asm_conj, size_t n_frames_history, size_t num_head);
                ~frame_sync_buffer();

                //// Buffer properties

                /** Return the size of the entire buffer */
                size_t buffer_size() const;

                /** Return the size of the overlap area */
                size_t overlap() const;


                //// Buffer input

                /** 
                 * \brief Read samples into buffer and compute their correlation and energy
                 * 
                 * \param in Array of input samples
                 * \param num_available Number of elements available in input array
                 * \param snr_end \ref snr structure that holds a SNR estimate for the last sample in the input
                 */
                size_t read_signal(const gr_complex *in, size_t num_available, ccsds::snr snr_end);

                /**
                 * \brief Shift the entire buffer by a block length.
                 * 
                 * The current block becomes the last block in the history and the first block
                 * in the history is discarded.
                 * 
                 * \throw exceptions::BufferAdvancedBeforeFull if the buffer is not full when this function is called
                 */
                void advance();


                // Read buffer content

                /** return signal at given absolute position */
                const gr_complex& signal(uint64_t abs_pos) const;

                /**
                 * \brief Return correlation at given absolute position
                 * 
                 * The correlation is computed with the signal at the provided position and len(asm)-1 samples into the future.
                 * As a consequence of this the last len(asm)-1 samples of the buffer are not available
                 * 
                 * \param abs_pos absolute position of the start of the correlation
                 */
                const gr_complex& correlation(uint64_t abs_pos) const;
                
                /** Return the energy correction for one sample at the given absolute position */
                const float& energy(uint64_t abs_pos) const;

                /** 
                 * \brief Provide a linear apprximation of the SNR at the given absolute position.
                 * 
                 * \param abs_pos absolute position
                 * 
                 * \return a linear approximation of the SNR that must be used with absolute offsets as well.
                 */
                snr_interpolation snr(uint64_t abs_pos) const;
                
                /** 
                 * \brief Return the signal at the given local offset in the current block
                 * 
                 * \param local_pos index from the start of the current block. Must be smaller than the block length plus the overlap length
                 */
                const gr_complex& signal_window(size_t local_pos) const;

                /** 
                 * \brief Return the correlation at the given local offset in the current block
                 * 
                 * \param local_pos index from the start of the current block. Must be smaller than the block length
                 */
                const gr_complex& correlation_window(size_t local_pos) const;
                
                /** 
                 * \brief Return the energy correction at the given local offset in the current block
                 * 
                 * \param local_pos index from the start of the current block. Must be smaller than the block length plus the overlap length
                 */
                const float& energy_window(size_t local_pos) const;

                /** 
                 * \brief Return a pointer to the energy correction at the given local offset in the current block
                 * 
                 * \param local_pos index from the start of the current block. Must be smaller than the block length plus the overlap length
                 * \param num number of items we want to access. Will throw an \ref exceptions::OutOfBounds exception if not enough elements left in the buffer
                 */
                const float* energy_window_ptr(size_t local_pos, size_t num) const;

                /** 
                 * \brief Provide a linear approximation for the SNR of the current frame
                 * 
                 * \return a linear approximation that must be used with local positions
                 */
                snr_interpolation snr_window() const;

                /**
                 * Provide the SNR at the start of the current frame
                 */
                ccsds::snr snr_current_start() const;
                
                /**
                 * \brief Provide the SNR at the end of the current frame.
                 * 
                 * If the buffer is not completely full this SNR refers to the last input
                 * symbol, rather than the end of the current block buffer.
                 */
                ccsds::snr snr_current_end() const;
                
                /** Set the SNR of the last symbol in the buffer */
                void set_snr_current_end(ccsds::snr snr);


                //// Buffer status

                /** Returns whether or not the buffer is full */
                bool full() const;
                
                /** Return the total number of symbols in the buffer */
                size_t buffer_items_now() const;

                /** Return the number of symbols in the buffer for the current block */
                size_t num_symbols_in_window() const;

                /** Return the number of symbols that are missing to make the buffer full */
                size_t num_symbols_missing() const;

                /** 
                 * \brief Return whether or not there are symbols in the buffer that are associated
                 * with the current block.
                 */
                bool buffer_current_empty() const;

                
                //// Offset counting

                /** Offset (absolute count) of first symbol in current frame (starting with 0) */
                uint64_t offset() const;
                
                /**
                 * \brief Offset of the first symbol in the buffer
                 * 
                 * If the offset would be negative a zero is returned insteead
                 */
                uint64_t offset_start() const;
                
                /**
                 * \brief Offset of the last actual symbol in he buffer
                 * 
                 * This offset refers to the last symbol that is actually in the buffer.
                 * \sa offset_end() for the offset in case the buffer is full.
                 */
                uint64_t offset_last_symbol() const;

                /**
                 * \brief Offset of the last buffer position
                 * 
                 * This offset refers to the last position in the buffer, regardless of how many
                 * symbols there are currently in the buffer.
                 * \sa offset_last_symbol() to obtain the offset based on the last symbol that
                 * is in the buffer.
                 */
                uint64_t offset_end() const;

                
                protected:
                    /** length of a single block */
                    const size_t d_BLOCK_LEN;

                    /** length of overlap after the block to compute a full correlation (ASM len -1) */
                    const size_t d_OVERLAP_LEN;

                    /** number of symbols to copy before start of first ASM */
                    const size_t d_HEAD_LEN;

                    /** number of full frames to keep in buffer before the current frame */
                    const size_t d_NUM_HISTORY_FRAMES;

                    /** number of symbols that are stored before the first symbol of the current frame */
                    const size_t d_HISTORY_LEN;

                    /** total number of symbols contained in the buffer */
                    const size_t d_BUFFER_LEN;

                    /** complex number 0+0j */
                    const gr_complex d_ZERO;

                    /** complex conjugate of the ASM symbols */
                    const volk::vector<gr_complex> d_ASM_CONJ;

                    /** buffer containing SNR tags. One at the start of each frame and an additional at the end of the current frame */
                    std::vector<ccsds::snr> d_snr_tags;

                    /** buffer to store the signal in */
                    volk::vector<gr_complex> d_buffer_signal;

                    /** buffer to store correlation results. Does not reach into the overlap */
                    volk::vector<gr_complex> d_buffer_correlation;

                    /** buffer to store the indiviual energy correction values, does reach into the overlap */
                    volk::vector<float> d_buffer_energy;

                    /** counter how many items are currently in the buffer */
                    size_t d_num_items_in_buffer;

                    /** absolute offset of the first symbol in the buffer */
                    uint64_t d_offset_counter;

                    /**
                     * \brief Convert the absolute offset to a local index in the buffer
                     * 
                     * \param abs_offset Absolute offset of the sample we are interested in
                     * \param max_len Sice of the local buffer. Raise an assertion error if the local index would overflow a buffer of this length
                     */
                    size_t localOffset(uint64_t abs_offset, size_t max_len) const;

                    /** Convert a local index to an absolute absolute offset */
                    size_t absoluteOffset(size_t local_index) const;
                public:
                    void printBufferStats() const;
            };

            
        }  // namespace aux
    }  // namespace ccsds
}  // namespace gr

#endif /* INCLUDED_CCSDS_AUX_frame_sync_BUFFER_H */
