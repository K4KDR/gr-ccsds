#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ccsds_mpsk_constellation.h"
#include <ccsds_mpsk_demod2_cb.h>
#include <gr_io_signature.h>
#include <stdio.h>
#include <math.h>
#include <volk/volk.h>
#include <fftw3.h>
#include <cmath>


ccsds_mpsk_demod2_cb_sptr ccsds_make_mpsk_demod2_cb (const unsigned int M, const unsigned char quant_bits)
{
    return ccsds_mpsk_demod2_cb_sptr (new ccsds_mpsk_demod2_cb (M, quant_bits));
}

ccsds_mpsk_demod2_cb::ccsds_mpsk_demod2_cb (const unsigned int M, const unsigned quant_bits)
  : gr_sync_block ("ccsds_mpsk_demod2_cb",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature (2, 2, sizeof (char))), d_M(M), d_QUANT_BITS(quant_bits), d_QUANT_MAX((unsigned char)(pow(2.0f,(float)quant_bits)-1)), d_TWOPI(2.0*M_PI)
{
	if(d_M > 256) {
		fprintf(stderr,"ERROR DEMOD2: modulation order M=%d to high (maximum 256).\n",d_M);
		exit(EXIT_FAILURE);
		return;
	}

	if(d_QUANT_BITS > 8) {
		fprintf(stderr,"ERROR DEMOD2: number of quantization bits %u to high (maximum 8).\n",d_QUANT_BITS);
		exit(EXIT_FAILURE);
		return;
	}

	map = ccsds_make_mpsk_map(M);
}

ccsds_mpsk_demod2_cb::~ccsds_mpsk_demod2_cb ()
{
	delete[] map;
}

unsigned char ccsds_mpsk_demod2_cb::quant(const float mag) const {

	// scale so that 1.0 is mapped to (float) d_QUANT_MAX, cut off
	// cut off any fractions by rounding
	// satturate all values to a maximum of d_QUANT_MAX
	return (unsigned char) std::min((unsigned int)round(mag * d_QUANT_MAX), (unsigned int)d_QUANT_MAX);
}

int  ccsds_mpsk_demod2_cb::work (int                     noutput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	unsigned char *out = (unsigned char *) output_items[0];
	unsigned char *out_conf = (unsigned char *) output_items[1];
	
	const unsigned int num = noutput_items;

	// if we are to output confidence values as well, initialize and
	// allocate them here
	float *tmp_angle = (float*)fftwf_malloc(num*sizeof(float));
	float *tmp_mag = (float*)fftwf_malloc(num*sizeof(float));

	if(tmp_angle == 0 || tmp_mag == 0) {
		fprintf(stderr,"ERROR DEMOD2: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return 0;
	}

	if(!is_unaligned()) {
		// input is aligned, use aligned call directly
		volk_32fc_s32f_atan2_32f_a(tmp_angle, in, 1.0f, num);

		volk_32fc_magnitude_squared_32f_a(tmp_mag, in, num);
	} else {
		// unaligned, copy input to aligned buffer

		// allocate buffer
		gr_complex *tmp_c = (gr_complex*)fftwf_malloc(num*sizeof(gr_complex));
		if(tmp_c == 0) {
			fprintf(stderr,"ERROR DEMOD2: allocation of memory failed\n");
			exit(EXIT_FAILURE);
			return 0;
		}

		// copy data to buffer
		memcpy(tmp_c, in, num*sizeof(gr_complex));

		// process data
		volk_32fc_s32f_atan2_32f_a(tmp_angle, tmp_c, 1.0f, num);

		// free buffer
		fftw_free(tmp_c);

		volk_32fc_magnitude_squared_32f_a(tmp_mag, in, num);
	}		

	const float add = 1.0f/d_M;
	const float factor = (float)d_M / 2.0f;
	//printf("add=%f, factor=%f\n",add, factor);

	float tmp;
	for(unsigned int i=0;i<num;i++) {
		tmp = (std::fmod(tmp_angle[i]+d_TWOPI,d_TWOPI)/M_PI + add ) * factor;
		out[i] = map[(unsigned int) std::floor(tmp)%d_M];
		out_conf[i] = quant(tmp_mag[i]);
		//printf("angle=%f, symb=%f, out=%d\n",tmp_angle[i]/M_PI, tmp, out[i]);
	}

	fftw_free(tmp_angle);
	fftw_free(tmp_mag);

	// Tell runtime system how many output items we produced.
	return num;
}
