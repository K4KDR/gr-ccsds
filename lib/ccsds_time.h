#ifndef INCLUDED_CCSDS_TIME_H
#define INCLUDED_CCSDS_TIME_H

#include <time.h>

#define G 1000000000
#define M 1000000
#define K 1000

namespace gr {
  namespace ccsds {
    
    /*! \brief Returns the difference between the two time instances in nanoseconds.
     *
     *  Time a needs to be later than b.
     */
    inline unsigned long time_diff_nano(timeval *a, timeval *b) {
    	return (a->tv_sec-b->tv_sec)*G+(a->tv_usec-b->tv_usec)*K;
    }
    
    inline double time_diff_seconds(timeval *a, timeval *b) {
    	return (a->tv_sec-b->tv_sec)+(a->tv_usec-b->tv_usec)/1e6;
    }
    
    inline void time_add_usecs(timeval *t, unsigned long diff) {
    	const unsigned long usec_sum = t->tv_usec+diff;
    
    	t->tv_sec += usec_sum / M;
    	t->tv_usec = usec_sum % M;
    
    	return ;
    }
    
    inline void time_diff_to_timespec(timespec *spec, unsigned long diff) {
    	spec->tv_sec  = diff / G;
    	spec->tv_nsec = diff % G;
    
    	return;
    }
    
    inline unsigned long time_to_nano(timespec *spec) {
    	return spec->tv_sec*G+spec->tv_nsec;
    }
    
    inline unsigned long time_to_micro(timeval *t) {
    	return t->tv_sec*M+t->tv_usec;
    }
    
    inline void time_print_timeval(timeval *t) {
    	printf("timeval(sec=%ld, usec=%ld)",t->tv_sec,t->tv_usec);
    	return;
    }
    
    inline void time_print_timespec(timespec *t) {
    	printf("timeval(sec=%ld, nsec=%ld)",t->tv_sec,t->tv_nsec);
    	return;
    }
    
    inline void time_reset_timespec(timespec *spec) {
    	spec->tv_sec  = 0;
    	spec->tv_nsec = 0;
    	return;
    }

  } // namespace ccsds
} // namespace gr
    
#endif /* INCLUDED_CCSDS_TIME_H */
