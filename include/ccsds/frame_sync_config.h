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


#ifndef INCLUDED_CCSDS_FRAME_H
#define INCLUDED_CCSDS_FRAME_H

#include <ccsds/api.h>

#include <gnuradio/digital/constellation.h>
#if __has_include(<volk/volk_alloc.hh>)
#include <volk/volk_alloc.hh>
#else
// TODO remove this once gr-ccsds is ported to GNURadio 3.8 (that has a recent version of VOLK with volk_alloc.hh). Also remove the copy shipped with gr-ccsds once ported.
#include <ccsds/volk_alloc.hh>
#endif

#include <boost/shared_ptr.hpp>

#include <cstdint>

namespace gr {
  namespace ccsds {

      /*!
      * \brief Common frame synchronisation options
      * \ingroup ccsds
      *
      */
      class CCSDS_API frame_sync_config {
        public:
          typedef boost::shared_ptr<frame_sync_config> sptr;

          /*!
        * \brief Return a shared_ptr to a new instance of ccsds::frame_config.
        *
        * To avoid accidental use of raw pointers, ccsds::frame_configuration's
        * constructor is in a private implementation class.
        * 
        * ccsds::frame_config::make is the public interface for creating new
        * instances.
        * 
        * \throws SymbolSyncNotPossibleException When the provided ASM, payload and block length are not an integer amount of symbols.
        * \throws ConstellationOneDimRequiredException When the constellation object maps a single symbol to multiple constellation points.
        * 
        * \param constellation Constellation object used for generating the ASM symbols and decoding the synchronized symbols
        * \param asm_hex String with attached sync marker (ASM) as hexadecimal string. Needs to have an even number of characters (full bytes)
        * \param asm_len_bits Length of the ASM in bits. Counting starts from the first bit of the provided ASM. This allows to ignore the last bits of the sync marker if the syncmarker is not an integer number of bytes.
        * \param payload_len_bits Length of the payload. The signal structure has to repeat after asm+payload number of bits.
        * \param head_len_bits Copy this additional number of bits in front of the payload into the output frame. If negative the first number of bits from the payload will be left out. If this is set to the (positive) number of asm_len_bits, the ASM will be copied to the output before the payload.
        * \param tail_len_bits Copy this additional number of bits in after the payload into the output frame. If negative the last number of bits from the payload will be left out. If this is set to the (positive) number of asm_len_bits, the next ASM after the payload will be copied to the output after the payload.
        */
          static sptr make(boost::shared_ptr<gr::digital::constellation> constellation, std::string asm_hex, size_t asm_len_bits, size_t payload_len_bits, int head_len_bits, int tail_len_bits);

          virtual boost::shared_ptr<gr::digital::constellation> constellation() const =0;

          virtual size_t asm_num_bits() const =0;
          virtual size_t asm_num_symbols() const =0;
          virtual const volk::vector<gr_complex>& asm_symbols() const =0;
          virtual volk::vector<gr_complex> asm_symbols_conj() const =0;
          virtual std::vector<uint8_t> asm_bits() const =0;
          virtual std::vector<uint8_t> asm_bytes() const noexcept(false) =0;

          virtual size_t block_num_bits() const =0;
          virtual size_t block_num_bytes() const =0;
          virtual size_t block_num_symbols() const =0;

          virtual size_t payload_num_bits() const =0;
          virtual size_t payload_num_bytes() const =0;
          virtual size_t payload_num_symbols() const =0;

          virtual size_t output_num_symbols() const =0;
          virtual size_t output_num_bits() const =0;

          virtual int head_num_bits() const =0;
          virtual int head_num_symbols() const =0;

          virtual int tail_num_bits() const =0;
          virtual int tail_num_symbols() const =0;

          /**
           * \brief How many symbols are required before the first ASM symbol in order to obtain n_frames with head symbols.
           * 
           * \param n_frames Number of frames to output before the current one.
           */
          virtual size_t history_for_n_frames(size_t n_frames) const =0;

          
          /** 
           * \brief Modulate data bit to constellation symbol.
           * 
           * Wrapper around constellation->extract_symbol().
           * 
           * \param symbol_bits Bits, or ambiguity number to modulate.
           * 
           * \return The modulated symbol.
           */
          virtual gr_complex modulate_symbol(unsigned int symbol_bits) const =0;

          /** 
           *  \brief Detect ambiguity for correlation result
           *  
           *  \param point Accumulated correlation. A zero ambiguity is always aligned with the positive real axis.
           * 
           *  \return Detected ambiguity. The ambiguity is defined by the constellation points, relative to the 0 symbol.
           */
          virtual size_t detect_ambiguity(const gr_complex point) const =0;

          /** 
           * \brief Give the complex ambiguity vector
           * 
           * This returns the complex multiplier (with magnitude 1) that will rotate the
           * 0 symbol to the chosen ambiguity.
           * 
           * By using the complex conjugate of this complex number we can remove the
           * ambiguity from a symbol and return it to its unambiguous position.
           * 
           * For BPSK this is a multiple of 180 degree, for QPSK this is a multiple of
           * 90 degree. Any offset of the 0 symbol from the real axis is not included here
           * as the ambiguity is relative to the 0 symbol, not the real axis.
           * 
           * \param ambiguity The ambiguity number that should be achieved
           * 
           * \return Complex number with magnitude 1 that will rotate the symbol to the chosen
           *         ambiguity.
           */
          virtual gr_complex get_ambiguity_symbol(size_t ambiguity) const =0;
          

          virtual ~frame_sync_config() {};
      };

  }  // namespace ccsds
}  // namespace gr

#endif /* INCLUDED_CCSDS_FRAME_H */

