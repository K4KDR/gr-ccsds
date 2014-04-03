#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dll_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <cstdio>

//#define DLL_DEBUG
namespace gr {
  namespace ccsds {

    dll_cc::sptr 
    dll_cc::make(unsigned int osf, float gamma) {
        return gnuradio::get_initial_sptr (new dll_cc_impl(osf,gamma) );
    }
    
    dll_cc_impl::dll_cc_impl (unsigned int osf, float gamma)
      : gr::block ("dll_cc",
    	gr::io_signature::make (1, 1, sizeof (gr_complex)),
    	gr::io_signature::make (1, 1, sizeof (gr_complex))), d_OSF(osf), d_OSF_HALF((float)osf/2.0f), d_GAMMA(gamma)
    {
    
    	set_fixed_rate(false);
    	
    	set_tag_propagation_policy(TPP_DONT);
    
    	// not initialized, we will init the other variables in general_work
    	d_init = false;
    
    	// temp values for interp_cvalue
    	d_real = new float[DLL_INTERP_NUMSAMPS];
    	d_imag = new float[DLL_INTERP_NUMSAMPS];
    
    	d_tag_count = 0;
    
    	#ifdef DLL_DEBUG
    		dbg_count = 0;
    		dbg_toggle = false;
    
    		dbg_file_o = fopen("/tmp/ccsds_dll_debug_orig.dat","w");
    		dbg_file_i = fopen("/tmp/ccsds_dll_debug_intp.dat","w");
    		dbg_file_s = fopen("/tmp/ccsds_dll_debug_symb.dat","w");
    		dbg_file_t = fopen("/tmp/ccsds_dll_debug_tauh.dat","w");
    		if(dbg_file_o == NULL || dbg_file_i == NULL || dbg_file_s == NULL || dbg_file_i == NULL) {
    			fprintf(stderr,"ERROR DLL: can not open debug file\n");
    			exit(EXIT_FAILURE);
    			return;
    		}
    		fprintf(dbg_file_o, "#k,real(y_hat),imag(y_hat),abs(y_hat),arg(y_hat)\n");
    		fprintf(dbg_file_s, "#k,real(y_hat),imag(y_hat),abs(y_hat),arg(y_hat)\n");
    		fprintf(dbg_file_i, "#k,real(y_hat),imag(y_hat),abs(y_hat),arg(y_hat)\n");
    		fprintf(dbg_file_t, "#k,e_tau,tau_hat,l,mu\n");
    	#endif
    }
    
    dll_cc_impl::~dll_cc_impl () {
    	#ifdef DLL_DEBUG
    		fflush(dbg_file_o);
    		fflush(dbg_file_s);
    		fflush(dbg_file_i);
    		fflush(dbg_file_t);
    
    		fclose(dbg_file_o);
    		fclose(dbg_file_s);
    		fclose(dbg_file_i);
    		fclose(dbg_file_t);
    	#endif
    
    	delete[] d_real;
    	delete[] d_imag;
    }
    
    inline float dll_cc_impl::get_frac(float value) {
    	return std::fmod(value,1.0f);
    }
    
    inline int dll_cc_impl::get_int(float value) {
    	return (int) std::floor(value);
    }
    
    
    void dll_cc_impl::forecast(int noutput_items,gr_vector_int &ninput_items_required){
    	// basically decimating by a factor of d_OSF, but for interpolation we
    	// need to see 2 more samples "into the future"
    	ninput_items_required[0] = d_OSF * noutput_items + 4;
    }
    
    inline void dll_cc_impl::to_real(float *out, const gr_complex *in, const unsigned int num) {
    	for(unsigned int i=0;i<num;i++) {
    		out[i] = std::real(in[i]);
    	}
    	return;
    }
    
    inline void dll_cc_impl::to_imag(float *out, const gr_complex *in, const unsigned int num) {
    	for(unsigned int i=0;i<num;i++) {
    		out[i] = std::imag(in[i]);
    	}
    	return;
    }
    
    inline void dll_cc_impl::to_real4(float *out, const gr_complex *in) {
    	out[0] = std::real(in[0]);
    	out[1] = std::real(in[1]);
    	out[2] = std::real(in[2]);
    	out[3] = std::real(in[3]);
    
    	return;
    }
    
    inline void dll_cc_impl::to_imag4(float *out, const gr_complex *in) {
    	out[0] = std::imag(in[0]);
    	out[1] = std::imag(in[1]);
    	out[2] = std::imag(in[2]);
    	out[3] = std::imag(in[3]);
    
    	return;
    }
    
