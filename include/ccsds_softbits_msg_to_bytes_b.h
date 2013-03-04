#ifndef INCLUDED_CCSDS_SOFTBITS_MSG_TO_BYTES_B_H
#define INCLUDED_CCSDS_SOFTBITS_MSG_TO_BYTES_B_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <queue>

class ccsds_softbits_msg_to_bytes_b;

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
typedef boost::shared_ptr<ccsds_softbits_msg_to_bytes_b> ccsds_softbits_msg_to_bytes_b_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_softbits_msg_to_bytes_b
 *
 *  Create an instance of ccsds_softbits_msg_to_bytes_b and return it's shared_ptr.
 *
 *  \param dummy Useless dummy parameter that is completley ignored, but the
 *	GNURadio SWIG block magic will shadow the class, if the make function
 *	has no parameters.
 *  \return Shared pointer to the created TICP frame sink.
 */
CCSDS_API ccsds_softbits_msg_to_bytes_b_sptr ccsds_make_softbits_msg_to_bytes_b(bool dummy=false);

/*! \brief Reads frames of softbits from message input port "in" and outputs the
 *	corresponding hard decisions in a packed byte.
 */
class CCSDS_API ccsds_softbits_msg_to_bytes_b : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_softbits_msg_to_bytes_b_sptr ccsds_make_softbits_msg_to_bytes_b(bool dummy);

	/*! \brief Private constructor of the TICP frame sink */
	ccsds_softbits_msg_to_bytes_b(void);
	
	/*! \brief Queue in which to buffer the pointer to the blobs. */
	std::queue<unsigned char> d_queue;

	/*! \brief Flag to indicate EOF arrival. When true and the buffers are
	 *	empty, signal EOF.
	 */
	bool d_stop;

	/*! \brief Store incomming message into buffer. */
	void process_message(pmt::pmt_t msg_in);

public:
	/*! \brief Public deconstructor of the ticp frame sink */	
	~ccsds_softbits_msg_to_bytes_b ();  // public destructor

	int work (  int                         noutput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};
#endif /* INCLUDED_CCSDS_MSG_BLOB_SOURCE_B_H */
