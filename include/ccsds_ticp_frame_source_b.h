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
// #define PROFILE_NUM_SAMPS 1000000u

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

/*! \brief Reads frames from a TicpServer and outputs it as a stream.
 *
 *  \ingroup ticp
 */
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
