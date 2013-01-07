#ifndef INCLUDED_LPF_H
#define INCLUDED_LPF_H

#include <stdio.h>

class lpf;

/*! \brief Create a linear first order low pass filter and return it's shared
 *	pointer.
 *
 *  \param loop_bw Bandwidth of the loop, which determins the loop stepsize.
 *  \return Shared pointer to the created filter.
 *
 *  Based on the loop bandwidth the filter stepsize alpha is calculated
 *  with: [\ref bib_mengali p. 126 (3.5.67)]
 *  \code alpha = 4 * loop_bw \endcode
 *
 *  The k-th filtered state x from the inputs y is calculated as
 *  \code x[k] = (1-alpha) * x[k-1] + alpha * y[k] \endcode
 */
lpf *ccsds_make_lpf(float loop_bw);

/*! \brief Linear first order low pass filter
 *  \ingroup filter
 *  \todo put \ref filter into ccsds_ name schema.
 */
class lpf
{
private:
	
	friend lpf * ccsds_make_lpf(float loop_bw);

	/*! \brief Loop stepsize alpha
	 *
	 *  \sa ::ccsds_make_lpf()
	 */
	const double ALPHA;

	/*! \brief Precalculated expression for (1-ALPHA).
	 *
	 *  \sa lpf::ALPHA
	 */
	double ALPHAB;

	/*! \brief Filtered state variable. */
	double d_state;

	/*! \brief Flag is filter already contains a value.
	 *
	 *  If false lpf::d_state is initialized with the first input. If true
	 *  input will be filtered. This way a constant input \a c will result
	 *  in a filtered value of \a c and not growing from zero to \a c first.
	 */
	bool d_state_init;

	/*! File pointer for debugging */
	FILE *debugFile;



	/*! \brief Private constructor of \ref lpf
	 *
	 *  \param alpha Loop stepsize.
	 *
	 *  Constructs a linear first order low pass filter.
	 */
	lpf(float alpha);

	/*! \brief Calculate next state based on single input.
	 *
	 *  \param in Value that should be filtered.
	 *
	 *  Next state is calculated by
	 *  \code d_state = ALPHAB * d_state + ALPHA * in \endcode
	 */
	double filter_step(double in);

public:
	/*! \brief Public deconstructor */
	~lpf();  // public destructor

	/*! \brief Filter elements of an array.
	 *
	 *  \param out Array to store the filtered values. Memory for at least
	 *	\c n elements must be allocated.
	 *  \param in Array of values that should be filtered. Must contain at
	 *	least \c n elements.
	 *  \param n Number of elements to filter.
	 *
	 *  Call filter_step() for each element of \c in and put output in
	 *  \c out.
	 */
	void filter(double *out, float *in, const unsigned int n);

	/*! \brief Filter elements of an array.
	 *
	 *  \param out Array to store the filtered values. Memory for at least
	 *	\c n elements must be allocated.
	 *  \param in Array of values that should be filtered. Must contain at
	 *	least \c n elements.
	 *  \param n Number of elements to filter.
	 *
	 *  Call filter_step() for each element of \c in, cast result to float
	 *  and put it in \c out.
	 */
	void filter(float *out, float *in, const unsigned int n);
};

#endif /* INCLUDED_LPF_H */
