#ifndef INCLUDED_CCSDS_MSG_NULL_SRC_IMPL_H
#define INCLUDED_CCSDS_MSG_NULL_SRC_IMPL_H

#include <ccsds/msg_null_src.h>



namespace gr {
  namespace ccsds {
    class msg_null_src_impl : public msg_null_src
    {
    public:
    msg_null_src_impl();
    ~msg_null_src_impl();
    
    int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
         
    private:
    };


  } // namespace ccsds
} // namespace gr
#endif /* INCLUDED_CCSDS_MSG_NULL_SRC_IMPL_H */
