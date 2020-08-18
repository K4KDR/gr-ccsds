/* -*- c++ -*- */

#include "qa_frame_sync_buffer.h"

#include "ccsds_utils.h"

#include "frame_sync_buffer.h"
#include <cppunit/TestAssert.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {

    void qa_frame_sync_buffer::test() {
      const size_t kBlockLen = 255;
      const volk::vector<gr_complex> kAsm = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
      const size_t kOverlap = kAsm.size() -1u;
      size_t num_items_in_buffer = 0u;
      aux::frame_sync_buffer buffer(kBlockLen, kAsm, 2u, 0u);
      
      CPPUNIT_ASSERT( !buffer.full() );
      CPPUNIT_ASSERT( buffer.offset() == 0u );
      CPPUNIT_ASSERT( buffer.num_symbols_missing() == kBlockLen + kAsm.size() -1u );
      
      
      std::vector<gr_complex> in(3u*kBlockLen + kAsm.size() -1u);

      for (size_t i=0; i<3u; i++) {
        for (size_t j=0; j<kBlockLen; j++) {
          in[j+i*kBlockLen] = static_cast<float>(j) + static_cast<float>(i)*kBlockLen;
        }
      }
      size_t num_read = buffer.read_signal(&in[0u], kBlockLen+kOverlap, snr(1.0f, 1e-5f));
      CPPUNIT_ASSERT_EQUAL(kBlockLen + kAsm.size() -1u, num_read);
      CPPUNIT_ASSERT( buffer.full() );
      num_items_in_buffer += num_read;

      for (size_t i=0; i<kBlockLen; i++) {
        CPPUNIT_ASSERT_COMPLEXES_EQUAL( buffer.signal_window(i), in[i], 1e-2);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL( buffer.signal(i), in[i], 1e-2);
      }

      CPPUNIT_ASSERT( buffer.full() );
      buffer.advance();
      CPPUNIT_ASSERT( !buffer.full() );

      // Frame 2
      num_read = buffer.read_signal(&in[num_items_in_buffer], kBlockLen, snr(1.0f, 1e-5f));
      CPPUNIT_ASSERT_EQUAL(kBlockLen, num_read);
      CPPUNIT_ASSERT( buffer.full() );
      num_items_in_buffer += num_read;

      for (size_t i=0; i<kBlockLen; i++) {
        CPPUNIT_ASSERT_COMPLEXES_EQUAL( in[i+kBlockLen], buffer.signal_window(i), 1e-2);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL( in[i+kBlockLen], buffer.signal(i+kBlockLen), 1e-2);
      }
      CPPUNIT_ASSERT( buffer.full() );
      buffer.advance();
      CPPUNIT_ASSERT( !buffer.full() );

      // Frame 3
      num_read = buffer.read_signal(&in[num_items_in_buffer], kBlockLen, snr(1.0f, 1e-5f));
      CPPUNIT_ASSERT_EQUAL(kBlockLen, num_read);
      CPPUNIT_ASSERT( buffer.full() );
      num_items_in_buffer += num_read;

      for (size_t i=0; i<kBlockLen; i++) {
        CPPUNIT_ASSERT_COMPLEXES_EQUAL( in[i+2u*kBlockLen], buffer.signal_window(i), 1e-2);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL( in[i+2u*kBlockLen], buffer.signal(i+2u*kBlockLen), 1e-2);
      }
      CPPUNIT_ASSERT( buffer.full() );
      buffer.advance();
      CPPUNIT_ASSERT( !buffer.full() );
    }
      
  } /* namespace ccsds */
} /* namespace gr */
