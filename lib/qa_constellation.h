/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef _QA_CCSDS_CONSTELLATION_H_
#define _QA_CCSDS_CONSTELLATION_H_

#include <ccsds/softbits.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

#include <gnuradio/gr_complex.h>

#include <boost/shared_ptr.hpp>

// Temporarily disable sign-conversion warning because we don't want to see warnings for 3rd party code
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <fmt/format.h>
#pragma GCC diagnostic pop

#include <array>
#include <vector>
#include <tuple>

namespace gr {
  namespace ccsds {
      namespace qa {
          namespace constellation {
              template<class C, unsigned int num_bits_out, unsigned int expected_arity> class generic_test_case : public CppUnit::TestCase {
                  protected:
                  
                    static gr_complex sample_at_angle(float deg) {
                        const float angle_rad = deg*static_cast<float>(M_PI)/180.0f;
                        return gr_complex(std::cos(angle_rad), std::sin(angle_rad));
                    }

                    using test_data_entry_t = std::tuple< gr_complex, unsigned int, std::array<bool, num_bits_out> >;
                    using test_data_t = std::vector<test_data_entry_t>;
                    
                    void test_dimensions() {
                        boost::shared_ptr<C> c = C::make();

                        CPPUNIT_ASSERT_EQUAL(
                            expected_arity,
                            c->arity()
                        );

                        CPPUNIT_ASSERT_EQUAL(
                            num_bits_out,
                            c->bits_per_symbol()
                        );

                        for (unsigned int i=0; i<c->arity(); i++) {
                            std::vector<gr_complex> points = c->map_to_points_v(i);

                            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                                "Number of constellation points for one symbol hast to be 1.",
                                static_cast<size_t>(1),
                                points.size()
                            );
                        }
                    }

                    void run_decisions_test(const test_data_t& test_data) {
                        boost::shared_ptr<C> c = C::make();

                        const size_t NUM_SYMBOLS = test_data.size();

                        for(size_t i=0u; i<NUM_SYMBOLS; i++) {
                            gr_complex input_symbol;
                            unsigned int expected_hard_dec;
                            std::array<bool, num_bits_out> bits;

                            std::tie(input_symbol, expected_hard_dec, bits) = test_data[i];

                            const unsigned int computed_hard_dec = c->decision_maker(&input_symbol);
                            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                                fmt::format("Input symbol number {} {:+f}{:+f}j is hard decoded as {}, but {} is the expected value.", i, std::real(input_symbol), std::imag(input_symbol), computed_hard_dec, expected_hard_dec),
                                expected_hard_dec,
                                computed_hard_dec
                            );

                            const std::vector<float> computed_softbits = c->calc_soft_dec(input_symbol, 1.0f);

                            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                                fmt::format("Computed number of softbits for symbol number {} was {}, but {} is the expected value.", i, computed_softbits.size(), num_bits_out),
                                computed_softbits.size(),
                                static_cast<size_t>(num_bits_out)
                            );

                            for (size_t j=0u; j<num_bits_out; j++) {
                                const float expected_softbit = bits[j];

                                CPPUNIT_ASSERT_MESSAGE(
                                    fmt::format("Computed softbit does not match expected value. Symbol number {} {:+f}{:+f}j, softbit number {}, computed softbit {:+f}, expected softbit {:+f}", i, std::real(input_symbol), std::imag(input_symbol),j, computed_softbits[j], expected_softbit),
                                    computed_softbits[j]*expected_softbit >= 0.0f
                                );
                            }
                        }
                    }

                    void test_chain() {
                        boost::shared_ptr<C> c = C::make();

                        for (unsigned int i=0; i<c->arity(); i++) {
                            std::vector<gr_complex> points = c->map_to_points_v(i);

                            CPPUNIT_ASSERT(points.size() == 1u);
                            
                            const unsigned int decision = c->decision_maker(points.data());

                            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "Decision of generated sample is not equal the input sample.",
                            i,
                            decision
                            );
                        }
                    }
              };

              
          }  // namespace constellation
      }  // namespace qa
  }  // namespace ccsds
}  // namespace gr

#endif  // _QA_CCSDS_CONSTELLATION_H_
