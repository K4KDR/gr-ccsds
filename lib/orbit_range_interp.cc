#include "orbit_range_interp.h"
#include <cassert>

namespace gr {
  namespace ccsds {
    namespace orbits {
      
      RangeInterpolator::RangeInterpolator(std::string tle1,
                                           std::string tle2,
                                           std::array<double,3> lla,
                                           double samp_rate_out,
                                           double compute_rate)
          : k_Tc_min(1.0/(compute_rate*60.0)),
            k_TsPerTc(compute_rate/samp_rate_out),
            _range_sgp4(tle1, tle2, lla),
            _i(0u),
            _init(false) {
      }
      
      void RangeInterpolator::setStartTimeNow() {
        _range_sgp4.setStartTimeNow();
        init();
      }
      void RangeInterpolator::setStartTime(int year4, int month, int day, int hour, int minute, double second) {
        _range_sgp4.setStartTime(year4, month, day, hour, minute, second);
        init();
      }
      
      void RangeInterpolator::init() {
        _j = 0u;
        std::vector<double> r = _range_sgp4.computeAt({0.0, k_Tc_min});
        _range0 = r[0];
        _range_left = 0.0;
        _range_right = r[1]-r[0];
        _m = _range_right-_range_left;
        _init = true;
      }
      
      void RangeInterpolator::update() {
        _range_left = _range_right;
        _j++;
        _range_right = getRangeAt(static_cast<double>(_j+1u)*k_Tc_min)-_range0;
        _m = _range_right-_range_left;
      }
      
      double RangeInterpolator::getRangeAt(double t_since) {
        std::vector<double> r = _range_sgp4.computeAt({t_since});
        assert(r.size() == 1u);
        return r[0];
      }
      
      void RangeInterpolator::computeDelta(double *results, size_t num_out) {
        assert(_init);
        double _t;
        double _mu;
        for(size_t i=0; i<num_out; i++) {
          _t = static_cast<double>(i+_i);
          _mu = _t*k_TsPerTc-static_cast<double>(_j);
          
          if(_mu >= 1.0) {
            update();
            _mu -= 1.0;
          }
          
          results[i] = _range_left+_m*_mu;
          //printf("i=%lu, mu = %lf, left=%lf, right=%lf, out=%lf\n", i, _mu, _range_left, _range_right, results[i]);
        }
        
        _i+=num_out;
      }

    }  // namespace orbits
  }  // namespace ccsds
}  // namespace gr
