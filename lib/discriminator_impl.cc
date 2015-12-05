/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "discriminator_impl.h"

namespace gr {
  namespace ccsds {

    discriminator::sptr
    discriminator::make(int status,std::string frame)
    {
      return gnuradio::get_initial_sptr
        (new discriminator_impl(status, frame));
    }

    /*
     * The private constructor
     */
    discriminator_impl::discriminator_impl(int status,std::string frame)
      : gr::sync_interpolator("discriminator",
              gr::io_signature::make(1, 1, sizeof (float)),
              gr::io_signature::make(0, 0, sizeof (float)), 0)
    {}

    /*
     * Our virtual destructor.
     */
    discriminator_impl::~discriminator_impl()
    {
    }

    int
    discriminator_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
       // const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
        //<+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

        // Do <+signal processing+>

        // Tell runtime system how many output items we produced.
 	//void rs_decode_impl()
	//{
	//	return frame_number
//}
        return noutput_items;
    }

  } /* namespace ccsds */
} /* namespace gr */

