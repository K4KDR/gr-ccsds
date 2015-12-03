
/*
 * This file was automatically generated using swig_doc.py.
 *
 * Any changes to it will be lost next time it is regenerated.
 */




%feature("docstring") ccsds_add_asm "Takes a frame and adds a sync marker in front of it.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the ASM sequence followed by the data of the incomming message. This new message is then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_add_asm.

Create an instance of ccsds_add_asm and return it's shared_ptr."

%feature("docstring") ccsds_add_asm::ccsds_add_asm "Public constructor of the add ASM block.

Params: (ASM, frame_len)"

%feature("docstring") ccsds_add_asm::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") ccsds_add_asm::~ccsds_add_asm "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") ccsds_add_asm::start "

Params: ()"

%feature("docstring") ccsds_add_asm::stop "

Params: ()"

%feature("docstring") ccsds_add_asm::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_add_asm "Takes a frame and adds a sync marker in front of it.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the ASM sequence followed by the data of the incomming message. This new message is then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_add_asm.

Create an instance of ccsds_add_asm and return it's shared_ptr.

Params: (ASM, frame_len)"

%feature("docstring") ccsds_blob_msg_sink_b "Reads frames from message input port \"in\" and outputs it to a TicpClient.

Return a shared_ptr to a new instance of ccsds_blob_msg_sink_b.

Create an instance of ccsds_blob_msg_sink_b and return it's shared_ptr."

%feature("docstring") ccsds_blob_msg_sink_b::ccsds_blob_msg_sink_b "Private constructor of the TICP frame sink.

Params: (blob_len)"

%feature("docstring") ccsds_blob_msg_sink_b::~ccsds_blob_msg_sink_b "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") ccsds_blob_msg_sink_b::stop "

Params: ()"

%feature("docstring") ccsds_blob_msg_sink_b::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_blob_msg_sink_b "Reads frames from message input port \"in\" and outputs it to a TicpClient.

Return a shared_ptr to a new instance of ccsds_blob_msg_sink_b.

Create an instance of ccsds_blob_msg_sink_b and return it's shared_ptr.

Params: (blob_len)"

%feature("docstring") ccsds_blob_msg_source_b "Reads frames from message input port \"in\" and outputs it to a TicpClient.

Return a shared_ptr to a new instance of ccsds_blob_msg_source_b.

Create an instance of ccsds_blob_msg_source_b and return it's shared_ptr."

%feature("docstring") ccsds_blob_msg_source_b::ccsds_blob_msg_source_b "Private constructor of the TICP frame sink.

Params: (blob_len)"

%feature("docstring") ccsds_blob_msg_source_b::process_message "Store incomming message into buffer.

Params: (msg_in)"

%feature("docstring") ccsds_blob_msg_source_b::~ccsds_blob_msg_source_b "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") ccsds_blob_msg_source_b::start "

Params: ()"

%feature("docstring") ccsds_blob_msg_source_b::stop "

Params: ()"

%feature("docstring") ccsds_blob_msg_source_b::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_blob_msg_source_b "Reads frames from message input port \"in\" and outputs it to a TicpClient.

Return a shared_ptr to a new instance of ccsds_blob_msg_source_b.

Create an instance of ccsds_blob_msg_source_b and return it's shared_ptr.

Params: (blob_len)"

%feature("docstring") ccsds_conv_encode27_bb "Perform convloutional encoding and puncturing on the input stream.

Create a Convolutional encoder block and return it's shared pointer.

The compact generator polynom contains two informations. Encoded in the MSB is the information whether the stream should be inverted (MSB set) or not (MSB unset). Encoded in the 7 less significant bits is the information about the connections between the shift registers and the output. The MSB-1 represents the influence of the current information bit i(t) while the LSB represents the influence of the information bit i(t-6). A set bit means there is a connection, while an unset bit means there is no.

The polynomials for the convolutional code defined in ECSS can be generated with 0x79 for c1 and 0x5B for c2. Together with the MSB for the inversion of c2 the compact polynomials are 0x79 and 0xDB."

%feature("docstring") ccsds_conv_encode27_bb::ccsds_conv_encode27_bb "Private constructor for the convolutional encoder block.

The compact generator polynom contains two informations. Encoded in the MSB is the information whether the stream should be inverted (MSB set) or not (MSB unset). Encoded in the 7 less significant bits is the information about the connections between the shift registers and the output. The MSB-1 represents the influence of the current information bit i(t) while the LSB represents the influence of the information bit i(t-6). A set bit means there is a connection, while an unset bit means there is no.

The polynomials for the convolutional code defined in ECSS can be generated with 0x79 for c1 and 0x5B for c2. Together with the MSB for the inversion of c2 the compact polynomials are 0x79 and 0xDB.

Params: (gen_poly_c1, gen_poly_c2, puncturing_type)"

%feature("docstring") ccsds_conv_encode27_bb::get_rate "Return relative rate in/out for the given puncturing.

Params: (puncturing_type)"

%feature("docstring") ccsds_conv_encode27_bb::~ccsds_conv_encode27_bb "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_conv_encode27_bb::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_conv_encode27_bb::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_conv_encode27_bb "Perform convloutional encoding and puncturing on the input stream.

Create a Convolutional encoder block and return it's shared pointer.

The compact generator polynom contains two informations. Encoded in the MSB is the information whether the stream should be inverted (MSB set) or not (MSB unset). Encoded in the 7 less significant bits is the information about the connections between the shift registers and the output. The MSB-1 represents the influence of the current information bit i(t) while the LSB represents the influence of the information bit i(t-6). A set bit means there is a connection, while an unset bit means there is no.

The polynomials for the convolutional code defined in ECSS can be generated with 0x79 for c1 and 0x5B for c2. Together with the MSB for the inversion of c2 the compact polynomials are 0x79 and 0xDB.

Params: (gen_poly_c1, gen_poly_c2, puncturing_type)"

%feature("docstring") ccsds_dll_cc "Delay locked loop decimating from sample- to symbolrate

Delay estimation based on the Gardner passband timing error detector. The synchronous samples are obtained by cubic interpolation.

Return a shared_ptr to a new instance of ccsds_dll_cc.

Create an instance of ccsds_dll_cc and return it's shared_ptr."

%feature("docstring") ccsds_dll_cc::ccsds_dll_cc "Private constructor of the DLL.

Params: (osf, gamma)"

%feature("docstring") ccsds_dll_cc::get_frac "Return fractional part of the value.

Params: (value)"

%feature("docstring") ccsds_dll_cc::get_int "Return integral part of the value.

Params: (value)"

%feature("docstring") ccsds_dll_cc::to_real "Calculate real components from complex array.

Params: (out, in, num)"

%feature("docstring") ccsds_dll_cc::to_imag "Calculate imaginary components from complex array.

Params: (out, in, num)"

%feature("docstring") ccsds_dll_cc::to_real4 "Calculate real components from complex 4-element array.

Params: (out, in)"

%feature("docstring") ccsds_dll_cc::to_imag4 "Calculate imaginary components from complex 4-element array.

Params: (out, in)"

%feature("docstring") ccsds_dll_cc::interpolate_value "Calculate cubic interpolant.

Calculate cubic interpolant between the second and thrid data point based on the fractional index. If mu is 0 the interpolant is equal to the second data point, if mu is 0.5 the interpolation point is exactly in the middle between the second and the third data point.

Params: (y, mu)"

%feature("docstring") ccsds_dll_cc::interpolate_cvalue "Calculate cubic interpolant if a complex number.

Calls interpolate_value for the real and imaginary component and reassambles interpolants back to a complex sample.

Params: (y, mu)"

%feature("docstring") ccsds_dll_cc::gardner "Calculate a timing error from the last three interpolants.

Gardner passband timing error estimation based on the last two interpolated symbols and an intermediate sample from between.

e[k] = re{ [ y(kT-T+tau[k-1]) - y(kT+tau[k]) ] y*(kT-T/2+tau[k]) }

Params: (previous, intermediate, current)"

%feature("docstring") ccsds_dll_cc::propagate_tags "Propagates stream tags of the current block to the decimated output stream.

Params: (num_in)"

%feature("docstring") ccsds_dll_cc::~ccsds_dll_cc "Public deconstructor of the DLL.

Params: (NONE)"

%feature("docstring") ccsds_dll_cc::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_dll_cc::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_dll_cc "Delay locked loop decimating from sample- to symbolrate

Delay estimation based on the Gardner passband timing error detector. The synchronous samples are obtained by cubic interpolation.

Return a shared_ptr to a new instance of ccsds_dll_cc.

Create an instance of ccsds_dll_cc and return it's shared_ptr.

Params: (osf, gamma)"

%feature("docstring") ccsds_fll_cc "Frequency locked loop.

Frequency estimation based on feedforward delay and multiply scheme. Sends frequency estimate to local_oscillator.

See also: Local oscillator frequency correction feedback loop

Create a frequency locked loop and return it's shared pointer."

%feature("docstring") ccsds_fll_cc::ccsds_fll_cc "Private constructor for frequency locked loop.

Params: (obsv_length, loop_bw, power, msgq)"

%feature("docstring") ccsds_fll_cc::calc_power "take an array of complex samples and compute their power.

Take the complex input array , calculate the samplewise power d_POWER and store them in .

Params: (out, in, num)"

%feature("docstring") ccsds_fll_cc::calc_diffs "Calculate the samplewise phase difference as a rotator.

Implements the delay and multiply frequency error detector tmp_c[k] = in[k] * conj(in[k-1]) where conj( . ) computes the complex conjugate.

The algorithm assumes a complex sample 1+0j at the very beginning and stores the last sample per block for the next iteration after the first run. This way  input samples generate  output samples without going out of array bounds.

This block is using volk,   be aligned,   be aligned for better performance.

Params: (tmp_c, in, num)"

%feature("docstring") ccsds_fll_cc::calc_summs "Accumulates blocks of samples from a complex array.

Accumulates blocks of ccsds_fll_cc::D_L samples and stores them at the begining of . Used to filter out modulations and smooth the frequency estimates.

Params: (phasors, num)"

%feature("docstring") ccsds_fll_cc::calc_phases "Calculates the phases of an array of complex numbers.

Params: (tmp_f, tmp_c, num)"

%feature("docstring") ccsds_fll_cc::adjust_phases "Divides array samplewise by d_POWER.

By taking the M-th power of the samples the frequency offset between the samples is amplified by a factor of M. In this block this effect is reversed by dividing by d_POWER.

Params: (phases, num)"

%feature("docstring") ccsds_fll_cc::get_lo_tags "Looks for new lo_frequency tags in blocks and returns their frequency value.

/note The current version of ccsds_fll_cc does not conform to the new lo frequency feedback scheme and needs to be updated.

Params: (lo_freqs, num)"

%feature("docstring") ccsds_fll_cc::fill_freqs "Sums up  and  elementwise and copies the sum into  ccsds_fll_cc::D_L times each.

By summing over ccsds_fll_cc::D_L samples each we decimated by a rate of ccsds_fll_cc::D_L. While summing up tmp_fs and tmp_lo we repeat them ccsds_fll_cc::D_L times each to return to the original input rate.

Params: (tmp_f, tmp_fs, tmp_lo, num_out)"

%feature("docstring") ccsds_fll_cc::send_freq_estimate "Send a frequency estimate to the asynchronous message queue.

Params: (est)"

%feature("docstring") ccsds_fll_cc::substract_lo_freq "

Params: (tmp_f, tmp_lo, num)"

