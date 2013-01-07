#ifndef INCLUDED_CCSDS_MPSK_MOD_BC_H
#define INCLUDED_CCSDS_MPSK_MOD_BC_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_complex.h>

class ccsds_mpsk_mod_bc;

typedef boost::shared_ptr<ccsds_mpsk_mod_bc> ccsds_mpsk_mod_bc_sptr;

/*! \brief Create a M-PSK modulator and return it's shared pointer.
 *
 *  \param M Modulation order.
 *  \return Shared pointer of the generated M-PSK modulator.
 */
CCSDS_API ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc (unsigned int M);

/*! \brief M-PSK modulator. 
 *  \ingroup modulation
 *
 *  On construction a constellation array is generated. This constellation is
 *  later used to map the individual data bits to symbols.
 */
class CCSDS_API ccsds_mpsk_mod_bc : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc (unsigned int M);

	/*! \brief Private constructor for the M-PSK modulator.
	 *
	 *  \param M Modulation order.
	 */
	ccsds_mpsk_mod_bc (unsigned int M);   // private constructor

	/*! \brief Storage for the modulation order. */
	unsigned int d_M;

	/*! \brief Storage for precomputed constellation. */
	gr_complex* d_constellation;

public:
	/*! \brief Public deconstructor. */
	~ccsds_mpsk_mod_bc ();  // public destructor

	int work (  int                         noutput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_MOD_BC_H */
