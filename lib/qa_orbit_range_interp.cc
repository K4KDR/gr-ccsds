/* -*- c++ -*- */

#include "qa_orbit_range_interp.h"
#include "orbit_range_interp.h"
#include "orbit_aux.h"
#include <cppunit/TestAssert.h>
#include <stdio.h>


namespace gr {
  namespace ccsds {

      void qa_orbit_range_interp::test() {
        const char *TLE_LINE1 = "1 43780U 18099Y   19033.19530161  .00000526  00000-0  53415-4 0  9996";
        const char *TLE_LINE2 = "2 43780  97.7595 106.7193 0014861  70.4175 289.8649 14.94903261  9012";

        std::array<double, 3> obs_pos = {
                                            +48.266278, // degree North
                                            +11.668361, // degree East
                                           +545.000000  // metre above ellipsoid
                                        };
        
        const double samp_rate = 200e3;
        const double compute_rate = 100.0; // update 100 times per second
        
        const double duration = 5.0;
        
        const size_t num_samps = static_cast<size_t>(samp_rate*duration);
        const size_t blocksize = 4000u;
        
        CPPUNIT_ASSERT_MESSAGE(
          "Test duration does not result in an integer number of blocks, please adjust the blocksize",
          num_samps%blocksize == 0u
        );
        
        const size_t num_blocks = num_samps/blocksize;
        
        orbits::range _range_sgp4(TLE_LINE1, TLE_LINE2, obs_pos);
        orbits::RangeInterpolator _range_interp(TLE_LINE1, TLE_LINE2, obs_pos, samp_rate, compute_rate);
        
        _range_sgp4.setStartTime(2019, 2, 2, 0, 0, 0.0);
        _range_interp.setStartTime(2019, 2, 2, 0, 0, 0.0);
        
        const double r0 = _range_interp.getRangeAt(0.0);
        
        const double Ts_min = 1.0/(samp_rate*60.0);
        std::vector<double> times(blocksize);
        std::vector<double> ranges_computed_sgp4(blocksize);
        double *ranges_computed_interp = new double[blocksize];
        for(size_t i=0; i<num_blocks; i++) {
          for(size_t j=0; j<blocksize; j++) {
            times[j] = static_cast<double>(i*blocksize+j)*Ts_min;
          }
          
          ranges_computed_sgp4 = _range_sgp4.computeAt(times);
          _range_interp.computeDelta(ranges_computed_interp, blocksize);
          
          for(size_t j=0; j<blocksize; j++) {
            const double sgp4_diff = ranges_computed_sgp4[j]-r0;
            
            //printf("i=%5lu, j=%5lu, sgp4=%lf, interp=%lf, diff=%lf\n", i, j, sgp4_diff, ranges_computed_interp[j], sgp4_diff-ranges_computed_interp[j]);
            
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
              "Computed range deltas differ",
              sgp4_diff,
              ranges_computed_interp[j],
              1e-2
            );
          }
          
        }
        
        delete[] ranges_computed_interp;
      }  // qa_orbit_range_interp::test()
      
  } /* namespace ccsds */
} /* namespace gr */
