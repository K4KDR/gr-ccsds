#include "orbit_aux.h"

#include <cassert>

#include "sgp4/sgp4unit.h"
#include "sgp4/sgp4ext.h"
#include "sgp4/sgp4tle.h"

namespace {
  struct Vector3 {
    double x;
    double y;
    double z;

    constexpr Vector3() : x(0.0), y(0.0), z(0.0) {}
    constexpr Vector3(double v[3]) : x(v[0]), y(v[1]), z(v[2]) {}
    Vector3(std::array<double, 3> v) : x(v[0]), y(v[1]), z(v[2]) {}
    constexpr Vector3(double x, double y, double z) : x(x), y(y), z(z) {}
    
    constexpr Vector3 operator + (Vector3 const &other) const {
      return {this->x+other.x, this->y+other.y, this->z+other.z};
    }
    
    constexpr Vector3 operator - (Vector3 const &other) const {
      return {this->x-other.x, this->y-other.y, this->z-other.z};
    }
    constexpr Vector3 operator * (const double scalar) const {
      return {this->x*scalar, this->y*scalar, this->z*scalar};
    }
    
    double magnitude() const {
      return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
    }
    
    Vector3 normalise(Vector3 const &vec) const {
      return vec*(1.0/vec.magnitude());
    }
    
    constexpr double dotP(Vector3 const &other) const {
      return this->x*other.x + this->y*other.y + this->z*other.z;
    }
    
    void print(const char *prefix) {
      printf("%s[%lf, %lf, %lf] (length %lf)\n", prefix, this->x, this->y, this->z, this->magnitude());
    }
  };  // struct Vector3

}  // namespace


namespace gr {
  namespace ccsds {
    namespace orbits {
      
      double getJD(int year4, int month, int day,
                                 int hour, int minute, double second) {
        // wrapper around sgp4/gstime that returns jd instead of writing it
        double jd;
        jday(year4, month, day, hour, minute, second, jd);
        return jd;
      }
      
      double getJD(std::array<uint16_t, 6> timearray) {
        return getJD(
                      timearray[0],
                      timearray[1],
                      timearray[2],
                      timearray[3],
                      timearray[4],
                      static_cast<double>(timearray[5])
                    );
      }
      
      double thetaG(double jd) {
        // wrapper around sgp4/gstime
        return gstime(jd);
      }
      
      std::vector< std::array<double,3> > getECIPos(
          std::array<double,3> lla_scaled,
          double jd0, std::vector<double> t_since_min) {
    
        constexpr double a = 6378.135;  // in km according to WGS-72
        constexpr double f = 1.0/298.26;
        constexpr double min2day = 1.0/(24.0*60.0);
        
        const double sinLat = sin(lla_scaled[0]);
        const double C = 1.0/sqrt(1.0 + f*(f - 2.0)*sinLat*sinLat);
        const double oneminusf = (1.0 - f);
        const double S = oneminusf*oneminusf*C;
        const double aCcosLat = (a*C + lla_scaled[2])*cos(lla_scaled[0]);
        const double aSsinLat = (a*S + lla_scaled[2])*sinLat;
        
        const size_t NUM_TIMES = t_since_min.size();
        std::vector< std::array<double,3> > ret(NUM_TIMES);
        
        for (size_t i=0; i<NUM_TIMES; i++) {
          const double jd = jd0 + min2day*t_since_min[i];
          const double theta = thetaG(jd) + lla_scaled[1];
          ret[i] = {aCcosLat*cos(theta), aCcosLat*sin(theta), aSsinLat};
        }
        
        return ret;
      }
      

      range::range(std::string tle1, std::string tle2, std::array<double,3> lla)
          : lla_scaled({lla[0]*M_PI/180.0, lla[1]*M_PI/180.0, lla[2]/1e3}),
            started(false) {
        assert(lla[0] >=  -90.0);
        assert(lla[0] <=  +90.0);
        assert(lla[1] >= -180.0);
        assert(lla[1] <= +180.0);
        sgp4_init_from_tle(tle1.c_str(), tle2.c_str(), wgs72, this->sat_struct);
      }

      void range::setStartTimeNow() {
        time_t rawtime;
        struct tm * ptm;

        time ( &rawtime );

        ptm = gmtime ( &rawtime );
        
        this->setStartTime(ptm->tm_year+1900,
                          ptm->tm_mon+1,
                          ptm->tm_mday,
                          ptm->tm_hour,
                          ptm->tm_min,
                          static_cast<double>(ptm->tm_sec)
                          );
      }

      void range::setStartTime(int year4, int month, int day, int hour, int minute, double second) {
        this->jd0 = getJD(year4, month, day, hour, minute, second);
        this->t_since0 = (this->jd0 - sat_struct.jdsatepoch)*(24.0*60.0);
        this->started = true;
      }
      
      void range::setStartTime(std::array<uint16_t, 6> timearray) {
        this->jd0 = getJD(timearray);
        this->t_since0 = (this->jd0 - sat_struct.jdsatepoch)*(24.0*60.0);
        this->started = true;
      }

      std::vector<double> range::computeAt(std::vector<double> t_since_start_min) {
        assert(this->started);
        
        const size_t NUM_T = t_since_start_min.size();
        std::vector<double> ret(NUM_T);
        
        std::vector< std::array<double,3> > pos_rec = getECIPos(this->lla_scaled, this->jd0, t_since_start_min);
        
        double pos_sat[3];
        double vel_sat[3];
        
        for (size_t i=0; i<NUM_T; i++) {
          bool success = sgp4(wgs72, this->sat_struct, this->t_since0+t_since_start_min[i], pos_sat, vel_sat);
          
          if (!success) {
            printf("Error: sgp4 returned %d\n", this->sat_struct.error);
            assert(false);
          }
          ret[i] = (Vector3(pos_rec[i]) - Vector3(pos_sat)).magnitude();
        }
        
        return ret;
      }

      double range::getJD0() const {
        assert(this->started);
        return this->jd0;
      }

    }  // namespace orbits
  }  // namespace ccsds
}  // namespace gr
