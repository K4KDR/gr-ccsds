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


#ifndef INCLUDED_CCSDS_ASM_POSITION_H
#define INCLUDED_CCSDS_ASM_POSITION_H

#include <ccsds/api.h>
#include <ccsds/asm_header.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief A gr::tag that holds information about an attached sync marker (ASM)
     * 
     * The value of this tag is a pmt dict containing metadata of the ASM
     * 
     * \param offset Stream offset of the tag \see gr::tag_t
     * \param ambiguity Ambiguity of the ASM
     * \param score A score value on how good the ASM matches the expected ASM. This score might differ from the correlation below by applying additoinal corrections, or might not be based on the correlation at all.
     * \param correlation The real-valued correlation result of the received ASM and the expected ASM.
     * \param primary A primary ASM is an ASM that was actually found in the received sequence. A secondary ASM (primary=false) is inserted because an ASM might be expected at the given position.
     *
     * \ingroup ccsds
     */
    class CCSDS_API asm_position
    {
    public:
      asm_position(uint64_t offset, size_t ambiguity, size_t sequence_number=asm_header::SEQUENCE_UNUSED);
      ~asm_position() = default;

      uint64_t offset() const;
      size_t ambiguity() const;
      size_t sequence() const;
      
      void set_offset(uint64_t offset);
      void set_ambiguity(size_t ambiguity);
      void set_sequence(size_t sequence=asm_header::SEQUENCE_UNUSED);

      bool hasSequence() const;

      bool matches(const asm_header &other) const;
      bool matches(const asm_position &other) const;

    protected:
      uint64_t d_offset;
      size_t d_ambiguity;
      size_t d_sequence;
    };


    class CCSDS_API asm_position_list {
    public:
      asm_position_list() = default;
      asm_position_list(std::vector<asm_position> positions);
      ~asm_position_list() = default;

      typedef boost::shared_ptr<asm_position_list> sptr;

      const std::vector<asm_position>& elements() const;
      std::string to_string() const;

      static sptr make();
      static sptr make(std::vector<asm_position> positions);
    protected:
        std::vector<asm_position> d_list;
    };

    


  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_ASM_POSITION_H */

