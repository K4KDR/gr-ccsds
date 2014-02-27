#ifndef INCLUDED_CCSDS_TRUNK_TAIL_H
#define INCLUDED_CCSDS_TRUNK_TAIL_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <cstdio>


class ccsds_trunk_tail;

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
typedef boost::shared_ptr<ccsds_trunk_tail> ccsds_trunk_tail_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_trunk_tail
 *
 *  Create an instance of ccsds_trunk_tail and return it's shared_ptr.
 *
 *  \param trunk_len Number of softbits to trunkate
 *
 *  \param frame_len Length of a frame (without ASM) in bytes.
 *  \return Shared pointer to the created block
 */
CCSDS_API ccsds_trunk_tail_sptr ccsds_make_trunk_tail(const unsigned int trunk_len, const unsigned int frame_len);

/*!
 *  \brief Takes a frame and trunkates a number of bits from the end.
 *
 *  \ingroup telemetry
 *
 *  The block has an input message port named "in" at which new frames can
 *  arrive. The message is trunkated then queued in the output message port named "out".
 */
class CCSDS_API ccsds_trunk_tail : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_trunk_tail_sptr ccsds_make_trunk_tail(const unsigned int trunk_len, const unsigned int frame_len);

	/*!
	 *  \brief Public constructor of the add ASM block.
	 *
 	 *  \param trunk_len Number of softbits to trunkate
	 *
	 *  \param frame_len Length of a frame (without ASM) in bytes.
	 */
	ccsds_trunk_tail(const unsigned int trunk_len, const unsigned int frame_len);
	
	/*! \brief Number of softbits to trunkate
	 *
	 *  \sa d_TRUNK_LEN
	 */
	const unsigned int d_TRUNK_LEN;

	/*! \brief Length of a frame (without ASM) in bytes. */
	const unsigned int d_FRAME_LEN;

	bool d_stop;

	/*! \brief Asynchronous work function which is processing the queues. */
	void process_frame(pmt::pmt_t msg);

public:
	/*! \brief Public deconstructor of the AR */	
	~ccsds_trunk_tail ();  // public destructor


	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);

};
#endif /* INCLUDED_CCSDS_TRUNK_TAIL_H */
