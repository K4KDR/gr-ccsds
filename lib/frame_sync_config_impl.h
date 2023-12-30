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

#ifndef INCLUDED_CCSDS_FRAME_SYNC_CONFIG_IMPL_H
#define INCLUDED_CCSDS_FRAME_SYNC_CONFIG_IMPL_H

#include <gnuradio/ccsds/frame_sync_config.h>


namespace gr {
  namespace ccsds {

    class CCSDS_API frame_sync_config_impl : virtual public frame_sync_config {
      public:
        frame_sync_config_impl(gr::digital::constellation_sptr constellation, std::string asm_hex, volk::vector<gr_complex> asm_symbols, size_t payload_len_symbols, int head_len_symbols, int tail_len_symbols);
        ~frame_sync_config_impl();

        std::shared_ptr<gr::digital::constellation> constellation() const override;

        size_t asm_num_bits() const override;
        size_t asm_num_symbols() const override;
        const volk::vector<gr_complex>& asm_symbols() const override;
        volk::vector<gr_complex> asm_symbols_conj() const override;
        std::vector<uint8_t> asm_bits() const override;
        std::vector<uint8_t> asm_bytes() const noexcept(false) override;

        size_t block_num_bits() const override;
        size_t block_num_bytes() const override;
        size_t block_num_symbols() const override;

        size_t payload_num_bits() const override;
        size_t payload_num_bytes() const override;
        size_t payload_num_symbols() const override;

        size_t output_num_symbols() const override;
        size_t output_num_bits() const override;

        int head_num_bits() const override;
        int head_num_symbols() const override;

        int tail_num_bits() const override;
        int tail_num_symbols() const override;

        gr_complex modulate_symbol(unsigned int symbol_bits) const override;
        size_t detect_ambiguity(const gr_complex point) const override;
        gr_complex get_ambiguity_symbol(size_t ambiguity) const override;

        size_t history_for_n_frames(size_t n_frames) const override;
          
      protected:
        const gr::digital::constellation_sptr d_CONSTELLATION;
        const size_t d_BLOCK_LEN_SYMBOLS;
        const size_t d_ASM_LEN_SYMBOLS;
        const size_t d_PAYLOAD_LEN_SYMBOLS;
        const std::string d_ASM_HEX;
        const volk::vector<gr_complex> d_ASM_SYMBOLS;
        const int d_HEAD_SYMBOLS;
        const int d_TAIL_SYMBOLS;
        const size_t d_OUTPUT_LEN_SYMBOLS;
    };

  }  // namespace ccsds
}  // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_SYNC_CONFIG_IMPL_H */