%feature("docstring") ccsds_fll_cc::calc_rotation "Rotates complex array according to frequency estimates.

Rotates samplewise by local phase ccsds_fll_cc::d_phase, which is increased by the estimates in  after each rotation.

Params: (out, in, tmp_f, num)"

%feature("docstring") ccsds_fll_cc::~ccsds_fll_cc "

Params: (NONE)"

%feature("docstring") ccsds_fll_cc::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_fll_cc "Frequency locked loop.

Frequency estimation based on feedforward delay and multiply scheme. Sends frequency estimate to local_oscillator.

See also: Local oscillator frequency correction feedback loop

Create a frequency locked loop and return it's shared pointer.

Params: (obsv_length, loop_bw, power, msgq)"

%feature("docstring") ccsds_frame_sync_b "Take the input streams and look for an ASM.

The block will start in searchmode, looking for every possible bit and byte offset. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again.

If the block finds an ASM it will create an asynchronous message containing the  bytes after the ASM. The input stream is copied the output stream in any case.

Return a shared_ptr to a new instance of ccsds_frame_sync_b.

Create an instance of ccsds_frame_sync_b and return it's shared_ptr."

%feature("docstring") ccsds_frame_sync_b::ccsds_frame_sync_b "Private constructor of the frame synchronizer.

Constructs a Frame sync block that searches for the ASM in the input streams and outputs an asynchronous message with the  bytes after the ASM. If no ASM is is found, no message is created. This block consumes the ASM in the output message.

The input of this block is copied one-to-one to the output stream.

Params: (ASM, threshold, ber_threshold, frame_length)"

%feature("docstring") ccsds_frame_sync_b::get_bytes_required "Calculates how many input bytes are needed to check for a known ASM position.

Params: ()"

%feature("docstring") ccsds_frame_sync_b::~ccsds_frame_sync_b "Public deconstructor of the FS.

Params: (NONE)"

%feature("docstring") ccsds_frame_sync_b::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_frame_sync_b::stop "

Params: ()"

%feature("docstring") ccsds_frame_sync_b::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_frame_sync_b "Take the input streams and look for an ASM.

The block will start in searchmode, looking for every possible bit and byte offset. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again.

If the block finds an ASM it will create an asynchronous message containing the  bytes after the ASM. The input stream is copied the output stream in any case.

Return a shared_ptr to a new instance of ccsds_frame_sync_b.

Create an instance of ccsds_frame_sync_b and return it's shared_ptr.

Params: (ASM, threshold, ber_threshold, frame_length)"

%feature("docstring") ccsds_framer_sink_dual "Given 2 parallel streams of bits and access_code flags, assemble TM Frames.

It is a fact of non-differential BPSK demodulation that an inverted bit stream may be produced. Two parallel gr_correlate_access_code_bb blocks have been setup, one looking for the non-inverted ASM, and the other for the inverted ASM. If the framer sink detects a frame marker on the inverted stream, it inverts the input.

There is no stream output, instead the MSB-first packed bytes of the synchronised frame are inserted into a dedicated output queue.

The input data consists of bytes that have two bits used. Bit 0, the LSB, contains the data bit. Bit 1 if set, indicates that the corresponding bit is the the first bit of the packet. That is, this bit is the first one after the access code."

%feature("docstring") ccsds_framer_sink_dual::bit "

Params: (input)"

%feature("docstring") ccsds_framer_sink_dual::inverse_bit "

Params: (input)"

%feature("docstring") ccsds_framer_sink_dual::d_bit "

Params: (input)"

%feature("docstring") ccsds_framer_sink_dual::check_flag "

Params: (byte)"

%feature("docstring") ccsds_framer_sink_dual::forecast "

Params: (, ninput_items_required)"

%feature("docstring") ccsds_framer_sink_dual::ccsds_framer_sink_dual "

Params: (target_queue, framebytes, sync_marker_bytes)"

%feature("docstring") ccsds_framer_sink_dual::enter_search "

Params: (NONE)"

%feature("docstring") ccsds_framer_sink_dual::enter_locking "

Params: (standard)"

%feature("docstring") ccsds_framer_sink_dual::enter_have_sync "

Params: (NONE)"

%feature("docstring") ccsds_framer_sink_dual::~ccsds_framer_sink_dual "

Params: (NONE)"

%feature("docstring") ccsds_framer_sink_dual::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_framer_sink_dual "Given 2 parallel streams of bits and access_code flags, assemble TM Frames.

It is a fact of non-differential BPSK demodulation that an inverted bit stream may be produced. Two parallel gr_correlate_access_code_bb blocks have been setup, one looking for the non-inverted ASM, and the other for the inverted ASM. If the framer sink detects a frame marker on the inverted stream, it inverts the input.

There is no stream output, instead the MSB-first packed bytes of the synchronised frame are inserted into a dedicated output queue.

The input data consists of bytes that have two bits used. Bit 0, the LSB, contains the data bit. Bit 1 if set, indicates that the corresponding bit is the the first bit of the packet. That is, this bit is the first one after the access code.

Params: (target_queue, framebytes, sync_marker_bytes)"

%feature("docstring") ccsds_local_oscillator_cc "Local oscillator controlled by asynchronous frequency corrections

Oscillates with an internal frequency (initially zero) that can be tuned by asynchronous frequency correction messages. Every time a new frequency is used a stream tag with the new frequency is attatched to the sample stream.

Frequency corrections are taken on the input port \"freq\"."

%feature("docstring") ccsds_local_oscillator_cc::ccsds_local_oscillator_cc "

Params: (block_length, osf)"

%feature("docstring") ccsds_local_oscillator_cc::wrap_phase "Keep d_phase in range [-pi,pi].

Params: ()"

%feature("docstring") ccsds_local_oscillator_cc::process_messages "Look for new frequency corrections and update the per sample phase increase.

Get new message from d_msgq and check for valid frequency update messages.

Params: (msg_in)"

%feature("docstring") ccsds_local_oscillator_cc::add_tag "Attach a stream tag with the current per symbol frequency estimate to the first sample in the current input block.

Params: ()"

%feature("docstring") ccsds_local_oscillator_cc::calc_rotation "Rotate the sample vector with current frequency estimate.

Rotate the complex input vector with the local phase d_phase, and a linear changing phase increase with constant slope and offset interpolated between d_phase_incr and d_last_phase_incr.

The phase increase from sample i to i+1 is phase[i+1] = phase[i] + phase_incr + i*phase_incr_slope

Params: (out, in, phase_incr, phase_incr_slope, num)"

%feature("docstring") ccsds_local_oscillator_cc::~ccsds_local_oscillator_cc "

Params: (NONE)"

%feature("docstring") ccsds_local_oscillator_cc::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_local_oscillator_cc "Local oscillator controlled by asynchronous frequency corrections

Oscillates with an internal frequency (initially zero) that can be tuned by asynchronous frequency correction messages. Every time a new frequency is used a stream tag with the new frequency is attatched to the sample stream.

Frequency corrections are taken on the input port \"freq\".

Params: (block_length, osf)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb "M-PSK Ambiguity resolution.

The input to this block is a stream of unpacked bytes, each containing  used information bits. The output of this block is a stream of packed bytes.

The block will start in searchmode, looking for every possible bit and byte offset in all possible  ambiguities. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position and ambiguity. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again.

The stream with the last known ambiguity and bit offset is send to the output. In locked state this will result in the correct byte stream beeing output, while in searchmode the output might be nonsense (since there is no ASM in it) or might still be correct if the block has lost it's lock to early.

Return a shared_ptr to a new instance of ccsds_mpsk_ambiguity_resolver_bb.

Create an instance of ccsds_mpsk_ambiguity_resolver_bb and return it's shared_ptr."

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb::ccsds_mpsk_ambiguity_resolver_bb "Private constructor of the AR.

Constructs a AR block that searches for the ASM in the  input streams and outputs the one that contains the ASM. If no ASM is found, no output is given. This block preserves the ASM, as it is still needed for frame synchronization after decoding.

Params: (M, ASM, threshold, ber_threshold, frame_length)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb::from_packed_to_unpacked "Returns the number of unpacked symbols (as float) that relate to the given number of packed bytes.

Params: (packed)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb::confine_offsets "Ensures that  is confined within [0,d_ldM). If not  is increased accordingly.

Params: ()"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb::convert_ambiguity "Change ambiguity of incomming symbol stream.

Params: (out, in, num, ambiguity)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb::~ccsds_mpsk_ambiguity_resolver_bb "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_bb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_ambiguity_resolver_bb "M-PSK Ambiguity resolution.

The input to this block is a stream of unpacked bytes, each containing  used information bits. The output of this block is a stream of packed bytes.

The block will start in searchmode, looking for every possible bit and byte offset in all possible  ambiguities. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position and ambiguity. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again.

The stream with the last known ambiguity and bit offset is send to the output. In locked state this will result in the correct byte stream beeing output, while in searchmode the output might be nonsense (since there is no ASM in it) or might still be correct if the block has lost it's lock to early.

Return a shared_ptr to a new instance of ccsds_mpsk_ambiguity_resolver_bb.

Create an instance of ccsds_mpsk_ambiguity_resolver_bb and return it's shared_ptr.

Params: (M, ASM, threshold, ber_threshold, frame_length)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f "M-PSK soft bit ambiguity resolution and frame synchronization.

The input to this block is a stream of  grouped soft bits. The output of this block is a asynchronous message containing a float vector with the soft bits of a frame. The bytes passed to this block as ASM are cut off, but any skipped bits will be in the output message.

So if you pass the two ASM bytes 0x00 0xAA to this block followed by two bytes frame data (FD1 and FD2), followed by another two bytes ASM and set  to zero the output will be a message containing FD1 and FD2. If  is set to two, the output will be FD1, FD2 followed by the first two zero bits from the ASM.

Using  (setting it to nonzero values)

The block will start in searchmode, looking for every possible bit offset in all possible  ambiguities. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position and ambiguity. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again.

Return a shared_ptr to a new instance of ccsds_mpsk_ambiguity_resolver_f.

Create an instance of ccsds_mpsk_ambiguity_resolver_f and return it's shared_ptr."

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::ccsds_mpsk_ambiguity_resolver_f "Private constructor of the AR.

Constructs a AR block that searches for the ASM in the  input streams and outputs the one that contains the ASM. If no ASM is found, no output is given. This block preserves the ASM, as it is still needed for frame synchronization after decoding.

Params: (M, ASM, asm_len, threshold, correlation_threshold, frame_length, num_tail_syms)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::from_packed_to_unpacked "Returns the number of unpacked symbols (as float) that relate to the given number of packed bytes.

Params: (packed)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::confine_offsets "Ensures that  is confined within [0,d_ldM). If not  is increased accordingly.

Params: ()"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::convert_ambiguity "Change ambiguity of incomming symbol stream.

Params: (out, in, num_bits, ambiguity)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::check_for_ASM "Return the correlation between the input sequence and the ASM.

Params: (in, offset)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::search_for_ASM "Search for ASM in input sequence and return maximum found correlation.

The search checks all possible offsets in the range of [0, num_in-d_ASM_LEN_BITS] starting from zero and abborts if a correlation of 1.0f is reached, or all offsets have been tried.

Params: (in, num_in, offset)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::convert_to_char "Converts an array of  softbits into a byte of  hardbits.

Params: (in)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::get_lower_mul "Get the highest multiple of  lower or equal than .

Params: (n)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::get_upper_mul "Get the lowest multiple of  higher or equal than .

