#ifndef INCLUDED_CCSDS_RS_DECODE_H
#define INCLUDED_CCSDS_RS_DECODE_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string> // required by fec

extern "C" {
#include <fec.h>
}

/*! \brief Verbosity level: Do not output */
#define CCSDS_RS_DECODE_OUTPUT_NONE 0

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_RS_DECODE_OUTPUT_FRAMEERR 1

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_RS_DECODE_OUTPUT_FRAMEINFO 2

/*! \brief Verbosity level: Do output debug information */
#define CCSDS_RS_DECODE_OUTPUT_DEBUG 3

/*! \brief Level of verbosity of this block.
 *
 *  \sa #CCSDS_RS_DECODE_OUTPUT_NONE
 *  \sa #CCSDS_RS_DECODE_OUTPUT_DEBUG
 */
#define CCSDS_RS_DECODE_VERBOSITY_LEVEL CCSDS_RS_DECODE_OUTPUT_NONE

class ccsds_rs_decode;

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
typedef boost::shared_ptr<ccsds_rs_decode> ccsds_rs_decode_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_rs_decode
 *
 *  Create an instance of ccsds_rs_decode and return it's shared_ptr.
 *
 *  \param I depth of interleaving.
 *  \return Shared pointer to the created block
 */
CCSDS_API ccsds_rs_decode_sptr ccsds_make_rs_decode(const unsigned int I);

/*!
 *  \brief Takes the transmitted codeblock and generates the frame out of it, or
 *	drops it, if recovery is not possible.
 *
 *  \ingroup channelcoding
 *
 *  The block has an input message port named "in" at which codeblocks of length
 *  \c 255*I bytes are received. On reception the data codeblock is interleaved
 *  and decoded. The generated frame consisting of \c 223*I bytes is then
 *  queued in the output message port named "out" or dropped if it can't be
 *  decoded.
 */
class CCSDS_API ccsds_rs_decode : public gr_block
{
private:
	friend CCSDS_API ccsds_rs_decode_sptr ccsds_make_rs_decode(const unsigned int I);

	/*!
	 *  \brief Public constructor of the add ASM block.
	 *
	 *  \param I Depth of interleaving
	 */
	ccsds_rs_decode(const unsigned int I);
	
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

	/*! \brief Number of bytes per codeblock. Value=(k+2E)*I */
	const unsigned int d_IN_LEN;

	/*  Storage for the first interleaving stage */
	unsigned char *d_buf_in;

	/*  Storage for the second interleaving stage */
	unsigned char *d_buf_data;

	/*  Storage for the output frames symbols */
	unsigned char *d_buf_out;

	#if CCSDS_RS_DECODE_VERBOSITY_LEVEL >= CCSDS_RS_DECODE_OUTPUT_DEBUG
		/*! \brief File pointer for debugging. */
		FILE *dbg_file_in;

		/*! \brief File pointer for debugging. */
		FILE *dbg_file_in_valid;

		/*! \brief File pointer for debugging. */
		FILE *dbg_file_out;
	#endif

	/*! \brief Asynchronous work function which is processing the queues. */
	void process_frame(pmt::pmt_t msg);

public:
	/*! \brief Public deconstructor of the AR */	
	~ccsds_rs_decode ();  // public destructor

};
#endif /* INCLUDED_CCSDS_RS_DECODE_H */
