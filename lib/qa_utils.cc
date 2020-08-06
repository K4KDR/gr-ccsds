/* -*- c++ -*- */

#include "qa_utils.h"

#include "ccsds_utils.h"

#include <cppunit/TestAssert.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {
    
    void qa_utils::test_update_nobranch() {
        size_t a = 0lu;
        CPPUNIT_ASSERT_EQUAL( 0lu, a );
        
        utils::update_nobranch(a, false, 5lu);
        CPPUNIT_ASSERT_EQUAL( 0lu, a );
        
        utils::update_nobranch(a, false, 0lu);
        CPPUNIT_ASSERT_EQUAL( 0lu, a );
        
        utils::update_nobranch(a, true, 5lu);
        CPPUNIT_ASSERT_EQUAL( 5lu, a );
        
        utils::update_nobranch(a, true, 2lu);
        CPPUNIT_ASSERT_EQUAL( 2lu, a );
        
        utils::update_nobranch(a, false, 0lu);
        CPPUNIT_ASSERT_EQUAL( 2lu, a );
        
        
        int b=0;
        CPPUNIT_ASSERT_EQUAL( 0, b );
        
        utils::update_nobranch(b, false, 5);
        CPPUNIT_ASSERT_EQUAL( 0, b );
        
        utils::update_nobranch(b, false, -5);
        CPPUNIT_ASSERT_EQUAL( 0, b );
        
        utils::update_nobranch(b, true, 5);
        CPPUNIT_ASSERT_EQUAL( 5, b );
        
        utils::update_nobranch(b, true, -2);
        CPPUNIT_ASSERT_EQUAL( -2, b );
        
        utils::update_nobranch(b, false, 0);
        CPPUNIT_ASSERT_EQUAL( -2, b );
        
        
        float c=0.0f;
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0f, c , 1e-3f);
        
        utils::update_nobranch(c, false, 5.123f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0f, c , 1e-3f);
        
        utils::update_nobranch(c, false, -5.432f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0f, c , 1e-3f);
        
        utils::update_nobranch(c, true, 5.789f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 5.789f, c , 1e-3f);
        
        utils::update_nobranch(c, true, -2.987f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -2.987f, c , 1e-3f);
        
        utils::update_nobranch(c, false, 42.23f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -2.987f, c , 1e-3f);
    }
    
    void qa_utils::test_pick_larger() {
        CPPUNIT_ASSERT_EQUAL(  5lu, utils::pick_larger( 0lu,  5lu) );
        CPPUNIT_ASSERT_EQUAL(  5lu, utils::pick_larger( 5lu,  0lu) );
        CPPUNIT_ASSERT_EQUAL( 42lu, utils::pick_larger(42lu, 23lu) );
        CPPUNIT_ASSERT_EQUAL( 42lu, utils::pick_larger(23lu, 42lu) );
        
        CPPUNIT_ASSERT_EQUAL(  5, utils::pick_larger(  0,   5) );
        CPPUNIT_ASSERT_EQUAL(  5, utils::pick_larger(  5,   0) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger( 42,  23) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger( 23,  42) );
        CPPUNIT_ASSERT_EQUAL( 23, utils::pick_larger( 23, -42) );
        CPPUNIT_ASSERT_EQUAL( 23, utils::pick_larger(-42,  23) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger(-23,  42) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger( 42, -23) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger( -0,  42) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger( 42,  -0) );
        CPPUNIT_ASSERT_EQUAL(  0, utils::pick_larger(  0, -42) );
        CPPUNIT_ASSERT_EQUAL(  0, utils::pick_larger(-42,   0) );
        CPPUNIT_ASSERT_EQUAL( -0, utils::pick_larger( -0, -42) );
        CPPUNIT_ASSERT_EQUAL( -0, utils::pick_larger(-42,  -0) );
        CPPUNIT_ASSERT_EQUAL(-23, utils::pick_larger(-23, -42) );
        CPPUNIT_ASSERT_EQUAL(-23, utils::pick_larger(-42, -23) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_larger( 42,  42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_larger(-42, -42) );
        
        CPPUNIT_ASSERT_DOUBLES_EQUAL(  5.0f, utils::pick_larger(  0.0f,   5.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(  5.0f, utils::pick_larger(  5.0f,   0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger( 42.0f,  23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger( 23.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 23.0f, utils::pick_larger( 23.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 23.0f, utils::pick_larger(-42.0f,  23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger(-23.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger( 42.0f, -23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger( -0.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger( 42.0f,  -0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0f, utils::pick_larger(  0.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0f, utils::pick_larger(-42.0f,   0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.0f, utils::pick_larger( -0.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.0f, utils::pick_larger(-42.0f,  -0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-23.0f, utils::pick_larger(-23.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-23.0f, utils::pick_larger(-42.0f, -23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-23.0f, utils::pick_larger(-23.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-23.0f, utils::pick_larger(-42.0f, -23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_larger( 42.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_larger(-42.0f, -42.0f) , 1e-3f);
    }
    
    void qa_utils::test_pick_smaller() {
        CPPUNIT_ASSERT_EQUAL(  0lu, utils::pick_smaller( 0lu,  5lu) );
        CPPUNIT_ASSERT_EQUAL(  0lu, utils::pick_smaller( 5lu,  0lu) );
        CPPUNIT_ASSERT_EQUAL( 23lu, utils::pick_smaller(42lu, 23lu) );
        CPPUNIT_ASSERT_EQUAL( 23lu, utils::pick_smaller(23lu, 42lu) );
        
        CPPUNIT_ASSERT_EQUAL(  0, utils::pick_smaller(  0,   5) );
        CPPUNIT_ASSERT_EQUAL(  0, utils::pick_smaller(  5,   0) );
        CPPUNIT_ASSERT_EQUAL( 23, utils::pick_smaller( 42,  23) );
        CPPUNIT_ASSERT_EQUAL( 23, utils::pick_smaller( 23,  42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller( 23, -42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(-42,  23) );
        CPPUNIT_ASSERT_EQUAL(-23, utils::pick_smaller(-23,  42) );
        CPPUNIT_ASSERT_EQUAL(-23, utils::pick_smaller( 42, -23) );
        CPPUNIT_ASSERT_EQUAL( -0, utils::pick_smaller( -0,  42) );
        CPPUNIT_ASSERT_EQUAL( -0, utils::pick_smaller( 42,  -0) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(  0, -42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(-42,   0) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller( -0, -42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(-42,  -0) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(-23, -42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(-42, -23) );
        CPPUNIT_ASSERT_EQUAL( 42, utils::pick_smaller( 42,  42) );
        CPPUNIT_ASSERT_EQUAL(-42, utils::pick_smaller(-42, -42) );
        
        CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0f, utils::pick_smaller(  0.0f,   5.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0f, utils::pick_smaller(  5.0f,   0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 23.0f, utils::pick_smaller( 42.0f,  23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 23.0f, utils::pick_smaller( 23.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller( 23.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-42.0f,  23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-23.0f, utils::pick_smaller(-23.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-23.0f, utils::pick_smaller( 42.0f, -23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.0f, utils::pick_smaller( -0.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.0f, utils::pick_smaller( 42.0f,  -0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(  0.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-42.0f,   0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller( -0.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-42.0f,  -0.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-23.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-42.0f, -23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-23.0f, -42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-42.0f, -23.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 42.0f, utils::pick_smaller( 42.0f,  42.0f) , 1e-3f);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-42.0f, utils::pick_smaller(-42.0f, -42.0f) , 1e-3f);
    }
    
    void qa_utils::test_add_signed_to_unsigned() {
        CPPUNIT_ASSERT_EQUAL(  0lu, utils::add_signed_to_unsigned(0lu,  0) );
        CPPUNIT_ASSERT_EQUAL(  5lu, utils::add_signed_to_unsigned(0lu,  5) );
        CPPUNIT_ASSERT_EQUAL(  0lu, utils::add_signed_to_unsigned(0lu, -5) );
        CPPUNIT_ASSERT_EQUAL( 10lu, utils::add_signed_to_unsigned(5lu,  5) );
        CPPUNIT_ASSERT_EQUAL(  4lu, utils::add_signed_to_unsigned(5lu, -1) );
        CPPUNIT_ASSERT_EQUAL(  0lu, utils::add_signed_to_unsigned(5lu, -5) );
        CPPUNIT_ASSERT_EQUAL(  0lu, utils::add_signed_to_unsigned(5lu,-10) );
    }
    
    void qa_utils::test_compare_infinite_floats() {
        const float pinv = std::numeric_limits<float>::infinity();
        const float ninv = -pinv;
        
        CPPUNIT_ASSERT_EQUAL( true, utils::compare_infinite_floats(pinv, pinv) );
        CPPUNIT_ASSERT_EQUAL( true, utils::compare_infinite_floats(ninv, ninv) );
        CPPUNIT_ASSERT_EQUAL( false, utils::compare_infinite_floats(ninv, pinv) );
        CPPUNIT_ASSERT_EQUAL( false, utils::compare_infinite_floats(pinv, ninv) );
    }
    
    void qa_utils::test_floats_equal_non_nan() {
        const float pinv = std::numeric_limits<float>::infinity();
        const float ninv = -pinv;
        
        CPPUNIT_ASSERT_EQUAL( true, utils::floats_equal_non_nan( 0.0f, 0.0f , 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( true, utils::floats_equal_non_nan(-5.0f,-5.0f , 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( true, utils::floats_equal_non_nan( 5.0f, 5.0f , 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( true, utils::floats_equal_non_nan(pinv,  pinv , 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( true, utils::floats_equal_non_nan(ninv,  ninv , 1e-3f) );
        
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(  0.0f,   pinv, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(  pinv,   0.0f, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(  0.0f,   ninv, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(  ninv,   0.0f, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(100.0f,   pinv, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(  pinv, 100.0f, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(100.0f,   ninv, 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(  ninv, 100.0f, 1e-3f) );
        
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan( 5.0f, 5.1f , 1e-3f) );
        CPPUNIT_ASSERT_EQUAL( false, utils::floats_equal_non_nan(-5.0f,-5.1f , 1e-3f) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::floats_equal_non_nan( 5.0f, 5.1f , 1e-0f) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::floats_equal_non_nan(-5.0f,-5.1f , 1e-0f) );
    }
    
    void qa_utils::test_extract_bit_left() {
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 0) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 1) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 2) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 3) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 4) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 5) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0x01, 6) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_left(0x01, 7) );
        
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_left(0xA5, 0) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0xA5, 1) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_left(0xA5, 2) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0xA5, 3) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0xA5, 4) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_left(0xA5, 5) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_left(0xA5, 6) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_left(0xA5, 7) );
    }
    
    void qa_utils::test_extract_bit_right() {
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 7) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 6) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 5) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 4) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 3) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 2) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0x01, 1) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_right(0x01, 0) );
        
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_right(0xA5, 7) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0xA5, 6) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_right(0xA5, 5) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0xA5, 4) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0xA5, 3) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_right(0xA5, 2) );
        CPPUNIT_ASSERT_EQUAL( false, utils::extract_bit_right(0xA5, 1) );
        CPPUNIT_ASSERT_EQUAL(  true, utils::extract_bit_right(0xA5, 0) );
    }
    
    void qa_utils::test_divide_ceil() {
      CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_ceil<const unsigned int>(0u, 1u) );
      CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_ceil<const unsigned int>(1u, 1u) );
      CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_ceil<const unsigned int>(2u, 1u) );
      
      CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_ceil<const unsigned int>(0u, 2u) );
      CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_ceil<const unsigned int>(1u, 2u) );
      CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_ceil<const unsigned int>(2u, 2u) );
      CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_ceil<const unsigned int>(3u, 2u) );
      CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_ceil<const unsigned int>(4u, 2u) );
      
      CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_ceil<const unsigned int>(0u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_ceil<const unsigned int>(1u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_ceil<const unsigned int>(2u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_ceil<const unsigned int>(3u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_ceil<const unsigned int>(4u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_ceil<const unsigned int>(5u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_ceil<const unsigned int>(6u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 3u, utils::divide_ceil<const unsigned int>(7u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 3u, utils::divide_ceil<const unsigned int>(8u, 3u) );
      CPPUNIT_ASSERT_EQUAL( 3u, utils::divide_ceil<const unsigned int>(9u, 3u) );
    }

    void qa_utils::test_divide_floor() {
        CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_floor<const unsigned int>(0u, 1u) );
        CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_floor<const unsigned int>(1u, 1u) );
        CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_floor<const unsigned int>(2u, 1u) );
        
        CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_floor<const unsigned int>(0u, 2u) );
        CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_floor<const unsigned int>(1u, 2u) );
        CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_floor<const unsigned int>(2u, 2u) );
        CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_floor<const unsigned int>(3u, 2u) );
        CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_floor<const unsigned int>(4u, 2u) );
        
        CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_floor<const unsigned int>(0u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_floor<const unsigned int>(1u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 0u, utils::divide_floor<const unsigned int>(2u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_floor<const unsigned int>(3u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_floor<const unsigned int>(4u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 1u, utils::divide_floor<const unsigned int>(5u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_floor<const unsigned int>(6u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_floor<const unsigned int>(7u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 2u, utils::divide_floor<const unsigned int>(8u, 3u) );
        CPPUNIT_ASSERT_EQUAL( 3u, utils::divide_floor<const unsigned int>(9u, 3u) );
    }

  } /* namespace ccsds */
} /* namespace gr */
