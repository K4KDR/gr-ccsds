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

#include <gnuradio/ccsds/snr_interpolation.h>
#include <gnuradio/attributes.h>
#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

namespace gr {
  namespace ccsds {

    BOOST_AUTO_TEST_CASE(test_snr_interpolation_t1, * utf::tolerance(0.001f)) {
        const snr a = snr(1.0, 0.5);
        const snr b = snr(1.0, 0.1);
        const snr c = snr(0.5, 0.5);
        
        const uint64_t block_len = 100lu;
        const uint64_t offset_start = 1000lu;
        const uint64_t offset_end = offset_start + block_len - 1lu;
        
        
        snr_interpolation i1(a, b, offset_start, block_len);
        
        BOOST_TEST(i1.block_length() == block_len);
        
        BOOST_TEST(i1.getFractionAbs(offset_start) == 0.0f);
        BOOST_TEST(i1.getFractionAbs(offset_end) == 1.0f);
        
        BOOST_TEST(i1.Es(offset_start) == a.Es());
        BOOST_TEST(i1.N0(offset_start) == a.N0());
        BOOST_TEST(i1.Es(offset_end) == b.Es());
        BOOST_TEST(i1.N0(offset_end) == b.N0());
    }
      
  } /* namespace ccsds */
} /* namespace gr */
