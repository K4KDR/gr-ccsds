#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_mpsk_ambiguity_resolver_f.h>
#include "ccsds_hexstring_to_binary.h"
#include "ccsds_mpsk_constellation.h"
#include <gr_io_signature.h>
#include <math.h>
#include <complex>
#include <cstdlib>
#include <fftw3.h>
#include <cstdio>
#include <gruel/pmt.h>
#include <boost/scoped_array.hpp>
#include <volk/volk.h>

ccsds_mpsk_ambiguity_resolver_f_sptr ccsds_make_mpsk_ambiguity_resolver_f(const unsigned int M, std::string ASM, const unsigned int asm_len, const unsigned int threshold, const float correlation_threshold, const unsigned int frame_length, const unsigned int num_tail_syms) {
    return ccsds_mpsk_ambiguity_resolver_f_sptr (new ccsds_mpsk_ambiguity_resolver_f(M, ASM,asm_len, threshold, correlation_threshold, frame_length, num_tail_syms) );
}

ccsds_mpsk_ambiguity_resolver_f::ccsds_mpsk_ambiguity_resolver_f (const unsigned int M, std::string ASM, const unsigned int asm_len, const unsigned int threshold, const float correlation_threshold, const unsigned int frame_length, const unsigned int num_tail_syms)
  : gr_block ("ccsds_mpsk_ambiguity_resolver_f",
	gr_make_io_signature (1, 1, sizeof (float)),
	gr_make_io_signature (0, 0, sizeof (float))),

	d_M(M),
	d_ldM((unsigned int)round(log(M)/log(2.0f))),
	d_THRESHOLD(threshold),
	d_CORRELATION_THRESHOLD(correlation_threshold),
	d_FRAME_LEN_BITS(frame_length*8),
	d_ASM_LEN_BITS(asm_len),
	d_NUM_TAIL_SYMS(num_tail_syms),
	d_SEARCH_LEN_MIN(get_upper_mul(d_ASM_LEN_BITS + d_ldM)),
	d_SEARCH_LEN_MAX(get_upper_mul(d_FRAME_LEN_BITS + d_NUM_TAIL_SYMS + 2u * d_ASM_LEN_BITS - 1u))
{
	
	// Mapping to translate from the input stream to any ambiguity stream
	// (assume the detector saw a rotated constellation)
	d_map_index_to_bits = ccsds_make_mpsk_map(M);
	d_map_bits_to_index = ccsds_make_mpsk_rev_map(M);

	// Copy ASM to private memory
	const unsigned int asm_len_byte = std::ceil((d_ASM_LEN_BITS)/8.0f);
	boost::scoped_array<unsigned char> tmp(new unsigned char[asm_len_byte]);
	ccsds_hexstring_to_binary(&ASM, tmp.get());

	d_ASM = (float*) fftw_malloc(d_ASM_LEN_BITS * sizeof(float));
	d_tmp_fv = (float*) fftw_malloc(d_ASM_LEN_BITS * sizeof(float)); // Aligned buffer for sequence to be checked against ASM
	d_tmp_f  = (float*) fftw_malloc(sizeof(float));			 // Aligned float for correlation result
	
	if(d_ASM == 0 || d_tmp_fv == 0 || d_tmp_f == 0) {
		fprintf(stderr,"ERROR AR SOFT: allocation of memory failed\n");
		exit(EXIT_FAILURE);
		return;
	}

	for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
		const uint8_t byte = tmp[i/8];
		
		const bool bit = ((byte >> (7-(i%8))) & 0x01) == 0x01;
		d_ASM[i] = (bit ? 1.0f : -1.0f);
	}

	// Initialize variables
	d_state = STATE_SEARCH;
	d_count = 0;
	d_locked_on_stream = 0;
	d_offset_bits =0;
	d_msg_buffer_count = 0;
	d_frame_count = 0;
	d_tag_lastupdate = 0;

	// register output
	message_port_register_out(pmt::mp("out"));

	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
		dbg_file_in = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_f_debug_in.dat","w");
		dbg_file_in_hard = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_f_debug_in_hard.dat","w");
		dbg_file_out = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_f_debug_out.dat","w");
		dbg_file_asms = fopen("/tmp/ccsds_mpsk_ambiguity_resolver_f_debug_asms.dat","w");
	#endif
	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_CHANGE
		dbg_count = 0;

		printf("==== Ambiguity resolution and Frame sync ====\n");
		printf("  Sym len:   %u bit\n",d_ldM);
		printf("  ASM len:   %u bit\n",d_ASM_LEN_BITS);
		printf("  Frame len: %u bit\n",d_FRAME_LEN_BITS);
		printf("  Tail len:  %u bit\n",d_NUM_TAIL_SYMS);
		printf("  Min search len: %u\n",d_SEARCH_LEN_MIN);
		printf("  Max search len: %u\n",d_SEARCH_LEN_MAX);
		printf("=============================================\n");
	#endif
}

