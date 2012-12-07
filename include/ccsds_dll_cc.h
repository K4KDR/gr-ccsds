#ifndef INCLUDED_CCSDS_DLL_CC_H
#define INCLUDED_CCSDS_DLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>

class ccsds_dll_cc;

typedef boost::shared_ptr<ccsds_dll_cc> ccsds_dll_cc_sptr;

CCSDS_API ccsds_dll_cc_sptr ccsds_make_dll_cc(unsigned int osf, float gamma);

class CCSDS_API ccsds_dll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_dll_cc_sptr ccsds_make_dll_cc(unsigned int osf, float gamma);

	ccsds_dll_cc(unsigned int osf, float gamma);   // private constructor
	
	const unsigned int d_OSF;
	const float d_OSF_HALF;
	const float d_GAMMA;

	// used to name the sampled in d_last_interp, starting from zero so last
	// dummy element equals to the ammount of elements in the enum
	enum INTERP {
		PREV=0,
		INTM,
		CURR,
		INTERP_COUNT};

	FILE *dbg_file_o;
	FILE *dbg_file_s;
	FILE *dbg_file_i;
	FILE *dbg_file_t;
	unsigned int dbg_count;

	gr_complex d_last_interp[INTERP_COUNT];
	bool d_init;

	float d_tau_hat;
	float d_gamma_eps;
	float d_mu;
	int d_l;

	float get_frac(float value);
	int get_int(float value);
	void to_real(float *out, const gr_complex *in, const unsigned int num);
	void to_imag(float *out, const gr_complex *in, const unsigned int num);
	float interpolate_value(const float *y, float mu);
	gr_complex interpolate_cvalue(const gr_complex *y, float mu);
	float gardner(gr_complex previous, gr_complex intermediate, gr_complex current);

public:
	~ccsds_dll_cc ();  // public destructor
	void forecast(int noutput_items,gr_vector_int &ninput_items_required);
	int  general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items);};

#endif /* INCLUDED_CCSDS_DLL_CC_H */
