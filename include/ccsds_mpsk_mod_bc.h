#ifndef INCLUDED_CCSDS_MPSK_MOD_BC_H
#define INCLUDED_CCSDS_MPSK_MOD_BC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <gr_complex.h>

class ccsds_mpsk_mod_bc;

typedef boost::shared_ptr<ccsds_mpsk_mod_bc> ccsds_mpsk_mod_bc_sptr;

CCSDS_API ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc (unsigned int M);
CCSDS_API ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc ();

class CCSDS_API ccsds_mpsk_mod_bc : public gr_block
{
private:
    friend CCSDS_API ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc (unsigned int M);
    friend CCSDS_API ccsds_mpsk_mod_bc_sptr ccsds_make_mpsk_mod_bc ();

    ccsds_mpsk_mod_bc (unsigned int M);   // private constructor

    unsigned int d_M;
    gr_complex* d_constellation;

public:
    ~ccsds_mpsk_mod_bc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_MOD_BC_H */
