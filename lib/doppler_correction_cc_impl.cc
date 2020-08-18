/* -*- c++ -*- */
/* 
 * Copyright 2019 Martin Luelf.
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

#include "doppler_correction_cc_impl.h"
#include "ccsds_utils.h"

// Always test assertions
#undef NDEBUG
#include <cassert>
#include <cstdlib>

#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace ccsds {

    doppler_correction_cc::sptr
    doppler_correction_cc::make(std::vector<std::string> tles,
                                double sampl_rate,
                                double t_update,
                                double frequency,
                                std::vector<double> lla,
                                unsigned int blocksize
                               )
    {
      assert(lla.size() == 3u);
      const std::array<double,3> lla_array = {lla[0], lla[1], lla[2]};
      
      assert(tles.size() == 2u || tles.size() == 3u);
      const size_t indx_line1 = tles.size()-2u;
      const size_t indx_line2 = tles.size()-1u;
      assert(tles[indx_line1][0] == '1');
      assert(tles[indx_line2][0] == '2');
      
      const std::array<std::string,2> tles2 = {tles[indx_line1], tles[indx_line2]};
      
      return gnuradio::get_initial_sptr
        (new doppler_correction_cc_impl(tles2, sampl_rate, t_update, frequency, lla_array, blocksize));
    }

    /*
     * The private constructor
     */
    doppler_correction_cc_impl::doppler_correction_cc_impl(
              std::array<std::string,2> tles,
              double sampl_rate,
              double t_update,
              double frequency,
              std::array<double,3> lla,
              unsigned int blocksize)
      : gr::sync_block("doppler_correction_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_scale(static_cast<float>(2.0*M_PI*frequency/299792.458)),  // to be multiplied with ranges in km
        d_blocksize(blocksize),
        d_range(tles[0], tles[1], lla, sampl_rate, 1.0/t_update) {
      const size_t _alignment = volk_get_alignment();
      set_alignment(utils::pick_larger<size_t, int>(1lu, _alignment/sizeof(gr_complex)));
      
      // Set output_mulitple after alignment, because alignment sets it back, but make sure we are not losing alignment
      if (d_blocksize % static_cast<size_t>(alignment()) != 0) {
        printf("gr-ccsds::doppler_correction_cc: ERROR: Blocksize %u must be a multiple of the SIMD alignment %d\n", d_blocksize, alignment());
        exit( EXIT_FAILURE );
      }
      set_output_multiple(static_cast<int>(d_blocksize));
      
      d_tmp_f1 = (float *) volk_malloc(d_blocksize*sizeof(float), _alignment);
      d_tmp_f2 = (float *) volk_malloc(d_blocksize*sizeof(float), _alignment);
      d_tmp_c  = (gr_complex *) volk_malloc(d_blocksize*sizeof(gr_complex), _alignment);
      d_tmp_d = (double *) d_tmp_c;
      
    }

    /*
     * Our virtual destructor.
     */
    doppler_correction_cc_impl::~doppler_correction_cc_impl()
    {
      volk_free(d_tmp_f1);
      volk_free(d_tmp_f2);
      volk_free(d_tmp_c);
    }

    bool doppler_correction_cc_impl::start() {
      d_range.setStartTimeNow();
      return true;
    }
    
    int
    doppler_correction_cc_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      
      assert(noutput_items >= 0);
      const size_t nout = static_cast<size_t>(noutput_items);
      if (nout < d_blocksize) {
        printf("gr-ccsds::doppler_correction_cc::work: WARNING: Requested number of output samples %lu is lower than blockl size %u, so no computation will be performed.\n", nout, d_blocksize);
      }
      assert(nout >= d_blocksize);
      
      const size_t num_iterations = nout/d_blocksize;
      for (size_t i=0; i<num_iterations; i++) {
        // write ranges to d_tmp_f1
        d_range.computeDelta(d_tmp_d, d_blocksize);
        
        for(size_t j=0; j<d_blocksize; j++) {
          d_tmp_f2[j] = static_cast<float>(std::remainder(d_tmp_d[j]*d_scale, 2.0*M_PI));
        }
        /*
        for(size_t j=1; j<d_blocksize; j++) {
          const double freq = d_scale/(2.0*M_PI)*(d_tmp_d[j]-d_tmp_d[j-1])/(1.0/200e3);
          printf("Doppler: %lf Hz, delta Range=%lf\n", freq, d_tmp_d[j]*1e3);
        }
        */
        // tmp_f2 = cos(tmp_f2)
        // tmp_f1 = sin(tmp_f2)
        volk_32f_cos_32f(d_tmp_f1, d_tmp_f2, d_blocksize);
        volk_32f_sin_32f(d_tmp_f2, d_tmp_f2, d_blocksize);
        
        // tmp_c = tmp_f2 + i*tmp_f1
        volk_32f_x2_interleave_32fc(d_tmp_c, d_tmp_f1, d_tmp_f2, d_blocksize);
        
        // out = in * tmp_c
        volk_32fc_x2_multiply_32fc(&out[i*d_blocksize], &in[i*d_blocksize], d_tmp_c, d_blocksize);
      }
      
      return static_cast<int>(num_iterations*d_blocksize);
    }

  } /* namespace ccsds */
} /* namespace gr */

