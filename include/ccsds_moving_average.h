#ifndef INCLUDED_MOVING_AVERAGE_H
#define INCLUDED_MOVING_AVERAGE_H

/*! \brief Moving average filter
 *  \ingroup filter
 *
 *  A dual moving average filter. The filter calculates the moving average of an
 *  input sequence. In order to not accumulate numerical computation errors the
 *  filter is reset after processing moving_average::D_NUM_RESET samples. A
 *  second (inactive) moving average filter starts to filter the input stream in
 *  parallel moving_average::D_NUM_COLLECT samples before the first (active)
 *  filter is reset.
 *
 *  The filtered output is always taken from the active filter.
 *
 *  When the active filter is reset, the inactive filter has filtered exactly
 *  moving_average::D_NUM_COLLECT samples and becomes active while the
 *  previously active filter becomes inactive.
 */
class moving_average
{
private:
	/*! \brief Number of samples the active filter should process before it
	 *	is reset.
	 */
	unsigned int D_NUM_RESET;

	/*! \brief Number of samples the inactive filter has to accumulate
	 *	before it can become active.
	 *
	 *  The inactive filter starts accumulating when the active filter has
	 *  processed (moving_average::D_NUM_RESET -
	 *  moving_average::D_NUM_COLLECT) samples.
	 */
	unsigned int D_NUM_COLLECT;

	/*! \brief Storage for the states of the two filters */
	double freq[2];

	/*! \brief Counter for the number of processed samples of the two
	 *	filters.
	 */
	unsigned int freq_count[2];

	/*! \brief Pointer to the active state value. */
	double* act_freq;

	/*! \brief Pointer to the inactive state value. */
	double* inact_freq;

	/*! \brief Pointer to the active state counter. */
	unsigned int* act_count;

	/*! \brief Pointer to the inactive state counter. */
	unsigned int* inact_count;

	/*! \brief Put array into inactive filter, but do not output filtered
	 *	states.
	 *
	 *  \param values Array of input values. Must contain at least \c n
	 *	elements.
	 *  \param n Number of samples to filter.
	 *
	 *  When we just have to filter a block of samples, but don't need the
	 *  individual filtered samples, we can update the state in a block,
	 *  instead of computing every single state individually.
	 *  \code x[m+n] = ( m * x[m] + sum( in[0] ... in[n-1]) \endcode
	 *  with \c x[k] beeing the moving average of k samples.
	 */
	void populate_filter(const float *values, unsigned int n);

	/*! \brief Put array int active filter and return filtered state for
	 *	every input element.
	 *
	 *  \param values Array with values to filter. The filtered states are
	 *	written back into this array. Must contain at least \c n
	 *	elements.
	 *  \param n Number of samples to filter.
	 *
	 *  Filter \c n elements of value and return the \c n filtered values in
	 *  the same array.
	 */
	void apply_filter(float *values, unsigned int n);

public:
	/*! \brief Public constructor of the dual moving average filter. */
	moving_average();   // public constructor

	/*! \brief Public destructor of the dual moving average filter. */
	~moving_average();  // public destructor

	/*! \brief Filter a block of values and take care of the filter
	 *	switching internally.
	 *
	 *  \param values Array of values to filter. The filtered values are
	 *	returned in this array. Must contain at least \c n elements.
	 *  \param n Number of values to filter.
	 *
	 *  Filter array and take care of the filter switching and populating
	 *  internally.
	 */
	void filter(float *values, const unsigned int n);

	/*! \brief Return current state (i.e. the current moving average)
	 *
	 *  \return Return current state *(moving_average::act_freq)
	 */
	float getState();
};

#endif /* INCLUDED_MOVING_AVERAGE_H */
