/* -*- c++ -*- */
/* 
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


#ifndef INCLUDED_CCSDS_SIMPLE_BPSK_SNR_QF_H
#define INCLUDED_CCSDS_SIMPLE_BPSK_SNR_QF_H

#include <ccsds/api.h>
#include <gnuradio/block.h>


namespace gr {
  namespace ccsds {

    /*!
     * \brief estimates the SNR
     * \ingroup ccsds
     *
     * This block takes a number of samples and computes the SNR of the 
     * real and imaginary part of the symbols and the symbols as well
     * as the complex SNR
     */
    class CCSDS_API simple_bpsk_SNR_qf : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<simple_bpsk_SNR_qf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::simple_bpsk_SNR_qf.
       *
       * To avoid accidental use of raw pointers, ccsds::simple_bpsk_SNR_qf's
       * constructor is in a private implementation
       * class. ccsds::simple_bpsk_SNR_qf::make is the public interface for
       * creating new instances.
       */
      static sptr make(unsigned long window_size);
      virtual float get_SNR_real() const = 0;	//!< SNR of the real part
      virtual float get_SNR_imag() const = 0;	//!< SNR of the imaginary part
      virtual float get_SNR_magn() const = 0;	//!< complex SNR 
      virtual unsigned long get_window_size() const = 0;
      virtual void set_window_size(unsigned long) = 0;
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SIMPLE_BPSK_SNR_QF_H */

