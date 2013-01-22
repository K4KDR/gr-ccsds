#ifndef INCLUDED_CCSDS_TICP_FRAME_SOURCE_H
#define INCLUDED_CCSDS_TICP_FRAME_SOURCE_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>
#include <ticp/TicpClient.hpp>
// #include <boost/thread.hpp>

/*! \brief If set to one, dump data to debug file */
#define TICP_FRAME_SOURCE_DEBUG 0

/*! \brief If defined the work function stops after sourcing at least the defined
 *	number of bytes.
 */
#define PROFILE_NUM_SAMPS 10000u

class ccsds_ticp_frame_source_b;

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
typedef boost::shared_ptr<ccsds_ticp_frame_source_b> ccsds_ticp_frame_source_b_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_ticp_frame_source_b
 *
 *  Create an instance of ccsds_ticp_frame_source_b and return it's shared_ptr.
 *
 *  \param hostname Hostname of the TicpServer from which to get the data.
 *  \param port Port of the TicpServer
 *  \param frame_length Length of a frame in bytes.
 *  \return Shared pointer to the created AR block
 */
CCSDS_API ccsds_ticp_frame_source_b_sptr ccsds_make_ticp_frame_source_b(std::string hostname, unsigned int port, const unsigned int frame_length);

/*! \brief Reads frames from a TicpServer and outputs it as a stream. */
class CCSDS_API ccsds_ticp_frame_source_b : public gr_block, public TicpClient
{
private:
	friend CCSDS_API ccsds_ticp_frame_source_b_sptr ccsds_make_ticp_frame_source_b(std::string hostname, unsigned int port, const unsigned int frame_length);

	/*!
	 * \brief Private constructor of the TICP frame source
	 *
	 *  \param hostname Hostname of the TicpServer from which to get the data.
	 *  \param port Port of the TicpServer
	 *  \param frame_length Length of a frame in bytes.
	 */
	ccsds_ticp_frame_source_b(std::string hostname, unsigned int port, const unsigned int frame_length);
	
	/*! \brief Length of a frame in bytes. */
	const unsigned int d_FRAME_LEN;

	const unsigned int d_BUFFER_SIZE;

	/*! \brief Buffer to store frames in */
	unsigned char *d_frame_buffer;

	/*! \brief Number of frames in buffer */
	unsigned int d_frames_buffered;

	/*! \brief Number of frames that are requested from the TICP server */
	unsigned int d_frames_requested;

	/*! \brief Mutex to synchronize the access to the buffer */
	boost::mutex d_mutex;

	/*! \brief Flag to indicate that the block is stopped and that all
	 *	requests should be droped.
	 */
	bool d_stop;

	#if TICP_FRAME_SOURCE_DEBUG == 1
		/*! \brief File pointer for debugging. */
		FILE *dbg_file;

		/*! \brief Counter for debugging. */
		unsigned int dbg_count;
	#endif

	#ifdef PROFILE_NUM_SAMPS
		unsigned long profile_count;
	#endif

	/*! \brief Function to call when there is a new frame. */
	boost::function<void(vector< unsigned char > const &)> d_ticp_add_function;

	/*! \brief Waits for lock and adds frame to the buffer once locked.
	 *
	 *  \param frame Frame to buffer. Must contain exactly d_FRAME_LEN
	 *	elements
	 */
	void addFrame(const std::vector< unsigned char > &frame);

public:
	/*! \brief Public deconstructor of the ticp frame source */	
	~ccsds_ticp_frame_source_b ();  // public destructor

	void forecast(int noutput_items,gr_vector_int &ninput_items_required);
	
	bool stop( void );

	int  general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items);};

#endif /* INCLUDED_CCSDS_TICP_FRAME_SOURCE_H */
