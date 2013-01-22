#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ccsds_frame_sync_bb.h>
#include "ccsds_hexstring_to_binary.h"
#include "ccsds_asm_operator.h"
#include <gr_io_signature.h>
#include <gr_message.h>



ccsds_frame_sync_bb_sptr ccsds_make_frame_sync_bb(std::string ASM, const unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length, gr_msg_queue_sptr msgq) {
    return ccsds_frame_sync_bb_sptr (new ccsds_frame_sync_bb(ASM, threshold, ber_threshold, frame_length, msgq) );
}

ccsds_frame_sync_bb::ccsds_frame_sync_bb (std::string ASM, unsigned int threshold, const unsigned int ber_threshold, const unsigned int frame_length, gr_msg_queue_sptr msgq)
  : gr_block ("ccsds_frame_sync_bb",
	gr_make_io_signature (1, 1, sizeof (unsigned char)),
	gr_make_io_signature (1, 1, sizeof (unsigned char))), d_ASM_LEN(std::floor(ASM.length()/2)), d_THRESHOLD(threshold), d_BER_THRESHOLD(ber_threshold), d_FRAME_LEN(frame_length),
	d_SEARCH_LEN(d_FRAME_LEN + 2u * d_ASM_LEN), d_COPY_LEN(d_FRAME_LEN+d_ASM_LEN)
{

	// If no ASM is found, samples will be dropped, so no rate can be
	// specified.
	set_fixed_rate(false);
	
	// Transfer ASM from Hex to bytes
	unsigned char *tmp = new unsigned char[d_ASM_LEN];
	ccsds_hexstring_to_binary(&ASM, tmp);

	d_asm_operator = new ccsds_asm_operator(tmp, d_ASM_LEN, d_BER_THRESHOLD);

	delete[] tmp;

	d_msgq = msgq;

	// Initialize variables
	d_state = STATE_SEARCH;
	d_count = 0;
	d_offset_bytes = 0;
	d_offset_bits =0;

	dbg_file_in       = fopen("/tmp/ccsds_frame_sync_debug_frame_in.dat","w");
	dbg_file_out      = fopen("/tmp/ccsds_frame_sync_debug_frame_out.dat","w");
	dbg_file_instream = fopen("/tmp/ccsds_frame_sync_debug_stream_in.dat","w");
	dbg_count = 0;
}

ccsds_frame_sync_bb::~ccsds_frame_sync_bb () {
	delete d_asm_operator;
	fflush(dbg_file_in);
	fflush(dbg_file_out);
	fflush(dbg_file_instream);
	fclose(dbg_file_in);
	fclose(dbg_file_out);
	fclose(dbg_file_instream);
}

void ccsds_frame_sync_bb::forecast(int noutput_items,gr_vector_int &ninput_items_required){
	// How many frames will we process if we produce the requested number of
	// samples
	unsigned int num_frames = std::floor(noutput_items / (d_ASM_LEN+d_FRAME_LEN) );

	// We want to process at least one frame, in order to do something.
	num_frames = std::max( 1u, num_frames);

	// If we know exactly where to look, we need one ASM plus the following
	// frame data per requested frame.
	unsigned int bytes_per_frame = d_ASM_LEN + d_FRAME_LEN;
		
	// If we have a bit offset, we need an additional byte
	unsigned int bit_offset = (d_offset_bits == 0) ? 0 : 1;
	
	// If we don't know where to look we need additional d_ASM_LEN-1 samples
	// to guarantee that there is one complete ASM in the first sequence.
	// Afterwards the state should be locked.
	if(d_state == STATE_SEARCH) {
		ninput_items_required[0] = bytes_per_frame*num_frames + bit_offset + d_ASM_LEN;
		
	} else { // d_state == STATE_LOCKED
		// No need to search again (hopefully)
		ninput_items_required[0] = bytes_per_frame*num_frames + bit_offset;
	}

	return;
}

const unsigned int ccsds_frame_sync_bb::get_bytes_required(void) {
	if(d_offset_bits == 0)
		return d_COPY_LEN;
	else
		return d_COPY_LEN+1u;
}

