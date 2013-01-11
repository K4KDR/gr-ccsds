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


ccsds_mpsk_demod2_cb_sptr ccsds_make_mpsk_demod2_cb (unsigned int M)
{
    return ccsds_mpsk_demod2_cb_sptr (new ccsds_mpsk_demod2_cb (M));
}

ccsds_mpsk_demod2_cb::ccsds_mpsk_demod2_cb (const unsigned int M)
  : gr_sync_block ("ccsds_mpsk_demod2_cb",
	gr_make_io_signature (1, 1, sizeof (gr_complex)),
	gr_make_io_signature (1, 1, sizeof (char))), d_M(M), d_TWOPI(2.0*M_PI)
{
	if(d_M > 256) {
		fprintf(stderr,"ERROR DEMOD2: modulation order M=%d to high (maximum 256).\n",d_M);
		exit(EXIT_FAILURE);
		return;
	}

	map = ccsds_make_mpsk_map(M);
}

ccsds_mpsk_demod2_cb::~ccsds_mpsk_demod2_cb ()
{
	delete[] map;
}

int  ccsds_mpsk_demod2_cb::work (int                     noutput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	char *out = (char *) output_items[0];

	const unsigned int num = noutput_items;

	float *tmp_angle = (float*)fftwf_malloc(num*sizeof(float));
	if(tmp_angle == 0) {
		fprintf(stderr,"ERROR DEMOD2: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return 0;
	}

	if(!is_unaligned()) {
		// input is aligned, use aligned call directly
		volk_32fc_s32f_atan2_32f_a(tmp_angle, in, 1.0f, num);
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
	}		

	const float add = 1.0f/d_M;
	const float factor = (float)d_M / 2.0f;
	//printf("add=%f, factor=%f\n",add, factor);

	float tmp;
	for(unsigned int i=0;i<num;i++) {
		tmp = (std::fmod(tmp_angle[i]+d_TWOPI,d_TWOPI)/M_PI + add ) * factor;
		out[i] = map[(unsigned int) std::floor(tmp)%d_M];
		//printf("angle=%f, symb=%f, out=%d\n",tmp_angle[i]/M_PI, tmp, out[i]);
	}

	fftw_free(tmp_angle);

	// Tell runtime system how many output items we produced.
	return num;
}
