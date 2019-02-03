/* -*- c++ -*- */

#include "qa_orbit_aux.h"
#include "qa_orbit_aux_testdata.h"
#include "orbit_aux.h"
#include <cppunit/TestAssert.h>
#include <stdio.h>

namespace gr {
  namespace ccsds {

      /*
       *  Test Julian Date computation
       */
      void qa_orbit_aux::test_jd() {
        // Compare computed Julian dates with some known dates from
        // From https://celestrak.com/columns/v02n02/
        
        
        /* First in canonical form */
        
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Julian date for the 1.10.1995 00:00:00 UTC does not match the expected value 2449991.5",
          orbits::getJD(1995,10, 1, 0, 0, 0.0),
          2449991.5,
          1e-5
        );
        
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Julian date for the 1.10.1995 09:00:00 UTC does not match the expected value 2449991.875",
          orbits::getJD(1995,10, 1, 9, 0, 0.0),
          2449991.875,
          1e-5
        );
        
        
        /* Then in array form (as is being used by the generated test data later on */
        
        std::array<uint16_t, 6> t1 = {1995u,10u, 1u, 0u, 0u, 0u};
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Julian date for the 1.10.1995 00:00:00 UTC does not match the expected value 2449991.5",
          orbits::getJD(t1),
          2449991.5,
          1e-5
        );
        
        std::array<uint16_t, 6> t2 = {1995u,10u, 1u, 9u, 0u, 0u};
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Julian date for the 1.10.1995 09:00:00 UTC does not match the expected value 2449991.875",
          orbits::getJD(t2),
          2449991.875,
          1e-5
        );
      }  // qa_orbit_aux::test_jd()
      
      
      /*
       * Test computation of Greenwich sidereal time (thetaG)
       */
      void qa_orbit_aux::test_thetag() {
        // Known values from https://celestrak.com/columns/v02n02/
        
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Computed Greenwich sidereal time does not match the expected value 2.524218",
          orbits::thetaG(2449991.875),
          2.524218,
          1e-5
        );
      }  // qa_orbit_aux::test_thetag()
      
      
      /*
       * Test conversion from latitude/longitude/altitude to ECI coordinates
       */
      void qa_orbit_aux::test_lla2eci() {
        // Known values from https://celestrak.com/columns/v02n02/
        
        
        /* Prepare input variables */
        
        // Position on Earth
        std::array<double, 3> lla({40.0, -75.0, 0.0});  // deg N, deg E, m
  
        // convert units to rad, rad, km
        constexpr double d2r = M_PI/180.0;
        std::array<double, 3> lla_scaled({lla[0]*d2r, lla[1]*d2r, lla[2]/1e3});
        
        // time instance
        const double jd0 = 2449991.875;
        std::vector<double> t_since{0.0};
        
        
        /* compute ECI position */
        std::vector< std::array<double,3> > pos = orbits::getECIPos(lla_scaled, jd0, t_since);
        
        
        /* Test results */
        
        // Check that exactly one position was computed
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
          "Computed position vector returned more than one time instance",
          pos.size(),
          1lu
        );
        
        // Check X
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Computed X-coordinate does not match expected value.",
          pos[0][0],
          1703.2950,
          1e-2
        );
        
        // Check Y
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Computed Y-coordinate does not match expected value.",
          pos[0][1],
          4586.6500,
          1e-2
        );
        
        // Check Z
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
          "Computed Z-coordinate does not match expected value.",
          pos[0][2],
          4077.9840,
          1e-2
        );
      }  // qa_orbit_aux::test_lla2eci
      
      
      /*
       * Test range computation
       */
      void qa_orbit_aux::test_range() {
        // Test computed ranges against known values from PyEphem
        //
        // The script to generate the testdata file qa_orbit_aux_testdata.h
        // is located in apps/generate_orbit_testdata.py
        
        
        /* Compute input variables */
        
        // Number of time instances in testdata set
        const size_t NUM_T = orbits::testdata::times.size();
  
        // Start in Julian Date
        const double jd0 = orbits::getJD(orbits::testdata::times[0]);
        
        // Compute timestamps relative to start
        std::vector<double> t_since(NUM_T);
        for (size_t i=0; i<NUM_T; i++) {
          const double tmp_jd = orbits::getJD(orbits::testdata::times[i]);
          t_since[i] = (tmp_jd-jd0)*(24.0*60.0);  // in minutes
        }
        
        
        /* Compute ranges */
        
        orbits::range r(
                        orbits::testdata::TLE_LINE1,
                        orbits::testdata::TLE_LINE2,
                        orbits::testdata::obs_pos
                       );
        r.setStartTime(orbits::testdata::times[0]);
        std::vector<double> ranges = r.computeAt(t_since);
        
        
        /* Verify results */
        
        // Check that there is the same amount of timestamps and solutions in
        // the test data set
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
          "Number of given timestamps and known ranges is not consistent.",
          NUM_T,
          orbits::testdata::range_km.size()
        );
        
        // Check that number of computed and known ranges is consistent
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
          "Number of computed ranges is not consistent with the given test data set.",
          orbits::testdata::range_km.size(),
          ranges.size()
        );
        
        for (size_t i=0; i<NUM_T; i++) {
          CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
            "Computed range does not match expected value.",
            ranges[i],
            orbits::testdata::range_km[i],
            0.5
          );
        }
      }  // qa_orbit_aux::test_range()
      
  } /* namespace ccsds */
} /* namespace gr */
