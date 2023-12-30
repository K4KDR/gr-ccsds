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

#include <gnuradio/io_signature.h>
#include <gnuradio/ccsds/asm_header.h>

#include <gnuradio/ccsds/exceptions.h>

#include <limits>

namespace gr {
  namespace ccsds {

    const pmt::pmt_t asm_header::KEY_OFFSET = pmt::intern("asm_offset");
    const pmt::pmt_t asm_header::KEY_ASM_NUMBER = pmt::intern("asm_number");
    const pmt::pmt_t asm_header::KEY_AMBIGUITY = pmt::intern("asm_ambiguity");
    const pmt::pmt_t asm_header::KEY_SCORE = pmt::intern("asm_score");
    const pmt::pmt_t asm_header::KEY_CORRELATION = pmt::intern("asm_correlation");
    const pmt::pmt_t asm_header::KEY_PRIMARY = pmt::intern("asm_primary");
    const pmt::pmt_t asm_header::KEY_SEQUENCE = pmt::intern("asm_sequence");
    const pmt::pmt_t asm_header::KEY_ENERGY_SIGNAL_START = pmt::intern("asm_Es_start");
    const pmt::pmt_t asm_header::KEY_ENERGY_SIGNAL_SLOPE = pmt::intern("asm_Es_slope");
    const pmt::pmt_t asm_header::KEY_ENERGY_NOISE_START = pmt::intern("asm_N0_start");
    const pmt::pmt_t asm_header::KEY_ENERGY_NOISE_SLOPE = pmt::intern("asm_N0_slope");
    const pmt::pmt_t asm_header::KEY_SNR_DB = pmt::intern("asm_snr_db");

    const size_t asm_header::SEQUENCE_UNUSED = 0u;

    asm_header::asm_header(uint64_t offset, size_t ambiguity, float score, float correlation, bool primary, snr_interpolation snr, uint64_t sequence_number)
    : d_offset(offset),
      d_ambiguity(ambiguity),
      d_score(score),
      d_correlation(correlation),
      d_primary(primary),
      d_snr(snr),
      d_sequence(sequence_number)
    {
      // Nothing to do
    }
      
    uint64_t asm_header::offset() const {
      return d_offset;
    }

    size_t asm_header::ambiguity() const {
      return d_ambiguity;
    }

    float asm_header::score() const {
      return d_score;
    }

    float asm_header::correlation() const {
      return d_correlation;
    }

    bool asm_header::primary() const {
      return d_primary;
    }

    void asm_header::set_primary(bool primary) {
      d_primary = primary;
    }


    bool asm_header::hasSequence() const {
      return d_sequence != SEQUENCE_UNUSED;
    }

    uint64_t asm_header::sequence() const noexcept(false) {
      return d_sequence;
    }

    void asm_header::set_sequence(uint64_t sequence) {
      d_sequence = sequence;
    }

    const snr_interpolation& asm_header::snr() const {
      return d_snr;
    }

    pmt::pmt_t asm_header::generate_header(size_t bits_per_symbol, size_t block_len_symbols, const pmt::pmt_t old_header) {
      if (!pmt::is_dict(old_header)) {
        throw exceptions::InvalidParameter("Header is not a PMT dictionary");
      }

      pmt::pmt_t header = old_header;

      const uint64_t frame_offset = offset()*bits_per_symbol;
      const uint64_t frame_number = frame_offset/(block_len_symbols*bits_per_symbol);
      header = pmt::dict_add(header, KEY_OFFSET,              pmt::from_uint64(frame_offset));
      header = pmt::dict_add(header, KEY_ASM_NUMBER,          pmt::from_uint64(frame_number));
      header = pmt::dict_add(header, KEY_AMBIGUITY,           pmt::from_uint64(ambiguity()));
      header = pmt::dict_add(header, KEY_SCORE,               pmt::from_float(score()));
      header = pmt::dict_add(header, KEY_CORRELATION,         pmt::from_float(correlation()));
      header = pmt::dict_add(header, KEY_PRIMARY,             pmt::from_bool(primary()));
      header = pmt::dict_add(header, KEY_SNR_DB,              pmt::from_float(snr().SNR_dB_start()));
      header = pmt::dict_add(header, KEY_ENERGY_SIGNAL_START, pmt::from_float(snr().Es_start()));
      header = pmt::dict_add(header, KEY_ENERGY_SIGNAL_SLOPE, pmt::from_float(snr().Es_slope()));
      header = pmt::dict_add(header, KEY_ENERGY_NOISE_START,  pmt::from_float(snr().N0_start()));
      header = pmt::dict_add(header, KEY_ENERGY_NOISE_SLOPE,  pmt::from_float(snr().N0_slope()));

      if (hasSequence()) {
        header = pmt::dict_add(header, KEY_SEQUENCE, pmt::from_uint64(sequence()));
      }
      //printf("Create new frame task #%lu at offset=%lu amb=%lu score=%f correlation=%f primary%d\n", frame_number, frame_offset, info.ambiguity, info.score, info.correlation, primary);

      return header;
    }

  } /* namespace ccsds */
} /* namespace gr */