ccsds_mpsk_ambiguity_resolver_f::~ccsds_mpsk_ambiguity_resolver_f () {
	delete[] d_map_index_to_bits;
	delete[] d_map_bits_to_index;
	
	fftw_free(d_ASM);
	fftw_free(d_tmp_fv);
	fftw_free(d_tmp_f);

	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
		fflush(dbg_file_in);
		fflush(dbg_file_in_hard);
		fflush(dbg_file_out);
		fflush(dbg_file_asms);

		fclose(dbg_file_in);
		fclose(dbg_file_in_hard);
		fclose(dbg_file_out);
		fclose(dbg_file_asms);
	#endif
}

bool ccsds_mpsk_ambiguity_resolver_f::stop(void) {
	// Signal EOF to next block
	message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
	return true;
}

float ccsds_mpsk_ambiguity_resolver_f::check_for_ASM(const float *in, const unsigned int offset) {
	float correlation = 0.0f;

	/* correlation without volk
	for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
		//TODO use volk for multiplication
		correlation += d_ASM[i] * in[i+offset];
	}
	// normalization
	correlation /= d_ASM_LEN_BITS;
	*/

	//* correlation with volk

	// copy input into aligned memory
	memcpy(d_tmp_fv, &in[offset], d_ASM_LEN_BITS*sizeof(float));

	// correlation
	volk_32f_x2_dot_prod_32f_a(d_tmp_f, d_ASM, d_tmp_fv, d_ASM_LEN_BITS);
	// normalization
	correlation = *d_tmp_f / d_ASM_LEN_BITS;
	//*/



	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
		/*
		printf("Check if ASM equals with offset %u:\n  in : ",offset);
		for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
			printf("%1u ", (in[i+offset]>0.0) ? 1 : 0);
		}
		printf("\n  asm: ");
		for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
			printf("%1u ", (d_ASM[i]>0.0) ? 1 : 0);
		}
		printf("\n  correlation: %lf\n",correlation);
		//*/
	#endif

	return correlation;
}
float ccsds_mpsk_ambiguity_resolver_f::search_for_ASM(const float *in, const unsigned int num_in, unsigned int *offset) {

	const unsigned int search_len = num_in-d_ASM_LEN_BITS;

	float max = -1.0f;
	*offset = 0;

	for(unsigned int i=0;i<=search_len;i++) {
		float corr = check_for_ASM(in,i);
		if(corr > max) {
			max = corr;
			*offset = i;

			if(max >= 1.0f) {
				// Maximum found, stop searching
				break;
			}
		}
	}

	return max;
}

unsigned char ccsds_mpsk_ambiguity_resolver_f::convert_to_char(const float *in) {
	uint8_t out = 0x00;

	for(unsigned int i=0;i<d_ldM;i++) {
		out = (out<<1) | ((in[i] > 0) ? 0x01 : 0x00);
	}

	return out;
}

void ccsds_mpsk_ambiguity_resolver_f::convert_ambiguity(float *out, const float *in, const unsigned int num_bits, unsigned int ambiguity) {

	// Get the number of symbols we should convert. If num_bits is not a
	// multiple of d_ldM, ignore the last few bits. (Doku explicitly says
	// it must be a multiple).
	const unsigned int num_syms = get_lower_mul(num_bits) / d_ldM;

	// shortcut if there is no ambiguity
	if(ambiguity == 0) {
		memcpy(out, in, num_bits*sizeof(float));
		return;
	}

	// there is a non zero ambiguity, so do the conversion
	for(unsigned int i=0;i<num_syms;i++) {
		// convert bits now
		
		// index of in shoul always be  smaller than num_bits, but as a
		// saftey measure, wrap it arround if some input parameters are
		// not well defined
		const unsigned char bits_in = convert_to_char(&in[(d_ldM*i) % num_bits]);
		const unsigned int index = d_map_bits_to_index[bits_in];
		const uint8_t bits_out = d_map_index_to_bits[(d_M+index-ambiguity)%d_M];

		// find out where positions differ, to assign the right value
		uint8_t diff = bits_in ^ bits_out;
		for(unsigned int j=0;j<d_ldM;j++) {
			// wrap output around num_bits as a saftey measure if 
			// d_ldM and num_bits do not match together.
			const unsigned int indx = (i*d_ldM+j) % num_bits;
			out[indx] = (diff & 0x01 << (d_ldM-1-j)) ? -in[indx] : in[indx];
		}
		
	}
	return;
}

