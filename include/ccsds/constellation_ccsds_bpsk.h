/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf <mail@mluelf.de>.
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


#ifndef INCLUDED_CCSDS_CONSTELLATION_CCSDS_BPSK_H
#define INCLUDED_CCSDS_CONSTELLATION_CCSDS_BPSK_H

#include <ccsds/api.h>

#include <gnuradio/digital/constellation.h>

namespace gr {
  namespace ccsds {

    /*!
     * \brief Digital constellation for BPSK.
     * 
     * \details BPSK symbols have unit energy and are consistent with both
     * CCSDS 401.0-B30 (RADIO FREQUENCY AND MODULATION SYSTEMS - Part 1)
     * as well as
     * ECSS-E-ST-50-05C Rev. 2 (Radio frequency and modulation).
     * 
     * The constellation points are on the real axis as depicted in the following:
     * \verbatim
     *     ^
     *     |
     * --1-|-0-->
     *     |
     * \endverbatim
     *
     * \ingroup ccsds
     */
    class CCSDS_API constellation_ccsds_bpsk : public gr::digital::constellation {
    public:
      typedef boost::shared_ptr<constellation_ccsds_bpsk> sptr;

      // public constructor
      static sptr make();

      ~constellation_ccsds_bpsk();

      unsigned int decision_maker(const gr_complex* sample) override;
      std::vector<float> calc_soft_dec(gr_complex sample, float npwr) override;

    protected:
      constellation_ccsds_bpsk();
    };
  } // namespace ccsds
} // namespace gr

#endif /* INCLUDED_CCSDS_CONSTELLATION_CCSDS_BPSK_H */

