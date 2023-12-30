#ifndef INCLUDED_CCSDS_REVERSEBITS_H
#define INCLUDED_CCSDS_REVERSEBITS_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ccsds {
     /*!
      *  \brief reverses bit order in selected bytes of the input message.
      *
      *  \ingroup Misc
      *
      *  The block has an input message port named "in" at which new frames can
      *  arrive. On arrival a new message is created consisting of the input message
      *  but were selected bytes have a reverse order. So if the byte started with
      *  MSB it will now start with the LSB. This new message is then queued in the
      *  output message port named "out".
      */
     class CCSDS_API reversebits : virtual public gr::sync_block
     {
     public:
	typedef std::shared_ptr<reversebits> sptr;
	/*
	 * We use std::shared_ptr's instead of raw pointers for all access
	 * to gr_blocks (and many other data structures).  The shared_ptr gets
	 * us transparent reference counting, which greatly simplifies storage
	 * management issues.  This is especially helpful in our hybrid
	 * C++ / Python system.
	 *
	 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
	 *
	 * As a convention, the _sptr suffix indicates a std::shared_ptr
	 */

	static sptr make(const unsigned int msg_length, const std::vector<unsigned int> &reverse_indices);
        /*!
         *  \brief Return a shared_ptr to a new instance of ccsds::reversebits
         *
         *  Create an instance of ccsds_reversebits and return it's shared_ptr.
         *
         *  \param msg_length Expected length of input and output messages in bytes.
	 *	Messages with a different size are discarded after printing a warning.
         *  \param reverse_indices Indices of the bytes within the message that should be
	 *	reversed. Values within this vector should be in the range from 0
	 *	to msg_length - 1. If all bytes should be reversed this array should
	 *	contain frame_length elements from 0 to msg_length-1.
         *  \return Shared pointer to the created block
         */
    };

  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_REVERSEBITS_H */
