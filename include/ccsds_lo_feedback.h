#ifndef INCLUDED_CCSDS_LO_FEEDBACK_H
#define INCLUDED_CCSDS_LO_FEEDBACK_H

#include <gruel/pmt.h>

// frequency estimator and local oscillator communicate with messages
#define MSG_FREQ_TYPE 1
#define MSG_FREQ_ARG2 -1.42

const pmt::pmt_t LO_TAG_KEY = pmt::mp("freq_offset");

#endif /* INCLUDED_CCSDS_LO_FEEDBACK_H */