Params: (n)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::updateTags "Scan all stream tags in scope and copy them into .

Scan for all tags after max(, ) and before .

Params: (until)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::extractTags "Reads all tags from the buffer and creates a frame metadata header out of it.

Calculates the reception time of the sample with the absolute count  based on the tag rx_time. Otherwise ignore the rx_time tag and label the calculated reception time rx_time in the created metadata.

Copies all other tags into the header.

Params: (from, len)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::~ccsds_mpsk_ambiguity_resolver_f "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::stop "

Params: ()"

%feature("docstring") ccsds_mpsk_ambiguity_resolver_f::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_ambiguity_resolver_f "M-PSK soft bit ambiguity resolution and frame synchronization.

The input to this block is a stream of  grouped soft bits. The output of this block is a asynchronous message containing a float vector with the soft bits of a frame. The bytes passed to this block as ASM are cut off, but any skipped bits will be in the output message.

So if you pass the two ASM bytes 0x00 0xAA to this block followed by two bytes frame data (FD1 and FD2), followed by another two bytes ASM and set  to zero the output will be a message containing FD1 and FD2. If  is set to two, the output will be FD1, FD2 followed by the first two zero bits from the ASM.

Using  (setting it to nonzero values)

The block will start in searchmode, looking for every possible bit offset in all possible  ambiguities. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position and ambiguity. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again.

Return a shared_ptr to a new instance of ccsds_mpsk_ambiguity_resolver_f.

Create an instance of ccsds_mpsk_ambiguity_resolver_f and return it's shared_ptr.

Params: (M, ASM, asm_len, threshold, correlation_threshold, frame_length, num_tail_syms)"

%feature("docstring") ccsds_mpsk_demod2_cb "M-PSK demodulator.

M-PSK demodulation based on the phase of the incomming signal.

The incomming phase is converted to a conteallation point (counting counterclockwise starting from zero at symbol 1+0j) with the non linear function:



And then the constellation point is mapped to the actual symbol bit sequence by a binary to gray code mapping []:

Create an M-PSK demodulator and return it's shared pointer.

If  is zero, only the detected symbols are generated. If set to nonzero a second output stream at the same rate as the detected symbols will be generated containing a  bit value representing the confidence in this detection, where 0 means this symbol is a guess as good as any other symbol and (2 ^) - 1 means highest confidence in the detected symbol.

The confidence value is generated by a linear quantization of the symbol's squared magnitude in the range from [0.0 to 1.0]. Any symbol with a squared magnitude higher than 1.0 will have highest confidence as well."

%feature("docstring") ccsds_mpsk_demod2_cb::ccsds_mpsk_demod2_cb "Public constructor of the M-PSK demodulator.

Demodulate M-PSK modulated symbols back to data bits. This demodulator detects the data bits by a non linear transformation of the sample's phase. See ccsds_mpsk_demod_cb for a demodulator based on the nearest neighbour principle.

The first output stream will output the detected symbols. A second output stream at the same rate as the detected symbols will be generated containing a  bit value representing the confidence in this detection, where 0 means this symbol is a guess as good as any other symbol and (2 ^) - 1 means highest confidence in the detected symbol.

The confidence value is generated by a linear quantization of the symbol's squared magnitude in the range from [0.0 to 1.0]. Any symbol with a squared magnitude higher than 1.0 will have highest confidence as well.

Params: (M, quant_bits)"

%feature("docstring") ccsds_mpsk_demod2_cb::quant "Perform the quantization.

Params: (mag)"

%feature("docstring") ccsds_mpsk_demod2_cb::~ccsds_mpsk_demod2_cb "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_mpsk_demod2_cb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_demod2_cb "M-PSK demodulator.

M-PSK demodulation based on the phase of the incomming signal.

The incomming phase is converted to a conteallation point (counting counterclockwise starting from zero at symbol 1+0j) with the non linear function:



And then the constellation point is mapped to the actual symbol bit sequence by a binary to gray code mapping []:

Create an M-PSK demodulator and return it's shared pointer.

If  is zero, only the detected symbols are generated. If set to nonzero a second output stream at the same rate as the detected symbols will be generated containing a  bit value representing the confidence in this detection, where 0 means this symbol is a guess as good as any other symbol and (2 ^) - 1 means highest confidence in the detected symbol.

The confidence value is generated by a linear quantization of the symbol's squared magnitude in the range from [0.0 to 1.0]. Any symbol with a squared magnitude higher than 1.0 will have highest confidence as well.

Params: (M, quant_bits)"

%feature("docstring") ccsds_mpsk_demod_cb "M-PSK demodulator.

M-PSK demodulation based on nearest neighbour principle.

For M=2 and M=4 detection is done by algorithmic decision borders.

For all other modulation orders a constellation is generated and for every every sample the symbol with the minimum distance to the constellation symbol is chosen from the precomputed constellation.

Create an M-PSK demodulator and return it's shared pointer."

%feature("docstring") ccsds_mpsk_demod_cb::ccsds_mpsk_demod_cb "Private constructor of the M-PSK demodulator.

Demodulate M-PSK modulated symbols back to data bits. This demodulator has an optimized detector for BPSK and QPSK. For other modulation orders it created a constellation and compares this constellation to the received sample to get a nearest enighbour decision.

Params: (M)"

%feature("docstring") ccsds_mpsk_demod_cb::detect_bpsk_symbol "Optimized BPSK detector.

BPSK detector based on the decision border on the Quadrature component axis.

Params: (symbol)"

%feature("docstring") ccsds_mpsk_demod_cb::detect_qpsk_symbol "Optimized QPSK detector.

QPSK detector based on the two angle bisectors as decision borders.

Params: (symbol)"

%feature("docstring") ccsds_mpsk_demod_cb::detect_mpsk_symbol "Detect general M-PSK modulated symbol.

The constructor ccsds_mpsk_demod_cb() generates a M-PSK constellation. For each sample the symbol with the minimum distance to the constellation symbol is chosen from the precomputed constellation.

Params: (symbol)"

%feature("docstring") ccsds_mpsk_demod_cb::~ccsds_mpsk_demod_cb "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_mpsk_demod_cb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_demod_cb "M-PSK demodulator.

M-PSK demodulation based on nearest neighbour principle.

For M=2 and M=4 detection is done by algorithmic decision borders.

For all other modulation orders a constellation is generated and for every every sample the symbol with the minimum distance to the constellation symbol is chosen from the precomputed constellation.

Create an M-PSK demodulator and return it's shared pointer.

Params: (M)"

%feature("docstring") ccsds_mpsk_detector_soft_cf "M-PSK soft bit detector.

M-PSK soft bit detector.

The incomming phase is converted to a conteallation point (counting counterclockwise starting from zero at symbol 1+0j) with the non linear function:



And then the constellation point is mapped to the actual symbol bit sequence by a binary to gray code mapping []:



For each incomming symbol  output symbols are generated containing the detected soft bit as float values, where -1.0f represents the most confident 0 bit, 0.0f is an erasure (in this case, the received symbol has no energy at all) and 1.0f represents the most confident 1 bit.

Create an M-PSK softbit detector and return it's shared pointer."

%feature("docstring") ccsds_mpsk_detector_soft_cf::ccsds_mpsk_detector_soft_cf "Public constructor of the M-PSK soft bit detector.

Detects data bits from M-PSK modulated symbols. This block detects the data bits by a non linear transformation of the sample's phase.

Params: (M)"

%feature("docstring") ccsds_mpsk_detector_soft_cf::get_ldM "Calculated the logariothmus dualis of M (log of base two)

Params: (M)"

%feature("docstring") ccsds_mpsk_detector_soft_cf::~ccsds_mpsk_detector_soft_cf "

Params: (NONE)"

%feature("docstring") ccsds_mpsk_detector_soft_cf::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_detector_soft_cf "M-PSK soft bit detector.

M-PSK soft bit detector.

The incomming phase is converted to a conteallation point (counting counterclockwise starting from zero at symbol 1+0j) with the non linear function:



And then the constellation point is mapped to the actual symbol bit sequence by a binary to gray code mapping []:



For each incomming symbol  output symbols are generated containing the detected soft bit as float values, where -1.0f represents the most confident 0 bit, 0.0f is an erasure (in this case, the received symbol has no energy at all) and 1.0f represents the most confident 1 bit.

Create an M-PSK softbit detector and return it's shared pointer.

Params: (M)"

%feature("docstring") ccsds_mpsk_mod_bc "M-PSK modulator.

On construction a constellation array is generated. This constellation is later used to map the individual data bits to symbols.

Create a M-PSK modulator and return it's shared pointer."

%feature("docstring") ccsds_mpsk_mod_bc::ccsds_mpsk_mod_bc "Private constructor for the M-PSK modulator.

Params: (M)"

%feature("docstring") ccsds_mpsk_mod_bc::~ccsds_mpsk_mod_bc "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_mpsk_mod_bc::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_mod_bc "M-PSK modulator.

On construction a constellation array is generated. This constellation is later used to map the individual data bits to symbols.

Create a M-PSK modulator and return it's shared pointer.

Params: (M)"

%feature("docstring") ccsds_mpsk_preamble_cc "Append a M-PSK preamble before the input stream.

Outputs  preamble symbols, before the input stream is copied to the output stream. This can be used to let the receiver synchronize before sending frames. This way even the first transmitted frame can be received and decoded.

The preable consists of alternating BPSK symbols (starting with 1+0j). BPSK symbols are also part of any other M-PSK constellation where M is a power of two and thus works for all M-PSK receivers.

Create a M-PSK preamble block and return it's shared pointer."

%feature("docstring") ccsds_mpsk_preamble_cc::ccsds_mpsk_preamble_cc "Private constructor for the M-PSK preamble block.

Params: (num_symbols)"

%feature("docstring") ccsds_mpsk_preamble_cc::~ccsds_mpsk_preamble_cc "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_mpsk_preamble_cc::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_mpsk_preamble_cc::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_mpsk_preamble_cc "Append a M-PSK preamble before the input stream.

Outputs  preamble symbols, before the input stream is copied to the output stream. This can be used to let the receiver synchronize before sending frames. This way even the first transmitted frame can be received and decoded.

The preable consists of alternating BPSK symbols (starting with 1+0j). BPSK symbols are also part of any other M-PSK constellation where M is a power of two and thus works for all M-PSK receivers.

Create a M-PSK preamble block and return it's shared pointer.

Params: (num_symbols)"



%feature("docstring") ccsds_msg_copy::ccsds_msg_copy "

Params: ()"

%feature("docstring") ccsds_msg_copy::process_frame "

Params: (msg)"

%feature("docstring") ccsds_make_msg_copy "

Params: ()"

%feature("docstring") ccsds_msg_null_src "Defines a message out port that is never used. This block can be used to connect asynchronous blocks to the flowgraph and then pass in messages to them directly from the python code.

This block should only be used in QA code."

%feature("docstring") ccsds_msg_null_src::ccsds_msg_null_src "Private constructor. Define a outgoing port witht the name . Otherwise do nothing.

Params: (dummy)"

%feature("docstring") ccsds_make_msg_null_src "Defines a message out port that is never used. This block can be used to connect asynchronous blocks to the flowgraph and then pass in messages to them directly from the python code.

This block should only be used in QA code.

Params: (dummy)"

%feature("docstring") ccsds_pll_cc "Phase locked loop that sends frequency correction messages.

Phase locked loop using the Viterbi&Viterbi method to estimate the sample's phase offset. Estimates are filtered with a linear second order low pass filter.

