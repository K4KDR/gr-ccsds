#ifndef INCLUDED_CCSDS_SOFTBITS_H
#define INCLUDED_CCSDS_SOFTBITS_H

#include <ccsds/api.h>
#include <cstdint>
#include <cmath>

namespace gr {
  namespace ccsds {

    /**
     * \brief classes to compute and convert log likelihood ratios (LLRs) to probabilities
     * 
     * All LLRs are natural (base e) logarithms with respect to one, i.e. LLR = ln( prob(1) / prob(0) )
     */
    class CCSDS_API softbits {
      public:

        /** 
         * \brief Compute the probability that the given LLR was a one
         * 
         * The probability is computed using
         * p(1) = exp(LLR)/(exp(LLR)+1)   // [1]
         *      = 1/(1+exp(-LLR))         // [2]
         * 
         * To avoid numeric issues [1] is used for negative LLRs and [2] is used for positive LLRs.
         */
        static constexpr float probability_one(float llr_one) {
          return (llr_one < 0.0)
            ? std::exp(-llr_one)/(1.0+std::exp(-llr_one))
            : 1.0/(std::exp(llr_one)+1.0)
          ;
        }

        /** 
         * \brief Compute the probability that the given LLR was a zero
         * 
         * The probability is computed using
         * p(0) = 1/(exp(LLR)+1)           // [1]
         *      = exp(-LLR)/(1+exp(-LLR))  // [2]
         * 
         * To avoid numeric issues [1] is used for negative LLRs and [2] is used for positive LLRs.
         */
        static constexpr float probability_zero(float llr_one) {
          return (llr_one < 0.0)
            ? 1.0/(std::exp(llr_one)+1.0)
            : std::exp(-llr_one)/(1.0+std::exp(-llr_one))
          ;
        }
        
        /** Round value to nearest integer number closer to zero and convert the result to the required integral type */
        template <typename T> static constexpr T floor_to(float in) {
          return (static_cast<float>(static_cast<T>(in)) == in)
            ? static_cast<T>(in)
            : (static_cast<T>(in) - static_cast<T>((in > 0) ? 0 : 1))
          ;
        }

        /** Round value to nearest integer number away from zero and convert the result to the required integral type */
        template <typename T> static constexpr T ceil_to(float in) {
          return (static_cast<float>(static_cast<T>(in)) == in)
            ? static_cast<T>(in)
            : (static_cast<T>(in) + static_cast<T>((in > 0) ? 1 : 0))
          ;
        }

        /**
         * \brief Map LLR range to a byte
         * 
         * Map value range from -inf to +inf onto the value range 0-255.
         *   0 is a strong zero bit
         * 127 is undecided
         * 255 is a string one bit
         */
        static constexpr uint8_t map_to_uint8(float llr_one) {
          return floor_to<uint64_t>( llr_one*255.0 );
        }

        /** 
         * \brief Convert a LLR into a hard bit
         * 
         * \return Either 0 or 1
         */
        static uint8_t hard_decision(float llr_one) {
          return (llr_one >= 0.0) ? 1u : 0u;
        }

        /**
         * \brief Compute LLR based on (real) symbol value. Assuming positive value means 1, negative value means 0
         */
        static constexpr float create_from_sample(float symbol, float noise_power=1.0) {
          return 2.0*symbol/noise_power;
        }

    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SOFTBITS_H */
