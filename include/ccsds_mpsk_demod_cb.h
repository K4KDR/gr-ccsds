#ifndef INCLUDED_CCSDS_MPSK_DEMOD_CB_H
#define INCLUDED_CCSDS_MPSK_DEMOD_CB_H

#include <ccsds_api.h>
#include <gr_sync_block.h>
#include <gr_complex.h>

class ccsds_mpsk_demod_cb;

typedef boost::shared_ptr<ccsds_mpsk_demod_cb> ccsds_mpsk_demod_cb_sptr;

CCSDS_API ccsds_mpsk_demod_cb_sptr ccsds_make_mpsk_demod_cb (unsigned int M);

class CCSDS_API ccsds_mpsk_demod_cb : public gr_sync_block
{
private:
	friend CCSDS_API ccsds_mpsk_demod_cb_sptr ccsds_make_mpsk_demod_cb (unsigned int M);

	ccsds_mpsk_demod_cb (unsigned int M);   // private constructor

	unsigned int d_M;
	gr_complex* d_constellation;

	// debug counter to terminate after a fixed amount of samples (for profiling)
	unsigned int count;

	char detect_bpsk_symbol(gr_complex symbol);
	char detect_qpsk_symbol(gr_complex symbol);
	char detect_mpsk_symbol(gr_complex symbol);

	char (ccsds_mpsk_demod_cb:: *detect_symbol)(gr_complex symbol);

public:
    ~ccsds_mpsk_demod_cb ();  // public destructor
    int work (  int                         noutput_items,
                gr_vector_const_void_star   &input_items,
                gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_MPSK_DEMOD_CB_H */
