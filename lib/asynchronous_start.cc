#include <boost/thread.hpp>
#include <asynchronous_start.hpp>

asynchronous_start::asynchronous_start(void) {}

bool asynchronous_start::wait_for_start(void) {
	boost::unique_lock<boost::mutex> lock(d_mutex);

	if(d_state == STOPPED) {
		return false;
	}

	while(d_state != RUNNING) {
		d_cond.wait(lock);
	}
	return true;
}

bool asynchronous_start::is_stopped(void) {
	boost::unique_lock<boost::mutex> lock(d_mutex);

	return (d_state == STOPPED);
}

void asynchronous_start::stop(void) {
	boost::unique_lock<boost::mutex> lock(d_mutex);

	d_state	= STOPPED;

	return;
}

void asynchronous_start::start(void) {
	d_mutex.lock();
	d_state	= RUNNING;
	d_mutex.unlock();

	d_cond.notify_all();

	return;
}

boost::condition_variable asynchronous_start::d_cond;
boost::mutex asynchronous_start::d_mutex;
asynchronous_start::STATE asynchronous_start::d_state = CONSTRUCTING;
