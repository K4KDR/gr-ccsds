#ifndef INCLUDED_CCSDS_ADD_ASM_H
#define INCLUDED_CCSDS_ADD_ASM_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <string>


class ccsds_add_asm;

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
typedef boost::shared_ptr<ccsds_add_asm> ccsds_add_asm_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_add_asm
 *
 *  Create an instance of ccsds_add_asm and return it's shared_ptr.
 *
 *  \param ASM String containing the attached sync marker to look for in
 *	hexadecimal representation. Must be of even length.
 *  \param frame_len Length of a frame (without ASM) in bytes.
 *  \return Shared pointer to the created block
 */
CCSDS_API ccsds_add_asm_sptr ccsds_make_add_asm(std::string ASM, const unsigned int frame_len);

/*!
 *  \brief Takes a frame and adds a sync marker in front of it.
 *
 *  \ingroup telemetry
 *
 *  The block has an input message port named "in" at which new frames can
 *  arrive. On arrival a new message is created consisting of the ASM sequence
 *  followed by the data of the incomming message. This new message is then
 *  queued in the output message port named "out".
 */
class CCSDS_API ccsds_add_asm : public gr_block
{
private:
	friend CCSDS_API ccsds_add_asm_sptr ccsds_make_add_asm(std::string ASM, const unsigned int frame_len);

	/*!
	 *  \brief Public constructor of the add ASM block.
	 *
	 *  \param ASM String containing the attached sync marker to look for in
	 *	hexadecimal representation. Must be of even length.
	 *  \param frame_len Length of a frame (without ASM) in bytes.
	 */
	ccsds_add_asm(std::string ASM, const unsigned int frame_len);
	
	/*! \brief Length of ASM in bytes 
	 *
	 *  \sa d_ASM
	 */
	const unsigned int d_ASM_LEN;

	/*! \brief Length of a frame (without ASM) in bytes. */
	const unsigned int d_FRAME_LEN;

	/*  Storage for the attached sync marker
	 *
	 *  \sa d_ASM_LEN
	 */
	unsigned char *d_ASM;

	/*! \brief Asynchronous work function which is processing the queues. */
	void process_frame(pmt::pmt_t msg);

public:
	/*! \brief Public deconstructor of the AR */	
	~ccsds_add_asm ();  // public destructor

};
#endif /* INCLUDED_CCSDS_ADD_ASM_H */
