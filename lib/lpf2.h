#ifndef INCLUDED_LPF2_H
#define INCLUDED_LPF2_H

#include <stdio.h>

namespace gr {
  namespace ccsds {

    class lpf2;
    
    /*! \brief Create a linear second order low pass filter and return it's shared
     *	pointer.
     *
     *  \param loop_bw Bandwidth of the loop. Used to compute the loop stepsizes.
     *  \param damping_factor_squared Squared value of the loop's damping factor.
     *	Used to compute the loop stepsizes.
     *  \param s_slope Estimated slope of the S-curve of the error detector at the
     *	origin. Used to compute the loop stepsizes.
     *  \return Shared pointer of the created filter.
     *
     *  The loop stepsizes gamma and rho are given by the following formulas:
     *  [\ref bib_mengali p. 220 ff.]
     *  \code
     *	loop_bw = ( 2*rho + gamma*s_slope*(2+rho) ) / ( 8 - 2*gamma*s_slope*(2+rho) )  (5.3.92)
     *	damping_factor = ( (1+rho)*sqrt(gamma*s_slope) ) / ( 2*sqrt(rho))  (5.3.86)
     *  \endcode
     *
     *  The stepsize rho can not be computed in explicit form and is computed with
     *  Newton's method by solving the root of the following polynomial:
     *  \code f(rho) = rho^3 + a * rho^2 + b * rho + c != 0 \endcode
     *  with
     *  \code
     *	a = - 4*loop_bw + 4*loop_bw*damping_factor_squared + 2 + 2*damping_factor_squared
     *	b = - 8*loop_bw + 8*loop_bw*damping_factor_squared + 1 + 4*damping_factor_squared
     *	c = - 4*loop_bw
     *  \endcode
     *
     *  Given rho, gamma is computed in closed form as
     *  \code gamma = ( 4*damping_factor_squared*rho ) / ( s_slope * (1+rho)^2 ) \endcode
     */
    lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope);
    
    /*! \brief Creates a linear second order low pass filter and return it's shared
     *	pointer.
     *
     *  \param loop_bw Bandwidth of the loop. Used to calculate loop stepsizes.
     *  \return Shared pointer of the created filter.
     *  \sa ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope)
     *
     *  Call to ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope)
     *  with the given loop bandwidth, a damping factor of sqrt(0.5) (critically
     *  damped) and a S-curve slope of 1.0 (assuming the error detector is an
     *  unscaled and unbiased estimator).
     */
    lpf2 * ccsds_make_lpf2(double loop_bw);
    
    /*! \brief Linear second order low pass filter.
     *  \ingroup filter
     *  \todo Put filter into ccsds_ naming scheme
     */
    class lpf2
    {
    private:
    	friend lpf2 * ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope);
    
    	/*! \brief Loop stepsize rho.
    	 *
    	 *  Computed by ccsds_make_lpf2().
    	 */
    	const double  RHO;
    
    	/*! \brief Loop stepsize rho.
    	 *
    	 *  Computed by ccsds_make_lpf2().
    	 */
    	const double GAMMA;
    
    	/*! \brief Loop stepsize (1+rho). */
    	const double RHOB;
    
    	/*! \brief State variable */
    	double d_phi;
    
    	/*! \brief State variable */
    	double d_xi;
    
    	/*! \brief Auxilliary state variable.
    	 *
    	 *  Store the difference values.
    	 */
    	double d_ephi;
    
    	/*! \brief File pointer for debugging. */
    	FILE *debugFile;
    
    	/*! \brief Counter for debugging. */
    	unsigned int debug_count;
    
    	/*! \brief Private constructor of the linear second order low pass
    	 *	filter.
    	 *
    	 *  \param gamma General loop stepsize.
    	 *  \param rho Differential loop stepsize. Set to zero for a first order
    	 *	filter.
    	 *  \sa ccsds_make_lpf2(const double loop_bw, const double damping_factor_squared, const double s_slope)
    	 *
    	 *  Loop stepsizes are derived from the more intuitive parameters loop
    	 *  bandwidth, damping factor and S-curve slope in ccsds_make_lpf2().
    	 */
    	lpf2(double  gamma, double  rho);   // private constructor
    
