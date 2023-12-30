/* -*- c++ -*- */

#include "qa_frame_sync_config.h"

#include "ccsds_utils.h"

#include <gnuradio/ccsds/frame_sync_config.h>
#include <gnuradio/ccsds/constellation_ccsds_bpsk.h>
#include <gnuradio/ccsds/constellation_ccsds_qpsk.h>
#include <gnuradio/ccsds/constellation_ccsds_qpsk_axis.h>

#include <cppunit/TestAssert.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {

    void qa_frame_sync_config::test_ambiguities_bpsk() {
        constellation_ccsds_bpsk::sptr bpsk = constellation_ccsds_bpsk::make();
        frame_sync_config::sptr fsc = frame_sync_config::make(bpsk->base(), "1ACFFC1D", 32lu, 1024lu, 0, 0);
        
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex(+1.0f, 0.0f), fsc->get_ambiguity_symbol(0lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex(-1.0f, 0.0f), fsc->get_ambiguity_symbol(1lu), 1e-2f);

        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex(  1.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 32.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 32.0f, 99.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 32.0f,-99.0f)));
        
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex( -1.0,  0.0)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(-32.0,  0.0)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(-32.0, 99.0)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(-32.0,-99.0)));
    }
    
    void qa_frame_sync_config::test_ambiguities_qpsk() {
        constellation_ccsds_qpsk::sptr qpsk = constellation_ccsds_qpsk::make();
        frame_sync_config::sptr fsc = frame_sync_config::make(qpsk->base(), "1ACFFC1D", 32lu, 1024lu, 0, 0);
        
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex( 1.0f,  0.0f), fsc->get_ambiguity_symbol(0lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex( 0.0f, -1.0f), fsc->get_ambiguity_symbol(1lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex( 0.0f,  1.0f), fsc->get_ambiguity_symbol(2lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex(-1.0f,  0.0f), fsc->get_ambiguity_symbol(3lu), 1e-2f);
        
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex(  1.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 99.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 99.0f, 98.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 99.0f,-98.0f)));

        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(  0.0f, -1.0f)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(  0.0f,-99.0f)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex( 98.0f,-99.0f)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(-98.0f,-99.0f)));

        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex(  0.0f,  1.0f)));
        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex(  0.0f, 99.0f)));
        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex( 98.0f, 99.0f)));
        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex(-98.0f, 99.0f)));

        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex( -1.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex(-99.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex(-99.0f, 98.0f)));
        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex(-99.0f,-98.0f)));
    }

     void qa_frame_sync_config::test_ambiguities_qpsk_axis() {
        constellation_ccsds_qpsk_axis::sptr qpsk = constellation_ccsds_qpsk_axis::make();
        frame_sync_config::sptr fsc = frame_sync_config::make(qpsk->base(), "1ACFFC1D", 32lu, 1024lu, 0, 0);
        
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex( 1.0f,  0.0f), fsc->get_ambiguity_symbol(0lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex( 0.0f, -1.0f), fsc->get_ambiguity_symbol(1lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex( 0.0f,  1.0f), fsc->get_ambiguity_symbol(2lu), 1e-2f);
        CPPUNIT_ASSERT_COMPLEXES_EQUAL(gr_complex(-1.0f,  0.0f), fsc->get_ambiguity_symbol(3lu), 1e-2f);
        
        
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex(  1.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 99.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 99.0f, 98.0f)));
        CPPUNIT_ASSERT_EQUAL(0lu, fsc->detect_ambiguity(gr_complex( 99.0f,-98.0f)));

        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(  0.0f, -1.0f)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(  0.0f,-99.0f)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex( 98.0f,-99.0f)));
        CPPUNIT_ASSERT_EQUAL(1lu, fsc->detect_ambiguity(gr_complex(-98.0f,-99.0f)));

        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex(  0.0f,  1.0f)));
        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex(  0.0f, 99.0f)));
        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex( 98.0f, 99.0f)));
        CPPUNIT_ASSERT_EQUAL(2lu, fsc->detect_ambiguity(gr_complex(-98.0f, 99.0f)));

        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex( -1.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex(-99.0f,  0.0f)));
        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex(-99.0f, 98.0f)));
        CPPUNIT_ASSERT_EQUAL(3lu, fsc->detect_ambiguity(gr_complex(-99.0f,-98.0f)));
    }
      
  } /* namespace ccsds */
} /* namespace gr */
