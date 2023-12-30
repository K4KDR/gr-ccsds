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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "frame_sync_config_impl.h"
#include "hexstring_to_binary.h"

#include <gnuradio/ccsds/exceptions.h>

namespace {

constexpr bool fits_symbols(size_t num_bits, size_t num_bits_per_symbol) {
  return (num_bits % num_bits_per_symbol) == 0u;
}

inline gr_complex extract_symbol(gr::digital::constellation_sptr constellation, unsigned int bits) {
  std::vector<gr_complex> points = constellation->map_to_points_v(bits);
  if (points.size() != 1u) {
    throw gr::ccsds::exceptions::ConstellationOneDimRequired("Constellation has to return exactly one constellation point for provided symbol.");
  }
  return points[0];
}

inline size_t compute_output_len(size_t payload_len, int head_len, int tail_len) {
  size_t output_len = payload_len;

  if (head_len < 0) {
    // negative, remove start of payload
    output_len -= static_cast<size_t>(-head_len);
  } else if (head_len > 0) {
    output_len += static_cast<size_t>(head_len);
  }

  if (tail_len < 0) {
    // negative, remove last symbols from payload
    output_len -= static_cast<size_t>(-tail_len);
  } else if (tail_len > 0) {
    output_len += static_cast<size_t>(tail_len);
  }
  
  return output_len;
}

}  // namespace

