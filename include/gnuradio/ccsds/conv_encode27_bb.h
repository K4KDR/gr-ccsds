#ifndef INCLUDED_CCSDS_conv_encode27_bb_H
#define INCLUDED_CCSDS_conv_encode27_bb_H

#include <gnuradio/ccsds/api.h>
#include <gnuradio/block.h>
#include <gnuradio/ccsds/conv_puncturing27.h>

namespace gr {
  namespace ccsds {

    /*! \brief Perform convloutional encoding and puncturing on the input stream.
     *  \ingroup channelcoding
     *
     *  \param gen_poly_c1 Compact generator polynom for c1 stream. Default is 0x79.
     *  \param gen_poly_c2 Compact generator polynom for c2 stream. Default is 0xDB.
     *  \param puncturing_type Symbol for type of puncturing. Default is NONE.
     */
    class CCSDS_API conv_encode27_bb : virtual public gr::block 
    {
    
    public:
    	typedef std::shared_ptr<conv_encode27_bb> sptr;
    	static sptr make (const unsigned char gen_poly_c1, const unsigned char gen_poly_c2, conv_puncturing27::punct_t puncturing_type);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_conv_encode27_bb_H */