After reception of a frequency_offset tag the loop waits for another PLL_FREQ_UPDATE samples before a new frequency correction message is send out to close the frequency feedback loop.

Frequency corrections are send out on the port \"freq\".

Generate a Phase locked loop and return it's shared pointer."

%feature("docstring") ccsds_pll_cc::ccsds_pll_cc "Private constructor.

Params: (m, loop_bandwidth)"

%feature("docstring") ccsds_pll_cc::remove_modulation "Remove phase modulation by taking the M-th power of the samples.

Remove modulation by following the M-power principle. The ccsds_pll_cc::d_M th power of the input samples is calculated and written to the complex array .

Params: (tmp_c, in, num)"

%feature("docstring") ccsds_pll_cc::calc_phases "Calculate the phases of an array of complex numbers.

Params: (tmp_f, tmp_c, num)"

%feature("docstring") ccsds_pll_cc::check_lo_tags "Checks input stream for possible frequency_offset tags.

If ccsds_pll_cc::d_lo_msg_tag is  the function will return immediatley. This means that the loop has allready eceived a frequency_offset tag within the last PLL_FREQ_UPDATE samples and is now waiting for the counter to hit zero so a new frequency correction message can be send.

If ccsds_pll_cc::d_lo_msg_tag is , the funktion will look for frequency_offset tags in range [from,from+num) and if any is found the earliest's relative offset to  plus PLL_FREQ_UPDATE is stored in ccsds_pll_cc::d_lo_msg_offset and ccsds_pll_cc::d_lo_msg_tag is set to . This will cause the loop to process another PLL_FREQ_UPDATE samples before new frequency correction messages are send.

Params: (from, num)"

%feature("docstring") ccsds_pll_cc::send_freq_estimate "Send a frequency correction message.

Sends given estimate  to output port \"freq\" according to Frequency correction messages

Params: (est)"

%feature("docstring") ccsds_pll_cc::calc_rotation "Rotates complex array according to phase estimates.

Rotates samplewise by estimated phase.

Params: (out, in, tmp_f, num)"

%feature("docstring") ccsds_pll_cc::~ccsds_pll_cc "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_pll_cc::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_pll_cc "Phase locked loop that sends frequency correction messages.

Phase locked loop using the Viterbi&Viterbi method to estimate the sample's phase offset. Estimates are filtered with a linear second order low pass filter.

After reception of a frequency_offset tag the loop waits for another PLL_FREQ_UPDATE samples before a new frequency correction message is send out to close the frequency feedback loop.

Frequency corrections are send out on the port \"freq\".

Generate a Phase locked loop and return it's shared pointer.

Params: (m, loop_bandwidth)"

%feature("docstring") ccsds_randomiser "Combines the input frame with a pseudo random sequence by XOR operation. The pseudo random sequence is generated by an 8-bit linear feedback shift register as defined in ECSS.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the input message XORed with the pseudo random sequence. The LFSR is set to the initial  state before every block.This new message is then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_randomiser.

Create an instance of ccsds_randomiser and return it's shared_ptr."

%feature("docstring") ccsds_randomiser::ccsds_randomiser "

Params: (polynomial, seed)"

%feature("docstring") ccsds_randomiser::reset "Reset LFSR to  state

Params: ()"

%feature("docstring") ccsds_randomiser::get_next_byte "Perform 8 updates on the LFSR and return it's output

Params: ()"

%feature("docstring") ccsds_randomiser::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") ccsds_randomiser::~ccsds_randomiser "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_randomiser::work "Dummy work function.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_randomiser "Combines the input frame with a pseudo random sequence by XOR operation. The pseudo random sequence is generated by an 8-bit linear feedback shift register as defined in ECSS.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the input message XORed with the pseudo random sequence. The LFSR is set to the initial  state before every block.This new message is then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_randomiser.

Create an instance of ccsds_randomiser and return it's shared_ptr.

Params: (polynomial, seed)"

%feature("docstring") ccsds_randomiser_bb "De/randomise a TM Frame using an LFSR.

The input is a stream of unpacked bits that represent the encoded frames only, i.e. the ASMs have been removed from the stream, and are synchronised on the first bit of the frame.

There is no stream output, instead the bits of the decoded frame are packed and inserted into a dedicated output queue.

The scrambler works by XORing the LSBs of the incoming bit stream by the output of the LFSR. After 'frame_bits' bits have been processed, the shift register is reset to the seed value. This allows processing fixed length frames according to CCSDS recommendations.

The gnuradio-core gri_lfsr has a bug in that can be worked-around by defining the length of the shift register (in the 'len' parameter) to be one less than its actual length"

%feature("docstring") ccsds_randomiser_bb::ccsds_randomiser_bb "

Params: (output_queue, frame_bits, mask, seed, len)"

%feature("docstring") ccsds_randomiser_bb::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_randomiser_bb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_randomiser_bb "De/randomise a TM Frame using an LFSR.

The input is a stream of unpacked bits that represent the encoded frames only, i.e. the ASMs have been removed from the stream, and are synchronised on the first bit of the frame.

There is no stream output, instead the bits of the decoded frame are packed and inserted into a dedicated output queue.

The scrambler works by XORing the LSBs of the incoming bit stream by the output of the LFSR. After 'frame_bits' bits have been processed, the shift register is reset to the seed value. This allows processing fixed length frames according to CCSDS recommendations.

The gnuradio-core gri_lfsr has a bug in that can be worked-around by defining the length of the shift register (in the 'len' parameter) to be one less than its actual length

Params: (output_queue, frame_bits, mask, seed, len)"

%feature("docstring") ccsds_rs_decode "Takes the transmitted codeblock and generates the frame out of it, or drops it, if recovery is not possible.

The block has an input message port named \"in\" at which codeblocks of length  bytes are received. On reception the data codeblock is interleaved and decoded. The generated frame consisting of  bytes is then queued in the output message port named \"out\" or dropped if it can't be decoded.

Return a shared_ptr to a new instance of ccsds_rs_decode.

Create an instance of ccsds_rs_decode and return it's shared_ptr."

%feature("docstring") ccsds_rs_decode::ccsds_rs_decode "Public constructor of the add ASM block.

Params: (I)"

%feature("docstring") ccsds_rs_decode::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") ccsds_rs_decode::~ccsds_rs_decode "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") ccsds_make_rs_decode "Takes the transmitted codeblock and generates the frame out of it, or drops it, if recovery is not possible.

The block has an input message port named \"in\" at which codeblocks of length  bytes are received. On reception the data codeblock is interleaved and decoded. The generated frame consisting of  bytes is then queued in the output message port named \"out\" or dropped if it can't be decoded.

Return a shared_ptr to a new instance of ccsds_rs_decode.

Create an instance of ccsds_rs_decode and return it's shared_ptr.

Params: (I)"

%feature("docstring") ccsds_rs_decode_bb "A (255,223) Reed-Solomon decoder for CCSDS TM Frames

The forward error correction library by Phil Karn is used.

The input is a stream of R-S symbols (8 bit packed bytes) that represent the codeblocks (encoded frames) only, i.e. the ASMs have been removed from the stream, and are synchronised on the first symbol of the block.

There is no stream output, instead the packed bytes of the decoded frame are inserted into a dedicated output queue (minus the R-S parity bits). Frames with uncorrectable errors are not propagated."

%feature("docstring") ccsds_rs_decode_bb::ccsds_rs_decode_bb "

Params: (output_queue, codeblock_bits)"

%feature("docstring") ccsds_rs_decode_bb::decode "

Params: (deinterleaved)"

%feature("docstring") ccsds_rs_decode_bb::forecast "

Params: (, ninput_items_required)"

%feature("docstring") ccsds_rs_decode_bb::~ccsds_rs_decode_bb "

Params: (NONE)"

%feature("docstring") ccsds_rs_decode_bb::stop "

Params: ()"

%feature("docstring") ccsds_rs_decode_bb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_rs_decode_bb "A (255,223) Reed-Solomon decoder for CCSDS TM Frames

The forward error correction library by Phil Karn is used.

The input is a stream of R-S symbols (8 bit packed bytes) that represent the codeblocks (encoded frames) only, i.e. the ASMs have been removed from the stream, and are synchronised on the first symbol of the block.

There is no stream output, instead the packed bytes of the decoded frame are inserted into a dedicated output queue (minus the R-S parity bits). Frames with uncorrectable errors are not propagated.

Params: (output_queue, codeblock_bits)"

%feature("docstring") ccsds_rs_encode "Takes I frames and generates the transmitted codeblock out of it.

The block has an input message port named \"in\" at which frames of length  bytes are received. On reception the frames are interleaved and encoded. The generated codeblock consisting of  bytes is then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_rs_encode.

Create an instance of ccsds_rs_encode and return it's shared_ptr."

%feature("docstring") ccsds_rs_encode::ccsds_rs_encode "Public constructor of the add ASM block.

Params: (I)"

%feature("docstring") ccsds_rs_encode::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") ccsds_rs_encode::~ccsds_rs_encode "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") ccsds_make_rs_encode "Takes I frames and generates the transmitted codeblock out of it.

The block has an input message port named \"in\" at which frames of length  bytes are received. On reception the frames are interleaved and encoded. The generated codeblock consisting of  bytes is then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_rs_encode.

Create an instance of ccsds_rs_encode and return it's shared_ptr.

Params: (I)"

%feature("docstring") ccsds_softbits_msg_to_bytes_b "Reads frames of softbits from message input port \"in\" and outputs the corresponding hard decisions in a packed byte.

Return a shared_ptr to a new instance of ccsds_softbits_msg_to_bytes_b.

Create an instance of ccsds_softbits_msg_to_bytes_b and return it's shared_ptr."

%feature("docstring") ccsds_softbits_msg_to_bytes_b::ccsds_softbits_msg_to_bytes_b "Private constructor of the TICP frame sink.

Params: ()"

%feature("docstring") ccsds_softbits_msg_to_bytes_b::process_message "Store incomming message into buffer.

Params: (msg_in)"

%feature("docstring") ccsds_softbits_msg_to_bytes_b::~ccsds_softbits_msg_to_bytes_b "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") ccsds_softbits_msg_to_bytes_b::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_softbits_msg_to_bytes_b "Reads frames of softbits from message input port \"in\" and outputs the corresponding hard decisions in a packed byte.

Return a shared_ptr to a new instance of ccsds_softbits_msg_to_bytes_b.

Create an instance of ccsds_softbits_msg_to_bytes_b and return it's shared_ptr.

Params: ()"

%feature("docstring") ccsds_sym_to_softbits_bb "Convert data symbols into soft bits for convolutional coding.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a byte is put to the output streams containing a 7 bit quantized soft bit as expected by Phil Karn's viterbi decoder. (0: strongest zero bit, 128: erasure, 255: strongest one bit).

Create a symbol to soft bits encoder block and return it's shared pointer.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a byte is put to the output streams containing a 7 bit quantized soft bit as expected by Phil Karn's viterbi decoder. (0: strongest zero bit, 128: erasure, 255: strongest one bit)."

%feature("docstring") ccsds_sym_to_softbits_bb::ccsds_sym_to_softbits_bb "Private constructor for the symbol to soft bits encoder block.

Params: (info_bits, num_quant_bits)"

%feature("docstring") ccsds_sym_to_softbits_bb::bit_to_softbit "Transforms a data bit and it's confidence into a soft bit.

Params: (data_bit, confidence)"

