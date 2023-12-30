/* -*- c++ -*- */

#include "qa_constellation_ccsds_bpsk.h"
#include <gnuradio/ccsds/constellation_ccsds_bpsk.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {
    namespace qa {
      namespace constellation {

    void bpsk::test_dimensions() {
      constellation_ccsds_bpsk::sptr c = constellation_ccsds_bpsk::make();

      CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Arity of BPSK incorrect",
        2u,
        c->arity()
      );

      CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Number of bts per BPSK symbol incorrect",
        1u,
        c->bits_per_symbol()
      );

      for (unsigned int i=0; i<c->arity(); i++) {
        std::vector<gr_complex> points = c->map_to_points_v(i);

        CPPUNIT_ASSERT_EQUAL_MESSAGE(
          "Number of constellation points for one symbol hast to be 1.",
          static_cast<size_t>(1),
          points.size()
        );
      }
    }

    void bpsk::test_chain() {
      constellation_ccsds_bpsk::sptr c = constellation_ccsds_bpsk::make();

      for (unsigned int i=0; i<c->arity(); i++) {
        std::vector<gr_complex> points = c->map_to_points_v(i);

        CPPUNIT_ASSERT(points.size() == 1u);
        
        const unsigned int decision = c->decision_maker(points.data());

        CPPUNIT_ASSERT_EQUAL_MESSAGE(
          "Decision of generated sample is not equal the input sample.",
          i,
          decision
        );
      }
    }

    void bpsk::test_decisions() {
      constellation_ccsds_bpsk::sptr c = constellation_ccsds_bpsk::make();

      const gr_complex input0 = gr_complex(+1.0, 0.0);
      const gr_complex input1 = gr_complex(-1.0, 0.0);
      const std::vector<float> output_soft0 = c->soft_decision_maker(input0);
      const std::vector<float> output_soft1 = c->soft_decision_maker(input1);
      
      /* Symbol 0 */
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "First BPSK symbol is not detected correctly",
        0u,
        c->decision_maker(&input0)
      );

      CPPUNIT_ASSERT_MESSAGE(
        "Number of bits for first BPSK symbol incorrect.",
        output_soft0.size() == 1
      );

      CPPUNIT_ASSERT(output_soft0[0] < 0.0);


      /* Symbol 1 */
      CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Second BPSK symbol is not detected correctly",
        1u,
        c->decision_maker(&input1)
      );

      CPPUNIT_ASSERT_MESSAGE(
        "Number of bits for second BPSK symbol incorrect.",
        output_soft1.size() == 1
      );

      CPPUNIT_ASSERT(output_soft1[0] > 0.0);
    }
      
      }  // namespace constellation
    }  // namespace qa
  }  // namespace ccsds
}  // namespace gr
