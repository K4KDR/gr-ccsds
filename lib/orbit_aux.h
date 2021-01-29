#ifndef __doppler_correction_aux_
#define __doppler_correction_aux_

#include <array>
#include <cstdint>
#include <string>
#include <vector>

//#include "SGP4/sgp4ext.h"
#include "sgp4/sgp4unit.h"

namespace gr {
  namespace ccsds {
    namespace orbits {
      double getJD(int year4, int month, int day, int hour, int minute, double second);
      double getJD(std::array<uint16_t, 6> timearray);
      
      std::vector< std::array<double,3> > getECIPos(std::array<double,3> lla_scaled, double jd0, std::vector<double> t_since_min);
      
      double thetaG(double jd);
      
      class range {
      private:
        double t_since0;
        double jd0;
        bool started;
        std::array<double, 3> lla_scaled;
        elsetrec sat_struct;
        
      public:
        range(std::string tle1, std::string tle2, std::array<double,3> lla);
        
        void setStartTimeNow();
        void setStartTime(int year4, int month, int day, int hour, int minute, double second);
        void setStartTime(std::array<uint16_t, 6> timearray);
        
        std::vector<double> computeAt(std::vector<double> t_since_start_min);
        
        double getJD0() const;
      };  // class range

    }  // namespace orbits
  }  // namespace ccsds
}  // namespace gr

#endif  // __doppler_correction_aux_
