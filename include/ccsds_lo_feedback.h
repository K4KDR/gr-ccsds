#ifndef INCLUDED_CCSDS_LO_FEEDBACK_H
#define INCLUDED_CCSDS_LO_FEEDBACK_H

#include <gruel/pmt.h>

/*! \file ccsds_lo_feedback.h
 *  \brief Defines all parameters for the frequency feedback loop
 *
 *  \ingroup synchronization
 *  
 *  Defines the parameters to be used by the frequency correction messages and
 *  the frequency offset tags used by ccsds_local_oscillator_cc.
 *
 *  See \ref page_lo_feedback for more details.
 */

/*! \brief Message type used for sending frequency corrections to the local
 *	oscillator.
 *
 *  Arbitrary chosen type number to be used for sending frequency correction
 *  values to the local oscillator.
 */
#define MSG_FREQ_TYPE 1

/*! \brief Value used for message's \c arg2 when sending frequency corrections
 *	to the local oscillator.
 *
 *  Arbitrary chosen number to be used for sending frequency correction values
 *  to the local oscillator. This value is checked by the local oscillator and
 *  the message is discarded if \c arg2 does not match. (Sanity check)
 */
#define MSG_FREQ_ARG2 -1.42

/*! \brief Key to be used by the frequency offset tag send by the local
 *	oscillator.
 */
const pmt::pmt_t LO_TAG_KEY = pmt::mp("freq_offset");

#endif /* INCLUDED_CCSDS_LO_FEEDBACK_H */