    /*
    	Gardner timing error detector for passband signals
    
    	e(k) = re{ [ y(kT-T+tau[k-1]) - y(kT+tau[k]) ] y*(kT-T/2+tau[k-1]) }
    	Mengali p. 431
    */
    inline float dll_cc_impl::gardner(gr_complex previous, gr_complex intermediate, gr_complex current) {
    	return std::real( (previous-current) * std::conj(intermediate) );
    }
    
    gr_complex dll_cc_impl::interpolate_cvalue(const gr_complex *y, float mu) {
    	to_real4(d_real, y);
    	to_imag4(d_imag, y);
    
    	gr_complex ret = gr_complex(interpolate_value(d_real, mu), interpolate_value(d_imag, mu));
    
    	return ret;
    }
    
    inline float dll_cc_impl::interpolate_value(const float *y, float mu) {
    
    
    	/* original code
    	float a,b,c,d;
    
    	a = y[3] - y[2] - y[0] + y[1];
    	b = y[0] - y[1] - a;
    	c = y[2] - y[0];
    	d = y[1];
    
    	return(a*mu*mu*mu+b*mu*mu+c*mu+d);
    	*/
    
    	// optimized code
    	float a,b,c,tmp,mu_squared;
    
    	// calculate values that have to be computed more than once
    	tmp = y[0] - y[1];
    	mu_squared = mu*mu;
    
    	a = y[3] - y[2] - tmp;
    	b = tmp - a;
    	c = y[2] - y[0];
    
    	return(a*mu_squared*mu+b*mu_squared+c*mu+y[1]);
    
    }
    
    void dll_cc_impl::propagate_tags(const unsigned int num_in) {
    	std::vector<gr::tag_t> tags;
    	uint64_t tmp_to = d_tag_count + (uint64_t)num_in;
    
    	this->get_tags_in_range(tags, 0, d_tag_count, tmp_to);
    
    
    	gr::tag_t *tag;
    	for(unsigned int i=0;i<tags.size();i++) {
    		// buffer array access
    		tag = &tags[i];
    
    		// rescale offset
    		tag->offset = tag->offset / d_OSF;
    
    		// add tag back into stream
    		this->add_item_tag(0, *tag);
    	}
    
    	d_tag_count += (uint64_t) num_in;
    }
    