namespace gr {
  namespace ccsds {

frame_sync_config::sptr frame_sync_config::make(std::shared_ptr<gr::digital::constellation> constellation, std::string asm_hex, size_t asm_len_bits, size_t payload_len_bits, int head_len_bits, int tail_len_bits) {
  const size_t bits_per_symbol = constellation->bits_per_symbol();
  
  if (asm_hex.length()*8/2 < asm_len_bits) {
    throw exceptions::InvalidParameter("The number of ASM bits is larger than the number of bytes in the provided hexstring.");
  }

  if (!fits_symbols(asm_len_bits, bits_per_symbol)) {
    throw exceptions::SymbolSyncNotPossible("Number of ASM bits is not divisible by the number of bits per symbol.");
  }

  if (!fits_symbols(payload_len_bits, bits_per_symbol)) {
    throw exceptions::SymbolSyncNotPossible("Number of ASM bits is not divisible by the number of bits per symbol.");
  }

  if (!fits_symbols(static_cast<size_t>(std::abs(head_len_bits)), bits_per_symbol)) {
    throw exceptions::SymbolSyncNotPossible("Number of head bits is not divisible by the number of bits per symbol.");
  }

  if (!fits_symbols(static_cast<size_t>(std::abs(tail_len_bits)), bits_per_symbol)) {
    throw exceptions::SymbolSyncNotPossible("Number of tail bits is not divisible by the number of bits per symbol.");
  }

  if (constellation->dimensionality() != 1u) {
    throw exceptions::ConstellationOneDimRequired("Frame synchronization assumes one constellation point per symbol.");
  }

  const size_t num_asm_symbols = asm_len_bits/bits_per_symbol;
  
  if (num_asm_symbols == 0u) {
    throw exceptions::InvalidParameter("Effective ASM is 0 symbols long.");
  }
  
  std::vector<uint8_t> asm_symbols_bin = byteutils::decode_hex_to_bits(asm_hex, bits_per_symbol, num_asm_symbols);
  volk::vector<gr_complex> asm_symbols(num_asm_symbols);
  for (size_t i=0u; i<num_asm_symbols; i++) {
    asm_symbols[i] = extract_symbol(constellation, asm_symbols_bin[i]);
  }

  const size_t payload_len_symbols = payload_len_bits/bits_per_symbol;
  const int head_len_symbols = head_len_bits/static_cast<int>(bits_per_symbol);
  const int tail_len_symbols = tail_len_bits/static_cast<int>(bits_per_symbol);

  return sptr(new frame_sync_config_impl(constellation, asm_hex, asm_symbols, payload_len_symbols, head_len_symbols, tail_len_symbols));
}

frame_sync_config_impl::~frame_sync_config_impl() {
  // nothing to do
}

std::shared_ptr<gr::digital::constellation> frame_sync_config_impl::constellation() const {
  return d_CONSTELLATION;
}

size_t frame_sync_config_impl::asm_num_bits() const {
  return d_ASM_LEN_SYMBOLS*d_CONSTELLATION->bits_per_symbol();
}

size_t frame_sync_config_impl::asm_num_symbols() const {
  return d_ASM_LEN_SYMBOLS;
}

const volk::vector<gr_complex>& frame_sync_config_impl::asm_symbols() const {
  return d_ASM_SYMBOLS;
}

volk::vector<gr_complex> frame_sync_config_impl::asm_symbols_conj() const {
  volk::vector<gr_complex> ret(d_ASM_SYMBOLS.size());
  for (size_t i=0u; i<d_ASM_SYMBOLS.size(); i++) {
    ret[i] = std::conj(d_ASM_SYMBOLS[i]);
  }
  return ret;
}

std::vector<uint8_t> frame_sync_config_impl::asm_bits() const {
  std::vector<uint8_t> ret = byteutils::decode_hex_to_bits(d_ASM_HEX, d_CONSTELLATION->bits_per_symbol(), d_ASM_LEN_SYMBOLS);
  return ret;
}

std::vector<uint8_t> frame_sync_config_impl::asm_bytes() const noexcept(false) {
  if (d_ASM_LEN_SYMBOLS*d_CONSTELLATION->bits_per_symbol() % 8u != 0u) {
    throw exceptions::NotAvailable("Number of ASM bits is not an integer number of bytes.");
  }

  std::vector<uint8_t> ret = byteutils::decode_hex(d_ASM_HEX);
  return ret;
}

size_t frame_sync_config_impl::block_num_bits() const {
  return d_BLOCK_LEN_SYMBOLS*d_CONSTELLATION->bits_per_symbol();
}

size_t frame_sync_config_impl::block_num_bytes() const {
  const size_t num_bits = block_num_bits();
  assert(num_bits % 8u == 0u);
  return num_bits/8u;
}

size_t frame_sync_config_impl::block_num_symbols() const {
  return d_BLOCK_LEN_SYMBOLS;
}

size_t frame_sync_config_impl::payload_num_bits() const {
  return payload_num_symbols()*d_CONSTELLATION->bits_per_symbol();
}

size_t frame_sync_config_impl::payload_num_bytes() const {
  const size_t num_bits = payload_num_bits();
  assert(num_bits % 8u == 0u);
  return num_bits/8u;
}

size_t frame_sync_config_impl::payload_num_symbols() const {
  assert(d_BLOCK_LEN_SYMBOLS >= d_ASM_LEN_SYMBOLS);
  return d_BLOCK_LEN_SYMBOLS - d_ASM_LEN_SYMBOLS;
}

size_t frame_sync_config_impl::output_num_symbols() const {
  return d_OUTPUT_LEN_SYMBOLS;
}

size_t frame_sync_config_impl::output_num_bits() const {
  return d_OUTPUT_LEN_SYMBOLS*d_CONSTELLATION->bits_per_symbol();
}

int frame_sync_config_impl::head_num_bits() const {
  return d_HEAD_SYMBOLS*static_cast<int>(d_CONSTELLATION->bits_per_symbol());
}

int frame_sync_config_impl::head_num_symbols() const {
  return d_HEAD_SYMBOLS;
}

int frame_sync_config_impl::tail_num_bits() const {
  return d_TAIL_SYMBOLS*static_cast<int>(d_CONSTELLATION->bits_per_symbol());
}

int frame_sync_config_impl::tail_num_symbols() const {
  return d_TAIL_SYMBOLS;
}


frame_sync_config_impl::frame_sync_config_impl(std::shared_ptr<gr::digital::constellation> constellation, std::string asm_hex, volk::vector<gr_complex> asm_symbols, size_t payload_len_symbols, int head_len_symbols, int tail_len_symbols)
  : d_CONSTELLATION(constellation),
    d_BLOCK_LEN_SYMBOLS(asm_symbols.size()+payload_len_symbols),
    d_ASM_LEN_SYMBOLS(asm_symbols.size()),
    d_PAYLOAD_LEN_SYMBOLS(payload_len_symbols),
    d_ASM_HEX(asm_hex),
    d_ASM_SYMBOLS(asm_symbols),
    d_HEAD_SYMBOLS(head_len_symbols),
    d_TAIL_SYMBOLS(tail_len_symbols),
    d_OUTPUT_LEN_SYMBOLS(compute_output_len(payload_len_symbols, head_len_symbols, tail_len_symbols))
{
  // nothing to do
}


gr_complex frame_sync_config_impl::modulate_symbol(unsigned int symbol_bits) const {
  return extract_symbol(d_CONSTELLATION, symbol_bits);
}

size_t frame_sync_config_impl::detect_ambiguity(const gr_complex point) const {
  // The correlation is mapped to the real axis. The ambiguity is relative to the 0 symbol, so rotate everything by that offset
  const gr_complex symbol = point * modulate_symbol(0lu);
  const size_t ambiguity = d_CONSTELLATION->decision_maker( &symbol );
  return ambiguity;
}

gr_complex frame_sync_config_impl::get_ambiguity_symbol(size_t ambiguity) const {
  return modulate_symbol(static_cast<unsigned int>(ambiguity)) * std::conj(modulate_symbol(0u));
}

size_t frame_sync_config_impl::history_for_n_frames(size_t n_frames) const {
  const int num_head = head_num_symbols();
  return n_frames*block_num_symbols() + ( (num_head > 0) ? static_cast<size_t>(num_head) : 0lu );
}
    

  } // namespace ccsds
} // namespace gr
