#ifndef INCLUDED_CCSDS_REVERSEBITS_IMPL_H
#define INCLUDED_CCSDS_REVERSEBITS_IMPL_H

#include <ccsds/reversebits.h>

namespace gr {
  namespace ccsds {
    class reversebits_impl : public reversebits
    {
    private:
    	/*! Message length */
    	const size_t d_MSG_LEN;

    	/*! Initial state of the LFSR */
    	const std::vector<size_t> d_POSITIONS;

    	/*! Flag to store whether EOF has been received or not */
    	bool d_stop;

	/* Perform reversal of a single byte */
	unsigned char reverse_bits(const unsigned char in);

    	/*! \brief Asynchronous work function which is processing the queues. */
    	void process_frame(pmt::pmt_t msg);

    public:
    	/*! \brief Public deconstructor */
    	reversebits_impl(const size_t msg_length, const std::vector<size_t> reverse_indices);
    	~reversebits_impl ();

    	/*! \brief Dummy work function */
    	int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);

    };
  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_REVERSEBITS_IMPL_H */
