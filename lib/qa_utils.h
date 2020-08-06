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
#ifndef _QA_CCSDS_UTILS_H_
#define _QA_CCSDS_UTILS_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace gr {
  namespace ccsds {

    class qa_utils : public CppUnit::TestCase {
      CPPUNIT_TEST_SUITE(qa_utils);
      CPPUNIT_TEST(test_update_nobranch);
      CPPUNIT_TEST(test_pick_larger);
      CPPUNIT_TEST(test_pick_smaller);
      CPPUNIT_TEST(test_add_signed_to_unsigned);
      CPPUNIT_TEST(test_compare_infinite_floats);
      CPPUNIT_TEST(test_floats_equal_non_nan);
      CPPUNIT_TEST(test_extract_bit_left);
      CPPUNIT_TEST(test_extract_bit_right);
      CPPUNIT_TEST(test_divide_ceil);
      CPPUNIT_TEST(test_divide_floor);
      CPPUNIT_TEST_SUITE_END();

    private:
      void test_update_nobranch();
      void test_pick_larger();
      void test_pick_smaller();
      void test_add_signed_to_unsigned();
      void test_compare_infinite_floats();
      void test_floats_equal_non_nan();
      void test_extract_bit_left();
      void test_extract_bit_right();
      void test_divide_ceil();
      void test_divide_floor();
    };

  } /* namespace ccsds */
} /* namespace gr */
#endif /* _QA_CCSDS_UTILS_H_ */
