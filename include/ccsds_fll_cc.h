#ifndef INCLUDED_CCSDS_FLL_CC_H
#define INCLUDED_CCSDS_FLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <ccsds_moving_average.h>

class ccsds_fll_cc;

typedef boost::shared_ptr<ccsds_fll_cc> ccsds_fll_cc_sptr;

CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc ();

class CCSDS_API ccsds_fll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc();

	ccsds_fll_cc();   // private constructor

	gr_complex d_last_sample;
	gr_complex d_rotator;
	
	bool d_last_sample_valid;
	moving_average d_filter;

public:
    ~ccsds_fll_cc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_FLL_CC_H */
