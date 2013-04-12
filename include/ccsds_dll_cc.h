#ifndef INCLUDED_CCSDS_DLL_CC_H
#define INCLUDED_CCSDS_DLL_CC_H

#include <ccsds_api.h>
#include <gr_block.h>

/*!
 *  \brief Number of samples used for one interpolation
 */
#define DLL_INTERP_NUMSAMPS 4

class ccsds_dll_cc;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<ccsds_dll_cc> ccsds_dll_cc_sptr;

/*!
 *  \brief Return a shared_ptr to a new instance of ccsds_dll_cc
 *
 *  Create an instance of ccsds_dll_cc and return it's shared_ptr.
 *
 *  \param osf Signal oversampling factor. A factor of two means that two
 *         samples contain one symbol. The oversampling factor also defines the 
 *         decimation factor of this block.
 *  \param gamma Block's filter stepsize. Each estimated timing error will be
 *         multiplied by gamma and then added to the Delay estimate. A high
 *         value (<=1) will allow the loop to follow timing changes very quickly
 *         (instantly for 1), but be very sensitive to noise, while lower values
 *         will reduce the noise impact but will take longer to settle for a
 *         certain delay
 *
 *  \return Shared pointer to the created DLL block
 */
CCSDS_API ccsds_dll_cc_sptr ccsds_make_dll_cc(unsigned int osf, float gamma);

/*!
 * \brief Delay locked loop decimating from sample- to symbolrate
 * \ingroup synchronization
 *
 * Delay estimation based on the Gardner passband timing error detector. The
 * synchronous samples are obtained by cubic interpolation.
 */
class CCSDS_API ccsds_dll_cc : public gr_block
{
private:
	friend CCSDS_API ccsds_dll_cc_sptr ccsds_make_dll_cc(unsigned int osf, float gamma);

	/*!
	 * \brief Private constructor of the DLL
	 *  \param osf Signal oversampling factor. A factor of two means that two
	 *         samples contain one symbol. The oversampling factor also defines the 
	 *         decimation factor of this block.
	 *  \param gamma Block's filter stepsize. Each estimated timing error will be
	 *         multiplied by gamma and then added to the Delay estimate. A high
	 *         value (<=1) will allow the loop to follow timing changes very quickly
	 *         (instantly for 1), but be very sensitive to noise, while lower values
	 *         will reduce the noise impact but will take longer to settle for a
	 *         certain delay
	 *  Constructs a DLL block with the given oversampling factor and loop
	 *  stepsize.
	 */
	ccsds_dll_cc(unsigned int osf, float gamma);   // private constructor
	
	/*! \brief Oversampling factor as specified in the constructor */
	const unsigned int d_OSF; 

	/*! \brief Half oversampling factor to obtain the intermediate sample
	 *  points for the Gardner TED
	 */
	const float d_OSF_HALF;

	/*! \brief Loop stepsize as specified in the constructor */
	const float d_GAMMA;

	/*!
	 * \brief Enum used to name the samples in d_last_interp.
	 *
	 * Index names for elements in d_last_interp starting from zero so the 
	 * last dummy element equals to the ammount of elements in the enum
	 */
	enum INTERP {
		PREV=0,
		INTM,
		CURR,
		INTERP_COUNT};

	/*! \brief Storage for interpolated samples
	 *
	 * Storage for the last interpolated samples to be re-used by the
	 * Gardner TED
	 */
	gr_complex d_last_interp[INTERP_COUNT];

	/*! \brief Indicator if the loop has been initialized yet
	 *
	 * Set to false by the constructor and set to true by the general_work
	 * function once the initial interpolants have been obtained.
	 */
	bool d_init;

	/*! \brief Storage for the current accumulated timing error */
	float d_tau_hat;

	/*! \brief Storage for the current timing error scaled by the stepsize */
	float d_gamma_eps;

	/*! \brief Storage for the current basepoint index.
	 *
	 *  The index of the basepoint sample in the input vector. The next
	 *  interpolant lies between (or on top of) this sample and the
	 *  following one.
	 */
	int d_l;

	/*! \brief Storage for the current fractional index .
	 *
	 *  The fractional index is in the range [0,1) and shows where in
	 *  between the basepoint sample and the following sample the
	 *  interpolation shall be done. A fractional index of 0 means the
	 *  interpolant is equal to the basepoint sample while 0.5 means the
	 *  interpolated sample lies exactly in the middle between the basepoint
	 *  sample and the following sample.
	 */
	double d_mu;

	/*! \brief Storage for complex to real conversion */
	float *d_real;
	/*! \brief Storage for complex to real conversion */
	float *d_imag;

	/*! \brief Storage for counting samples for stream tag propagation */
	uint64_t d_tag_count;

