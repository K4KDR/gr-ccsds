#ifndef INCLUDED_CCSDS_PLL_CC_H
#define INCLUDED_CCSDS_PLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <ccsds_lpf2.h>
#include <math.h>
#include <gr_msg_queue.h>

#define PLL_FREQ_UPDATE 5000
#define PLL_FREQ_SKIP_FIRST 350

class ccsds_pll_cc;

typedef boost::shared_ptr<ccsds_pll_cc> ccsds_pll_cc_sptr;

CCSDS_API ccsds_pll_cc_sptr ccsds_make_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq);

class CCSDS_API ccsds_pll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_pll_cc_sptr ccsds_make_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq);

	ccsds_pll_cc(unsigned int m, float loop_bandwidth, gr_msg_queue_sptr msgq);   // private constructor
	
	void rotate_constellation(gr_complex *out, const gr_complex *in, const float angle, const unsigned int num);
	void remove_modulation(gr_complex *tmp_c, const gr_complex *in, const unsigned int num);
	void calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num);
	void check_lo_tags(const uint64_t from, const unsigned int num);
	void send_freq_estimate(double est);
	void calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num);

	static const double D_TWOPI;
	const unsigned int d_M;
	
	gr_msg_queue_sptr d_msgq;

	bool d_lo_msg_tag;
	unsigned int d_lo_msg_offset;

	lpf2* d_filter;
	float d_phi_hat;

	FILE *dbg_file;
	FILE *dbg_file_lo;
	unsigned int dbg_count;
	unsigned int dbg_count_lo;


public:
    ~ccsds_pll_cc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_PLL_CC_H */
