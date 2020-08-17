#ifndef INCLUDED_CCSDS_DECODER_VERBOSITY_H
#define INCLUDED_CCSDS_DECODER_VERBOSITY_H

#include <ccsds/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ccsds {
     
    /*! \brief Enum of decoder verbosity levels */
    enum decoder_verbosity_t {
        DECODER_VERBOSITY_QUIET = 0,  ///< No output
        DECODER_VERBOSITY_DROPPED,    ///< Only print output if a frame is invalid and it is dropped
        DECODER_VERBOSITY_FAILED,     ///< Only print output if a frame is invalid
        DECODER_VERBOSITY_ALL,        ///< Print output for every frame
        DECODER_VERBOSITY_DEBUG       ///< Print output for every frame and additional debug output
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_DECODER_VERBOSITY_H */