    	/*! \brief Wrap value centered arround zero to given bounds.
    	 *
    	 *  \param in Vaue to be wrapped.
    	 *  \param max upper bound to wrap arround.
    	 *
    	 *  Wraps \c in to lie in the interval (-max,max].
    	 *
    	 *  Most prominent usecase is to wrap a phase \a phi to be contained in
    	 *  the interval between -pi and pi. This could be achieved by:
    	 *  \code wrap(phi, pi); \endcode
    	 */
    	float wrap(float in, float max);
    
    	/*! \brief Update filtered state from a single input.
    	 *
    	 *  \param in Value to be filtered.
    	 *
    	 *  \image html lpf2_filter_step.png "Block diagram of the \c filter_step() function"
    	 *
    	 */
    	double filter_step(float in);
    
    	/*! \brief Update filtered state from a single input. And keep state
    	 *	variables contained in a fixed interval.
    	 *
    	 *  \param in Value to be filtered.
    	 *  \param wrap_max Bound arround which the input value is wrapped.
    	 *
    	 *  \image html lpf2_filter_step_wrapped.png "Block diagram of the \c filter_step_wrapped() function"
    	 *
    	 *  \sa wrap()
    	 */
    	double filter_step_wrapped(float in, float wrap_max);
    	
    
    public:
    	/*! \brief Public deconstructor */
    	~lpf2();  // public destructor
    
    	/*! \brief Filter elements of an array.
    	 *
    	 *  \param values Array of values that should be filtered. Must contain
    	 *	at least \c n elements. Filtered results are written back into
    	 *	this array.
    	 *  \param n Number of elements to filter.
    	 *
    	 *  Call filter_step() for each element of \c value and put restul in
    	 *  \c out.
    	 */
    	void filter(float *values, const unsigned int n);
    
    	/*! \brief Filter elements of an array.
    	 *
    	 *  \param out Array to store the filtered values. Memory for at least
    	 *	\c n elements must be allocated.
    	 *  \param in Array of values that should be filtered. Must contain at
    	 *	least \c n elements.
    	 *  \param n Number of elements to filter.
    	 *
    	 *  Call filter_step() for each element of \c value and put result back
    	 *  into \c value.
    	 */
    	void filter(float *out, const float *in, const unsigned int n);
    
    	/*! \brief Filter elements of an array.
    	 *
    	 *  \param out Array to store the filtered values. Memory for at least
    	 *	\c n elements must be allocated.
    	 *  \param in Array of values that should be filtered. Must contain at
    	 *	least \c n elements.
    	 *  \param n Number of elements to filter.
    	 *
    	 *  Call filter_step() for each element of \c in and put result in
    	 *  \c out.
    	 */
    	void filter(double *out, const float *in, const unsigned int n);
    
    	/*! \brief Filter elements of an array and keep state variables
    	 *	contained.
    	 *
    	 *  \param values Array of values that should be filtered. Must contain
    	 *	at least \c n elements. Filtered results are written back into
    	 *	this array.
    	 *  \param freq_est Array to store frequency estimates in. Memory for at
    	 *	least \c n elements must be allocated.
    	 *  \param wrap_max Upper bound to wrap input arround.
    	 *  \param n Number of elements to filter.
    	 *
    	 *  Call filter_step_wrapped() for each element of \c value and put
    	 *  result back into \c value.
    	 */
    	void filter_wrapped(float *values, float *freq_est, float wrap_max, const unsigned int n);
    
    	/*! \brief Return current difference state.
    	 *
    	 *  \return Current difference state lpf2::d_xi
    	 *
    	 *  When filtering carrier phase estimates with this filter, the
    	 *  current difference state acts as a frequency estimate.
    	 */
    	double get_freq_estimate(void);
    
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_LPF2_H */