%feature("docstring") ccsds_sym_to_softbits_bb::~ccsds_sym_to_softbits_bb "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_sym_to_softbits_bb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_sym_to_softbits_bb "Convert data symbols into soft bits for convolutional coding.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a byte is put to the output streams containing a 7 bit quantized soft bit as expected by Phil Karn's viterbi decoder. (0: strongest zero bit, 128: erasure, 255: strongest one bit).

Create a symbol to soft bits encoder block and return it's shared pointer.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a byte is put to the output streams containing a 7 bit quantized soft bit as expected by Phil Karn's viterbi decoder. (0: strongest zero bit, 128: erasure, 255: strongest one bit).

Params: (info_bits, num_quant_bits)"

%feature("docstring") ccsds_sym_to_softbits_bf "Convert data symbols into soft bits for convolutional coding.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a float is put to the output streams containing soft bit as expected by GNURadios decode_ccsds27_fb block (-1.0f: strongest zero bit, 0.0f: erasure, 1.0f: strongest one bit).

Create a symbol to soft bits encoder block and return it's shared pointer.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a float is put to the output streams containing soft bit as expected by GNURadios decode_ccsds27_fb block (-1.0f: strongest zero bit, 0.0f: erasure, 1.0f: strongest one bit)."

%feature("docstring") ccsds_sym_to_softbits_bf::ccsds_sym_to_softbits_bf "Private constructor for the symbol to soft bits encoder block.

Params: (info_bits, num_quant_bits)"

%feature("docstring") ccsds_sym_to_softbits_bf::bit_to_softbit "Transforms a data bit and it's confidence into a soft bit.

Params: (data_bit, confidence)"

%feature("docstring") ccsds_sym_to_softbits_bf::~ccsds_sym_to_softbits_bf "Public deconstructor.

Params: (NONE)"

%feature("docstring") ccsds_sym_to_softbits_bf::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_sym_to_softbits_bf::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_sym_to_softbits_bf "Convert data symbols into soft bits for convolutional coding.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a float is put to the output streams containing soft bit as expected by GNURadios decode_ccsds27_fb block (-1.0f: strongest zero bit, 0.0f: erasure, 1.0f: strongest one bit).

Create a symbol to soft bits encoder block and return it's shared pointer.

Takes two input sequences with the same rate. The first stream contains decoded symbols (sequences of  data bits) and the second contains a  value which represents the confidence in the data bits, where a lower value means less confidence.

For each information bit in the input streams a float is put to the output streams containing soft bit as expected by GNURadios decode_ccsds27_fb block (-1.0f: strongest zero bit, 0.0f: erasure, 1.0f: strongest one bit).

Params: (info_bits, num_quant_bits)"

%feature("docstring") ccsds_ticp_frame_sink "Reads frames from message input port \"in\" and outputs it to a TicpClient.

Return a shared_ptr to a new instance of ccsds_ticp_frame_sink.

Create an instance of ccsds_ticp_frame_sink and return it's shared_ptr."

%feature("docstring") ccsds_ticp_frame_sink::ccsds_ticp_frame_sink "Private constructor of the TICP frame sink.

Params: (port, frame_length, data_type, map_names, map_types)"

%feature("docstring") ccsds_ticp_frame_sink::getFrame "Pops a frame from the queue and send it to the client. Blocks until there is a frame in the queue.

Params: ()"

%feature("docstring") ccsds_ticp_frame_sink::process_message "Store incomming message into buffer.

Params: (msg_in)"

%feature("docstring") ccsds_ticp_frame_sink::~ccsds_ticp_frame_sink "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") ccsds_make_ticp_frame_sink "Reads frames from message input port \"in\" and outputs it to a TicpClient.

Return a shared_ptr to a new instance of ccsds_ticp_frame_sink.

Create an instance of ccsds_ticp_frame_sink and return it's shared_ptr.

Params: (port, frame_length, data_type, map_names, map_types)"

%feature("docstring") ccsds_ticp_frame_source "Reads frames from a TicpServer and outputs it as a stream.

Return a shared_ptr to a new instance of ccsds_ticp_frame_source.

Create an instance of ccsds_ticp_frame_source and return it's shared_ptr."

%feature("docstring") ccsds_ticp_frame_source::ccsds_ticp_frame_source "Private constructor of the TICP frame source.

Params: (hostname, port, frame_length, data_type, map_names, map_types, frame_rate)"

%feature("docstring") ccsds_ticp_frame_source::asynchronous_work "Asynchronous work function which is processing the queues.

Params: ()"

%feature("docstring") ccsds_ticp_frame_source::~ccsds_ticp_frame_source "Public deconstructor of the ticp frame source.

Params: (NONE)"

%feature("docstring") ccsds_make_ticp_frame_source "Reads frames from a TicpServer and outputs it as a stream.

Return a shared_ptr to a new instance of ccsds_ticp_frame_source.

Create an instance of ccsds_ticp_frame_source and return it's shared_ptr.

Params: (hostname, port, frame_length, data_type, map_names, map_types, frame_rate)"

%feature("docstring") ccsds_ticp_frame_source_b "Reads frames from a TicpServer and outputs it as a stream.

Return a shared_ptr to a new instance of ccsds_ticp_frame_source_b.

Create an instance of ccsds_ticp_frame_source_b and return it's shared_ptr."

%feature("docstring") ccsds_ticp_frame_source_b::ccsds_ticp_frame_source_b "Private constructor of the TICP frame source.

Params: (hostname, port, frame_length)"

%feature("docstring") ccsds_ticp_frame_source_b::~ccsds_ticp_frame_source_b "Public deconstructor of the ticp frame source.

Params: (NONE)"

%feature("docstring") ccsds_ticp_frame_source_b::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") ccsds_ticp_frame_source_b::stop "

Params: ()"

%feature("docstring") ccsds_ticp_frame_source_b::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") ccsds_make_ticp_frame_source_b "Reads frames from a TicpServer and outputs it as a stream.

Return a shared_ptr to a new instance of ccsds_ticp_frame_source_b.

Create an instance of ccsds_ticp_frame_source_b and return it's shared_ptr.

Params: (hostname, port, frame_length)"

%feature("docstring") ccsds_trunk_tail "Takes a frame and trunkates a number of bits from the end.

The block has an input message port named \"in\" at which new frames can arrive. The message is trunkated then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_trunk_tail.

Create an instance of ccsds_trunk_tail and return it's shared_ptr."

%feature("docstring") ccsds_trunk_tail::ccsds_trunk_tail "Public constructor of the add ASM block.

Params: (trunk_len, frame_len)"

%feature("docstring") ccsds_trunk_tail::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") ccsds_trunk_tail::~ccsds_trunk_tail "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") ccsds_trunk_tail::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_trunk_tail "Takes a frame and trunkates a number of bits from the end.

The block has an input message port named \"in\" at which new frames can arrive. The message is trunkated then queued in the output message port named \"out\".

Return a shared_ptr to a new instance of ccsds_trunk_tail.

Create an instance of ccsds_trunk_tail and return it's shared_ptr.

Params: (trunk_len, frame_len)"

%feature("docstring") ccsds_viterbi27_bb "A rate 1/2, k=7 convolutional frame decoder for the CCSDS standard

This block performs hard-decision convolutional decoding using the Viterbi algorithm. The fec library by Phil Karn is used.

The input is a stream of unpacked bits (hard channel symbols) that represent the encoded frames only, i.e. the ASMs have been removed from the stream, and are synchronised on the first bit of the frame.

There is no stream output, instead the MSB-first packed bytes of the decoded frame are inserted into a dedicated output queue.

This block is designed for framed data, not continuous data streaming."

%feature("docstring") ccsds_viterbi27_bb::ccsds_viterbi27_bb "

Params: (output_queue, framesyms, sync_marker, asm_length)"

%feature("docstring") ccsds_viterbi27_bb::forecast "

Params: (, ninput_items_required)"

%feature("docstring") ccsds_viterbi27_bb::~ccsds_viterbi27_bb "

Params: (NONE)"

%feature("docstring") ccsds_viterbi27_bb::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_make_viterbi27_bb "A rate 1/2, k=7 convolutional frame decoder for the CCSDS standard

This block performs hard-decision convolutional decoding using the Viterbi algorithm. The fec library by Phil Karn is used.

The input is a stream of unpacked bits (hard channel symbols) that represent the encoded frames only, i.e. the ASMs have been removed from the stream, and are synchronised on the first bit of the frame.

There is no stream output, instead the MSB-first packed bytes of the decoded frame are inserted into a dedicated output queue.

This block is designed for framed data, not continuous data streaming.

Params: (output_queue, framesyms, sync_marker, asm_length)"

%feature("docstring") ccsds_update_crc16 "update running CRC-16

Update a running CRC with the bytes buf[0..len-1] The CRC should be initialized to all 1's, and the transmitted value is the 1's complement of the final running CRC. The resulting CRC should be transmitted in big endian order.

Params: (crc, buf, len)"

%feature("docstring") ccsds_crc16 "

Params: (buf, len)"

%feature("docstring") ccsds_interl_indx "

Params: (index, interleaving_depth, num_elements)"

%feature("docstring") ccsds_make_lpf "Create a linear first order low pass filter and return it's shared pointer.

