#ifndef ASYNCRHONOUS_START_H
#define ASYNCRHONOUS_START_H

#include <boost/thread.hpp>

namespace gr {
  namespace ccsds {
    class asynchronous_start {
    public:
    	enum STATE { CONSTRUCTING, RUNNING, STOPPED};
    
    	asynchronous_start(void);
    	~asynchronous_start(void);
    
    	static bool wait_for_start(void);
    
    	static bool is_stopped(void);
    
    	static void stop(void);
    
    	static void start(void);

    private:
    	static boost::condition_variable d_cond;
    	static boost::mutex d_mutex;
    
    	static STATE d_state;
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* ASYNCRHONOUS_START_H */
