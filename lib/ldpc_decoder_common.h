#ifndef _IN_LDPC_DECODER_COMMON_H_
#define _IN_LDPC_DECODER_COMMON_H_

#include <pmt/pmt.h>

namespace gr {
    namespace ccsds {
        namespace ldpc_common {

            /** Frame number keyword */
            const pmt::pmt_t HEADER_KEY_FRAME_NUMBER = pmt::intern("frame_number");
            
            /** Overall result of error correction. Contains whether the last FEC was succesfully decoded */
            const pmt::pmt_t HEADER_KEY_FEC_SUCCESS = pmt::intern("fec_success");
            
            /** Detected/corrected bit error rate in frame for all FEC blocks (additive for consecutive FEC blocks */
            const pmt::pmt_t HEADER_KEY_FEC_BER = pmt::intern("fec_ber");
            
            /** Success flag for LDPC decoder */
            const pmt::pmt_t HEADER_KEY_LDPC_SUCCESS = pmt::intern("ldpc_success");
            
            /** Detected/corrected bit error rate of the LDPC decoder */
            const pmt::pmt_t HEADER_KEY_LDPC_BER = pmt::intern("ldpc_ber");
            
            /** Number of decoding iterations of LDPC decoder */
            const pmt::pmt_t HEADER_KEY_LDPC_NUM_ITERATIONS = pmt::intern("ldpc_num_iterations");
            
            /** Maximum number of decoding iterations of LDPC decoder */
            const pmt::pmt_t HEADER_KEY_LDPC_NUM_ITERATIONS_MAX = pmt::intern("ldpc_num_iterations_max");
            
            inline void attachMetadata(pmt::pmt_t &header, ldpc::decoder::metadata_t *meta) {
                if(!pmt::is_dict(header)) {
                    printf("ERROR ldpc_common::attachMetadata: Header is not of dictionary type\n");
                    return;
                }
                
                double ber = static_cast<double>(meta->num_corrected) / static_cast<double>(meta->num_bits_total);
                
                // //  All FEC blocks // //
                // FEC results
                header = pmt::dict_add(header, HEADER_KEY_FEC_SUCCESS, pmt::from_bool(meta->success));
                
                // Set total ber
                double ber_total = pmt::to_double( pmt::dict_ref(header, HEADER_KEY_FEC_BER, pmt::from_double(0.0)) );
                header = pmt::dict_add(header, HEADER_KEY_FEC_BER, pmt::from_double(ber_total + ber));
                
                // // LDPC block // //
                // FEC result
                header = pmt::dict_add(header, HEADER_KEY_LDPC_SUCCESS, pmt::from_bool(meta->success));
                
                // ber
                header = pmt::dict_add(header, HEADER_KEY_LDPC_BER, pmt::from_double(ber));
                
                // number of iterations
                header = pmt::dict_add(header, HEADER_KEY_LDPC_NUM_ITERATIONS, pmt::from_uint64(meta->num_iterations));
                
                // maximum number of iterations
                header = pmt::dict_add(header, HEADER_KEY_LDPC_NUM_ITERATIONS_MAX, pmt::from_uint64(meta->num_iterations_max));
            }
            
            inline uint64_t getFrameNumberFromPDU(pmt::pmt_t &pdu) {
                // Check for header/payload pair
                if(!pmt::is_pair(pdu)) {
                    printf("ERROR: cannot extract frame number from PDU. Returning 0 instead. Offending PDU:\n  %s\n", pmt::write_string(pdu).c_str());
                    return 0;
                }
                
                pmt::pmt_t header = pmt::car(pdu);
                // Check header
                if(!pmt::is_dict( header )) {
                    printf("ERROR: cannot extract frame number from PDU header. Returning 0 instead. Offending PDU header:\n  %s\n", pmt::write_string(header).c_str());
                    return 0;
                }
                
                if(!pmt::dict_has_key(header, HEADER_KEY_FRAME_NUMBER)) {
                    printf("ERROR: cannot extract frame number from PDU. Returning 0 instead. Offending PDU header:\n  %s\n", pmt::write_string(header).c_str());
                    return 0;
                }   
                
                return pmt::to_uint64( pmt::dict_ref( header, HEADER_KEY_FRAME_NUMBER, pmt::from_uint64(0) ) );
            }
            
        } // end namespace ldpc_common
    } // end namespace ccsds
} // end namespace gr

#endif /* _IN_LDPC_DECODER_COMMON_H_ */
