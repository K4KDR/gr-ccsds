/* -*- c++ -*- */
/* 
 * Copyright 2020 Martin Luelf.
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

#include "frame_sync_impl.h"
#include "hexstring_to_binary.h"
#include "ccsds_utils.h"

#include <ccsds/exceptions.h>

#include <gnuradio/io_signature.h>

#include <limits>
#include <exception>

namespace gr {
  namespace ccsds {

    frame_sync::sptr
    frame_sync::make(boost::shared_ptr<frame_sync_config> sync_conf,
                     boost::shared_ptr<frame_generation_config> gen_conf,
                     asm_position_list::sptr additional_asm
    ) {
      return gnuradio::get_initial_sptr(new frame_sync_impl(sync_conf, gen_conf, additional_asm));
    }

    /*
     * The private constructor
     */
    frame_sync_impl::frame_sync_impl(boost::shared_ptr<frame_sync_config> sync_conf,
                                     boost::shared_ptr<frame_generation_config> gen_conf,
                                     asm_position_list::sptr additional_asm
                                    )
      : gr::sync_block("frame_sync",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
      d_FRAME_SYNC_CONFIG(sync_conf),
      d_FRAME_GEN_CONFIG(gen_conf),
      d_NUM_BITS_PER_SYMBOL(sync_conf->constellation()->bits_per_symbol()),
      d_BLOCK_LEN_SYMBOLS(sync_conf->block_num_symbols()),
      d_PAYLOAD_LEN_SYMBOLS(sync_conf->payload_num_symbols()),
      d_ASM_LEN_SYMBOLS(sync_conf->asm_num_symbols()),
      d_TOTAL_SYMBOLS_TO_COPY(sync_conf->output_num_symbols()),
      d_PORT_OUT(pmt::intern("out")),
      d_SOFT_DECISION_PRECISION(3),
      d_CONSTELLATION(sync_conf->constellation()),
      d_buffer(d_BLOCK_LEN_SYMBOLS, sync_conf->asm_symbols_conj(), gen_conf->frames_before(), static_cast<size_t>((sync_conf->head_num_symbols() > 0) ? sync_conf->head_num_symbols() : 0)),
      d_sequence_number(0u)
    {
      // register output port
    	message_port_register_out(d_PORT_OUT);

      for (asm_position pos : additional_asm->elements()) {
        queueFrameOutput(pos, false);
      }
    }

    int frame_sync_impl::work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items __attribute__((unused))) {
      assert(input_items.size() == 1);
      assert(output_items.size() == 0);
      assert(noutput_items >= 0);
      
      const size_t num_out = static_cast<size_t>(noutput_items);
      const gr_complex *in = (const gr_complex *) input_items[0];
      
      if (nitems_read(0) == 0lu) {
          // Very first run
          std::vector<gr::tag_t> tags;
          get_tags_in_range(tags, 0lu, 0lu, 1lu);
          
          d_extra_tags = pmt::make_dict();
          
          for ( const auto& tag : tags) {
              d_extra_tags = dict_add (d_extra_tags, tag.key, tag.value);
          }
      }
      
      size_t processed = 0u;
      while(processed < num_out) {
        const size_t num_copy = utils::pick_smaller(utils::minus_cap_zero<size_t>(num_out,processed), d_buffer.num_symbols_missing());
        
        d_snr_est.update(num_copy, &in[processed]);
        const snr snr_end = d_snr_est.get_snr();
        assert(snr_end.valid());
        
        const size_t num_buffered = d_buffer.read_signal(&in[processed], num_copy, snr_end);
        assert(num_copy == num_buffered);

        processed += num_buffered;
        
        if (d_buffer.full()) {
          processBuffer(d_BLOCK_LEN_SYMBOLS);

          // Copy data from buffer into queued frames (before advancing)
          processFrameQueue();

          d_buffer.advance();
        } else {
          //printf("buffer still missing %lu symbols\n", d_buffer.buffer_size()-d_buffer.buffer_items_now());
        }
      }

      // Copy data from buffer into queued frames
      processFrameQueue();

      // Tell runtime system how many output items we produced.
      return static_cast<int>(processed);
    }

    void frame_sync_impl::processBuffer(size_t last_indx) {
      assert(last_indx <= d_buffer.num_symbols_in_window());
      
      asm_position frame = findMaximumInBuffer(last_indx);
      frame.set_sequence(++d_sequence_number);

      queueFrameOutput(frame, true);

      // Generate secondary frames before this ASM
      for (size_t i=1u; i<=d_FRAME_GEN_CONFIG->frames_before(); i++) {
        if (frame.offset() < i*d_BLOCK_LEN_SYMBOLS) {
          // ASM would start before the buffer, skip
          continue;
        }

        queueFrameOutput(asm_position(frame.offset()-i*d_BLOCK_LEN_SYMBOLS, frame.ambiguity(), frame.sequence()), false);
      }

      //*
      // Generate secondary frames after this ASM
      for (size_t i=1u; i<=d_FRAME_GEN_CONFIG->frames_after(); i++) {
        queueFrameOutput(asm_position(frame.offset()+i*d_BLOCK_LEN_SYMBOLS, frame.ambiguity(), frame.sequence()), false);
      }
      //*/
      
    }

    bool frame_sync_impl::stop(void) {
      // if there is are least one unprocessed sample in the buffer, fill the buffer with overlap zeros so the (partial) correlation of the input in the buffer can be computed
      if (!d_buffer.buffer_current_empty()) {
        
        const size_t last_valid_index = utils::pick_smaller(d_buffer.num_symbols_in_window(), d_BLOCK_LEN_SYMBOLS);
        std::vector<gr_complex> fill(d_buffer.overlap());
        d_buffer.read_signal(fill.data(), fill.size(), d_snr_est.get_snr());

        // Search for remaining ASMs in the buffer
        processBuffer(last_valid_index);
      }

      
      // Read queued frames from buffer
      processFrameQueue();

      // Flush out unfinished frames
      for (auto &task : d_frame_queue) {
        if (task.is_send()) {
          continue;
        }

        if (task.num_symbols_copied == 0) {
          // don't flush out completely empty frames
          continue;
        }

        // Flush out frame
        if (task.num_remaining() > 0) {
          task.header = pmt::dict_add(task.header, pmt::intern("frame_complete"), pmt::PMT_F);
        }
        
        message_port_pub(d_PORT_OUT, task.generate_pmt_to_send());
      }


    	// Signal EOF to next block
    	message_port_pub(d_PORT_OUT, pmt::PMT_EOF);
      //printf("stop() called, sending EOF\n");
    	return true;
    }

    asm_position frame_sync_impl::findMaximumInBuffer(size_t max_num_items) {
      assert(max_num_items <= d_BLOCK_LEN_SYMBOLS);
      
      bool initialized = false;

      size_t max_index = 0u;
      size_t max_ambiguity = 0u;
      float max_score = 0.0f;
      gr_complex max_correlation = gr_complex(0.0f, 0.0f);
      float max_correlation_abs = 0.0f;
      
      for (size_t i=0; i<max_num_items; i++) {
        float energy_correction = 0.0;
        volk_32f_accumulator_s32f(&energy_correction, d_buffer.energy_window_ptr(i, d_ASM_LEN_SYMBOLS), static_cast<unsigned int>(d_ASM_LEN_SYMBOLS));

        const gr_complex correlation = d_buffer.correlation_window(i);
        const float correlation_abs = std::abs(correlation);
        const float score = correlation_abs - energy_correction;

        //printf("  at %3lu: correlation=%f, correction=%f, score=%f, score(exp)=%e, max_score=%f\n", i, std::abs(d_buffer_correlation[i]), energy_correction, score, std::exp(score), max_score);
        const bool better_finite_score = std::isfinite(score) && (score > max_score);
        const bool both_scores_infinite = (std::isfinite(max_score) == false) && (std::isfinite(score) == false);
        const bool infinite_scores_but_better_correlation = both_scores_infinite && (correlation_abs > max_correlation_abs);
        
        const bool update = (!initialized || better_finite_score || infinite_scores_but_better_correlation);

        utils::update_nobranch(initialized, update, true);
        utils::update_nobranch(max_index, update, i);
        utils::update_nobranch(max_score, update, score);
        utils::update_nobranch(max_correlation_abs, update, correlation_abs);
        utils::update_nobranch(max_correlation, update, correlation);
      }
      assert(initialized);
      assert(max_index < max_num_items);
      max_ambiguity = d_FRAME_SYNC_CONFIG->detect_ambiguity(max_correlation);
      //printf("Found maximum at local offset (in current frame) %lu, abs %lu with ambiguity %lu\n", max_index, d_buffer.offset()+max_index, max_ambiguity);
      return asm_position(d_buffer.offset()+max_index, max_ambiguity);
    }

    asm_header frame_sync_impl::computeSingleScore(asm_position asm_pos, bool primary) {
      // complex multiplier that will rotate the ambiguous symbol onto the unambiguous one.
      const gr_complex amb_correction = std::conj(d_FRAME_SYNC_CONFIG->get_ambiguity_symbol(asm_pos.ambiguity()));

      float energy_correction = 0.0;
      for (size_t j=0u; j<d_ASM_LEN_SYMBOLS; j++) {
        energy_correction += d_buffer.energy(asm_pos.offset()+j);
      }
      const gr_complex correlation = d_buffer.correlation(asm_pos.offset());
      const gr_complex correlation_corrected = correlation * amb_correction;
      float correlation_corrected_re = std::real(correlation_corrected);
      float score = correlation_corrected_re - energy_correction;
      
      // Transform score s \in (-inf, 0] to 100*exp(s) \in (0, 100]
      score = 100.0f*std::exp(score);
      correlation_corrected_re = 100.0f * correlation_corrected_re / static_cast<float>(d_ASM_LEN_SYMBOLS);

      return asm_header(asm_pos.offset(), asm_pos.ambiguity(), score, correlation_corrected_re, primary, d_buffer.snr(asm_pos.offset()), asm_pos.sequence());
    }

    void frame_sync_impl::queueFrameOutput(asm_position info, bool primary) {
      const size_t num_additional_head_symbols = (d_FRAME_SYNC_CONFIG->head_num_symbols() > 0) ? static_cast<size_t>(d_FRAME_SYNC_CONFIG->head_num_symbols()) : 0u;
      if (info.offset() < num_additional_head_symbols) {
        // Ask to copy from before start of frame
        
        // TODO: figure out what would be the correct behavior here, for now, throw assertion if enabled, else ignore the frame
        assert(false);
        printf("Warning: attempting to copy frame data before the start of the stream. Ignoring frame.\n");
        throw exceptions::InvalidParameter("Requested to copy a frame from before the start of the stream.");
      }

      // Check if there already is a frame queued for this positoin secondary frame that 
      auto existing_task_it = std::find_if(d_frame_queue.begin(), d_frame_queue.end(), 
        [&info](const frame_copy_task_t &t) {
          return info.matches(t.asm_pos);
      });
      

      assert(info.offset() + d_ASM_LEN_SYMBOLS >= num_additional_head_symbols);
      const uint64_t copy_from = utils::add_signed_to_unsigned(info.offset() + d_ASM_LEN_SYMBOLS, -d_FRAME_SYNC_CONFIG->head_num_symbols());

      const gr_complex amb_correction = std::conj(d_FRAME_SYNC_CONFIG->get_ambiguity_symbol(info.ambiguity()));
      
      frame_copy_task_t t(info, copy_from, amb_correction, primary, d_FRAME_SYNC_CONFIG->output_num_symbols(), d_NUM_BITS_PER_SYMBOL);
      
      if (existing_task_it == d_frame_queue.end()) {
        // No frame found in queue, creating new entry
        d_frame_queue.push_back(t);
        //printf("Creating a new frame\n");
      } else if (primary) {
        d_frame_queue.erase(existing_task_it);
        d_frame_queue.push_back(t);
        //printf("Frame is already in queue and a primary frame.\n");
      } else {
        //printf("Frame is already in queue and a secondary frame, ignoring.\n");
      }
      
    }

    void frame_sync_impl::processFrameQueue() {
      const uint64_t avail_from = d_buffer.offset_start();
      const uint64_t avail_until = d_buffer.offset_last_symbol();
      
      for (auto &task : d_frame_queue) {
        if (task.data_complete()) {
          continue;
        }
        
        const uint64_t first_sample_of_interest = (task.header_computed) ? task.copy_from : utils::pick_smaller(task.asm_pos.offset(), task.copy_from);

        if (first_sample_of_interest >= avail_from && first_sample_of_interest <= avail_until) {
          // Frame is within current buffer range

          const size_t num_avail = utils::minus_cap_zero(avail_until+1u, task.copy_from);
          const bool asm_available = (avail_from <= task.asm_pos.offset()) && (avail_until >= task.asm_pos.offset()+d_ASM_LEN_SYMBOLS);
          
          if (!task.header_computed && asm_available) {
            // Start frame
            asm_header frame_meta = computeSingleScore(task.asm_pos, task.primary);
            task.header = frame_meta.generate_header(d_NUM_BITS_PER_SYMBOL, d_BLOCK_LEN_SYMBOLS, task.header);
            task.header = pmt::dict_update(task.header, d_extra_tags);
            
            task.header_computed = true;
          }

          const size_t to_copy = utils::pick_smaller<size_t>(task.num_remaining(), num_avail);
          if (to_copy == 0u) {
            continue;
          }

          // Compute softbit-lookup table
          const float n0_average = d_buffer.snr(task.copy_from).N0_window(to_copy/2u);
          d_CONSTELLATION->gen_soft_dec_lut(d_SOFT_DECISION_PRECISION, n0_average);
          
          for(size_t i=0u; i<to_copy; i++) {
            // remove ambiguity
            const gr_complex symbol_unambiguous = d_buffer.signal(task.copy_from+i) * task.amb_correction;
            
            // decode (soft)
            std::vector<float> softbits = d_CONSTELLATION->soft_decision_maker(symbol_unambiguous);
            assert(softbits.size() == d_NUM_BITS_PER_SYMBOL);
            
            // copy softbits to frame buffer
            const size_t indx_out = (task.num_symbols_copied+i)*d_NUM_BITS_PER_SYMBOL;
            for (size_t j=0; j<softbits.size(); j++) {
              assert(std::isnan(softbits[j]) == false);
              pmt::f32vector_set(task.msg_buffer, indx_out+j, softbits[j]);
              assert( utils::floats_equal_non_nan(pmt::f32vector_ref(task.msg_buffer, indx_out+j), softbits[j], 1e-5f) );
            }
          }

          task.copy_from += to_copy;
          task.num_symbols_copied += to_copy;
        }

        if (task.data_complete()) {
          // mark frame as complete
          task.header = pmt::dict_add(task.header, pmt::intern("frame_complete"), pmt::PMT_T);
          task.header = pmt::dict_add(task.header, pmt::intern("frame_payload_offset"), pmt::from_long(-d_FRAME_SYNC_CONFIG->head_num_bits()));
          task.header = pmt::dict_add(task.header, pmt::intern("frame_number"), pmt::dict_ref(task.header, asm_header::KEY_ASM_NUMBER, pmt::PMT_NIL));
        }
      }

      // Figure out which frames can be sent out

      // Only send out frames that can no longer be generated by the current frame buffer
      const uint64_t indx_threshold = utils::minus_cap_zero(d_buffer.offset(), d_FRAME_GEN_CONFIG->frames_before()*d_BLOCK_LEN_SYMBOLS);
      for (auto& task : d_frame_queue) {
        if (task.data_complete() && !task.is_send() && task.asm_pos.offset() < indx_threshold) {
          message_port_pub(d_PORT_OUT, task.generate_pmt_to_send());
        }
      }

      // remove all frames that have been send out
      d_frame_queue.remove_if([](const frame_copy_task_t &t){ return t.is_send(); });
    }

    frame_copy_task_t::frame_copy_task_t(asm_position asm_pos_in, uint64_t frame_start, gr_complex amb_correction, bool primary_in, size_t num_symbols_total, size_t num_bits_per_symbol)
      : num_symbols_total(num_symbols_total),
        amb_correction(amb_correction/std::abs(amb_correction)),
        asm_pos(asm_pos_in),
        copy_from(frame_start),
        primary(primary_in),
        header(pmt::make_dict()),
        msg_buffer(pmt::make_f32vector(num_symbols_total*num_bits_per_symbol, 0.0)),
        num_symbols_copied(0u),
        header_computed(false),
        message_send_out(false)
      {
      // Nothing to do
    }

    bool frame_copy_task_t::data_complete() const {
      assert(num_symbols_copied <= num_symbols_total);
      return num_symbols_copied == num_symbols_total;
    }

    size_t frame_copy_task_t::num_remaining() const {
      assert(num_symbols_total >= num_symbols_copied);
      return utils::minus_cap_zero(num_symbols_total, num_symbols_copied);
    }

    bool frame_copy_task_t::is_send() const {
      return message_send_out;
    }

    pmt::pmt_t frame_copy_task_t::generate_pmt_to_send() {
      assert( header_computed );
      message_send_out = true;
      return pmt::cons(header, msg_buffer);
    }

  } /* namespace ccsds */
} /* namespace gr */

