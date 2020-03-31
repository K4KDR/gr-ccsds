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

#ifndef INCLUDED_CCSDS_CCSDS_UTILS_H
#define INCLUDED_CCSDS_CCSDS_UTILS_H

#include <limits>
#include <cassert>
#include <cmath>
#include <cstdint>

namespace gr {
  namespace ccsds {
    namespace utils {
      
      template <typename T> constexpr void update_nobranch(T& value, bool update, T new_value) {
        value = update ? new_value : value;
        return;
      }

      template<typename Tin, typename Tout=Tin> constexpr Tout pick_larger(Tin a, Tin b) {
        return static_cast<Tout>((a > b) ? a : b);
      }

      template<typename Tin, typename Tout=Tin> constexpr Tout pick_smaller(Tin a, Tin b) {
        return static_cast<Tout>((a < b) ? a : b);
      }

      template<typename T> constexpr T minus_cap_zero(T minuend, T subtrahend) {
        return (minuend>=subtrahend) ? minuend-subtrahend : static_cast<T>(0);
      }

      template<typename A, typename B> constexpr A add_signed_to_unsigned(A summand_unsigned, B summand_signed) {
        static_assert( std::numeric_limits<A>::is_signed == false, "First argument must be an unsigned number" );
        static_assert( std::numeric_limits<B>::is_signed == true , "Second argument must be a signed number" );
        return (summand_signed >= static_cast<B>(0)) 
          ? summand_unsigned+static_cast<A>(summand_signed)       // Signed value is positive, add up
          : ((summand_unsigned >= static_cast<A>(-summand_signed))  // Signed value is negative, but smaller than unsigned value, so we can savely compute the difference 
            ? summand_unsigned-static_cast<A>(-summand_signed)    // Signed value is negative and larger than unsigned value, result would be unsigned negative, return 0 instead
            : static_cast<A>(0)
            );
      }

      /** \brief Compare two infinite floats
       * 
       * \return if both infinite floats have the same sign
       */
      inline bool compare_infinite_floats(float a, float b) {
        assert(!std::isfinite(a) && !std::isfinite(b));
        return (a>0.0 && b>0.0) || (a<0.0 && b<0.0);
      }

      inline bool floats_equal_non_nan(float a, float b, float precision) {
        assert(!std::isnan(a));
        assert(!std::isnan(b));
        
        if (std::isfinite(a) && std::isfinite(b)) {
          assert(precision >= 0.0);
          return std::abs(a-b) < precision;
        } else if (std::isfinite(a)) {
          return false;
        } else if (std::isfinite(b)) {
          return false;
        } else {
          return compare_infinite_floats(a,b);
        }
      }

      /** \brief Extract bit counting from left to right
       * 
       * \param byte The byte from which the bit should be extracted
       * \param bit_pos The bit number that should be extracted, counting
       * from left to right (0=MSB, 7=LSB).
       * \return True if the bit was set (1), false otherwise
       */
      constexpr bool extract_bit_left(uint8_t byte, size_t bit_pos) {
        return (byte & (0x80 >> bit_pos)) != 0x00;
      }

      /** \brief Extract bit counting from right to left
       * 
       * \param byte The byte from which the bit should be extracted
       * \param bit_pos The bit number that should be extracted, counting
       * from right to left (7=MSB, 0=LSB).
       * \return True if the bit was set (1), false otherwise
       */
      constexpr bool extract_bit_right(uint8_t byte, size_t bit_pos) {
        return (byte & (0x01 << bit_pos)) != 0x00;
      }

      /**
       * Divied num by divisor and return the closest unsigned integer that is lower or equal than the floating point fraction
       */
      template<typename T> constexpr T divide_floor(T num, T divisor) {
        // C++ division of unsigneds works exactly like we want to, but provide this function as alias and for symmertry reasons (divide_floor)
        return num/divisor;
      }

      /**
       * Divied num by divisor and return the closest unsigned integer that is heigher or equal than the floating point fraction
       */
      template<typename T> constexpr T divide_ceil(T num, T divisor) {
        const T frac = num/divisor;
        return (num % divisor == 0) ? frac : frac+static_cast<T>(1);
      }

      
    }  // namespace utils
  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_CCSDS_UTILS_H */