    int  dll_cc_impl::general_work (int                     noutput_items,
                                    gr_vector_int               &ninput_items,
                                    gr_vector_const_void_star   &input_items,
                                    gr_vector_void_star         &output_items)
    {
    	const gr_complex *in = (const gr_complex *) input_items[0];
    	gr_complex *out = (gr_complex *) output_items[0];
    	//float *freq = (float *) output_items[1];
    	
    	// how many samples can we process?
    	unsigned int num = ninput_items[0];
    	
    	// how many samples to output, when we want to have 2*d_OSF spare
    	// samples at the end and one at the front, to ensure the interpolator
    	// never has to extrapolate
    	//int num_out = std::floor((num-1)/d_OSF) -2;
    
    	// not enough samples for one iteration
    	if(num < d_OSF+4) {
    		consume_each(0);
    		return 0;
    	}
    
    	// //
    	// // //
    	// // // // Init
    	// // //
    	// //
    
    	if(!d_init) {
    		d_tau_hat = 0.0f;
    		d_l = 1;
    		d_mu = 0.0f;
    		
    		#ifdef DLL_DEBUG
    			// debug initial parameters
    			fprintf(dbg_file_t, "%f,%2.10f,%2.10f,%d,%2.10f\n",0.0f, 0.0f, d_tau_hat, d_l, d_mu);
    		#endif
    
    		// retrieve first symbol
    		// mu is zero, so take the sample directly
    		d_last_interp[PREV] = in[1];
    
    		// update indices
    		d_l += get_int(d_mu + d_OSF_HALF);
    		d_mu = get_frac(d_mu + d_OSF_HALF);
    
    		#ifdef DLL_DEBUG
    			// debug parameters, updated mu and d_l
    			fprintf(dbg_file_t, "%f,%2.10f,%2.10f,%d,%2.10f\n",(float)d_l/(float)d_OSF, 0.0f, d_tau_hat, d_l, d_mu);
    		#endif
    
    		// retrieve intermediate sample
    		d_last_interp[INTM] = interpolate_cvalue(&in[d_l-1], d_mu);
    
    		// update indices
    		d_l += get_int(d_mu + d_OSF_HALF);
    		d_mu = get_frac(d_mu + d_OSF_HALF);
    
    		#ifdef DLL_DEBUG
    			// debug parameters, update new values
    			fprintf(dbg_file_t, "%f,%2.10f,%2.10f,%d,%2.10f\n",(float)d_l/(float)d_OSF, 0.0f, d_tau_hat, d_l, d_mu);
    		#endif
    
    		// retrieve second symbol
    		d_last_interp[CURR] = interpolate_cvalue(&in[d_l-1], d_mu);
    
    		// update tau_hat
    		d_gamma_eps = gardner(d_last_interp[PREV], d_last_interp[INTM], d_last_interp[CURR]);
    		if(d_gamma_eps != 0.0f) {
    			d_gamma_eps *= d_GAMMA;
    		}
    
    		d_tau_hat += d_gamma_eps/(float)d_OSF;
    		// update indices
    		d_l += get_int(d_mu + d_OSF_HALF + d_gamma_eps);
    		d_mu = get_frac(d_mu + d_OSF_HALF + d_gamma_eps);
    
    		#ifdef DLL_DEBUG
    			// debug interpolated symbols
    			fprintf(dbg_file_i, "%f,%f,%f,%f,%f\n",(float)1.0f             /(float)d_OSF ,real(d_last_interp[PREV]),imag(d_last_interp[PREV]),
    								abs(d_last_interp[PREV]),arg(d_last_interp[PREV])/M_PI);
    
    			fprintf(dbg_file_i, "%f,%f,%f,%f,%f\n",(float)(1.0f+d_OSF_HALF)/(float)d_OSF ,real(d_last_interp[INTM]),imag(d_last_interp[INTM]),
    								abs(d_last_interp[INTM]),arg(d_last_interp[INTM])/M_PI);
    
    			fprintf(dbg_file_i, "%f,%f,%f,%f,%f\n",(float)(1.0f+d_OSF     )/(float)d_OSF ,real(d_last_interp[CURR]),imag(d_last_interp[CURR]),
    								abs(d_last_interp[CURR]),arg(d_last_interp[CURR])/M_PI);
    
    
    
    			fprintf(dbg_file_s, "%f,%f,%f,%f,%f\n",(float)(1.0f           )/(float)d_OSF,real(d_last_interp[PREV]),imag(d_last_interp[PREV]),
    								abs(d_last_interp[PREV]),arg(d_last_interp[PREV])/M_PI);
    			
    			fprintf(dbg_file_s, "%f,%f,%f,%f,%f\n",(float)(1.0f+d_OSF     )/(float)d_OSF,real(d_last_interp[CURR]),imag(d_last_interp[CURR]),
    								abs(d_last_interp[CURR]),arg(d_last_interp[CURR])/M_PI);
    			
    			// debug input samples
    			for(unsigned int i=0;i<(unsigned int) std::max(d_l-1, 0);i++) {
    				fprintf(dbg_file_o, "%f,%f,%f,%f,%f\n",(float)(dbg_count++)/(float)d_OSF,real(in[i]),imag(in[i]),abs(in[i]),arg(in[i])/M_PI);
    			}
    
    			// debug
    			fprintf(dbg_file_t, "%f,%2.10f,%2.10f,%d,%2.10f\n",(float)(d_OSF+d_OSF_HALF)/(float)d_OSF, d_gamma_eps, d_tau_hat, d_l, d_mu);
    		#endif
    
    		// initialization done
    		d_init = true;
    
    		// output the two first symbols
    		out[0] = d_last_interp[PREV];
    		out[1] = d_last_interp[CURR];
    
    		// Tell runtime how many input samples we used
    		// next intermediate sample will have basepoint index d_l and
    		// we nedd one previous sample for interpolation
    		consume_each(d_l-1);
    
    		// make sure tags are propagated to the following blocks
    		propagate_tags(d_l-1);
    
    		// next block will contain exactly one sample prior to the
    		// basepoint sample
    		d_l = 1;
    
    		dbg_toggle = !dbg_toggle;
    
    		// Tell runtime system how many output items we produced
    		return 2;
    	} // end init
    
    	// number of samples missing for next intermediate sample
    	int missing = 0;
    	
    	//					         //
    	// //					      // //
    	// // //				   // // //
    	// // // // 		Loop		// // // //
    	// // //				   // // //
    	// //					      // //
    	//					         //
    
    	// number of output symbols
    	unsigned int num_out = 0;
    	while(true) {
    
    		// drop last two interpolants and shift buffer
    		d_last_interp[PREV] = d_last_interp[CURR];
    
    		//
    		// Intermediate Sample
    		//
    
    		// retrieve intermediate sample
    		d_last_interp[INTM] = interpolate_cvalue(&in[d_l-1], d_mu);
    		#ifdef DLL_DEBUG
    			// debug intermediate sample
    			fprintf(dbg_file_i, "%f,%f,%f,%f,%f\n",(float)(dbg_count+d_l+d_mu)/(float)d_OSF,real(d_last_interp[INTM]),imag(d_last_interp[INTM]),
    								abs(d_last_interp[INTM]),arg(d_last_interp[INTM])/M_PI);
    		#endif
    
    
    		//
    		// Second symbol
    		//
    
    		// update indices
    		d_l += get_int(d_mu + d_OSF_HALF);
    		d_mu = get_frac(d_mu + d_OSF_HALF);
    
    		#ifdef DLL_DEBUG
    			// debug parameters for next symbol
    			fprintf(dbg_file_t, "%f,%2.10f,%2.10f,%d,%2.10f\n",(float)(dbg_count+d_l)/(float)d_OSF, d_gamma_eps, d_tau_hat, d_l, d_mu);
    		#endif
    
    		// retrieve second symbol
    		d_last_interp[CURR] = interpolate_cvalue(&in[d_l-1], d_mu);
    		
    		// output the new symbol
    		out[num_out] = d_last_interp[CURR];
    		num_out++;
    
    		#ifdef DLL_DEBUG
    			// debug next symbol
    			fprintf(dbg_file_i, "%f,%f,%f,%f,%f\n",(float)(dbg_count+d_l+d_mu)/(float)d_OSF,real(d_last_interp[CURR]),imag(d_last_interp[CURR]),
    								abs(d_last_interp[CURR]),arg(d_last_interp[CURR])/M_PI);
    
    			fprintf(dbg_file_s, "%f,%f,%f,%f,%f\n",(float)(dbg_count+d_l+d_mu)/(float)d_OSF,real(d_last_interp[CURR]),imag(d_last_interp[CURR]),
    								abs(d_last_interp[CURR]),arg(d_last_interp[CURR])/M_PI);
    		#endif
    
    		//
    		// Next intermediate sample
    		//
    
    		// update tau_hat
    		d_gamma_eps = d_GAMMA * gardner(d_last_interp[PREV], d_last_interp[INTM], d_last_interp[CURR]);
    		d_tau_hat += d_gamma_eps;
    
    		// update indices
    		d_l += get_int(d_mu + d_OSF_HALF + d_gamma_eps);
    		d_mu = get_frac(d_mu + d_OSF_HALF + d_gamma_eps);
    
    		#ifdef DLL_DEBUG
    			// debug parameters for next intermediate sample			
    			fprintf(dbg_file_t, "%f,%2.10f,%2.10f,%d,%2.10f\n",(float)(dbg_count+d_l)/(float)d_OSF, d_gamma_eps, d_tau_hat, d_l, d_mu);
    		#endif
    
    
    
    		// see if we have enough samples left with the new  tau estimate
    		// d_l 		: index of next intermediate symbol
    		// get_int(...) : advance to next symbol
    		// 3		: additional samples for interpolation
    		missing = d_l + get_int(d_mu + d_OSF_HALF) + 3 - num;
    		missing = (missing > 0) ? missing : 0;
    		if( missing > 0 || num_out >= (unsigned int)noutput_items) {
    			break;
    		}
    	}
    
    	#ifdef DLL_DEBUG
    		// debug original incomming samples
    		for(unsigned int i=0;i<(unsigned int) std::max(d_l-1-missing, 0);i++) {
    			fprintf(dbg_file_o, "%f,%f,%f,%f,%f,%d\n",(float)(dbg_count++)/(float)d_OSF,real(in[i]),imag(in[i]),abs(in[i]),arg(in[i])/M_PI,dbg_toggle);
    		}
    		dbg_toggle = !dbg_toggle;
    	#endif
    
    	// Tell runtime how many input samples we used
    	// try to get rid of as many samples as possible, so next call there is
    	// only one sample before the basepoint index left (d_l-1). But do not
    	// consume more samples than we have (missing).
    //	consume_each(d_l-1-missing);
    	consume_each(d_l-1);
    
    	propagate_tags(d_l-1);
    
    	// next block will contain exactly one sample prior to the basepoint
    	// sample or $missing$ more, if we could not consume the full last block
    	// above
    //	d_l = 1+missing;
    	d_l = 1;
    
    	// Tell runtime system how many output items we produced
    	return num_out;
    }

  } // namespace ccsds
} // namespace gr
