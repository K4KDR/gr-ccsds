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

#include "frame_sync_buffer.h"
#include "ccsds_utils.h"
#include <gnuradio/ccsds/exceptions.h>

#include <gnuradio/gr_complex.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

namespace gr {
    namespace ccsds {
        namespace aux {


frame_sync_buffer::frame_sync_buffer(size_t block_len, const volk::vector<gr_complex> &asm_conj, size_t n_frames_history, size_t num_head) 
    : d_BLOCK_LEN(block_len),
      d_OVERLAP_LEN(asm_conj.size()-1u),
      d_HEAD_LEN(num_head),
      d_NUM_HISTORY_FRAMES(n_frames_history),
      d_HISTORY_LEN(n_frames_history*block_len+num_head),
      d_BUFFER_LEN(d_HISTORY_LEN+block_len+d_OVERLAP_LEN),
      d_ZERO(0.0, 0.0),
      d_ASM_CONJ(asm_conj),
      d_snr_tags(1u+n_frames_history+1u, snr::INVALID),
      d_buffer_signal(d_BUFFER_LEN),
      d_buffer_correlation(d_BUFFER_LEN-d_OVERLAP_LEN),
      d_buffer_energy(d_BUFFER_LEN),
      d_num_items_in_buffer(0u),
      d_offset_counter(0u)
{
    // Init buffers with zeros
    for(size_t i=0u; i<d_HISTORY_LEN; i++) {
        d_buffer_signal[i] = d_ZERO;
        d_buffer_correlation[i] = d_ZERO;
        d_buffer_energy[i] = std::numeric_limits<float>::lowest();
    }
    d_num_items_in_buffer = d_HISTORY_LEN;

    //printBufferStats();
}

frame_sync_buffer::~frame_sync_buffer() {}

bool frame_sync_buffer::full() const {
    assert(d_num_items_in_buffer <= d_BUFFER_LEN);
    return d_num_items_in_buffer == d_BUFFER_LEN;
}

bool frame_sync_buffer::buffer_current_empty() const {
    return d_num_items_in_buffer <= d_HISTORY_LEN;
}

size_t frame_sync_buffer::num_symbols_in_window() const {
    return utils::minus_cap_zero(d_num_items_in_buffer, d_HISTORY_LEN);
}

size_t frame_sync_buffer::overlap() const {
    return d_OVERLAP_LEN;
}

uint64_t frame_sync_buffer::offset() const {
    return d_offset_counter;
}

uint64_t frame_sync_buffer::offset_start() const {
    return utils::minus_cap_zero<uint64_t>(d_offset_counter, d_HISTORY_LEN);
}

uint64_t frame_sync_buffer::offset_last_symbol() const {
    return d_offset_counter + utils::minus_cap_zero<uint64_t>(d_num_items_in_buffer, d_HISTORY_LEN + 1u);
}

uint64_t frame_sync_buffer::offset_end() const {
    return utils::minus_cap_zero<uint64_t>(d_offset_counter + d_BLOCK_LEN + d_OVERLAP_LEN, 1u);
}

size_t frame_sync_buffer::buffer_size() const {
    return d_BUFFER_LEN;
}

size_t frame_sync_buffer::buffer_items_now() const {
    return d_num_items_in_buffer;
}

size_t frame_sync_buffer::num_symbols_missing() const {
    assert(d_BUFFER_LEN >= d_num_items_in_buffer);
    return d_BUFFER_LEN-d_num_items_in_buffer;
}

void frame_sync_buffer::advance() {
    if (!full()) {
        throw exceptions::BufferAdvancedBeforeFull("The frame_sync_buffer is not full. Cannot advance buffer.");
    }
    assert(full());

    // Shift each buffer to the left by a full block length
    const int kBlockLenInt = static_cast<int>(d_BLOCK_LEN);  // iterator arithmetic is performed with int, so do the conversion here once
    std::rotate(d_buffer_signal.begin(), d_buffer_signal.begin()+kBlockLenInt, d_buffer_signal.end());
    std::rotate(d_buffer_correlation.begin(), d_buffer_correlation.begin()+kBlockLenInt, d_buffer_correlation.end());
    std::rotate(d_buffer_energy.begin(), d_buffer_energy.begin()+kBlockLenInt, d_buffer_energy.end());
    std::rotate(d_snr_tags.begin(), d_snr_tags.begin()+1, d_snr_tags.end());
    set_snr_current_end(ccsds::snr::INVALID);

    d_num_items_in_buffer -= d_BLOCK_LEN;
    d_offset_counter += d_BLOCK_LEN;
}

size_t frame_sync_buffer::read_signal(const gr_complex *in, size_t num_available, ccsds::snr snr_end) {
    assert(d_BUFFER_LEN >= d_num_items_in_buffer);
    const size_t num_until_full = d_BUFFER_LEN - d_num_items_in_buffer;
    const size_t num_copy = (num_available < num_until_full) ? num_available : num_until_full;

    const size_t indx_start = d_num_items_in_buffer;

    std::memcpy(&d_buffer_signal[indx_start], in, num_copy*sizeof(gr_complex));
    
    d_num_items_in_buffer += num_copy;
    const size_t indx_stop = d_num_items_in_buffer;

    const size_t asm_len = d_ASM_CONJ.size();
    const unsigned int asm_len_uint = static_cast<unsigned int>(asm_len);
    assert(d_BUFFER_LEN > asm_len);
    for(size_t i=utils::minus_cap_zero(indx_start,asm_len); i<utils::minus_cap_zero(indx_stop,asm_len); i++) {
        volk_32fc_x2_dot_prod_32fc(&d_buffer_correlation[i], &d_buffer_signal[i], &d_ASM_CONJ[0], asm_len_uint);
    }
    
    set_snr_current_end(snr_end);
    const unsigned int num_copy_uint = static_cast<unsigned int>(num_copy);
    const snr_interpolation snr{snr_current_start(), snr_current_end(), d_offset_counter, num_copy_uint};

    volk::vector<float> factors(num_copy);
    volk::vector<float> abs_value(num_copy);
    volk::vector<float> correction(num_copy);
    const float e = std::exp(1.0f);

    for (size_t i=0u; i<num_copy; i++) {
        factors[i] = snr.sqrtEs_window(i)/snr.N0_window(i);
    }

    volk_32fc_magnitude_32f(&abs_value[0], &d_buffer_signal[indx_start], num_copy_uint);
    volk_32f_x2_multiply_32f(&correction[0], &abs_value[0], &factors[0], num_copy_uint);
    volk_32f_s32f_multiply_32f(&correction[0], &correction[0], -2.0f, num_copy_uint);
    volk_32f_expfast_32f(&correction[0], &correction[0], num_copy_uint);
    for (size_t i=0u; i<num_copy; i++) {
        correction[i] = correction[i]+1.0f;
    }
    volk_32f_log2_32f(&correction[0], &correction[0], num_copy_uint);
    // volk only has log2, so we need to divide results by log2(e).
    volk_32f_s32f_multiply_32f(&correction[0], &correction[0], 1.0f/std::log2(e), num_copy_uint);
    volk_32f_x2_divide_32f(&correction[0], &correction[0], &factors[0], num_copy_uint);

    volk_32f_x2_add_32f(&d_buffer_energy[indx_start], &abs_value[0], &correction[0], num_copy_uint);


    return num_copy;
}

size_t frame_sync_buffer::localOffset(uint64_t abs_offset, size_t max_len __attribute__((unused))) const{
    assert(abs_offset >= offset_start());
    assert(abs_offset <= offset_end());
    const size_t local_offset = utils::minus_cap_zero(abs_offset + d_HISTORY_LEN, d_offset_counter);
    assert(local_offset < max_len);
    return local_offset;
}

size_t frame_sync_buffer::absoluteOffset(size_t local_index) const {
    assert(local_index < d_BUFFER_LEN);
    assert(d_offset_counter + local_index >= d_HISTORY_LEN);
    return d_offset_counter + local_index - d_HISTORY_LEN;
}

const gr_complex& frame_sync_buffer::signal(uint64_t abs_pos) const {
    return d_buffer_signal[localOffset(abs_pos, d_buffer_signal.size())];
}
const gr_complex& frame_sync_buffer::correlation(uint64_t abs_pos) const {
    return d_buffer_correlation[localOffset(abs_pos, d_buffer_correlation.size())];
}

const float& frame_sync_buffer::energy(uint64_t abs_pos) const {
    return d_buffer_energy[localOffset(abs_pos, d_buffer_energy.size())];
}

const gr_complex& frame_sync_buffer::signal_window(size_t local_pos) const {
    assert(local_pos < d_BLOCK_LEN+d_OVERLAP_LEN);
    return d_buffer_signal[d_HISTORY_LEN+local_pos];
}

const gr_complex& frame_sync_buffer::correlation_window(size_t local_pos) const {
    assert(local_pos < d_BLOCK_LEN+d_OVERLAP_LEN);
    return d_buffer_correlation[d_HISTORY_LEN+local_pos];
}

const float& frame_sync_buffer::energy_window(size_t local_pos) const {
    assert(local_pos < d_BLOCK_LEN+d_OVERLAP_LEN);
    return d_buffer_energy[d_HISTORY_LEN+local_pos];
}

const float* frame_sync_buffer::energy_window_ptr(size_t local_pos, size_t num) const {
    if (local_pos+num > d_BLOCK_LEN+d_OVERLAP_LEN) {
        throw exceptions::OutOfBounds("Energy window out of bounds.");
    }
    return &d_buffer_energy[d_HISTORY_LEN+local_pos];
}

void frame_sync_buffer::set_snr_current_end(ccsds::snr snr) {
    d_snr_tags[utils::minus_cap_zero<size_t>(d_snr_tags.size(),1u)] = snr;
}

snr_interpolation frame_sync_buffer::snr(uint64_t abs_pos) const {
    assert(abs_pos >= offset_start());
    assert(abs_pos <= offset_last_symbol());
    const size_t local_pos = localOffset(abs_pos, d_BUFFER_LEN);
    assert(local_pos >= d_HEAD_LEN);
    const size_t local_block = (local_pos-d_HEAD_LEN) / d_BLOCK_LEN;
    
    const size_t tag_indx_start = (local_block+1u < d_snr_tags.size()) ? local_block : d_snr_tags.size()-2u;
    const size_t tag_indx_end = tag_indx_start +1u;
    assert(tag_indx_end < d_snr_tags.size());
    const uint64_t offset_start = absoluteOffset(d_HEAD_LEN + tag_indx_start*d_BLOCK_LEN);
    return ccsds::snr_interpolation(d_snr_tags[tag_indx_start], d_snr_tags[tag_indx_end], offset_start, d_BLOCK_LEN);
}

snr_interpolation frame_sync_buffer::snr_window() const {
    return snr(d_offset_counter+d_HISTORY_LEN);
}

ccsds::snr frame_sync_buffer::snr_current_start() const {
    assert(d_snr_tags.size() >= 2);
    return d_snr_tags[utils::minus_cap_zero<size_t>(d_snr_tags.size(), 2u)];
}

ccsds::snr frame_sync_buffer::snr_current_end() const {
    assert(d_snr_tags.size() >= 2);
    return d_snr_tags[utils::minus_cap_zero<size_t>(d_snr_tags.size(), 1u)];
}

void frame_sync_buffer::printBufferStats() const {
    printf("%lu head, %lu previous (%lu frames), %lu current, %lu tail, %lu total\n", d_HEAD_LEN, d_HISTORY_LEN-d_HEAD_LEN, d_NUM_HISTORY_FRAMES, d_BLOCK_LEN, d_OVERLAP_LEN, d_BUFFER_LEN);
    printf("currently at absolute offset %lu (front), %lu (current) up to %lu (last current symbol), %lu (end)\n", offset_start(), offset(), offset_last_symbol(), offset_end());
    /*
        d_BLOCK_LEN(block_len),
      d_OVERLAP_LEN(asm_conj.size()-1u),
      d_HEAD_LEN(num_head),
      d_NUM_HISTORY_FRAMES(n_frames_history),
      d_HISTORY_LEN(n_frames_history*block_len+num_head),
      d_BUFFER_LEN(d_HISTORY_LEN+block_len+d_OVERLAP_LEN),
      d_ZERO(0.0, 0.0),
      d_ASM_CONJ(asm_conj),
      d_snr_tags(1u+n_frames_history+1u, snr::INVALID),
      d_buffer_signal(d_BUFFER_LEN),
      d_buffer_correlation(d_BUFFER_LEN-d_OVERLAP_LEN),
      d_buffer_energy(d_BUFFER_LEN),
      d_num_items_in_buffer(0u),
      d_offset_counter(0u)
    */
}




        }  // namespace aux
    }  // namespace ccsds
}  // namespace gr