Based on the loop bandwidth the filter stepsize alpha is calculated with: [Mengali & D'Andrea p. 126 (3.5.67)] 

The k-th filtered state x from the inputs y is calculated as

Params: (loop_bw)"

%feature("docstring") ccsds_make_lpf2 "Create a linear second order low pass filter and return it's shared pointer.

The loop stepsizes gamma and rho are given by the following formulas: [Mengali & D'Andrea p. 220 ff.] 

The stepsize rho can not be computed in explicit form and is computed with Newton's method by solving the root of the following polynomial:  with 

Given rho, gamma is computed in closed form as

Params: (loop_bw, damping_factor_squared, s_slope)"

%feature("docstring") ccsds_time_diff_nano "Returns the difference between the two time instances in nanoseconds.

Time a needs to be later than b.

Params: (a, b)"

%feature("docstring") ccsds_time_diff_seconds "

Params: (a, b)"

%feature("docstring") ccsds_time_add_usecs "

Params: (t, diff)"

%feature("docstring") ccsds_time_diff_to_timespec "

Params: (spec, diff)"

%feature("docstring") ccsds_time_to_nano "

Params: (spec)"

%feature("docstring") ccsds_time_to_micro "

Params: (t)"

%feature("docstring") ccsds_time_print_timeval "

Params: (t)"

%feature("docstring") ccsds_time_print_timespec "

Params: (t)"

%feature("docstring") ccsds_time_reset_timespec "

Params: (spec)"

%feature("docstring") hex2bin "Converts hexadecimal value into decimal one.

Params: (hex)"

%feature("docstring") ccsds_hexstring_to_binary "Converts a hexadecimal string to an array of unsigned chars (bytes).

Params: (str, bin)"

%feature("docstring") togray "Converts binary number into corresponding gray coded number.

Code from

Params: (binary)"

%feature("docstring") tobinary "Converts gray coded number into corresponding binary number.

Code from

Params: (num)"

%feature("docstring") ccsds_make_psk_const "Create general M-PSK constellation.

Gray coded M-PSK constellation starting with zero symbol on the real axis and rotating counterclockwise afterwards.

Params: (M)"

%feature("docstring") ccsds_make_qpsk_const "Create QPSK constellation according to ECSS-E-ST-50-05C.

In contrast to the general M-PSK constellation above, ECSS QPSK mapping is in reverse direction and rotated by 45 degree, so this constellation is created manually.

According to ECSS-E-ST-50-05C Rev.2 from 4th October 2011, Figure 6-3

Params: ()"

%feature("docstring") ccsds_make_mpsk_const "Convenience wrapper to generate a ECSS compatible M-PSK constellation.

Create a M-PSK constellation. For M=4 i.e. QPSK ccsds_make_qpsk_const, otherwise ccsds_make_psk_const is called

Params: (M)"

%feature("docstring") ccsds_make_psk_map "Create general M-PSK mapping.

M-PSK constellation mapping with binary coded index and gray coded output.

Params: (M)"

%feature("docstring") ccsds_make_qpsk_map "Create QPSK constellation mapping according to ECSS-E-ST-50-05C.

In contrast to the general M-PSK mapping above, ECSS QPSK mapping is in reverse direction and rotated by 45 degree, so this constellation is created manually.

According to ECSS-E-ST-50-05C Rev.2 from 4th October 2011, Figure 6-3

Params: ()"

%feature("docstring") ccsds_make_mpsk_map "Convenience wrapper to generate a ECSS compatible M-PSK constellation mapping.

Create a M-PSK constellation mapping. For M=4 i.e. QPSK ccsds_make_qpsk_map, otherwise ccsds_make_psk_map is called

Params: (M)"

%feature("docstring") ccsds_make_psk_rev_map "Create general reverse M-PSK mapping.

Reverse M-PSK constellation mapping with gray coded index and binary coded output.

Params: (M)"

%feature("docstring") ccsds_make_qpsk_rev_map "Create reverse QPSK constellation mapping according to ECSS-E-ST-50-05C.

In contrast to the general reverse M-PSK mapping above, ECSS QPSK mapping is in reverse direction and rotated by 45 degree, so this mapping is created manually.

According to ECSS-E-ST-50-05C Rev.2 from 4th October 2011, Figure 6-3

Params: ()"

%feature("docstring") ccsds_make_mpsk_rev_map "Convenience wrapper to generate a ECSS compatible reverse M-PSK constellation mapping.

Create a reverse M-PSK constellation mapping. For M=4 i.e. QPSK ccsds_make_qpsk_rev_map, otherwise ccsds_make_psk_rev_map is called

Params: (M)"

%feature("docstring") gr::ccsds::add_asm "Takes a frame and adds a sync marker in front of it.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the ASM sequence followed by the data of the incomming message. This new message is then queued in the output message port named \"out\"."

%feature("docstring") gr::ccsds::add_asm::make "

Params: (ASM, frame_len)"



%feature("docstring") gr::ccsds::add_asm_impl::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") gr::ccsds::add_asm_impl::add_asm_impl "

Params: (ASM, frame_len)"

%feature("docstring") gr::ccsds::add_asm_impl::~add_asm_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::add_asm_impl::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::blob_msg_sink_b "Reads frames from message input port \"in\" and outputs it to a TicpClient."

%feature("docstring") gr::ccsds::blob_msg_sink_b::make "

Params: (blob_len)"



%feature("docstring") gr::ccsds::blob_msg_sink_b_impl::blob_msg_sink_b_impl "

Params: (blob_len)"

%feature("docstring") gr::ccsds::blob_msg_sink_b_impl::~blob_msg_sink_b_impl "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") gr::ccsds::blob_msg_sink_b_impl::stop "

Params: ()"

%feature("docstring") gr::ccsds::blob_msg_sink_b_impl::work "

Params: (noutput_items, input_items, output_items)"



%feature("docstring") gr::ccsds::blob_msg_source_b::make "

Params: (blob_len)"



%feature("docstring") gr::ccsds::blob_msg_source_b_impl::process_message "Store incomming message into buffer.

Params: (msg_in)"

%feature("docstring") gr::ccsds::blob_msg_source_b_impl::blob_msg_source_b_impl "Public constructor of the TICP frame sink.

Params: (blob_len)"

%feature("docstring") gr::ccsds::blob_msg_source_b_impl::~blob_msg_source_b_impl "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") gr::ccsds::blob_msg_source_b_impl::start "

Params: ()"

%feature("docstring") gr::ccsds::blob_msg_source_b_impl::stop "

Params: ()"

%feature("docstring") gr::ccsds::blob_msg_source_b_impl::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") ccsds_asm_operator "Outputs debug messages to stdout if defined.

Helper class for unpacked bit wise ASM search functions"

%feature("docstring") ccsds_asm_operator::ccsds_asm_operator "Constructor of ASM operator.

Params: (ASM, ASM_LEN, BER_THRESHOLD, INFO_BITS_PER_SYMBOL)"

%feature("docstring") ccsds_asm_operator::~ccsds_asm_operator "Deconstructor of ASM operator.

Params: (NONE)"

%feature("docstring") ccsds_asm_operator::get_asm_sym_len "

Params: ()"

%feature("docstring") ccsds_asm_operator::check_for_asm_hamming "Checks if stream matches an ASM at given bit offset and return the number of bit errors found.

Params: (symbols, offset_bits)"

%feature("docstring") ccsds_asm_operator::check_for_asm "Checks if stream matches an ASM at given bit offset.

The stream is considered to match if  or less bits differ.

Params: (symbols, offset_bits)"

%feature("docstring") ccsds_asm_operator::search_asm "Searches for ASM in the given stream.

The match with the minimum Hamming distance is searched. If this minimum Hamming distance is less or equal the  a match is found, otherwise no match is found and  and  are set to zero.

In case there are two matches with the same Hamming distance the one with the lower byte offset or lower bit offset (in this priority) is chosen.

Params: (stream, stream_len, offset_bytes, offset_bits)"

%feature("docstring") ccsds_asm_operator::copy_stream "Copy bitstream from a byte array with a bit offset into an aligned byte array.

The first  starting with the MSB from the first element of  are skipped, afterwards all bits from  are copied to  until  bytes have been copied to .

Params: (stream_out, stream_in, len, offset_bits)"

%feature("docstring") ccsds_conv_encode "Helper class for convolutional encoding."

%feature("docstring") ccsds_conv_encode::popCount "

Params: (x)"

%feature("docstring") ccsds_conv_encode::ccsds_conv_encode "

Params: (seed, reg_len)"

%feature("docstring") ccsds_conv_encode::encode "

Params: (syms, data, len, POLYA, POLYB)"



%feature("docstring") ccsds_conv_encode27_punct_aux::encode "Encode input byte without puncturing. The output are 2 bytes.

Params: (output_bits, input_byte)"

%feature("docstring") ccsds_conv_encode27_punct_aux::puncture "Perfrom puncturing and report how many bits of the output contain valid information bits.

Params: (output_bits, num_out_bits, input_bits)"

%feature("docstring") ccsds_conv_encode27_punct_aux::ccsds_conv_encode27_punct_aux "Constructor for the encoder helper class.

The generator polynomial is a condensed information about how to combine the shift register outputs and whether or not to invert the output stream.

If the MSB is set the output stream will be inverted.

The lower 7 bits encode the actual polynomials. The generator polynomial is masked with 0x7F (set MSB to zero) and the copied into the  variables.

Params: (gen_poly_c1, gen_poly_c2, puncturing)"

%feature("docstring") ccsds_conv_encode27_punct_aux::~ccsds_conv_encode27_punct_aux "Destructor.

Params: (NONE)"

%feature("docstring") ccsds_conv_encode27_punct_aux::encode_punct "Encode information byte including puncturing.

Params: (output_bits, num_out_bits, input_byte)"

%feature("docstring") gr::ccsds::conv_decode27 "Perform unpuncturing and convloutional decoding on the input stream.

This block takes an asynchronous message of punctured convolutional encoded softbits. The input softbits are frame synchronized, which means that the leading and following ASM sequences have been stripped off and only contains the encoded symbols from the actual frame data.

It performs unpuncturing and viterbi decoding of the message and outputs the decoded data stream as a BLOB message of packed bytes.

Since before and after each message a known ASM is send, the knowledge of this ASM is used to determine the start and and state of the viterbi decoder as well as the tail sequence of the block, which is the start of the new ASM sequence that still depends on the data bits of the current block due to the code's constraint length."

%feature("docstring") gr::ccsds::conv_decode27::make "

Params: (gen_poly_c1, gen_poly_c2, puncturing_type, block_len, ASM)"



%feature("docstring") gr::ccsds::conv_encode27_aux::encode "Encode input byte without puncturing. The output are 2 bytes.

Params: (output_bits, input_byte)"

%feature("docstring") gr::ccsds::conv_encode27_aux::puncture "Perfrom puncturing and report how many bits of the output contain valid information bits.

Params: (output_bits, num_out_bits, input_bits)"

%feature("docstring") gr::ccsds::conv_encode27_aux::conv_encode27_aux "Constructor for the encoder helper class.

The generator polynomial is a condensed information about how to combine the shift register outputs and whether or not to invert the output stream.

If the MSB is set the output stream will be inverted.

The lower 7 bits encode the actual polynomials. The generator polynomial is masked with 0x7F (set MSB to zero) and the copied into the  variables.

Params: (gen_poly_c1, gen_poly_c2, puncturing)"

%feature("docstring") gr::ccsds::conv_encode27_aux::~conv_encode27_aux "Destructor.

Params: (NONE)"

%feature("docstring") gr::ccsds::conv_encode27_aux::encode_punct "Encode information byte including puncturing.

Params: (output_bits, num_out_bits, input_byte)"

%feature("docstring") gr::ccsds::conv_encode27_bb "Perform convloutional encoding and puncturing on the input stream."

%feature("docstring") gr::ccsds::conv_encode27_bb::make "

Params: (gen_poly_c1, gen_poly_c2, puncturing_type)"



%feature("docstring") gr::ccsds::conv_encode27_bb_impl::get_rate "Return relative rate in/out for the given puncturing.

Params: (puncturing_type)"

%feature("docstring") gr::ccsds::conv_encode27_bb_impl::conv_encode27_bb_impl "Public constructor for the convolutional encoder block.

The compact generator polynom contains two informations. Encoded in the MSB is the information whether the stream should be inverted (MSB set) or not (MSB unset). Encoded in the 7 less significant bits is the information about the connections between the shift registers and the output. The MSB-1 represents the influence of the current information bit i(t) while the LSB represents the influence of the information bit i(t-6). A set bit means there is a connection, while an unset bit means there is no.

The polynomials for the convolutional code defined in ECSS can be generated with 0x79 for c1 and 0x5B for c2. Together with the MSB for the inversion of c2 the compact polynomials are 0x79 and 0xDB.

Params: (gen_poly_c1, gen_poly_c2, puncturing_type)"

%feature("docstring") gr::ccsds::conv_encode27_bb_impl::~conv_encode27_bb_impl "Public deconstructor.

Params: (NONE)"

%feature("docstring") gr::ccsds::conv_encode27_bb_impl::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr::ccsds::conv_encode27_bb_impl::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::discriminator "<+description of block+>"

%feature("docstring") gr::ccsds::discriminator::make "Return a shared_ptr to a new instance of ccsds::discriminator.

To avoid accidental use of raw pointers, ccsds::discriminator's constructor is in a private implementation class. ccsds::discriminator::make is the public interface for creating new instances.

Params: (status, frame)"



%feature("docstring") gr::ccsds::discriminator_impl::discriminator_impl "

Params: (status, frame)"

%feature("docstring") gr::ccsds::discriminator_impl::~discriminator_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::discriminator_impl::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::dll_cc "Delay locked loop decimating from sample- to symbolrate

