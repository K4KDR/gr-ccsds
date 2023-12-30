#ifndef INCLUDED_CCSDS_RANDOMIZER_IMPL_H
#define INCLUDED_CCSDS_RANDOMIZER_IMPL_H

#include <gnuradio/ccsds/randomiser.h>
//#include <string>

namespace gr {
  namespace ccsds {
    class randomiser_impl : public randomiser 
    {
    private:
    	/*! Polynomial to define the LFSR network */
    	const uint8_t d_POLY;
    
    	/*! Initial state of the LFSR */
    	const uint8_t d_SEED;
    
    	/*! Current state of the LFSR */
    	uint8_t d_state;
    
    	/*! Flag to store whether EOF has been received or not */
    	bool d_stop;
    
    	/*! Reset LFSR to \c d_SEED state */
    	void reset(void);
    
    	/*! Perform 8 updates on the LFSR and return it's output */
    	uint8_t get_next_byte(void);
    
    	/*! \brief Asynchronous work function which is processing the queues. */
    	void process_frame(pmt::pmt_t msg);
    
    public:
    	/*! \brief Public deconstructor */
    	randomiser_impl(const uint8_t polynomial, const uint8_t seed);
    	~randomiser_impl ();
    
    	/*! \brief Dummy work function */
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
    
    };
  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_RANDOMIZER_IMPL_H */
