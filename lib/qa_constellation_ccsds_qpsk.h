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
#ifndef _QA_CCSDS_CONSTELLATION_CCSDS_QPSK_H_
#define _QA_CCSDS_CONSTELLATION_CCSDS_QPSK_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

#include "qa_constellation.h"
#include <gnuradio/ccsds/constellation_ccsds_qpsk.h>


namespace gr {
  namespace ccsds {
    namespace qa {
      namespace constellation {

      class qpsk : public constellation::generic_test_case<ccsds::constellation_ccsds_qpsk, 2u, 4u> {
        CPPUNIT_TEST_SUITE(qpsk);
        CPPUNIT_TEST(test_dimensions);
        CPPUNIT_TEST(test_decisions);
        CPPUNIT_TEST(test_chain);
        CPPUNIT_TEST_SUITE_END();

      protected:
        void test_decisions();
      };

      }  // namespace constellation
    }  // namespace qa
  }  // namespace ccsds
}  // namespace gr
#endif  // _QA_CCSDS_CONSTELLATION_CCSDS_QPSK_H_
