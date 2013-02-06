#ifndef INCLUDED_CCSDS_RS_ENCODE_H
#define INCLUDED_CCSDS_RS_ENCODE_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string> // required by fec

extern "C" {
#include <fec.h>
}

class ccsds_rs_encode;

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
typedef boost::shared_ptr<ccsds_rs_encode> ccsds_rs_encode_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_rs_encode
 *
 *  Create an instance of ccsds_rs_encode and return it's shared_ptr.
 *
 *  \param I depth of interleaving.
 *  \return Shared pointer to the created block
 */
CCSDS_API ccsds_rs_encode_sptr ccsds_make_rs_encode(const unsigned int I);

/*!
 *  \brief Takes I frames and generates the transmitted codeblock out of it.
 *
 *  \ingroup channelcoding
 *
 *  The block has an input message port named "in" at which frames of length
 *  \c 223*I bytes are received. On reception the frames are interleaved and
 *  encoded. The generated codeblock consisting of \c 255*I bytes is then
 *  queued in the output message port named "out".
 */
class CCSDS_API ccsds_rs_encode : public gr_block
{
private:
	friend CCSDS_API ccsds_rs_encode_sptr ccsds_make_rs_encode(const unsigned int I);

	/*!
	 *  \brief Public constructor of the add ASM block.
	 *
	 *  \param I Depth of interleaving
	 */
	ccsds_rs_encode(const unsigned int I);
	
	/*! \brief Depth of interleaving */
	const unsigned int d_I;

	/*! \brief Number of information symbols per codeword. Value=223 */
	const unsigned int d_k;

	/*! \brief Number of symbols per codeword. Value=255 */
	const unsigned int d_n;

	/*! \brief Number of parity check symbols per codeword. Value=32 */
	const unsigned int d_2E;

	/*! \brief Number of data bytes per codeblock. Value=k*I */
	const unsigned int d_DATA_LEN;

	/*! \brief Number of parity bytes per codeblock. Value=2E*I */
	const unsigned int d_PARITY_LEN;

	/*! \brief Number of bytes per codeblock. Value=(k+2E)*I */
	const unsigned int d_OUT_LEN;

	/*  Storage for the first interleaving stage */
	unsigned char *d_buf_data;

	/*  Storage for the parity check symbols */
	unsigned char *d_buf_parity;

	/*  Storage for the parity check symbols after 2nd interleaving step */
	unsigned char *d_buf_parity2;

	/*  Storage for the parity check symbols */
	unsigned char *d_buf_out;

	/*! \brief Asynchronous work function which is processing the queues. */
	void process_frame(pmt::pmt_t msg);

public:
	/*! \brief Public deconstructor of the AR */	
	~ccsds_rs_encode ();  // public destructor

};
#endif /* INCLUDED_CCSDS_RS_ENCODE_H */