Delay estimation based on the Gardner passband timing error detector. The synchronous samples are obtained by cubic interpolation."

%feature("docstring") gr::ccsds::dll_cc::make "

Params: (osf, gamma)"



%feature("docstring") gr::ccsds::dll_cc_impl::get_frac "Return fractional part of the value.

Params: (value)"

%feature("docstring") gr::ccsds::dll_cc_impl::get_int "Return integral part of the value.

Params: (value)"

%feature("docstring") gr::ccsds::dll_cc_impl::to_real "Calculate real components from complex array.

Params: (out, in, num)"

%feature("docstring") gr::ccsds::dll_cc_impl::to_imag "Calculate imaginary components from complex array.

Params: (out, in, num)"

%feature("docstring") gr::ccsds::dll_cc_impl::to_real4 "Calculate real components from complex 4-element array.

Params: (out, in)"

%feature("docstring") gr::ccsds::dll_cc_impl::to_imag4 "Calculate imaginary components from complex 4-element array.

Params: (out, in)"

%feature("docstring") gr::ccsds::dll_cc_impl::interpolate_value "Calculate cubic interpolant.

Calculate cubic interpolant between the second and thrid data point based on the fractional index. If mu is 0 the interpolant is equal to the second data point, if mu is 0.5 the interpolation point is exactly in the middle between the second and the third data point.

Params: (y, mu)"

%feature("docstring") gr::ccsds::dll_cc_impl::interpolate_cvalue "Calculate cubic interpolant if a complex number.

Calls interpolate_value for the real and imaginary component and reassambles interpolants back to a complex sample.

Params: (y, mu)"

%feature("docstring") gr::ccsds::dll_cc_impl::gardner "Calculate a timing error from the last three interpolants.

Gardner passband timing error estimation based on the last two interpolated symbols and an intermediate sample from between.

e[k] = re{ [ y(kT-T+tau[k-1]) - y(kT+tau[k]) ] y*(kT-T/2+tau[k]) }

Params: (previous, intermediate, current)"

%feature("docstring") gr::ccsds::dll_cc_impl::propagate_tags "Propagates stream tags of the current block to the decimated output stream.

Params: (num_in)"

%feature("docstring") gr::ccsds::dll_cc_impl::dll_cc_impl "Public constructor of the DLL.

Params: (osf, gamma)"

%feature("docstring") gr::ccsds::dll_cc_impl::~dll_cc_impl "Public deconstructor of the DLL.

Params: (NONE)"

%feature("docstring") gr::ccsds::dll_cc_impl::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr::ccsds::dll_cc_impl::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") lpf "Linear first order low pass filter."

%feature("docstring") lpf::lpf "Private constructor of lpf.

Constructs a linear first order low pass filter.

Params: (alpha)"

%feature("docstring") lpf::filter_step "Calculate next state based on single input.

Next state is calculated by

Params: (in)"

%feature("docstring") lpf::~lpf "Public deconstructor.

Params: (NONE)"

%feature("docstring") lpf::filter "Filter elements of an array.

Call filter_step() for each element of  and put output in .

Params: (out, in, n)"

%feature("docstring") lpf2 "Linear second order low pass filter."

%feature("docstring") lpf2::lpf2 "Private constructor of the linear second order low pass filter.

Loop stepsizes are derived from the more intuitive parameters loop bandwidth, damping factor and S-curve slope in ccsds_make_lpf2().

Params: (gamma, rho)"

%feature("docstring") lpf2::wrap "Wrap value centered arround zero to given bounds.

Wraps  to lie in the interval (-max,max].

Most prominent usecase is to wrap a phase  to be contained in the interval between -pi and pi. This could be achieved by:

Params: (in, max)"

%feature("docstring") lpf2::filter_step "Update filtered state from a single input.

Params: (in)"

%feature("docstring") lpf2::filter_step_wrapped "Update filtered state from a single input. And keep state variables contained in a fixed interval.

Params: (in, wrap_max)"

%feature("docstring") lpf2::~lpf2 "Public deconstructor.

Params: (NONE)"

%feature("docstring") lpf2::filter "Filter elements of an array.

Call filter_step() for each element of  and put restul in .

Params: (values, n)"

%feature("docstring") lpf2::filter_wrapped "Filter elements of an array and keep state variables contained.

Call filter_step_wrapped() for each element of  and put result back into .

Params: (values, freq_est, wrap_max, n)"

%feature("docstring") lpf2::get_freq_estimate "Return current difference state.

When filtering carrier phase estimates with this filter, the current difference state acts as a frequency estimate.

Params: ()"

%feature("docstring") gr::ccsds::lpf2 "Linear second order low pass filter."

%feature("docstring") gr::ccsds::lpf2::lpf2 "Private constructor of the linear second order low pass filter.

Loop stepsizes are derived from the more intuitive parameters loop bandwidth, damping factor and S-curve slope in ccsds_make_lpf2().

Params: (gamma, rho)"

%feature("docstring") gr::ccsds::lpf2::wrap "Wrap value centered arround zero to given bounds.

Wraps  to lie in the interval (-max,max].

Most prominent usecase is to wrap a phase  to be contained in the interval between -pi and pi. This could be achieved by:

Params: (in, max)"

%feature("docstring") gr::ccsds::lpf2::filter_step "Update filtered state from a single input.

Params: (in)"

%feature("docstring") gr::ccsds::lpf2::filter_step_wrapped "Update filtered state from a single input. And keep state variables contained in a fixed interval.

Params: (in, wrap_max)"

%feature("docstring") gr::ccsds::lpf2::~lpf2 "Public deconstructor.

Params: (NONE)"

%feature("docstring") gr::ccsds::lpf2::filter "Filter elements of an array.

Call filter_step() for each element of  and put restul in .

Params: (values, n)"

%feature("docstring") gr::ccsds::lpf2::filter_wrapped "Filter elements of an array and keep state variables contained.

Call filter_step_wrapped() for each element of  and put result back into .

Params: (values, freq_est, wrap_max, n)"

%feature("docstring") gr::ccsds::lpf2::get_freq_estimate "Return current difference state.

When filtering carrier phase estimates with this filter, the current difference state acts as a frequency estimate.

Params: ()"

%feature("docstring") moving_average "Moving average filter

A dual moving average filter. The filter calculates the moving average of an input sequence. In order to not accumulate numerical computation errors the filter is reset after processing moving_average::D_NUM_RESET samples. A second (inactive) moving average filter starts to filter the input stream in parallel moving_average::D_NUM_COLLECT samples before the first (active) filter is reset.

The filtered output is always taken from the active filter.

When the active filter is reset, the inactive filter has filtered exactly moving_average::D_NUM_COLLECT samples and becomes active while the previously active filter becomes inactive."

%feature("docstring") moving_average::populate_filter "Put array into inactive filter, but do not output filtered states.

When we just have to filter a block of samples, but don't need the individual filtered samples, we can update the state in a block, instead of computing every single state individually.  with [k] beeing the moving average of k samples.

Params: (values, n)"

%feature("docstring") moving_average::apply_filter "Put array int active filter and return filtered state for every input element.

Filter  elements of value and return the  filtered values in the same array.

Params: (values, n)"

%feature("docstring") moving_average::moving_average "Public constructor of the dual moving average filter.

Params: (NONE)"

%feature("docstring") moving_average::~moving_average "Public destructor of the dual moving average filter.

Params: (NONE)"

%feature("docstring") moving_average::filter "Filter a block of values and take care of the filter switching internally.

Filter array and take care of the filter switching and populating internally.

Params: (values, n)"

%feature("docstring") moving_average::getState "Return current state (i.e. the current moving average)

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f "M-PSK soft bit ambiguity resolution and frame synchronization.

The input to this block is a stream of  grouped soft bits. The output of this block is a asynchronous message containing a float vector with the soft bits of a frame. The bytes passed to this block as ASM are cut off, but any skipped bits will be in the output message.

So if you pass the two ASM bytes 0x00 0xAA to this block followed by two bytes frame data (FD1 and FD2), followed by another two bytes ASM and set  to zero the output will be a message containing FD1 and FD2. If  is set to two, the output will be FD1, FD2 followed by the first two zero bits from the ASM.

Using  (setting it to nonzero values)

The block will start in searchmode, looking for every possible bit offset in all possible  ambiguities. Once an ASM is found it will enter locked state where it will only check for the ASM at the expected position and ambiguity. If the ASM is found a counter is increased up to . If an exprected ASM is not found the counter is decreased. If the counter reaches zero the block goes into search mode again."

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f::make "

Params: (M, ASM, asm_len, threshold, correlation_threshold, frame_length, num_tail_syms)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f::get_correlation "

Params: ()"



%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::from_packed_to_unpacked "Returns the number of unpacked symbols (as float) that relate to the given number of packed bytes.

Params: (packed)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::confine_offsets "Ensures that  is confined within [0,d_ldM). If not  is increased accordingly.

Params: ()"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::convert_ambiguity "Change ambiguity of incomming symbol stream.

Params: (out, in, num_bits, ambiguity)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::check_for_ASM "Return the correlation between the input sequence and the ASM.

Params: (in, offset)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::search_for_ASM "Search for ASM in input sequence and return maximum found correlation.

The search checks all possible offsets in the range of [0, num_in-d_ASM_LEN_BITS] starting from zero and abborts if a correlation of 1.0f is reached, or all offsets have been tried.

Params: (in, num_in, offset)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::convert_to_char "Converts an array of  softbits into a byte of  hardbits.

Params: (in)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::get_lower_mul "Get the highest multiple of  lower or equal than .

Params: (n)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::get_upper_mul "Get the lowest multiple of  higher or equal than .

Params: (n)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::updateTags "Scan all stream tags in scope and copy them into .

Scan for all tags after max(, ) and before .

Params: (until)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::extractTags "Reads all tags from the buffer and creates a frame metadata header out of it.

Calculates the reception time of the sample with the absolute count  based on the tag rx_time. Otherwise ignore the rx_time tag and label the calculated reception time rx_time in the created metadata.

Copies all other tags into the header.

Params: (from, len)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::mpsk_ambiguity_resolver_f_impl "Public constructor of the AR.

Constructs a AR block that searches for the ASM in the  input streams and outputs the one that contains the ASM. If no ASM is found, no output is given. This block preserves the ASM, as it is still needed for frame synchronization after decoding.

Params: (M, ASM, asm_len, threshold, correlation_threshold, frame_length, num_tail_syms)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::~mpsk_ambiguity_resolver_f_impl "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::stop "

Params: ()"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::get_correlation "

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::setup_rpc "

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_ambiguity_resolver_f_impl::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::mpsk_detector_soft_cf "M-PSK soft bit detector.

M-PSK soft bit detector.

The incomming phase is converted to a conteallation point (counting counterclockwise starting from zero at symbol 1+0j) with the non linear function:



And then the constellation point is mapped to the actual symbol bit sequence by a binary to gray code mapping []:



For each incomming symbol  output symbols are generated containing the detected soft bit as float values, where -1.0f represents the most confident 0 bit, 0.0f is an erasure (in this case, the received symbol has no energy at all) and 1.0f represents the most confident 1 bit."

%feature("docstring") gr::ccsds::mpsk_detector_soft_cf::make "

Params: (M)"



%feature("docstring") gr::ccsds::mpsk_detector_soft_cf_impl::get_ldM "Calculated the logariothmus dualis of M (log of base two)

Params: (M)"

