#ifndef __orbit_range_interp_
#define __orbit_range_interp_

#include "orbit_aux.h"

namespace gr {
  namespace ccsds {
    namespace orbits {
      
      class RangeInterpolator {
      private:
        const double k_Tc_min;
        const double k_TsPerTc;
        range _range_sgp4;
        
        size_t _i;
        size_t _j;
        bool _init;
        
        double _range_left;
        double _range_right;
        double _range0;
        double _m;
        
        void update();
        void init();
        
      public:
        RangeInterpolator(std::string tle1, std::string tle2, std::array<double,3> lla, double samp_rate_out, double compute_rate);
        
        void setStartTimeNow();
        void setStartTime(int year4, int month, int day, int hour, int minute, double second);
        
        double getRangeAt(double t_since);
        
        void computeDelta(double *results, size_t num_out);
      };  // class RangeInterpolator
      
    }  // namespace orbits
  }  // namespace ccsds
}  // namespace gr

#endif  // __orbit_range_interp_
