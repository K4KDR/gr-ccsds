#ifndef INCLUDED_CCSDS_FLL_CC_H
#define INCLUDED_CCSDS_FLL_CC_H

#include <gr_block.h>

class ccsds_fll_cc;

typedef boost::shared_ptr<ccsds_fll_cc> ccsds_fll_cc_sptr;

ccsds_fll_cc_sptr ccsds_make_fll_cc ();

class ccsds_fll_cc : public gr_block
{
private:
    friend ccsds_fll_cc_sptr ccsds_make_fll_cc ();

    ccsds_fll_cc ();   // private constructor

public:
    ~ccsds_fll_cc ();  // public destructor
    int general_work (  int                         noutput_items,
                        gr_vector_int               &ninput_items,
                        gr_vector_const_void_star   &input_items,
                        gr_vector_void_star         &output_items);
};

#endif /* INCLUDED_CCSDS_FLL_CC_H */
