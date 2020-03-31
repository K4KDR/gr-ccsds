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
#include <ccsds/asm_position.h>
#include <ccsds/exceptions.h>

namespace gr {
  namespace ccsds {

    asm_position::asm_position(uint64_t offset, size_t ambiguity, size_t sequence_number)
    : d_offset(offset),
      d_ambiguity(ambiguity),
      d_sequence(sequence_number)
    {
      // Nothing to do
    }

    uint64_t asm_position::offset() const {
      return d_offset;
    }

    size_t asm_position::ambiguity() const {
      return d_ambiguity;
    }

    size_t asm_position::sequence() const {
      return d_sequence;
    }

    void asm_position::set_offset(uint64_t offset) {
      d_offset = offset;
    }

    void asm_position::set_ambiguity(size_t ambiguity) {
      d_ambiguity = ambiguity;
    }

    void asm_position::set_sequence(size_t sequence) {
      d_sequence = sequence;
    }

    bool asm_position::hasSequence() const {
      return d_sequence != asm_header::SEQUENCE_UNUSED;
    }

    bool asm_position::matches(const asm_header &other) const {
      return other.offset() == d_offset && other.ambiguity() == d_ambiguity;
    }

    bool asm_position::matches(const asm_position &other) const {
      return other.offset() == d_offset && other.ambiguity() == d_ambiguity;
    }


    asm_position_list::asm_position_list(std::vector<asm_position> pos)
      : d_list(pos)
    {
      // Nothing to do
    }

    asm_position_list::sptr asm_position_list::make() {
       return sptr(new asm_position_list());
    }

    asm_position_list::sptr asm_position_list::make(std::vector<asm_position> pos) {
       return sptr(new asm_position_list(pos));
    }

    const std::vector<asm_position>& asm_position_list::elements() const {
      return d_list;
    }

    std::string asm_position_list::to_string() const {
      return std::string("ASM list (") +
        (
          d_list.empty() 
          ? std::string("empty")
          : std::to_string(d_list.size())
        )
        + std::string(")");
    }

  } /* namespace ccsds */
} /* namespace gr */

