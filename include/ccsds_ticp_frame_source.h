#ifndef INCLUDED_CCSDS_TICP_FRAME_SOURCE_H
#define INCLUDED_CCSDS_TICP_FRAME_SOURCE_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include <boost/thread.hpp>
#include <ticp/TicpClient.hpp>
#include <asynchronous_start.hpp>

/*! \brief If set to one, dump data to debug file */
#define TICP_FRAME_SOURCE_DEBUG 1

/*! \brief If defined the work function stops after sourcing at least the defined
 *	number of bytes.
 */
// #define PROFILE_NUM_SAMPS 1000000u

class ccsds_ticp_frame_source;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<ccsds_ticp_frame_source> ccsds_ticp_frame_source_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_ticp_frame_source
 *
 *  Create an instance of ccsds_ticp_frame_source and return it's shared_ptr.
 *
 *  \param hostname Hostname of the TicpServer from which to get the data.
 *  \param port Port of the TicpServer
 *  \param frame_length Length of a frame in bytes.
 *	should be copied to.
 *  \return Shared pointer to the created AR block
 */
CCSDS_API ccsds_ticp_frame_source_sptr ccsds_make_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length);

/*! \brief Reads frames from a TicpServer and outputs it as a stream.
 *
 *  \ingroup ticp
 */
class CCSDS_API ccsds_ticp_frame_source : public gr_block, public TicpClient
{
private:
	friend CCSDS_API ccsds_ticp_frame_source_sptr ccsds_make_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length);

	/*!
	 * \brief Private constructor of the TICP frame source
	 *
	 *  \param hostname Hostname of the TicpServer from which to get the data.
	 *  \param port Port of the TicpServer
	 *  \param frame_length Length of a frame in bytes.
	 *	should be copied to.
	 */
	ccsds_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length);
	
	/*! \brief Length of a frame in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Mutex to lock access on \c d_stop between GNURadio and this
	 *	block's worker thread.
	 *
	 *  \sa d_stop
	 */
	boost::mutex d_mutex;

	/*! \brief Flag to indicate the worker thread to stop working.
	 *
	 *  \sa d_mutex
	 *
	 *  Access to this flag is controled by \c d_mutex.
	 *  Default is \a false, if \a true the worker thread will return.
	 */
	bool d_stop;

	/*! \brief Reference to the worker thread. */
	boost::shared_ptr<boost::thread> d_worker_thread;

	#if TICP_FRAME_SOURCE_DEBUG == 1
		/*! \brief File pointer for debugging. */
		FILE *dbg_file;

		/*! \brief Counter for debugging. */
		unsigned int dbg_count;
	#endif

	#ifdef PROFILE_NUM_SAMPS
		unsigned long profile_count;
	#endif
	
	/*! \brief workarround to ensure we only start transmitting, when the
	 *	flowgraph is started.
	 * \todo reove workarround
	 */
	asynchronous_start d_astart;

	/*! \brief Asynchronous work function which is processing the queues. */
	void asynchronous_work(void);

public:
	/*! \brief Public deconstructor of the ticp frame source */	
	~ccsds_ticp_frame_source ();  // public destructor

	bool start(void);
};
#endif /* INCLUDED_CCSDS_TICP_FRAME_SOURCE_H */
