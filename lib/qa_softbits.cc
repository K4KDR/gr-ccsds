/* -*- c++ -*- */

#include "qa_softbits.h"
#include <ccsds/softbits.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>
#include <limits>

namespace gr {
  namespace ccsds {
    
      //CPPUNIT_ASSERT(output_soft1[0] > 0.0);
    
    void qa_softbits::test_probability_one() {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5f, softbits::probability_one(0.0), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, softbits::probability_one(std::numeric_limits<float>::max()), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, softbits::probability_one(std::numeric_limits<float>::lowest()), 1e-2);
    }

    void qa_softbits::test_probability_zero() {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5f, softbits::probability_zero(0.0), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0f, softbits::probability_zero(std::numeric_limits<float>::max()), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0f, softbits::probability_zero(std::numeric_limits<float>::lowest()), 1e-2);
    }

    void qa_softbits::test_floor_to_size_t() {
      CPPUNIT_ASSERT_EQUAL( 5lu, softbits::floor_to<size_t>( 5.6f ));
      CPPUNIT_ASSERT_EQUAL( 1lu, softbits::floor_to<size_t>( 1.0f ));
      CPPUNIT_ASSERT_EQUAL(27lu, softbits::floor_to<size_t>(27.99f));
      CPPUNIT_ASSERT_EQUAL(27lu, softbits::floor_to<size_t>(27.01f));
    }

    void qa_softbits::test_floor_to_int() {
      CPPUNIT_ASSERT_EQUAL(  5, softbits::floor_to<int>(  5.6f ));
      CPPUNIT_ASSERT_EQUAL(  1, softbits::floor_to<int>(  1.0f ));
      CPPUNIT_ASSERT_EQUAL( 27, softbits::floor_to<int>( 27.99f));
      CPPUNIT_ASSERT_EQUAL( 27, softbits::floor_to<int>( 27.01f));
      CPPUNIT_ASSERT_EQUAL( -6, softbits::floor_to<int>( -5.6f ));
      CPPUNIT_ASSERT_EQUAL( -1, softbits::floor_to<int>( -1.0f ));
      CPPUNIT_ASSERT_EQUAL(-28, softbits::floor_to<int>(-27.99f));
      CPPUNIT_ASSERT_EQUAL(-28, softbits::floor_to<int>(-27.01f));

    }

    void qa_softbits::test_ceil_to_size_t() {
      CPPUNIT_ASSERT_EQUAL( 6lu, softbits::ceil_to<size_t>( 5.6f ));
      CPPUNIT_ASSERT_EQUAL( 1lu, softbits::ceil_to<size_t>( 1.0f ));
      CPPUNIT_ASSERT_EQUAL(28lu, softbits::ceil_to<size_t>(27.99f));
      CPPUNIT_ASSERT_EQUAL(28lu, softbits::ceil_to<size_t>(27.01f));
    }

    void qa_softbits::test_ceil_to_int() {
      CPPUNIT_ASSERT_EQUAL(  6, softbits::ceil_to<int>(  5.6f ));
      CPPUNIT_ASSERT_EQUAL(  1, softbits::ceil_to<int>(  1.0f ));
      CPPUNIT_ASSERT_EQUAL( 28, softbits::ceil_to<int>( 27.99f));
      CPPUNIT_ASSERT_EQUAL( 28, softbits::ceil_to<int>( 27.01f));
      CPPUNIT_ASSERT_EQUAL( -5, softbits::ceil_to<int>( -5.6f ));
      CPPUNIT_ASSERT_EQUAL( -1, softbits::ceil_to<int>( -1.0f ));
      CPPUNIT_ASSERT_EQUAL(-27, softbits::ceil_to<int>(-27.99f));
      CPPUNIT_ASSERT_EQUAL(-27, softbits::ceil_to<int>(-27.01f));
    }

    void qa_softbits::test_map_to_uint8() {
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(127), softbits::map_to_uint8(1e-4f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(  0), softbits::map_to_uint8(std::numeric_limits<float>::lowest()));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(255), softbits::map_to_uint8(std::numeric_limits<float>::max()));
    }

    void qa_softbits::test_hard_decision() {
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1), softbits::hard_decision( 1e-3f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1), softbits::hard_decision( 1e0f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1), softbits::hard_decision( 1e5f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(1), softbits::hard_decision( std::numeric_limits<float>::max()));

      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(0), softbits::hard_decision(-1e-3f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(0), softbits::hard_decision(-1e0f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(0), softbits::hard_decision(-1e5f));
      CPPUNIT_ASSERT_EQUAL(static_cast<uint8_t>(0), softbits::hard_decision( std::numeric_limits<float>::lowest()));
    }

    void qa_softbits::test_create_from_sample() {
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 2000.0f, softbits::create_from_sample( 1.0f, 1e-3f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(-2000.0f, softbits::create_from_sample(-1.0f, 1e-3f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(    0.0f, softbits::create_from_sample( 0.0f, 1e-3f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(   20.0f, softbits::create_from_sample( 1.0f, 1e-1f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(  -20.0f, softbits::create_from_sample(-1.0f, 1e-1f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(    0.0f, softbits::create_from_sample( 0.0f, 1e-1f), 1e-2);
    }

    void qa_softbits::test_create_from_bit() {
      CPPUNIT_ASSERT_DOUBLES_EQUAL( 2000.0f, softbits::create_from_bit( true, 1e-3f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(-2000.0f, softbits::create_from_bit(false, 1e-3f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(   20.0f, softbits::create_from_bit( true, 1e-1f), 1e-2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(  -20.0f, softbits::create_from_bit(false, 1e-1f), 1e-2);
    }

  }  // namespace ccsds
}  // namespace gr
