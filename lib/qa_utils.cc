/* -*- c++ -*- */
/*
 * Copyright 2021 gr-ccsds Martin Luelf <mail@mluelf.de>.
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
#include <config.h>
#endif

#include "ccsds_utils.h"
#include <gnuradio/attributes.h>
#include <boost/test/unit_test.hpp>

namespace gr {
  namespace ccsds {

    BOOST_AUTO_TEST_CASE(test_utils_update_nobranch) {
        size_t a = 0lu;
        BOOST_CHECK_EQUAL( 0lu, a );
        
        utils::update_nobranch(a, false, 5lu);
        BOOST_CHECK_EQUAL( 0lu, a );
        
        utils::update_nobranch(a, false, 0lu);
        BOOST_CHECK_EQUAL( 0lu, a );
        
        utils::update_nobranch(a, true, 5lu);
        BOOST_CHECK_EQUAL( 5lu, a );
        
        utils::update_nobranch(a, true, 2lu);
        BOOST_CHECK_EQUAL( 2lu, a );
        
        utils::update_nobranch(a, false, 0lu);
        BOOST_CHECK_EQUAL( 2lu, a );
        
        
        int b=0;
        BOOST_CHECK_EQUAL( 0, b );
        
        utils::update_nobranch(b, false, 5);
        BOOST_CHECK_EQUAL( 0, b );
        
        utils::update_nobranch(b, false, -5);
        BOOST_CHECK_EQUAL( 0, b );
        
        utils::update_nobranch(b, true, 5);
        BOOST_CHECK_EQUAL( 5, b );
        
        utils::update_nobranch(b, true, -2);
        BOOST_CHECK_EQUAL( -2, b );
        
        utils::update_nobranch(b, false, 0);
        BOOST_CHECK_EQUAL( -2, b );
        
        
        float c=0.0f;
        BOOST_CHECK_EQUAL( 0.0f, c);
        
        utils::update_nobranch(c, false, 5.123f);
        BOOST_CHECK_EQUAL( 0.0f, c);
        
        utils::update_nobranch(c, false, -5.432f);
        BOOST_CHECK_EQUAL( 0.0f, c);
        
        utils::update_nobranch(c, true, 5.789f);
        BOOST_CHECK_EQUAL( 5.789f, c);
        
        utils::update_nobranch(c, true, -2.987f);
        BOOST_CHECK_EQUAL( -2.987f, c);
        
        utils::update_nobranch(c, false, 42.23f);
        BOOST_CHECK_EQUAL( -2.987f, c);
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_pick_larger) {
        BOOST_CHECK_EQUAL(  5lu, utils::pick_larger( 0lu,  5lu) );
        BOOST_CHECK_EQUAL(  5lu, utils::pick_larger( 5lu,  0lu) );
        BOOST_CHECK_EQUAL( 42lu, utils::pick_larger(42lu, 23lu) );
        BOOST_CHECK_EQUAL( 42lu, utils::pick_larger(23lu, 42lu) );
        
        BOOST_CHECK_EQUAL(  5, utils::pick_larger(  0,   5) );
        BOOST_CHECK_EQUAL(  5, utils::pick_larger(  5,   0) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger( 42,  23) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger( 23,  42) );
        BOOST_CHECK_EQUAL( 23, utils::pick_larger( 23, -42) );
        BOOST_CHECK_EQUAL( 23, utils::pick_larger(-42,  23) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger(-23,  42) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger( 42, -23) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger( -0,  42) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger( 42,  -0) );
        BOOST_CHECK_EQUAL(  0, utils::pick_larger(  0, -42) );
        BOOST_CHECK_EQUAL(  0, utils::pick_larger(-42,   0) );
        BOOST_CHECK_EQUAL( -0, utils::pick_larger( -0, -42) );
        BOOST_CHECK_EQUAL( -0, utils::pick_larger(-42,  -0) );
        BOOST_CHECK_EQUAL(-23, utils::pick_larger(-23, -42) );
        BOOST_CHECK_EQUAL(-23, utils::pick_larger(-42, -23) );
        BOOST_CHECK_EQUAL( 42, utils::pick_larger( 42,  42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_larger(-42, -42) );
        
        BOOST_CHECK_EQUAL(  5.0f, utils::pick_larger(  0.0f,   5.0f));
        BOOST_CHECK_EQUAL(  5.0f, utils::pick_larger(  5.0f,   0.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger( 42.0f,  23.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger( 23.0f,  42.0f));
        BOOST_CHECK_EQUAL( 23.0f, utils::pick_larger( 23.0f, -42.0f));
        BOOST_CHECK_EQUAL( 23.0f, utils::pick_larger(-42.0f,  23.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger(-23.0f,  42.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger( 42.0f, -23.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger( -0.0f,  42.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger( 42.0f,  -0.0f));
        BOOST_CHECK_EQUAL(  0.0f, utils::pick_larger(  0.0f, -42.0f));
        BOOST_CHECK_EQUAL(  0.0f, utils::pick_larger(-42.0f,   0.0f));
        BOOST_CHECK_EQUAL( -0.0f, utils::pick_larger( -0.0f, -42.0f));
        BOOST_CHECK_EQUAL( -0.0f, utils::pick_larger(-42.0f,  -0.0f));
        BOOST_CHECK_EQUAL(-23.0f, utils::pick_larger(-23.0f, -42.0f));
        BOOST_CHECK_EQUAL(-23.0f, utils::pick_larger(-42.0f, -23.0f));
        BOOST_CHECK_EQUAL(-23.0f, utils::pick_larger(-23.0f, -42.0f));
        BOOST_CHECK_EQUAL(-23.0f, utils::pick_larger(-42.0f, -23.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_larger( 42.0f,  42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_larger(-42.0f, -42.0f));
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_pick_smaller) {
        BOOST_CHECK_EQUAL(  0lu, utils::pick_smaller( 0lu,  5lu) );
        BOOST_CHECK_EQUAL(  0lu, utils::pick_smaller( 5lu,  0lu) );
        BOOST_CHECK_EQUAL( 23lu, utils::pick_smaller(42lu, 23lu) );
        BOOST_CHECK_EQUAL( 23lu, utils::pick_smaller(23lu, 42lu) );
        
        BOOST_CHECK_EQUAL(  0, utils::pick_smaller(  0,   5) );
        BOOST_CHECK_EQUAL(  0, utils::pick_smaller(  5,   0) );
        BOOST_CHECK_EQUAL( 23, utils::pick_smaller( 42,  23) );
        BOOST_CHECK_EQUAL( 23, utils::pick_smaller( 23,  42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller( 23, -42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(-42,  23) );
        BOOST_CHECK_EQUAL(-23, utils::pick_smaller(-23,  42) );
        BOOST_CHECK_EQUAL(-23, utils::pick_smaller( 42, -23) );
        BOOST_CHECK_EQUAL( -0, utils::pick_smaller( -0,  42) );
        BOOST_CHECK_EQUAL( -0, utils::pick_smaller( 42,  -0) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(  0, -42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(-42,   0) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller( -0, -42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(-42,  -0) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(-23, -42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(-42, -23) );
        BOOST_CHECK_EQUAL( 42, utils::pick_smaller( 42,  42) );
        BOOST_CHECK_EQUAL(-42, utils::pick_smaller(-42, -42) );
        
        BOOST_CHECK_EQUAL(  0.0f, utils::pick_smaller(  0.0f,   5.0f));
        BOOST_CHECK_EQUAL(  0.0f, utils::pick_smaller(  5.0f,   0.0f));
        BOOST_CHECK_EQUAL( 23.0f, utils::pick_smaller( 42.0f,  23.0f));
        BOOST_CHECK_EQUAL( 23.0f, utils::pick_smaller( 23.0f,  42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller( 23.0f, -42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-42.0f,  23.0f));
        BOOST_CHECK_EQUAL(-23.0f, utils::pick_smaller(-23.0f,  42.0f));
        BOOST_CHECK_EQUAL(-23.0f, utils::pick_smaller( 42.0f, -23.0f));
        BOOST_CHECK_EQUAL( -0.0f, utils::pick_smaller( -0.0f,  42.0f));
        BOOST_CHECK_EQUAL( -0.0f, utils::pick_smaller( 42.0f,  -0.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(  0.0f, -42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-42.0f,   0.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller( -0.0f, -42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-42.0f,  -0.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-23.0f, -42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-42.0f, -23.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-23.0f, -42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-42.0f, -23.0f));
        BOOST_CHECK_EQUAL( 42.0f, utils::pick_smaller( 42.0f,  42.0f));
        BOOST_CHECK_EQUAL(-42.0f, utils::pick_smaller(-42.0f, -42.0f));
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_add_signed_to_unsigned) {
        BOOST_CHECK_EQUAL(  0lu, utils::add_signed_to_unsigned(0lu,  0) );
        BOOST_CHECK_EQUAL(  5lu, utils::add_signed_to_unsigned(0lu,  5) );
        BOOST_CHECK_EQUAL(  0lu, utils::add_signed_to_unsigned(0lu, -5) );
        BOOST_CHECK_EQUAL( 10lu, utils::add_signed_to_unsigned(5lu,  5) );
        BOOST_CHECK_EQUAL(  4lu, utils::add_signed_to_unsigned(5lu, -1) );
        BOOST_CHECK_EQUAL(  0lu, utils::add_signed_to_unsigned(5lu, -5) );
        BOOST_CHECK_EQUAL(  0lu, utils::add_signed_to_unsigned(5lu,-10) );
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_compare_infinite_floats) {
        const float pinv = std::numeric_limits<float>::infinity();
        const float ninv = -pinv;
        
        BOOST_CHECK_EQUAL( true, utils::compare_infinite_floats(pinv, pinv) );
        BOOST_CHECK_EQUAL( true, utils::compare_infinite_floats(ninv, ninv) );
        BOOST_CHECK_EQUAL( false, utils::compare_infinite_floats(ninv, pinv) );
        BOOST_CHECK_EQUAL( false, utils::compare_infinite_floats(pinv, ninv) );
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_floats_equal_non_nan) {
        const float pinv = std::numeric_limits<float>::infinity();
        const float ninv = -pinv;
        
        BOOST_CHECK_EQUAL( true, utils::floats_equal_non_nan( 0.0f, 0.0f , 1e-3f) );
        BOOST_CHECK_EQUAL( true, utils::floats_equal_non_nan(-5.0f,-5.0f , 1e-3f) );
        BOOST_CHECK_EQUAL( true, utils::floats_equal_non_nan( 5.0f, 5.0f , 1e-3f) );
        BOOST_CHECK_EQUAL( true, utils::floats_equal_non_nan(pinv,  pinv , 1e-3f) );
        BOOST_CHECK_EQUAL( true, utils::floats_equal_non_nan(ninv,  ninv , 1e-3f) );
        
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(  0.0f,   pinv, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(  pinv,   0.0f, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(  0.0f,   ninv, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(  ninv,   0.0f, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(100.0f,   pinv, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(  pinv, 100.0f, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(100.0f,   ninv, 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(  ninv, 100.0f, 1e-3f) );
        
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan( 5.0f, 5.1f , 1e-3f) );
        BOOST_CHECK_EQUAL( false, utils::floats_equal_non_nan(-5.0f,-5.1f , 1e-3f) );
        BOOST_CHECK_EQUAL(  true, utils::floats_equal_non_nan( 5.0f, 5.1f , 1e-0f) );
        BOOST_CHECK_EQUAL(  true, utils::floats_equal_non_nan(-5.0f,-5.1f , 1e-0f) );
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_extract_bit_left) {
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 0) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 1) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 2) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 3) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 4) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 5) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0x01, 6) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_left(0x01, 7) );
        
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_left(0xA5, 0) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0xA5, 1) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_left(0xA5, 2) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0xA5, 3) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0xA5, 4) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_left(0xA5, 5) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_left(0xA5, 6) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_left(0xA5, 7) );
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_extract_bit_right) {
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 7) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 6) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 5) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 4) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 3) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 2) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0x01, 1) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_right(0x01, 0) );
        
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_right(0xA5, 7) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0xA5, 6) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_right(0xA5, 5) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0xA5, 4) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0xA5, 3) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_right(0xA5, 2) );
        BOOST_CHECK_EQUAL( false, utils::extract_bit_right(0xA5, 1) );
        BOOST_CHECK_EQUAL(  true, utils::extract_bit_right(0xA5, 0) );
    }
    
    BOOST_AUTO_TEST_CASE(test_utils_divide_ceil) {
      BOOST_CHECK_EQUAL( 0u, utils::divide_ceil<const unsigned int>(0u, 1u) );
      BOOST_CHECK_EQUAL( 1u, utils::divide_ceil<const unsigned int>(1u, 1u) );
      BOOST_CHECK_EQUAL( 2u, utils::divide_ceil<const unsigned int>(2u, 1u) );
      
      BOOST_CHECK_EQUAL( 0u, utils::divide_ceil<const unsigned int>(0u, 2u) );
      BOOST_CHECK_EQUAL( 1u, utils::divide_ceil<const unsigned int>(1u, 2u) );
      BOOST_CHECK_EQUAL( 1u, utils::divide_ceil<const unsigned int>(2u, 2u) );
      BOOST_CHECK_EQUAL( 2u, utils::divide_ceil<const unsigned int>(3u, 2u) );
      BOOST_CHECK_EQUAL( 2u, utils::divide_ceil<const unsigned int>(4u, 2u) );
      
      BOOST_CHECK_EQUAL( 0u, utils::divide_ceil<const unsigned int>(0u, 3u) );
      BOOST_CHECK_EQUAL( 1u, utils::divide_ceil<const unsigned int>(1u, 3u) );
      BOOST_CHECK_EQUAL( 1u, utils::divide_ceil<const unsigned int>(2u, 3u) );
      BOOST_CHECK_EQUAL( 1u, utils::divide_ceil<const unsigned int>(3u, 3u) );
      BOOST_CHECK_EQUAL( 2u, utils::divide_ceil<const unsigned int>(4u, 3u) );
      BOOST_CHECK_EQUAL( 2u, utils::divide_ceil<const unsigned int>(5u, 3u) );
      BOOST_CHECK_EQUAL( 2u, utils::divide_ceil<const unsigned int>(6u, 3u) );
      BOOST_CHECK_EQUAL( 3u, utils::divide_ceil<const unsigned int>(7u, 3u) );
      BOOST_CHECK_EQUAL( 3u, utils::divide_ceil<const unsigned int>(8u, 3u) );
      BOOST_CHECK_EQUAL( 3u, utils::divide_ceil<const unsigned int>(9u, 3u) );
    }

    BOOST_AUTO_TEST_CASE(test_utils_divide_floor) {
        BOOST_CHECK_EQUAL( 0u, utils::divide_floor<const unsigned int>(0u, 1u) );
        BOOST_CHECK_EQUAL( 1u, utils::divide_floor<const unsigned int>(1u, 1u) );
        BOOST_CHECK_EQUAL( 2u, utils::divide_floor<const unsigned int>(2u, 1u) );
        
        BOOST_CHECK_EQUAL( 0u, utils::divide_floor<const unsigned int>(0u, 2u) );
        BOOST_CHECK_EQUAL( 0u, utils::divide_floor<const unsigned int>(1u, 2u) );
        BOOST_CHECK_EQUAL( 1u, utils::divide_floor<const unsigned int>(2u, 2u) );
        BOOST_CHECK_EQUAL( 1u, utils::divide_floor<const unsigned int>(3u, 2u) );
        BOOST_CHECK_EQUAL( 2u, utils::divide_floor<const unsigned int>(4u, 2u) );
        
        BOOST_CHECK_EQUAL( 0u, utils::divide_floor<const unsigned int>(0u, 3u) );
        BOOST_CHECK_EQUAL( 0u, utils::divide_floor<const unsigned int>(1u, 3u) );
        BOOST_CHECK_EQUAL( 0u, utils::divide_floor<const unsigned int>(2u, 3u) );
        BOOST_CHECK_EQUAL( 1u, utils::divide_floor<const unsigned int>(3u, 3u) );
        BOOST_CHECK_EQUAL( 1u, utils::divide_floor<const unsigned int>(4u, 3u) );
        BOOST_CHECK_EQUAL( 1u, utils::divide_floor<const unsigned int>(5u, 3u) );
        BOOST_CHECK_EQUAL( 2u, utils::divide_floor<const unsigned int>(6u, 3u) );
        BOOST_CHECK_EQUAL( 2u, utils::divide_floor<const unsigned int>(7u, 3u) );
        BOOST_CHECK_EQUAL( 2u, utils::divide_floor<const unsigned int>(8u, 3u) );
        BOOST_CHECK_EQUAL( 3u, utils::divide_floor<const unsigned int>(9u, 3u) );
    }

  } /* namespace ccsds */
} /* namespace gr */