void ccsds_mpsk_ambiguity_resolver_f::forecast(int /*noutput_items*/,gr_vector_int &ninput_items_required){
	// In Search mode we only search for a ASM, we don't need the Frame yet
	// to make sure we do not request more than necessary and GNURadio drops
	// everything

	switch(d_state) {
		case STATE_SEARCH:	ninput_items_required[0] = d_SEARCH_LEN_MIN;
					break;
		case STATE_LOCK:	ninput_items_required[0] = get_upper_mul( d_msg_buffer_fill ? 1 : (d_ASM_LEN_BITS + d_offset_bits));
					break;
	}
	//printf("AR: need at least %d input items in this state\n",ninput_items_required[0]);
	return;
}

unsigned int ccsds_mpsk_ambiguity_resolver_f::get_lower_mul(const unsigned int n) {
	return (unsigned int)std::floor((float)n/d_ldM)*d_ldM;
}

unsigned int ccsds_mpsk_ambiguity_resolver_f::get_upper_mul(const unsigned int n) {
	return (unsigned int)std::ceil((float)n/d_ldM)*d_ldM;
}

void ccsds_mpsk_ambiguity_resolver_f::updateTags(const uint64_t until) {
	// buffer for tags
	std::vector< gr_tag_t > tagvec;

	// just update, don't repeat work
	const uint64_t from = std::max(this->nitems_read(0), d_tag_lastupdate+1lu);

	// nothing to do here. Note: if equal there is one sample to check
	if(until < from) {
		return;
	}

	// get tags
	this->get_tags_in_range(tagvec, 0, from, until);

	// loop through tags
	for(unsigned int i=0;i<tagvec.size();i++) {
		// copy tag to buffer
		d_tag_buffer[ pmt::pmt_symbol_to_string(tagvec[i].key) ] = tagvec[i].value;

		// if we copied rx_time, update sample counter
		if(pmt::pmt_symbol_to_string(tagvec[i].key).compare("rx_time") == 0) {
			d_tag_last_rx_time = tagvec[i].offset;
		}
	}

	// update counter
	d_tag_lastupdate = from;

	return;
}

pmt::pmt_t ccsds_mpsk_ambiguity_resolver_f::extractTags(const uint64_t from, const unsigned int len) {
	// create empty header
	pmt::pmt_t dict = pmt::pmt_make_dict();

	// If rx_rate and rx_time tags are available, we can calculate the frame rx_time
	if(d_tag_buffer.count(std::string("rx_rate")) == 1 && d_tag_buffer.count(std::string("rx_time")) == 1) {

		// bring buffer for rx_time and rx_rate up to date (which meanst at the start of the frame)
		updateTags(from);

		// how many samples have passed since last update
		uint64_t samples_diff = from-d_tag_last_rx_time;
		// what is the current sampling rate
		const double rate = pmt::pmt_to_double( d_tag_buffer["rx_rate"] );
		// time passed in seconds since last update
		const uint64_t passed_sec = samples_diff / (uint64_t)rate;
		// additional time passed in fractional seconds since last update
		const double passed_frac = (samples_diff % (uint64_t)rate) / rate;

		// time tag of last update in seconds
		uint64_t time_sec = pmt::pmt_to_double(pmt::pmt_car( d_tag_buffer["rx_time"] ));
		// time tag of last update in additional fractional seconds
		double time_frac  = pmt::pmt_to_uint64(pmt::pmt_cdr( d_tag_buffer["rx_time"] ));

		// update fractional time passed
		time_frac += passed_frac;
		// update seconds past
		time_sec += passed_sec;
		// update carry seconds
		time_sec += (uint64_t)std::floor(time_frac);
		// remove additional seconds from the fractional part
		time_frac = fmod(time_frac, 1.0);

		// create time tuple
		const pmt::pmt_t time_val = pmt::pmt_cons(pmt::pmt_from_uint64(time_sec), pmt::pmt_from_double(time_frac));

		// add time tuple to dict
		dict = pmt::pmt_dict_add(dict, pmt::pmt_intern("rx_time"), time_val);
	}


	// update buffer to contain tags that are received within this frame
	updateTags(from+len);


	// now copy all updated tags into our header except for rx_time, which has been set before

	// loop through all tags in our buffer
	for(std::map<std::string, pmt::pmt_t>::const_iterator it=d_tag_buffer.begin();it != d_tag_buffer.end();it++) {
		// we already processed rx_time tag and don't want to copy it to the frame
		if(it->first.compare("rx_time") == 0) {
			continue;
		}

		// add tag
		dict = pmt::pmt_dict_add(dict, pmt::pmt_intern(it->first), it->second);
	}

	return dict;
}

