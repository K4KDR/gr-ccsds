#ifndef INCLUDED_LPF_H
#define INCLUDED_LPF_H

#include <stdio.h>

class lpf;

/*! \brief Create a linear first order low pass filter and return it's shared
 *	pointer.
 *
 *  \param loop_bw Bandwidth of the loop, which determins the loop stepsize.
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
 *  \todo continue documentation of lpf.
 *  \todo put \ref filter into ccsds_ name schema.
 */
class lpf
{
private:
	
	friend lpf * ccsds_make_lpf(float loop_bw);

	const double ALPHA, ALPHAB;
	double d_state;
	bool d_state_init;

	FILE *debugFile;

	double filter_step(double in);
	lpf(float alpha);

public:
	~lpf();  // public destructor

	void filter(double *out, float *in, const unsigned int n);
	void filter(float *out, float *in, const unsigned int n);
};

#endif /* INCLUDED_LPF_H */
