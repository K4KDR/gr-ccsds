#ifndef INCLUDED_CCSDS_TICP_FRAME_SOURCE_H
#define INCLUDED_CCSDS_TICP_FRAME_SOURCE_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include <ticp/TicpClient.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <asynchronous_start.hpp>
#include <ccsds_time.h>

/*! \brief If set to one, dump data to debug file */
// #define TICP_FRAME_SOURCE_DEBUG 1

/*! \brief If defined the work function stops after sourcing at least the defined
 *	number of bytes.
 */
// #define PROFILE_NUM_SAMPS 10000000u

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
 *  \param data_type Type of the dataset which's data should be used for the
 *	actuall output data.
 *  \param map_names Vector of names that should be used for the PDU header
 *	metadata identifier. Can be empty. Must have the same length as
 *	map_types. All elements in this vector must be unique.
 *  \param map_types Vector of datatypes that should be extracted into the PDU
 *	header. Can be empty. Must have the same length as map_names. All
 *	elements in this vector must be unique.
 *  \param frame_rate Rate in frames per second at which new frames should be
 *	requested from the TICP server. This provides an upper bound for the
 *	frame rate, based on the data received from the TICP server the actual
 *	rate can be lower. If set to 0.0, no rate limitation is applied and the
 *	frames are read in and send out as fast as possible.
 *  \return Shared pointer to the created AR block
 */
CCSDS_API ccsds_ticp_frame_source_sptr ccsds_make_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types, const double frame_rate=0.0);

/*! \brief Reads frames from a TicpServer and outputs it as a stream.
 *
 *  \ingroup ticp
 */
class CCSDS_API ccsds_ticp_frame_source : public gr_block
{
private:
	friend CCSDS_API ccsds_ticp_frame_source_sptr ccsds_make_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types, const double frame_rate);

	/*!
	 * \brief Private constructor of the TICP frame source
	 *
	 *  \param hostname Hostname of the TicpServer from which to get the data.
	 *  \param port Port of the TicpServer
	 *  \param frame_length Length of a frame in bytes.
	 *	should be copied to.
	 *  \param data_type Type of the dataset which's data should be used for the
	 *	actuall output data.
	 *  \param map_names Vector of names that should be used for the PDU header
	 *	metadata identifier. Can be empty. Must have the same length as
	 *	map_types. All elements in this vector must be unique.
	 *  \param map_types Vector of datatypes that should be extracted into the PDU
	 *	header. Can be empty. Must have the same length as map_names. All
	 *	elements in this vector must be unique.
	 *  \param frame_rate Rate in frames per second at which new frames should be
	 *	requested from the TICP server. This provides an upper bound for the
	 *	frame rate, based on the data received from the TICP server the actual
	 *	rate can be lower. If set to 0.0, no rate limitation is applied and the
	 *	frames are read in and send out as fast as possible.
	 */
	ccsds_ticp_frame_source(std::string hostname, unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types, const double frame_rate=0.0);
	
	/*! \brief Length of a frame in bytes. */
	const unsigned int d_FRAME_LEN;

	/*! \brief Type of the dataset's data that should be used as PDU payload. */
	const uint8_t d_DATA_TYPE;

	/*! \brief Maximum frame rate in frames per second. */
	const double d_FRAME_RATE;

	/*! \brief Flag whether or not to apply rate control. */
	const bool d_RATE_CONTROL;

	/*! \brief Time in nanoseconds a frame needs for transmission */
	const unsigned long d_TIME_PER_FRAME;

	/*! \brief Shared pointer to the TICP Client */
	boost::shared_ptr< ticp::Client<ticp::data_v2_t> > d_ticp_sptr;

	/*! \brief Map to store the metadata mapping in. */
	std::map<uint8_t, std::string> d_metadata_map;

	/*! \brief Time instance to which the frame counter refers. */
	timeval d_rate_time_ref;

	/*!
	 *  \brief Number of frames that have been transmitted since
	 *	\c d_rate_time_ref
	 */
	unsigned int d_rate_frame_count;

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

		/*! \brief File pointer for debugging. */
		FILE *dbg_file_timing;

		/*! \brief Counter for debugging. */
		unsigned int dbg_count;

		/* \brief Time value of block start. */
		timeval dbg_time_start;
	#endif

	unsigned long d_frame_count;

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
};
#endif /* INCLUDED_CCSDS_TICP_FRAME_SOURCE_H */