int  ccsds_mpsk_ambiguity_resolver_f::general_work (int 		    /*noutput_items*/,
					        gr_vector_int&              ninput_items,
					        gr_vector_const_void_star&  input_items,
					        gr_vector_void_star&        /*output_items*/)
{
	const float *in = (const float *) input_items[0];

	unsigned int num_consumed = 0;

	// If a state detects that it needs more samples to continue, it can
	// signal an abort with this flag. A new try is done once the work
	// function is called again by the scheduler
	bool abort = false;
	

	// How many samples are necessary to perform a check for an expected
	// ASM. Can vary depending on d_offset_bits and is updated by the
	// statemachine.
	unsigned int check_len;

	while(!abort) {

		switch(d_state) {
			case STATE_SEARCH:
				// We want to search, make sure we have enough
				// samples
				if(ninput_items[0]-num_consumed < d_SEARCH_LEN_MIN) {
					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
						printf("AR: skip because we need %u symbols for the next search, while there are only %d left\n",d_SEARCH_LEN_MIN,ninput_items[0]-num_consumed);
					#endif

					abort = true;
					break;
				} else {

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
						printf("AR: state=SEARCH at symbol %lu\n",dbg_count+num_consumed);
					#endif

					const unsigned int search_len = get_lower_mul(std::min(ninput_items[0]-num_consumed, d_SEARCH_LEN_MAX));

					// We have enough samples, let's search in each
					// stream
					float max_corr = -1.0f;
					unsigned int max_amb = 0;

					for(unsigned int i=0;i<d_M;i++) {
						boost::scoped_array<float> in_syms(new float[search_len]);
					
						convert_ambiguity(in_syms.get(), &in[num_consumed], search_len, i);

						unsigned int offset;
						const float correlation = search_for_ASM(in_syms.get(), search_len, &offset);

						// Do we have a better match than the previous?
						if(correlation > max_corr) {
							max_corr = correlation;
							max_amb  = i;
							d_offset_bits = offset;

							if(max_corr >= 1.0f) {
								// Maximum found, stop searching
								break;
							}
						}

						// continue search for other ambiguities
					} // END search all ambiguity streams

					// Did we find something?
					if(max_corr > d_CORRELATION_THRESHOLD) {
						// ASM found, enter LOCK
						d_state = STATE_LOCK;
						d_count = 0;
						d_locked_on_stream = max_amb;

						// Since ASM is already found we will
						// copy to the buffer immediatley
						d_msg_buffer_fill = true;

						// This is a new frame, increase counter
						d_frame_count++;

						#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
							boost::scoped_array<float> in_syms(new float[d_ASM_LEN_BITS+get_upper_mul(d_offset_bits)]);
					
							convert_ambiguity(in_syms.get(), &in[num_consumed], d_ASM_LEN_BITS+get_upper_mul(d_offset_bits), d_locked_on_stream);

							fprintf(dbg_file_asms,"frame %6lu: ",d_frame_count);
							for(unsigned int j=0;j<d_ASM_LEN_BITS;j++) {
								fprintf(dbg_file_asms,"%+1.2f ",in_syms[d_offset_bits+j]);
							}
							fprintf(dbg_file_asms,"      ");
							for(unsigned int j=0;j<d_ASM_LEN_BITS;j++) {
								fprintf(dbg_file_asms,"%1u ",(in_syms[d_offset_bits+j]>0.0) ? 1:0);
							}
							fprintf(dbg_file_asms," found correlation=%lf\n", max_corr);
						#endif

						// Consume samples up to the next
						// frame data (including ASM)
						num_consumed += get_lower_mul(d_offset_bits+d_ASM_LEN_BITS);
						d_offset_bits = (d_offset_bits+d_ASM_LEN_BITS) % d_ldM;

						#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_CHANGE
							printf("AR: Found ASM for frame number %lu => LOCK\n", d_frame_count);
						#endif
					
					} else { // We found nothing
						// Consume samples if we did not find anything
					
						/*
							printf("ASM             : ");
							for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
								printf("%+1.1f ",d_ASM[i]);
							}
							boost::scoped_array<float> in_syms(new float[search_len]);
							convert_ambiguity(in_syms.get(), &in[num_consumed], search_len, max_amb);
							printf("\nbest match amb=%d: ",max_amb);
							for(unsigned int i=0;i<d_ASM_LEN_BITS;i++) {
								printf("%+1.1f ",in_syms[i+d_offset_bits]);
							}
							printf("\n");
						//*/

						#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
							fprintf(dbg_file_in,"%10lu: ",dbg_count+num_consumed);
							fprintf(dbg_file_in_hard,"%10lu: ",dbg_count+num_consumed);
							for(unsigned int j=0;j<get_lower_mul(search_len-(d_ASM_LEN_BITS-1));j++) {
								fprintf(dbg_file_in,"%+1.2f ",in[num_consumed+j]);
								fprintf(dbg_file_in_hard,"%1u ",(in[num_consumed+j]>0.0) ? 1:0);
							}
							fprintf(dbg_file_in,"\n");
							fprintf(dbg_file_in_hard,"\n");
						#endif

						// the last d_ASM_BITS-1 have not been searched yet, everything
						// else can be consumed as long as throw away multiples of the
						// symbol bits. Otherwise we will loose track of the symbol periods
						num_consumed += get_lower_mul(search_len-(d_ASM_LEN_BITS-1));

						d_offset_bits = 0;

						#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
							printf("AR: nothing found, consume %u samples.\n",get_lower_mul(search_len-(d_ASM_LEN_BITS-1)));
						#endif
					}

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
						printf("AR:   maximum correlation: %f => %s\n",max_corr,((d_state==STATE_LOCK)?"LOCK":"SEARCH"));
					#endif

				} // END num_in >= search_len_min
				break; // End state SEARCH

			case STATE_LOCK:
				// How many samples do we need if locked? If ASM is not aligned we need
				// d_offset_bits additional bits.
				check_len = get_upper_mul( d_msg_buffer_fill ? 1 : (d_ASM_LEN_BITS + d_offset_bits));

				// Make sure we have enough samples
				if(ninput_items[0]-num_consumed < check_len) {
					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
						printf("AR: skip because we need %u symbols for the next lock, while there are only %d left\n",check_len,ninput_items[0]-num_consumed);
					#endif

					abort = true;
					break;
				} else if(d_msg_buffer_fill) {
					// Put as much samples into the buffer
					// as we can.
					
					// How many samples do we have
					const unsigned int to_buffer = std::min(
							get_lower_mul(ninput_items[0]-num_consumed),					 // Number of symbols available
							get_upper_mul(d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS-d_msg_buffer_count+d_offset_bits) // Number of symbols that are needed to fill the buffer
						);

					// Convert ambiguity
					boost::scoped_array<float> in_syms(new float[to_buffer]);
					convert_ambiguity(in_syms.get(), &in[num_consumed], to_buffer, d_locked_on_stream);

					// Fill buffer
					// Is this the first item, then create a new buffer
					if(d_msg_buffer_count == 0) {
						d_msg_buffer = pmt::pmt_make_f32vector(d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS, 0.0f);
					}

					const unsigned int to_copy = std::min(d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS-d_msg_buffer_count, to_buffer-d_offset_bits);

					for(unsigned int i=0;i<to_copy;i++) {
						pmt::pmt_f32vector_set(d_msg_buffer, d_msg_buffer_count+i, in_syms[i+d_offset_bits]);
					}
					d_msg_buffer_count += to_copy;

					// Update consumed counter
					num_consumed += get_lower_mul(to_copy+d_offset_bits);

					// Update offset pointer
					d_offset_bits = (to_copy+d_offset_bits) % d_ldM;

					// Did we will the buffer?
					if(d_msg_buffer_count >= d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS) {
						// buffer is now full, create message

						// extract Tags from buffer and add them to the header
						const unsigned int len = d_FRAME_LEN_BITS+d_NUM_TAIL_SYMS;
						const uint64_t from = this->nitems_read(0)+num_consumed+d_offset_bits-len;
						pmt::pmt_t meta = extractTags(from, len);

						// Add frame count for debugging
						meta = pmt::pmt_dict_add(meta, pmt::mp("frame_number"), pmt::pmt_from_long(d_frame_count));

						// create PDU
						pmt::pmt_t msg = pmt::pmt_cons(meta, d_msg_buffer);
						
						// send PDU
						message_port_pub(pmt::mp("out"), msg);

						// reset buffer
						d_msg_buffer_count = 0;

						// we are no longer filling the buffer (need to check next ASM first)
						d_msg_buffer_fill = false;
					}
					break; // END state LOCK

				} else {
					// We have enough samples and we need to
					// check for the next ASM

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_STATE
								printf("AR: state=LOCK, stream=%u, bit_offset=%u, confidence=%u/%u\n",
									d_locked_on_stream, d_offset_bits, d_count, d_THRESHOLD);
					#endif

					boost::scoped_array<float> in_syms(new float[check_len]);
					convert_ambiguity(in_syms.get(), &in[num_consumed], check_len, d_locked_on_stream);

					const float correlation = check_for_ASM(in_syms.get(), d_offset_bits);
					
					// increase counter, next time we will come here it will be a new potential frame
					d_frame_count++;

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
						fprintf(dbg_file_asms,"frame %6lu: ",d_frame_count);
						for(unsigned int j=0;j<d_ASM_LEN_BITS;j++) {
							fprintf(dbg_file_asms,"%+1.2f ",in_syms[d_offset_bits+j]);
						}
						fprintf(dbg_file_asms,"      ");
						for(unsigned int j=0;j<d_ASM_LEN_BITS;j++) {
							fprintf(dbg_file_asms,"%1u ",(in_syms[d_offset_bits+j]>0.0) ? 1:0);
						}
						fprintf(dbg_file_asms," %s correlation=%lf\n", (correlation > d_CORRELATION_THRESHOLD)?"checked":"not found", correlation);
					#endif

					// We have enough samples, lets check for the ASM
					if(correlation > d_CORRELATION_THRESHOLD) {				
						// ASM found
						d_count = std::min(d_count+1, d_THRESHOLD);
					} else if(d_count > 1) {
						// No match, but we still have
						// confidence in out locking
						d_count--;
					} else {
						// Lost lock, enter search
						d_state = STATE_SEARCH;
						d_count = 0;
						d_offset_bits = 0;

						// Do not consume any samples here. Our
						// lock on this stream seems to be wrong
						// so start new search with the same
						// samples again

						#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_CHANGE
							printf("AR: ASM lost after frame %lu => SEARCH\n",d_frame_count);
						#endif

						break;
					}

					// We are still locked


					// consume ASM and set flag to fill the
					// buffer
					num_consumed += get_lower_mul(d_ASM_LEN_BITS+d_offset_bits);

					// We might not have consumed every bit, so adjust the
					// offsets to point to the exact start of the frame
					d_offset_bits = (d_ASM_LEN_BITS+d_offset_bits) % d_ldM;

					d_msg_buffer_fill = true;

					#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_DEBUG
						printf("AR:  %u samples consumed, fill frame buffer in next step.\n",get_lower_mul(d_ASM_LEN_BITS+d_offset_bits));
					#endif

				}
				break; // End state LOCK
			default:
				fprintf(stderr,"ERROR AR: undefined state, entering search\n");
				d_state = STATE_SEARCH;
				d_count = 0;

				break;

		} // end switch(d_state)
	} // end while(!abort)

	// make sure we dont miss any tags
	updateTags(this->nitems_read(0)+num_consumed);

	//
	//// Cleanup
	//

	#if CCSDS_AR_SOFT_VERBOSITY_LEVEL >= CCSDS_AR_SOFT_OUTPUT_CHANGE
		dbg_count += num_consumed;
	#endif

	consume_each(num_consumed);

	// Stream sink, so no output
	return 0;
}
