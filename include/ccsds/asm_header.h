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


#ifndef INCLUDED_CCSDS_ASM_HEADER_H
#define INCLUDED_CCSDS_ASM_HEADER_H

#include <ccsds/api.h>
#include <ccsds/snr_interpolation.h>

#include <pmt/pmt.h>


namespace gr {
  namespace ccsds {

    /*!
     * \brief A tag that holds information about an attached sync marker (ASM)
     * 
     * The value of this tag is a pmt dict containing metadata of the ASM
     * 
     * \param offset Stream offset of the ASM
     * \param ambiguity Ambiguity of the ASM
     * \param score A score value on how good the ASM matches the expected ASM. This score might differ from the correlation below by applying additoinal corrections, or might not be based on the correlation at all.
     * \param correlation The real-valued correlation result of the received ASM and the expected ASM.
     * \param primary A primary ASM is an ASM that was actually found in the received sequence. A secondary ASM (primary=false) is inserted because an ASM might be expected at the given position.
     * \param snr Signal to noise ratio over the course of a full block (linear interpolation)
     * \param sequence_number A counting number to identify the primary frame for each secondary frame.
     *
     */
    class CCSDS_API asm_header
    {
    public:
      static const pmt::pmt_t KEY_OFFSET;
      static const pmt::pmt_t KEY_ASM_NUMBER;
      static const pmt::pmt_t KEY_AMBIGUITY;
      static const pmt::pmt_t KEY_SCORE;
      static const pmt::pmt_t KEY_CORRELATION;
      static const pmt::pmt_t KEY_PRIMARY;
      static const pmt::pmt_t KEY_SEQUENCE;
      static const pmt::pmt_t KEY_ENERGY_SIGNAL_START;
      static const pmt::pmt_t KEY_ENERGY_SIGNAL_SLOPE;
      static const pmt::pmt_t KEY_ENERGY_NOISE_START;
      static const pmt::pmt_t KEY_ENERGY_NOISE_SLOPE;
      static const pmt::pmt_t KEY_SNR_DB;
      static const uint64_t SEQUENCE_UNUSED;
      
      asm_header() =default;
      asm_header(uint64_t offset, size_t ambiguity, float score, float correlation, bool primary, snr_interpolation snr, uint64_t sequence_number=SEQUENCE_UNUSED);
      
      ~asm_header() = default;

      uint64_t offset() const;
      size_t ambiguity() const;
      float score() const;
      float correlation() const;
      bool primary() const;
      bool hasSequence() const;
      uint64_t sequence() const;
      const snr_interpolation& snr() const;

      void set_primary(bool primary);
      void set_sequence(uint64_t sequence);

      pmt::pmt_t generate_header(size_t bits_per_symbol, size_t block_len_symbols, const pmt::pmt_t old_header=pmt::make_dict());

      
    protected:
      uint64_t d_offset;
      size_t d_ambiguity;
      float d_score;
      float d_correlation;
      bool d_primary;
      snr_interpolation d_snr;
      uint64_t d_sequence;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_ASM_HEADER_H */

