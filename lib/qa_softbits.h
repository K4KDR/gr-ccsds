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
#ifndef _QA_CCSDS_SOFTBITS_H_
#define _QA_CCSDS_SOFTBITS_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace gr {
  namespace ccsds {
    class qa_softbits : public CppUnit::TestCase {
      CPPUNIT_TEST_SUITE(qa_softbits);
      CPPUNIT_TEST(test_probability_one);
      CPPUNIT_TEST(test_probability_zero);
      CPPUNIT_TEST(test_floor_to_size_t);
      CPPUNIT_TEST(test_floor_to_int);
      CPPUNIT_TEST(test_ceil_to_size_t);
      CPPUNIT_TEST(test_ceil_to_int);
      CPPUNIT_TEST(test_map_to_uint8);
      CPPUNIT_TEST(test_hard_decision);
      CPPUNIT_TEST(test_create_from_sample);
      CPPUNIT_TEST(test_create_from_bit);
      CPPUNIT_TEST_SUITE_END();

    private:
      void test_probability_one();
      void test_probability_zero();
      void test_floor_to_size_t();
      void test_floor_to_int();
      void test_ceil_to_size_t();
      void test_ceil_to_int();
      void test_map_to_uint8();
      void test_hard_decision();
      void test_create_from_sample();
      void test_create_from_bit();
    };

  }  // namespace ccsds
}  // namespace gr
#endif  // _QA_CCSDS_SOFTBITS_H_
