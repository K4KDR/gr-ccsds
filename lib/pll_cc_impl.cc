#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pll_cc_impl.h"
#include <gnuradio/io_signature.h>
#include "ccsds_utils.h"
#include <volk/volk.h>
#include <stdio.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <volk/volk.h>

//#define PLL_DEBUG

namespace gr {
  namespace ccsds {

    pll_cc::sptr 
    pll_cc::make(unsigned int m, float loop_bandwidth) {
        return gnuradio::get_initial_sptr (new pll_cc_impl(m, loop_bandwidth) );
    }
    
    pll_cc_impl::pll_cc_impl (unsigned int m, float loop_bandwidth)
      : gr::block ("pll_cc",
    	gr::io_signature::make (1, 1, sizeof (gr_complex)),
    	gr::io_signature::make (1, 1, sizeof (gr_complex))), d_M(m)
    {
    	const int alignment_multiple = static_cast<int>(volk_get_alignment() / sizeof(gr_complex));
    	set_alignment(utils::pick_larger<int>(1, alignment_multiple));
    
    	d_filter = ccsds_make_lpf2(loop_bandwidth, 0.5, 1.0);
    
    	////message_port_register_out(pmt::intern("freq"));
    
    /*
     *removed lo_feedback
     *
    	d_lo_msg_tag = false;
    	d_lo_msg_offset = 0;
    	send_freq_estimate(0.0);
   */ 
    
    	d_phi_hat = 0.0f;
    
    	#ifdef PLL_DEBUG
    		dbg_count    = 0;
    		dbg_count_lo = 0;
    
    		dbg_file = fopen("/tmp/ccsds_pll_debug.dat","w");
    		dbg_file_lo = fopen("/tmp/ccsds_pll_debug_lo.dat","w");
    		if(dbg_file == NULL || dbg_file_lo == NULL) {
    			fprintf(stderr,"ERROR PLL: can not open debug file\n");
    			exit(EXIT_FAILURE);
    			return;
    		}
    		fprintf(dbg_file, "#k,phi_raw,phi_hat,arg(in),arg(in^M),abs(in),real(in),imag(in),freq_est\n");
    		fprintf(dbg_file_lo, "#k,freq_raw, freq_filtered\n");
    	#endif
    }
    
    pll_cc_impl::~pll_cc_impl ()
    {
    	delete d_filter;
    
    	#ifdef DLL_DEBUG
    		fflush(dbg_file);
    		fflush(dbg_file_lo);
    
    		fclose(dbg_file);
    		fclose(dbg_file_lo);
    	#endif
    }
    
    const double pll_cc_impl::D_TWOPI=2.0f*M_PI;
    
    void pll_cc_impl::remove_modulation(gr_complex *out, const gr_complex *in, const unsigned int num) {
    	// FIXME volk power block seems to calculate wrong results 
    	// (e.g. 1 squared is -1)	
    	if(true || is_unaligned()) {
    		for(unsigned int i=0;i<num;i++) {
    			out[i] = std::pow(in[i],d_M);
    		}
    	} else {
    		volk_32fc_s32f_power_32fc_a(out,in,(float)d_M,num);
    	}
    	return;
    }
    
    void pll_cc_impl::calc_phases(float *tmp_f, const gr_complex *tmp_c, const unsigned int num) {
    	// tmp_c and tmp_f are guaranteed to be aligned
    	volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, (float)d_M, num);
    	//volk_32fc_s32f_atan2_32f_a(tmp_f, tmp_c, 1.0f, num);
    
    	/* without volk
    	for(unsigned int i=0;i<num;i++) {
    			tmp_f[i] = std::arg(tmp_c[i]);
    	}
    	//*/
    	return;
    }
    
    /*
     *removed lo_feedback
     *
    void pll_cc_impl::check_lo_tags(const uint64_t from, const unsigned int num) {
    	if(d_lo_msg_tag) {
    		// we already have received a tag and are waiting
    		return;
    	}
    
    	std::vector<gr_tag_t> tags;
    
    	// default tag is out of range
    	uint64_t offset = from+num;
    
    	//read all tags associated with port 0 in given range
    	this->get_tags_in_range(tags, 0, from, from+num, LO_TAG_KEY);
    
    	// did we receive new tags?
    	if(tags.size() == 0) {
    		return;
    	} else {
    
    		// go through all tags and find the earliest one
    		for(unsigned int i=0;i<tags.size();i++) {
    			offset = (tags[i].offset < offset) ? tags[i].offset : offset;
    		}
    
    		d_lo_msg_tag = true;
    		d_lo_msg_offset = (unsigned int) offset-from + PLL_FREQ_UPDATE;
    
    		return;
    	}
    }
    */