	/*! \brief File pointer for debugging */
	FILE *dbg_file_o;
	/*! \brief File pointer for debugging */
	FILE *dbg_file_s;
	/*! \brief File pointer for debugging */
	FILE *dbg_file_i;
	/*! \brief File pointer for debugging */
	FILE *dbg_file_t;
	/*! \brief Counter for debugging */
	unsigned int dbg_count;
	/*! \brief Block toggle for debugging */
	bool dbg_toggle;

	

	/*! \brief Return fractional part of the value
	 *  \param value Input value from which the fractional part should be
	 *	determined.
	 *  \return Fractional part of value in the range [0,1)
	 */
	inline float get_frac(float value);

	/*! \brief Return integral part of the value
	 *  \param value Input value from which the integral part should be
	 *	determined.
	 *  \return Integral part of value
	 */
	inline int get_int(float value);

	/*! \brief Calculate real components from complex array
	 *  \param out pointer to array where the real components should be
	 *	stored. Memory needs to be allocated for at least num elements.
	 *  \param in pointer to array of complex numbers which's real
	 *	components should be calculated. Must contain at least num
	 *	complex numbers.
	 *  \param num Number of elements to process in the arrays.
	 */
	void to_real(float *out, const gr_complex *in, const unsigned int num);

	/*! \brief Calculate imaginary components from complex array
	 *  \param out pointer to array where the imaginary components should be
	 *	stored. Memory needs to be allocated for at least num elements.
	 *  \param in pointer to array of complex numbers which's imaginary
	 *	components should be calculated. Must contain at least num
	 *	complex numbers.
	 *  \param num Number of elements to process in the arrays.
	 */
	void to_imag(float *out, const gr_complex *in, const unsigned int num);

	/*! \brief Calculate real components from complex 4-element array
	 *  \param out pointer to array where the real components should be
	 *	stored. Memory needs to be allocated for 4 elements.
	 *  \param in pointer to array of complex numbers which's real
	 *	components should be calculated. Must contain at least 4 complex
	 *	numbers.
	 */
	void to_real4(float *out, const gr_complex *in);

	/*! \brief Calculate imaginary components from complex 4-element array
	 *  \param out pointer to array where the imaginary components should be
	 *	stored. Memory needs to be allocated for 4 elements.
	 *  \param in pointer to array of complex numbers which's imaginary
	 *	components should be calculated. Must contain at least 4 complex
	 *	numbers.
	 */
	void to_imag4(float *out, const gr_complex *in);

	/*! \brief Calculate cubic interpolant
	 *  \param y Array with DLL_INTERP_NUMSAMPS data points.
	 *  \param mu Fractional index ot the interpolation point in range [0,1)
	 *  \return Interpolated value
	 *
	 *  Calculate cubic interpolant between the second and thrid data point
	 *  based on the fractional index. If mu is 0 the interpolant is equal
	 *  to the second data point, if mu is 0.5 the interpolation point is
	 *  exactly in the middle between the second and the third data point.
	 */
	float interpolate_value(const float *y, float mu);

	/*! \brief Calculate cubic interpolant if a complex number
	 *  \param y Array with DLL_INTERP_NUMSAMPS complex data points.
	 *  \param mu Fractional index ot the interpolation point in range [0,1)
	 *  \return Interpolated value
	 *
	 *  Calls interpolate_value for the real and imaginary component and
	 *  reassambles interpolants back to a complex sample.
	 */
	gr_complex interpolate_cvalue(const gr_complex *y, float mu);

	/*! \brief Calculate a timing error from the last three interpolants.
	 *  \param previous Previous interpolated symbol.
	 *  \param intermediate Interpolated sample between last and current
	 *	symbol.
	 *  \param current Current interpolated symbol.
	 *  \return Current timing error estimation
	 *
	 *  Gardner passband timing error estimation based on the last two
	 *  interpolated symbols and an intermediate sample from between.
	 *
	 *  e[k] = re{ [ y(kT-T+tau[k-1]) - y(kT+tau[k]) ] y*(kT-T/2+tau[k]) }
	 *  
	 */
	inline float gardner(gr_complex previous, gr_complex intermediate, gr_complex current);

	/*! \brief Propagates stream tags of the current block to the decimated
	 *	output stream.
	 *  \param num_in Number of samples read during this block operation.
	 *
	 *  \todo Tag propagation is done with a fixed rate conversion. For
	 *	higher timing accuracy use the timing estimations for rate
	 *	conversion.
	 */
	void propagate_tags(const unsigned int num_in);

public:
	/*! \brief Public deconstructor of the DLL */	
	~ccsds_dll_cc ();  // public destructor

	void forecast(int noutput_items,gr_vector_int &ninput_items_required);

	int  general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items);};

#endif /* INCLUDED_CCSDS_DLL_CC_H */
