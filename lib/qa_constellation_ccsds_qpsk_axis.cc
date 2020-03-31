/* -*- c++ -*- */

#include "qa_constellation_ccsds_qpsk_axis.h"


namespace gr {
  namespace ccsds {
    namespace qa {
      namespace constellation {
        
        void qpsk_axis::test_decisions() {
          constexpr float delta = 1.0f;

          const test_data_t test_data = {
            // correct symbols
            test_data_entry_t( gr_complex(+1.0,  0.0), 0u, {0, 0}),  // 00
            test_data_entry_t( gr_complex( 0.0, -1.0), 1u, {0, 1}), // 01
            test_data_entry_t( gr_complex( 0.0,  1.0), 2u, {1, 0}), // 10
            test_data_entry_t( gr_complex(-1.0,  0.0), 3u, {1, 1}), // 11
            // correct symbols again (to test sample_at_angle_function)
            test_data_entry_t( sample_at_angle(  0.0f), 0u, {0, 0}), // 00
            test_data_entry_t( sample_at_angle(270.0f), 1u, {0, 1}), // 01
            test_data_entry_t( sample_at_angle( 90.0f), 2u, {1, 0}), // 10
            test_data_entry_t( sample_at_angle(180.0f), 3u, {1, 1}), // 11
            // symbols closer to the decision region (to test that there is no 45 degree offset in the decision regions)
            test_data_entry_t( sample_at_angle( 315.0f+delta), 0u, {0, 0}), // 00
            test_data_entry_t( sample_at_angle(  45.0f-delta), 0u, {0, 0}), // 00
            test_data_entry_t( sample_at_angle(  45.0f+delta), 2u, {1, 0}), // 10
            test_data_entry_t( sample_at_angle( 135.0f-delta), 2u, {1, 0}), // 10
            test_data_entry_t( sample_at_angle( 135.0f+delta), 3u, {1, 1}), // 11
            test_data_entry_t( sample_at_angle( 225.0f-delta), 3u, {1, 1}), // 11
            test_data_entry_t( sample_at_angle( 225.0f+delta), 1u, {0, 1}), // 01
            test_data_entry_t( sample_at_angle( 315.0f-delta), 1u, {0, 1}), // 01
          };

          run_decisions_test(test_data);
        }

      }  // namespace constellation
    }  // namespace qa
  }  // namespace ccsds
}  // namespace gr
