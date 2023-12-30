#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mpsk_detector_soft_cf_impl.h"
#include <gnuradio/ccsds/softbits.h>
#include "ccsds_utils.h"
#include <gnuradio/io_signature.h>


namespace gr {
  namespace ccsds {

    mpsk_detector_soft_cf::sptr
    mpsk_detector_soft_cf::make (const unsigned int M, float phase_offset_points)
    {
        return gnuradio::get_initial_sptr (new mpsk_detector_soft_cf_impl (M, phase_offset_points));
    }
    
    mpsk_detector_soft_cf_impl::mpsk_detector_soft_cf_impl (const unsigned int M, float phase_offset_points) 
    	: gr::sync_interpolator("mpsk_detector_soft_cf",
    	  gr::io_signature::make (1, 1, sizeof (gr_complex)),
    	  gr::io_signature::make (1, 1, sizeof (float)),get_ldM(M)),
    	d_M(M),
    	d_ldM(get_ldM(M)),
    	d_TWOPI(static_cast<float>(2.0*M_PI)),
		d_PHASE_OFFSET_RAD(phase_offset_points*d_TWOPI/static_cast<float>(M))
    {
    	if(d_M > 256) {
    		fprintf(stderr,"ERROR MPSK DETECTOR SOFT: modulation order M=%d to high (maximum 256).\n",d_M);
    		exit(EXIT_FAILURE);
    		return;
    	}
    
    	map = make_mpsk_map(M);
    
    	set_alignment(utils::pick_smaller<size_t, int>(1lu, volk_get_alignment() / sizeof(gr_complex)));
    
    	set_output_multiple(static_cast<int>(d_ldM));
    }
    
    mpsk_detector_soft_cf_impl::~mpsk_detector_soft_cf_impl ()
    {
    	delete[] map;
    }
    
    unsigned int mpsk_detector_soft_cf_impl::get_ldM(const unsigned int M) {
    	const unsigned int ret = (unsigned int) round(log(M) / log(2.0f));
    	return ret;
    }
    
    int  mpsk_detector_soft_cf_impl::work (int                 noutput_items,
                                    gr_vector_const_void_star   &input_items,
                                    gr_vector_void_star         &output_items)
    {
    	const gr_complex *in = (const gr_complex *) input_items[0];
    	float *out = (float *) output_items[0];
    
    	if(static_cast<unsigned int>(noutput_items)%d_ldM != 0u) {
    		fprintf(stderr,"ERROR MPSK DETECTOR SOFT: Requested %u output samples, which is not a multiple of %u.\n",noutput_items, d_ldM);
    		exit(EXIT_FAILURE);
    		return 0;
    	}
    
    	const unsigned int num_in = static_cast<unsigned int>(noutput_items)/d_ldM;
    
    	if(num_in == 0) {
    		printf("nothing to do, because of lacking input items\n");
    		return 0;
    	}
    
    	float *tmp_angle = (float*)volk_malloc(num_in*sizeof(float), volk_get_alignment());
    	float *tmp_mag = (float*)volk_malloc(num_in*sizeof(float), volk_get_alignment());
    
    	if(tmp_angle == 0 || tmp_mag == 0) {
    		fprintf(stderr,"ERROR MPSK DETECTOR SOFT: allocation of memory failed\n");
    		exit(EXIT_FAILURE);
    		return 0;
    	}
    
		// input is aligned, use aligned call directly
		volk_32fc_s32f_atan2_32f(tmp_angle, in, 1.0f, num_in);
    
		volk_32fc_magnitude_squared_32f(tmp_mag, in, num_in);
    	
    	// preprocessing for whole vector is completed now, create the single
    	// bits out of phase and magnitude information 
    
		const float shift = d_TWOPI / static_cast<float>(d_M) / 2.0f;  // shift by half  of the angle spanning the decision area so we can floor to the correct result
    	const float factor = static_cast<float>(d_M) / d_TWOPI;
		
    	// go trough all symbols
    	for(unsigned int i=0;i<num_in;i++) {
			const float tmp = std::fmod(tmp_angle[i]+d_TWOPI-d_PHASE_OFFSET_RAD+shift,d_TWOPI) * factor;
    		
			uint8_t sym_bits = map[(unsigned int) std::floor(tmp)%d_M];
    		for(unsigned int j=0;j<d_ldM;j++) {
    			const bool bit = (sym_bits>>(d_ldM-1-j)) & 0x01;
    			out[i*d_ldM+j] = softbits::create_from_sample(bit ? tmp_mag[i] : -tmp_mag[i]);
			}
    	}
    
    	volk_free(tmp_angle);
    	volk_free(tmp_mag);
    
    	// Tell runtime system how many output items we produced.
    	return noutput_items;
    }

  } // namespace ccsds
} // namespace gr
