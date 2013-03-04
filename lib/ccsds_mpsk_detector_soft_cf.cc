#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_mpsk_detector_soft_cf.h>
#include "ccsds_mpsk_constellation.h"
#include <gr_io_signature.h>
#include <stdio.h>
#include <math.h>
#include <volk/volk.h>
#include <fftw3.h>


ccsds_mpsk_detector_soft_cf_sptr ccsds_make_mpsk_detector_soft_cf (const unsigned int M)
{
    return ccsds_mpsk_detector_soft_cf_sptr (new ccsds_mpsk_detector_soft_cf (M));
}

ccsds_mpsk_detector_soft_cf::ccsds_mpsk_detector_soft_cf (const unsigned int M) :
	gr_sync_interpolator("ccsds_mpsk_detector_soft_cf",
		gr_make_io_signature (1, 1, sizeof (gr_complex)),
		gr_make_io_signature (1, 1, sizeof (float)),get_ldM(M)),
	d_M(M),
	d_ldM(get_ldM(M)),
	d_TWOPI(2.0*M_PI)
{
	if(d_M > 256) {
		fprintf(stderr,"ERROR MPSK DETECTOR SOFT: modulation order M=%d to high (maximum 256).\n",d_M);
		exit(EXIT_FAILURE);
		return;
	}

	map = ccsds_make_mpsk_map(M);

	const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
	set_alignment(std::max(1, alignment_multiple));

	set_output_multiple(d_ldM);
}

ccsds_mpsk_detector_soft_cf::~ccsds_mpsk_detector_soft_cf ()
{
	delete[] map;
}

unsigned int ccsds_mpsk_detector_soft_cf::get_ldM(const unsigned int M) {
	const unsigned int ret = (unsigned int) round(log(M) / log(2.0f));
	return ret;
}

int  ccsds_mpsk_detector_soft_cf::work (int                 noutput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	float *out = (float *) output_items[0];

	if(noutput_items%d_ldM != 0) {
		fprintf(stderr,"ERROR MPSK DETECTOR SOFT: Requested %u output samples, which is not a multiple of %u.\n",noutput_items, d_ldM);
		exit(EXIT_FAILURE);
		return 0;
	}

	const unsigned int num_in = noutput_items/d_ldM;

	if(num_in == 0) {
		printf("nothing to do, because of lacking input items\n");
		return 0;
	}

	float *tmp_angle = (float*)fftwf_malloc(num_in*sizeof(float));
	float *tmp_mag = (float*)fftwf_malloc(num_in*sizeof(float));

	if(tmp_angle == 0 || tmp_mag == 0) {
		fprintf(stderr,"ERROR MPSK DETECTOR SOFT: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return 0;
	}

	if(!is_unaligned()) {
		// input is aligned, use aligned call directly
		volk_32fc_s32f_atan2_32f_a(tmp_angle, in, 1.0f, num_in);

		volk_32fc_magnitude_squared_32f_a(tmp_mag, in, num_in);
		
	} else {
		// unaligned, copy input to aligned buffer

		// allocate buffer
		gr_complex *tmp_c = (gr_complex*)fftwf_malloc(num_in*sizeof(gr_complex));
		if(tmp_c == 0) {
			fprintf(stderr,"ERROR MPSK DETECTOR SOFT: allocation of memory failed\n");
			exit(EXIT_FAILURE);
			return 0;
		}

		// copy data to buffer
		memcpy(tmp_c, in, num_in*sizeof(gr_complex));

		// process data
		volk_32fc_s32f_atan2_32f_a(tmp_angle, tmp_c, 1.0f, num_in);

		// free buffer
		fftw_free(tmp_c);

		// process magnitude
		volk_32fc_magnitude_squared_32f_u(tmp_mag, in, num_in);
	}		

	// preprocessing for whole vector is completed now, create the single
	// bits out of phase and magnitude information 

	const float add = 1.0f/d_M;
	const float factor = (float)d_M / 2.0f;

	// go trough all symbols
	for(unsigned int i=0;i<num_in;i++) {
		const float tmp = (std::fmod(tmp_angle[i]+d_TWOPI,d_TWOPI)/M_PI + add ) * factor;
		
		uint8_t sym_bits = map[(unsigned int) std::floor(tmp)%d_M];
		
		for(unsigned int j=0;j<d_ldM;j++) {
			const bool bit = (sym_bits>>(d_ldM-1-j)) & 0x01;
		
			out[i*d_ldM+j] = (bit ? 1.0f : -1.0f) * std::min(tmp_mag[i], 1.0f);
		}
	}

	fftw_free(tmp_angle);
	fftw_free(tmp_mag);

	// Tell runtime system how many output items we produced.
	return num_in*d_ldM;
}
