#ifndef INCLUDED_CCSDS_MPSK_PREAMBLE_CC_H
#define INCLUDED_CCSDS_MPSK_PREAMBLE_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <gr_complex.h>

class ccsds_mpsk_preamble_cc;

typedef boost::shared_ptr<ccsds_mpsk_preamble_cc> ccsds_mpsk_preamble_cc_sptr;

/*! \brief Create a M-PSK preamble block and return it's shared pointer.
 *
 *  \param num_symbols Number of preamble symbols.
 *  \return Shared pointer of the M-PSK preamble block.
 */
CCSDS_API ccsds_mpsk_preamble_cc_sptr ccsds_make_mpsk_preamble_cc (const unsigned int num_symbols);

/*! \brief Append a M-PSK preamble before the input stream. 
 *  \ingroup sender
 *
 *  \param num Number of preamble symbols.
 *
 *  Outputs \c num preamble symbols, before the input stream is copied to
 *  the output stream. This can be used to let the receiver synchronize before
 *  sending frames. This way even the first transmitted frame can be received
 *  and decoded.
 *
 *  The preable consists of alternating BPSK symbols (starting with 1+0j). BPSK
 *  symbols are also part of any other M-PSK constellation where M is a power of
 *  two and thus works for all M-PSK receivers.
 */
class CCSDS_API ccsds_mpsk_preamble_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_mpsk_preamble_cc_sptr ccsds_make_mpsk_preamble_cc (const unsigned int num_symbols);

	/*! \brief Private constructor for the M-PSK preamble block.
	 *
	 *  \param num_symbols Number of preamble symbols.
	 */
	ccsds_mpsk_preamble_cc (const unsigned int num_symbols);   // private constructor

	/*! \brief Storage for the number of preable symbols to generate. */
	const unsigned int d_NUM_SYMBOLS;

	/*! \brief Counter for the number of symbols already generated. */
	unsigned int d_preamble_count;

	/*! \brief Buffer for the two preamble symbols. */
	gr_complex d_preamble_syms[2];
public:
	/*! \brief Public deconstructor. */
	~ccsds_mpsk_preamble_cc ();  // public destructor

	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

	int general_work (int                     noutput_items,
			 gr_vector_int 		     &ninput_items,
                         gr_vector_const_void_star   &input_items,
                         gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_PREAMBLE_CC_H */
