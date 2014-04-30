#ifndef INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_F_H
#define INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_F_H

#include <ccsds/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ccsds {
     
    /*!
     * \brief M-PSK soft bit ambiguity resolution and frame synchronization.
     *
     * \ingroup synchronization
     *
     * The input to this block is a stream of \c d_ldM grouped soft bits. The output
     * of this block is a asynchronous message containing a float vector with the
     * soft bits of a frame. The bytes passed to this block as ASM are cut off, but
     * any skipped bits will be in the output message.
     *
     * So if you pass the two ASM bytes 0x00 0xAA to this block followed by two
     * bytes frame data (FD1 and FD2), followed by another two bytes ASM and set
     * \c asm_skip_first_bits to zero the output will be a message containing FD1
     * and FD2. If \c asm_skip_first_bits is set to two, the output will be FD1, FD2
     * followed by the first two zero bits from the ASM.
     *
     * Using \c asm_skip_first_bits (setting it to nonzero values)
     *
     * The block will start in searchmode, looking for every possible bit offset
     * in all possible \c M ambiguities. Once an ASM is found it will enter
     * locked state where it will only check for the ASM at the expected position
     * and ambiguity. If the ASM is found a counter is increased up to
     * \c d_THRESHOLD. If an exprected ASM is not found the counter is decreased.
     * If the counter reaches zero the block goes into search mode again.
     */
    class CCSDS_API mpsk_ambiguity_resolver_f : virtual public gr::block
    {
    public:
    	typedef boost::shared_ptr<mpsk_ambiguity_resolver_f> sptr;
    	static sptr make(const unsigned int M, std::string ASM, const unsigned int asm_len, const unsigned int threshold, 
				const float correlation_threshold, const unsigned int frame_length, const unsigned int num_tail_syms=0);
	virtual float get_correlation(void) = 0;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_MPSK_AMBIGUITY_RESOLVER_F_H */
