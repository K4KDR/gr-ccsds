#ifndef CCSDS_LDPC_H
#define CCSDS_LDPC_H

#include <stdio.h>
#include <ldpc/ldpc.h>

namespace gr {
  namespace ccsds{

        /*! \brief Enum of supported puncturing schemes */
        enum punct_t {
            LDPC_PUNCT_NONE  =ldpc::puncturing::NONE,
            LDPC_PUNCT_FRONT =ldpc::puncturing::FRONT,
            LDPC_PUNCT_BACK  =ldpc::puncturing::BACK,
            LDPC_PUNCT_CUSTOM=ldpc::puncturing::CUSTOM
        };
        
        /*! \brief Enum of supported systematic schemes */
        enum sys_t {
            LDPC_SYS_NONE  =ldpc::systematic::NONE,
            LDPC_SYS_FRONT =ldpc::systematic::FRONT,
            LDPC_SYS_BACK  =ldpc::systematic::BACK
        };

  }
}

#endif /* CCSDS_LDPC_H */
