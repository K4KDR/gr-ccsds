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
#ifndef _QA_CCSDS_CONV_ENCODE27_AUX_H_
#define _QA_CCSDS_CONV_ENCODE27_AUX_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace gr {
  namespace ccsds {

    class qa_conv_encode27_aux : public CppUnit::TestCase {
      CPPUNIT_TEST_SUITE(qa_conv_encode27_aux);
      CPPUNIT_TEST(test_zeroes_12);
      CPPUNIT_TEST(test_ones_12);
      CPPUNIT_TEST(test_asm_12);
      CPPUNIT_TEST(test_asm_23);
      CPPUNIT_TEST(test_asm_34);
      CPPUNIT_TEST(test_asm_56);
      CPPUNIT_TEST(test_asm_78);
      CPPUNIT_TEST_SUITE_END();

    private:
      void test_zeroes_12();
      void test_ones_12();
      void test_asm_12();
      void test_asm_23();
      void test_asm_34();
      void test_asm_56();
      void test_asm_78();
    };

  } /* namespace ccsds */
} /* namespace gr */
#endif /* _QA_CCSDS_CONV_ENCODE27_AUX_H_ */