int  ccsds_frame_sync_bb::general_work (int                     noutput_items,
                                gr_vector_int               &ninput_items,
                                gr_vector_const_void_star   &input_items,
                                gr_vector_void_star         &output_items)
{
	// pointer to pointer to a const gr_complex
	const unsigned char *in = (const unsigned char *) input_items[0];
	unsigned char *out = (unsigned char *) output_items[0];
	
	// how many samples can we still process, without violating the array
	// bounds of the output or one of the input buffers?
	unsigned int bytes_remain = std::min(noutput_items, ninput_items[0]);
	
	// If a state detects that it needs more samples to continue, it can
	// signal an abort with this flag. A new try is done once the work
	// function is called again by the scheduler
	bool abort = false;
	
	unsigned int bytes_consumed = 0;

	while(!abort) {
		switch(d_state) {
			case STATE_SEARCH:
				// We want to search, make sure we have enough
				// samples
				if(bytes_remain < d_SEARCH_LEN) {
					abort = true;
					break;
				}

				#if CCSDS_FS_VERBOSITY_LEVEL >= CCSDS_FS_OUTPUT_STATE
					printf("FRAME SYNC: state=SEARCH\n");
				#endif

				// We have enough samples, let's search in each
				// stream
				if(d_asm_operator->search_asm(&in[bytes_consumed], d_SEARCH_LEN, &d_offset_bytes, &d_offset_bits)) {
					// ASM found, enter LOCK
					d_state = STATE_LOCK;

					// Do not output the first one,
					// just consume the samples and
					// shift the indices.
					bytes_consumed += d_offset_bytes;
					bytes_remain -= d_offset_bytes;
					d_offset_bytes = 0;
				} else {
					// Consume samples if we did not find anything
					bytes_consumed += d_SEARCH_LEN - d_ASM_LEN;
					bytes_remain -= d_SEARCH_LEN - d_ASM_LEN;
				}
				break; // End state SEARCH

			case STATE_LOCK:
				// Make sure we have enough samples
				if(bytes_remain < get_bytes_required()) {
					abort = true;
					break;
				} else {
					// We have enough samples, continue in
					// else clause to have an own local 
					// context for this state.

					#if CCSDS_FS_VERBOSITY_LEVEL >= CCSDS_FS_OUTPUT_STATE
								printf("FRAME SYNC: state=LOCK, bit_offset=%u, byte_offset=%u, confidence=%u/%u\n",
									d_offset_bits, d_offset_bytes, d_count, d_THRESHOLD);
					#endif

					// We have enough samples, lets check for the ASM
					if(d_asm_operator->check_for_asm(&in[bytes_consumed+d_offset_bytes], d_offset_bits)) {
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

						// Reset variables that are undefined in
						// search state
						d_offset_bytes = 0;
						d_offset_bits  = 0;
					
						// Do not consume any samples here. Our
						// lock seems to be wrong so start new
						// search with the same samples again

						break;
					}

					// We are still locked

					// create message
					gr_message_sptr msg = gr_make_message(0, 0, 0, d_FRAME_LEN);
					d_asm_operator->copy_stream(msg->msg(), &in[bytes_consumed+d_offset_bytes+d_ASM_LEN], d_FRAME_LEN, d_offset_bits);
					d_msgq->insert_tail(msg);
					
					fprintf(dbg_file_in,"%3u  ",dbg_count);
					fprintf(dbg_file_out,"%3u  ",dbg_count);
					unsigned char tmp[d_FRAME_LEN];
					d_asm_operator->copy_stream(tmp, &in[bytes_consumed+d_offset_bytes+d_ASM_LEN], d_FRAME_LEN, d_offset_bits);
					for(unsigned int i=0u;i<d_FRAME_LEN;i++) {
						fprintf(dbg_file_in,"%2X ",tmp[i]);
						fprintf(dbg_file_out,"%2X ",msg->msg()[i]);
					}
					fprintf(dbg_file_in,"\n");
					fprintf(dbg_file_out,"\n");
					dbg_count++;
					
					// consume samples
					bytes_consumed += d_COPY_LEN;
					bytes_remain -= d_COPY_LEN;
				}
				break; // End state LOCK
			default:
				fprintf(stderr,"ERROR FRAME SYNC: undefined state, entering search\n");
				d_state = STATE_SEARCH;
				d_count = 0;
				break;
		}
	}

	//
	//// Cleanup
	//

	// copy input to output
	memcpy(out, in, bytes_consumed*sizeof(unsigned char));

	for(unsigned int i=0u;i<bytes_consumed;i++) {
		fprintf(dbg_file_instream,"%2X ",in[i]);
	}
	fprintf(dbg_file_instream,"\n");

	// Tell runtime system, how many input samples per stream are not
	// obsolete
	consume_each(bytes_consumed);

	// Tell runtime system how many output items we produced
	return bytes_consumed;
}
