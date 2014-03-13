#ifndef INCLUDED_CCSDS_TICP_FRAME_SINK_H
#define INCLUDED_CCSDS_TICP_FRAME_SINK_H

#include <ccsds/api.h>
#include <gnuradio/block.h>
#include <string>
#include <vector>

namespace gr {
  namespace ccsds {

     /*	TicpClient.
     *
     *  \ingroup ticp
     */
    class CCSDS_API ticp_frame_sink : virtual public gr::block
    {
    public:
    	typedef boost::shared_ptr<ticp_frame_sink> sptr;
    	static sptr make(unsigned int port, const unsigned int frame_length, const uint8_t data_type, std::vector<std::string> map_names, std::vector<uint8_t> map_types);
    
    };

  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_TICP_FRAME_SINK_H */
