#ifndef INCLUDED_CCSDS_TICP_FRAME_SOURCE_H
#define INCLUDED_CCSDS_TICP_FRAME_SOURCE_H

#include <ccsds/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ccsds {
    
    /*! \brief Reads frames from a TicpServer and outputs it as a stream.
     *
     *  \ingroup ticp
     */
    class CCSDS_API ticp_frame_source : virtual public gr::block
    {
    public:
    	typedef boost::shared_ptr<ticp_frame_source> sptr;
    
    	CCSDS_API sptr make(std::string hostname, unsigned int port, const unsigned int frame_length, const uint8_t data_type, 
    	   std::vector<std::string> map_names, std::vector<uint8_t> map_types, const double frame_rate=0.0);
    };

  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_TICP_FRAME_SOURCE_H */
