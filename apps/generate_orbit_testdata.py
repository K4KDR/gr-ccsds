#!/usr/bin/python3

"""
Compute a number of range values with PyEphem and write the results into a
header file to be used in the gr-ccsds C++ unittests.
"""

import datetime
import os
import sys

import ephem

if __name__ == '__main__':
  namespace_name = 'testdata'
  
  gr_ccsds_app_dir = os.path.dirname(sys.argv[0])
  testdata_filename = 'qa_orbit_aux_testdata.h'
  
  file_out = os.path.join(gr_ccsds_app_dir, '..', 'lib', testdata_filename)
  
  LINE1 = '1 43780U 18099Y   19033.19530161  .00000526  00000-0  53415-4 0  9996'
  LINE2 = '2 43780  97.7595 106.7193 0014861  70.4175 289.8649 14.94903261  9012'
  
  obs_pos = (48.266278, 11.668361, 545.0)
  
  gs = ephem.Observer()
  gs.lat = str(obs_pos[0]) # as str to enforce degree parsing
  gs.lon = str(obs_pos[1]) # as str to enforce degree parsing
  gs.elevation = float(obs_pos[2])
  
  satellite = ephem.readtle('TESTSAT', LINE1, LINE2)
  
  time_start = datetime.datetime(2019, 2, 2, 0, 0, 0)
  time_end   = datetime.datetime(2019, 2, 3,23,59,59)
  time_delta = datetime.timedelta(seconds=300)
  
  times = []
  _t = time_start
  while _t <= time_end:
    times.append(_t)
    _t += time_delta
  
  ranges = []
  velocities = []
  dopplers = []
  for t in times:
    gs.date = ephem.Date(t)
    satellite.compute(gs)
    
    _range = satellite.range/1e3
    _velocity = satellite.range_velocity/1e3
    _doppler = - 100e6 / 299792458.0 * satellite.range_velocity
    #print('At %s UTC: range=%9.3f km, range_rate=%6.2f km/s, doppler@100M=%+8.1f Hz' % (t, _range, _velocity, _doppler))
    
    ranges.append(_range)
    velocities.append(_velocity)
    dopplers.append(_doppler)

  with open(file_out, 'w') as f:
    
    print('#ifndef INCLUDED_CCSDS_ORBITS_TESTDATA', file=f)
    print('#define INCLUDED_CCSDS_ORBITS_TESTDATA', file=f)
    print('', file=f)
    
    print('#include <array>', file=f)
    print('#include <vector>', file=f)
    print('', file=f)
    
    print('namespace gr {', file=f)
    print('  namespace ccsds {', file=f)
    print('    namespace orbits {', file=f)
    print('      namespace %s {' % namespace_name, file=f)
    print('', file=f)
    
    print('        const char *TLE_LINE1 = "%s";' % LINE1, file=f)
    print('        const char *TLE_LINE2 = "%s";' % LINE2, file=f)
    print('', file=f)
    
    print('        std::array<double, 3> obs_pos = {', file=f)
    print('                                         % +13.6f, // degree North' % obs_pos[0], file=f)
    print('                                         % +13.6f, // degree East' % obs_pos[1], file=f)
    print('                                         % +13.6f  // metre above ellipsoid' % obs_pos[2], file=f)
    print('                                        };', file=f)
    print('', file=f)
    
    print('        std::vector< std::array<uint16_t, 6> > times = {', file=f)
    print('        //    year, month, day, hour, min, sec UTC', file=f)
    for t in times:
      print('            {%4du,   %2du, %2du,  %2du, %2du, %2du},' % (t.year, t.month, t.day, t.hour, t.minute, t.second), file=f)
    print('        };', file=f)
    print('', file=f)
    
    print('        std::vector<double> range_km = {', file=f)
    for r in ranges:
      print('            %11.4f,' % r, file=f)
    print('        };', file=f)
    print('', file=f)
    
    print('      }  // namespace %s' % namespace_name, file=f)
    print('    }  // namespace orbits', file=f)
    print('  }  // namespace ccsds', file=f)
    print('}  // namespace gr', file=f)
    print('', file=f)
    
    print('#endif  // INCLUDED_CCSDS_ORBITS_TESTDATA', file=f)

  print('%d time values written to %s' % (len(times), file_out))
