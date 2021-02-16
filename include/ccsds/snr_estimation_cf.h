/* -*- c++ -*- */
/*
 * Copyright 2021 Martin Luelf <mail@mluelf.de>.
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

#ifndef INCLUDED_CCSDS_SNR_ESTIMATION_CF_H
#define INCLUDED_CCSDS_SNR_ESTIMATION_CF_H

#include <ccsds/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief <+description of block+>
     * \ingroup ccsds
     *
     */
    class CCSDS_API snr_estimation_cf : virtual public gr::sync_decimator
    {
     public:
      typedef boost::shared_ptr<snr_estimation_cf> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ccsds::snr_estimation_cf.
       *
       * To avoid accidental use of raw pointers, ccsds::snr_estimation_cf's
       * constructor is in a private implementation
       * class. ccsds::snr_estimation_cf::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t decimation);
    };

  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_SNR_ESTIMATION_CF_H */