    /*
     *removed lo_feedback
     *
    void pll_cc_impl::send_freq_estimate(double est) {
    	pmt::pmt_t freq = pmt::from_double(est);
    
    	// now put the new message in the queue
    	message_port_pub(pmt::intern("freq"), freq);
    	return;
    }
    */
    void pll_cc_impl::calc_rotation(gr_complex *out, const gr_complex *in, const float *tmp_f, const unsigned int num) {
    	//* with volk	
    	gr_complex *rot = (gr_complex*)volk_malloc(sizeof(gr_complex)*num, volk_get_alignment());
    
    	// temp variable for sine and cosine part of rotator
    	float tmp_s, tmp_c;
    	// create the inidividual rotos
    	for(unsigned int i=0;i<num;i++) {
    		// calculate sine and cosine values for this phase. joint cal-
    		// culation for the same angle is faster, than two individual
    		// calls to sin and cos.
    		sincosf(tmp_f[i],&tmp_s, &tmp_c);
    
    		// assemble the rotator and store it in an array
    		rot[i] = gr_complex(tmp_c,-tmp_s);
    	}
    
    	if(is_unaligned()) {
    		volk_32fc_x2_multiply_32fc_u(out, in, rot, num);
    	} else {
    		volk_32fc_x2_multiply_32fc_a(out, in, rot, num);
    	}
    
    	volk_free(rot);
    	// */
    
    	/* without volk
    	gr_complex rotator;
    	for(unsigned int i=0;i<num;i++) {
    		// do the phase integration in double precision
    		d_phase = std::fmod(d_phase-(double)tmp_f[i], D_TWOPI);
    		// the rotation itself can be single precision
    		rotator = std::polar(1.0f,(float)d_phase);
    		out[i] = in[i] * rotator;
    	}
    	//*/
    	return;
    }
    
    int  pll_cc_impl::general_work (int                     noutput_items,
                                    gr_vector_int               &ninput_items,
                                    gr_vector_const_void_star   &input_items,
                                    gr_vector_void_star         &output_items)
    {
    	const gr_complex *in = (const gr_complex *) input_items[0];
    	gr_complex *out = (gr_complex *) output_items[0];
    	
    	// how many samples can we process?
    	const unsigned int num = static_cast<unsigned int>(utils::pick_smaller(noutput_items, ninput_items[0]));
    	
    	// auxilliary variables
    	gr_complex *tmp_c;
    	float *tmp_f, *tmp_freq;
    
    	// allocate temporary memory
    	tmp_c = (gr_complex *) volk_malloc(num * sizeof(gr_complex), volk_get_alignment());
    	tmp_f = (float *)      volk_malloc(num * sizeof(float), volk_get_alignment());
    	tmp_freq = (float *)      volk_malloc(num * sizeof(float), volk_get_alignment());
    	if (tmp_c == 0 || tmp_f == 0 || tmp_freq == 0) {
    		fprintf(stderr,"ERROR: allocation of memory failed\n");
    		exit(EXIT_FAILURE);
    		return 0;
    	}
    
    	//
    	// do the synchronization
    	//
    	
    	memcpy(tmp_c,in,num*sizeof(gr_complex));
    	
    	// remove the modulation
    	remove_modulation(tmp_c,tmp_c,num);
    
    	#ifdef PLL_DEBUG
    		gr_complex *mod_removed;
    		mod_removed = new gr_complex[num];
    		memcpy(mod_removed, tmp_c, num*sizeof(gr_complex));
    	#endif
    
    	// take the calculated phasors and calculate the phase
    	calc_phases(tmp_f, tmp_c, num);
    	
    	#ifdef PLL_DEBUG
    		float *phase_unfiltered;
    		phase_unfiltered = new float[num];
    		memcpy(phase_unfiltered, tmp_f, num*sizeof(float));
    	#endif
    
    	// Put these calculations into the filter
    	d_filter->filter_wrapped(tmp_f, tmp_freq, static_cast<float>(M_PI)/static_cast<float>(d_M),num);
    	
    	
    	// rotate the samples according to the filtered phase
    	calc_rotation(out, in, tmp_f, num);
    
    	#ifdef PLL_DEBUG
    		for(unsigned int i=0;i<num;i++) {
    			fprintf(dbg_file, "%d,%f,%f,%f,%f,%f,%f,%f,%f\n",dbg_count++,phase_unfiltered[i]/M_PI,tmp_f[i]/M_PI,std::arg(in[i])/M_PI,std::arg(mod_removed[i])/M_PI,
    					std::abs(in[i]),std::real(in[i]),std::imag(in[i]),tmp_freq[i]);
    		}
    		delete[] phase_unfiltered;
    		delete[] mod_removed;
    	#endif
    
    	// free resources
    	volk_free(tmp_f);
    	volk_free(tmp_freq);
    	volk_free(tmp_c);
    
    	// Tell runtime how many input samples we used
    	consume_each(static_cast<int>(num));
    
    	// Tell runtime system how many output items we produced
    	return static_cast<int>(num);
    }

  } // namespace ccsds
} // namespace gr
