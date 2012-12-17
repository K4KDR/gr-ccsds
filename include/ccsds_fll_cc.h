#ifndef INCLUDED_CCSDS_FLL_CC_H
#define INCLUDED_CCSDS_FLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>
#include <ccsds_lpf.h>
#include <math.h>
#include <gr_msg_queue.h>

class ccsds_fll_cc;

typedef boost::shared_ptr<ccsds_fll_cc> ccsds_fll_cc_sptr;

CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc (unsigned int obsv_length, float  loop_bw, unsigned int power, gr_msg_queue_sptr msgq);

class CCSDS_API ccsds_fll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_fll_cc_sptr ccsds_make_fll_cc(unsigned int obsv_length, float  loop_bw, unsigned int power, gr_msg_queue_sptr msgq);

	ccsds_fll_cc(unsigned int obsv_length, float  loop_bw, unsigned int power, gr_msg_queue_sptr msgq);   // private constructor
	
	void calc_power(gr_complex *out, const gr_complex *in, const unsigned int num);
	void calc_diffs(gr_complex *tmp_c, const gr_complex *in, const unsigned int num);
	void calc_summs(gr_complex *phasors, unsigned int num);
	void calc_phases(float  *tmp_f, const gr_complex *tmp_c, const unsigned int num);
	void adjust_phases(float  *phases, unsigned int num);
	void get_lo_tags(float  *lo_freqs, const unsigned int num);	
	void fill_freqs(float  *tmp_f, float  *tmp_fs, float  *tmp_lo, const unsigned int num_out);
	void send_freq_estimate(double  est);
	void substract_lo_freq(double *tmp_f, float  *tmp_lo, const unsigned int num);
	void calc_rotation(gr_complex *out, const gr_complex *in, const double  *tmp_f, const unsigned int num);

	const unsigned int D_L;
	const unsigned int d_POWER;

	gr_msg_queue_sptr d_msgq;

	gr_complex d_last_sample;
	float d_lo_freq;
	float d_phase;
	static const float D_TWOPI;

	FILE *dbg_file;
	FILE *dbg_file_pow;
	FILE *dbg_file_dif;
	FILE *dbg_file_sum;
	FILE *dbg_file_tag;
	unsigned int dbg_count;
	unsigned int dbg_count_pow;
	unsigned int dbg_count_dif;
	unsigned int dbg_count_sum;
	unsigned int dbg_count_tag;
	unsigned int dbg_input_toggle;
	float dbg_last_msg;

	lpf *d_filter;

public:
    ~ccsds_fll_cc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_FLL_CC_H */