%feature("docstring") gr::ccsds::mpsk_detector_soft_cf_impl::mpsk_detector_soft_cf_impl "Public constructor of the M-PSK soft bit detector.

Detects data bits from M-PSK modulated symbols. This block detects the data bits by a non linear transformation of the sample's phase.

Params: (M)"

%feature("docstring") gr::ccsds::mpsk_detector_soft_cf_impl::~mpsk_detector_soft_cf_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_detector_soft_cf_impl::work "

Params: (noutput_items, input_items, output_items)"



%feature("docstring") gr::ccsds::mpsk_mod_bc::make "

Params: (M)"



%feature("docstring") gr::ccsds::mpsk_mod_bc_impl::mpsk_mod_bc_impl "Public constructor for the M-PSK modulator.

Params: (M)"

%feature("docstring") gr::ccsds::mpsk_mod_bc_impl::~mpsk_mod_bc_impl "Public deconstructor.

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_mod_bc_impl::work "

Params: (noutput_items, input_items, output_items)"



%feature("docstring") gr::ccsds::mpsk_preamble_cc::make "

Params: (num_symbols)"



%feature("docstring") gr::ccsds::mpsk_preamble_cc_impl::mpsk_preamble_cc_impl "Public constructor for the M-PSK preamble block.

Params: (num_symbols)"

%feature("docstring") gr::ccsds::mpsk_preamble_cc_impl::~mpsk_preamble_cc_impl "Public deconstructor.

Params: (NONE)"

%feature("docstring") gr::ccsds::mpsk_preamble_cc_impl::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr::ccsds::mpsk_preamble_cc_impl::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::msg_null_src "Defines a message out port that is never used. This block can be used to connect asynchronous blocks to the flowgraph and then pass in messages to them directly from the python code. This block should only be used in QA code."

%feature("docstring") gr::ccsds::msg_null_src::make "

Params: (NONE)"



%feature("docstring") gr::ccsds::msg_null_src_impl::msg_null_src_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::msg_null_src_impl::~msg_null_src_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::pll_cc "Phase locked loop that sends frequency correction messages.

Phase locked loop using the Viterbi&Viterbi method to estimate the sample's phase offset. Estimates are filtered with a linear second order low pass filter."

%feature("docstring") gr::ccsds::pll_cc::make "

Params: (m, loop_bandwidth)"



%feature("docstring") gr::ccsds::pll_cc_impl::remove_modulation "Remove phase modulation by taking the M-th power of the samples.

Remove modulation by following the M-power principle. The ccsds_pll_cc::d_M th power of the input samples is calculated and written to the complex array .

Params: (tmp_c, in, num)"

%feature("docstring") gr::ccsds::pll_cc_impl::calc_phases "Calculate the phases of an array of complex numbers.

Params: (tmp_f, tmp_c, num)"

%feature("docstring") gr::ccsds::pll_cc_impl::calc_rotation "Rotates complex array according to phase estimates.

Rotates samplewise by estimated phase.

Params: (out, in, tmp_f, num)"

%feature("docstring") gr::ccsds::pll_cc_impl::pll_cc_impl "Public constructor.

Params: (m, loop_bandwidth)"

%feature("docstring") gr::ccsds::pll_cc_impl::~pll_cc_impl "Public deconstructor.

Params: (NONE)"

%feature("docstring") gr::ccsds::pll_cc_impl::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::randomiser "Combines the input frame with a pseudo random sequence by XOR operation. The pseudo random sequence is generated by an 8-bit linear feedback shift register as defined in ECSS.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the input message XORed with the pseudo random sequence. The LFSR is set to the initial  state before every block.This new message is then queued in the output message port named \"out\"."

%feature("docstring") gr::ccsds::randomiser::make "

Params: (polynomial, seed)"



%feature("docstring") gr::ccsds::randomiser_impl::reset "Reset LFSR to  state

Params: ()"

%feature("docstring") gr::ccsds::randomiser_impl::get_next_byte "Perform 8 updates on the LFSR and return it's output

Params: ()"

%feature("docstring") gr::ccsds::randomiser_impl::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") gr::ccsds::randomiser_impl::randomiser_impl "Public deconstructor.

Params: (polynomial, seed)"

%feature("docstring") gr::ccsds::randomiser_impl::~randomiser_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::randomiser_impl::work "Dummy work function.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::reversebits "reverses bit order in selected bytes of the input message.

The block has an input message port named \"in\" at which new frames can arrive. On arrival a new message is created consisting of the input message but were selected bytes have a reverse order. So if the byte started with MSB it will now start with the LSB. This new message is then queued in the output message port named \"out\"."

%feature("docstring") gr::ccsds::reversebits::make "

Params: (msg_length, reverse_indices)"



%feature("docstring") gr::ccsds::reversebits_impl::reverse_bits "

Params: (in)"

%feature("docstring") gr::ccsds::reversebits_impl::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") gr::ccsds::reversebits_impl::reversebits_impl "Public deconstructor.

Params: (msg_length, reverse_indices)"

%feature("docstring") gr::ccsds::reversebits_impl::~reversebits_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::reversebits_impl::work "Dummy work function.

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::rs_decode "Takes the transmitted codeblock and generates the frame out of it, or drops it, if recovery is not possible.

The block has an input message port named \"in\" at which codeblocks of length  bytes are received. On reception the data codeblock is interleaved and decoded. The generated frame consisting of  bytes is then queued in the output message port named \"out\" or dropped if it can't be decoded."

%feature("docstring") gr::ccsds::rs_decode::make "Create RS decoder.

Params: (I, representation)"



%feature("docstring") gr::ccsds::rs_decode_impl::rs_decode_impl "Public constructor of the RS_Decode block.

Params: (I, representation)"

%feature("docstring") gr::ccsds::rs_decode_impl::~rs_decode_impl "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") gr::ccsds::rs_decode_impl::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") gr::ccsds::rs_encode "Takes I frames and generates the transmitted codeblock out of it.

The block has an input message port named \"in\" at which frames of length  bytes are received. On reception the frames are interleaved and encoded. The generated codeblock consisting of  bytes is then queued in the output message port named \"out\"."

%feature("docstring") gr::ccsds::rs_encode::make "

Params: (I)"



%feature("docstring") gr::ccsds::rs_encode_impl::rs_encode_impl "Public constructor of the RS Encode block.

Params: (I)"

%feature("docstring") gr::ccsds::rs_encode_impl::~rs_encode_impl "Public deconstructor of the RS Encode block.

Params: (NONE)"

%feature("docstring") gr::ccsds::rs_encode_impl::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf "<+description of block+>"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf::make "Return a shared_ptr to a new instance of ccsds::simple_bpsk_SNR_qf.

To avoid accidental use of raw pointers, ccsds::simple_bpsk_SNR_qf's constructor is in a private implementation class. ccsds::simple_bpsk_SNR_qf::make is the public interface for creating new instances.

Params: (window_size)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf::SNR_real "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf::SNR_imag "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf::SNR_magn "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf::window_size "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf::set_window_size "

Params: ()"



%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::variance "

Params: (variance, inputBuffer, num_points)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::simple_bpsk_SNR_qf_impl "

Params: (window_size)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::~simple_bpsk_SNR_qf_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::SNR_real "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::SNR_imag "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::SNR_magn "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::window_size "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::set_window_size "

Params: (window_size)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::setup_rpc "

Params: (NONE)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::forecast "

Params: (noutput_items, ninput_items_required)"

%feature("docstring") gr::ccsds::simple_bpsk_SNR_qf_impl::general_work "

Params: (noutput_items, ninput_items, input_items, output_items)"



%feature("docstring") gr::ccsds::softbits_msg_to_bytes_b::make "

Params: (NONE)"



%feature("docstring") gr::ccsds::softbits_msg_to_bytes_b_impl::softbits_msg_to_bytes_b_impl "Store incomming message into buffer.

Public constructor of the TICP frame sink

Params: (NONE)"

%feature("docstring") gr::ccsds::softbits_msg_to_bytes_b_impl::~softbits_msg_to_bytes_b_impl "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") gr::ccsds::softbits_msg_to_bytes_b_impl::process_message "

Params: (msg_in)"

%feature("docstring") gr::ccsds::softbits_msg_to_bytes_b_impl::work "

Params: (noutput_items, input_items, output_items)"

%feature("docstring") gr::ccsds::softbittobit "<+description of block+>"

%feature("docstring") gr::ccsds::softbittobit::make "Return a shared_ptr to a new instance of ccsds::softbittobit.

To avoid accidental use of raw pointers, ccsds::softbittobit's constructor is in a private implementation class. ccsds::softbittobit::make is the public interface for creating new instances.

Params: (NONE)"



%feature("docstring") gr::ccsds::softbittobit_impl::softbittobit_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::softbittobit_impl::~softbittobit_impl "

Params: (NONE)"

%feature("docstring") gr::ccsds::softbittobit_impl::work "

Params: (noutput_items, input_items, output_items)"



%feature("docstring") gr::ccsds::ticp_frame_sink::make "

Params: (port, frame_length, data_type, map_names, map_types)"



%feature("docstring") gr::ccsds::ticp_frame_sink_impl::getFrame "Pops a frame from the queue and send it to the client. Blocks until there is a frame in the queue.

Params: ()"

%feature("docstring") gr::ccsds::ticp_frame_sink_impl::ticp_frame_sink_impl "Public constructor of the TICP frame sink.

Params: (port, frame_length, data_type, map_names, map_types)"

%feature("docstring") gr::ccsds::ticp_frame_sink_impl::~ticp_frame_sink_impl "Public deconstructor of the ticp frame sink.

Params: (NONE)"

%feature("docstring") gr::ccsds::ticp_frame_sink_impl::process_message "Store incomming message into buffer.

Params: (msg_in)"

%feature("docstring") gr::ccsds::ticp_frame_source "Reads frames from a TicpServer and outputs it as a stream."

%feature("docstring") gr::ccsds::ticp_frame_source::make "

Params: (hostname, port, frame_length, data_type, map_names, map_types, frame_rate)"



%feature("docstring") gr::ccsds::ticp_frame_source_impl::ticp_frame_source_impl "Public constructor of the TICP frame source.

Params: (hostname, port, frame_length, data_type, map_names, map_types, frame_rate)"

%feature("docstring") gr::ccsds::ticp_frame_source_impl::~ticp_frame_source_impl "Public deconstructor of the ticp frame source.

Params: (NONE)"

%feature("docstring") gr::ccsds::ticp_frame_source_impl::asynchronous_work "Asynchronous work function which is processing the queues.

Params: ()"

%feature("docstring") gr::ccsds::trunk_tail "Takes a frame and trunkates a number of bits from the end.

The block has an input message port named \"in\" at which new frames can arrive. The message is trunkated then queued in the output message port named \"out\"."

%feature("docstring") gr::ccsds::trunk_tail::make "

Params: (trunc_len, frame_len, blanc_bits)"



%feature("docstring") gr::ccsds::trunk_tail_impl::trunk_tail_impl "Public constructor of the Trunk Tail block.

Params: (trunk_len, frame_len, blanc_bits)"

%feature("docstring") gr::ccsds::trunk_tail_impl::~trunk_tail_impl "Public deconstructor of the AR.

Params: (NONE)"

%feature("docstring") gr::ccsds::trunk_tail_impl::process_frame "Asynchronous work function which is processing the queues.

Params: (msg)"

%feature("docstring") gr::ccsds::trunk_tail_impl::work "

Params: (noutput_items, input_items, output_items)"