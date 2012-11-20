#ifndef INCLUDED_CCSDS_FLL_CC_H
#define INCLUDED_CCSDS_FLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <ccsds_lpf.h>
#include <math.h>

class ccsds_fll_cc;

typedef boost::shared_ptr<ccsds_fll_cc> ccsds_fll_cc_sptr;

CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc (unsigned int obsv_length);

class CCSDS_API ccsds_fll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc(unsigned int obsv_length);

	ccsds_fll_cc(unsigned int obsv_length);   // private constructor
	
	void calc_diffs(gr_complex *tmp_c, const gr_complex *in, const unsigned int num);
	void calc_summs(gr_complex *phasors, unsigned int num);
	void calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num);
	void calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num);

	const unsigned int D_L;

	gr_complex d_last_sample;
	double d_phase;
	static const double D_TWOPI;

	bool d_last_sample_valid;
	lpf *d_filter;

public:
    ~ccsds_fll_cc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_FLL_CC_H */
