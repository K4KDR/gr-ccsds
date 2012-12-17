#ifndef INCLUDED_CCSDS_MPSK_DEMOD2_CB_H
#define INCLUDED_CCSDS_MPSK_DEMOD2_CB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_complex.h>

class ccsds_mpsk_demod2_cb;

typedef boost::shared_ptr<ccsds_mpsk_demod2_cb> ccsds_mpsk_demod2_cb_sptr;

CCSDS_API ccsds_mpsk_demod2_cb_sptr ccsds_make_mpsk_demod2_cb (unsigned int M);

class CCSDS_API ccsds_mpsk_demod2_cb : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_mpsk_demod2_cb_sptr ccsds_make_mpsk_demod2_cb (unsigned int M);

	ccsds_mpsk_demod2_cb (const unsigned int M);   // private constructor

	const unsigned int d_M;
	const float d_TWOPI;

	char *map;
	
public:
    ~ccsds_mpsk_demod2_cb ();  // public destructor
    int work (  int                         noutput_items,
                gr_vector_const_void_star   &input_items,
                gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_DEMOD2_CB_H */
