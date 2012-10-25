#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Bpsk Demod
# Generated: Wed Aug 22 08:53:26 2012
##################################################

from gnuradio import digital
from gnuradio import gr
from gnuradio import uhd
from gnuradio import window
from gnuradio.gr import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import scopesink2
# for dynamicly named file output of the recorded live usrp raw output
from datetime import datetime

class bpsk_demod(gr.hier_block2):

	def __init__(self, centre_freq, usrp_gain, samp_per_sym, sym_rate):
		gr.hier_block2.__init__(
			self, "Bpsk Demod",
			gr.io_signature(0, 0, 0),
			gr.io_signature(1, 1, gr.sizeof_char*1),
		)

		##################################################
		# Variables
		##################################################
		self.omega = omega = samp_per_sym # 2
		self.bit_rate = bit_rate = sym_rate # 2e6
		self.usrp_gain = usrp_gain # 20
		self.samp_rate = samp_rate = bit_rate*omega
		self.lpf_interp = lpf_interp = 1
		self.loop_bw = loop_bw = 6.28/100
		self.freq = freq = centre_freq # 2.235e9
		self.bp_transition_width = bp_transition_width = bit_rate/2
		self.Beta = Beta = 1

		##################################################
		# Blocks
		##################################################
		# self.wxgui_scopesink2_0_0 = scopesink2.scope_sink_f(
			# window=self.GetWin(),
			# title="Scope Plot",
			# sample_rate=samp_rate,
			# v_scale=0,
			# v_offset=0,
			# t_scale=0,
			# ac_couple=False,
			# xy_mode=False,
			# num_inputs=1,
			# trig_mode=gr.gr_TRIG_MODE_AUTO,
			# y_axis_label="Counts"
		# )
		# self.Add(self.wxgui_scopesink2_0_0.win)
		# self.wxgui_fftsink2_0 = fftsink2.fft_sink_c(
			# self.GetWin(),
			# baseband_freq=0,
			# y_per_div=1,
			# y_divs=10,
			# ref_level=0,
			# ref_scale=2.0,
			# sample_rate=samp_rate,
			# fft_size=1024,
			# fft_rate=15,
			# average=True,
			# avg_alpha=None,
			# title="FFT Plot",
			# peak_hold=False,
			# win=window.hamming,
		# )
		# self.Add(self.wxgui_fftsink2_0.win)
		self.uhd_usrp_source = uhd.usrp_source(
			device_addr="addr=192.168.10.2",
			stream_args=uhd.stream_args(
				cpu_format="fc32",
				channels=range(1),
			),
		)
		self.uhd_usrp_source.set_samp_rate(samp_rate)
		self.uhd_usrp_source.set_center_freq(freq, 0)
		self.uhd_usrp_source.set_gain(usrp_gain, 0)
		self.uhd_usrp_source.set_bandwidth(10000000, 0)
		self.low_pass_filter_0 = gr.interp_fir_filter_ccf(lpf_interp, firdes.low_pass(
			1, samp_rate, bit_rate*1, bp_transition_width, firdes.WIN_HAMMING, 6.76))
		self.gr_complex_to_real_0 = gr.complex_to_real(1)
		self.gr_agc_xx_0 = gr.agc_cc(1e-5, 1.0, 1, 10.0)
		self.digital_costas_loop_cc_0 = digital.costas_loop_cc(loop_bw, 2)
		self.digital_clock_recovery_mm_xx_0_0 = digital.clock_recovery_mm_cc(omega, 0.00005, 0.5, 0.5, 0.05)
		self.digital_binary_slicer_fb_0_0 = digital.binary_slicer_fb()
		self.band_pass_filter_0 = gr.interp_fir_filter_ccf(1, firdes.band_pass(
			1, samp_rate, 1, bit_rate, bp_transition_width, firdes.WIN_HAMMING, 6.76))
		#self.file_out = file_out = "/home/ngs1/usrp_raw_output/" + datetime.now().strftime("%Y.%m.%d.%H.%M.%S") + ".dat"
		#self.raw_sink = gr.file_sink(gr.sizeof_gr_complex*1, file_out);

		##################################################
		# Connections
		##################################################
		self.connect((self.band_pass_filter_0, 0), (self.digital_costas_loop_cc_0, 0))
		self.connect((self.digital_costas_loop_cc_0, 0), (self.low_pass_filter_0, 0))
		# self.connect((self.gr_complex_to_real_0, 0), (self.wxgui_scopesink2_0_0, 0))
		self.connect((self.gr_complex_to_real_0, 0), (self.digital_binary_slicer_fb_0_0, 0))
		self.connect((self.digital_clock_recovery_mm_xx_0_0, 0), (self.gr_complex_to_real_0, 0))
		self.connect((self.low_pass_filter_0, 0), (self.digital_clock_recovery_mm_xx_0_0, 0))
		self.connect((self.uhd_usrp_source, 0), (self.gr_agc_xx_0, 0))
		# self.connect((self.uhd_usrp_source, 0), (self.wxgui_fftsink2_0, 0))
		#self.connect((self.uhd_usrp_source, 0), (self.file_out, 0))
		self.connect((self.gr_agc_xx_0, 0), (self.band_pass_filter_0, 0))
		self.connect((self.digital_binary_slicer_fb_0_0, 0), (self, 0))

	def get_omega(self):
		return self.omega

	def set_omega(self, omega):
		self.omega = omega
		self.set_samp_rate(self.bit_rate*self.omega)
		self.digital_clock_recovery_mm_xx_0_0.set_omega(self.omega)

	def get_bit_rate(self):
		return self.bit_rate

	def set_bit_rate(self, bit_rate):
		self.bit_rate = bit_rate
		self.set_samp_rate(self.bit_rate*self.omega)
		self.band_pass_filter_0.set_taps(firdes.band_pass(1, self.samp_rate, 1, self.bit_rate, self.bp_transition_width, firdes.WIN_HAMMING, 6.76))
		self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.bit_rate*1, self.bp_transition_width, firdes.WIN_HAMMING, 6.76))
		self.set_bp_transition_width(self.bit_rate/2)

	def get_usrp_gain(self):
		return self.usrp_gain

	def set_usrp_gain(self, usrp_gain):
		self.usrp_gain = usrp_gain
		self.uhd_usrp_source.set_gain(self.usrp_gain, 0)

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.band_pass_filter_0.set_taps(firdes.band_pass(1, self.samp_rate, 1, self.bit_rate, self.bp_transition_width, firdes.WIN_HAMMING, 6.76))
		self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.bit_rate*1, self.bp_transition_width, firdes.WIN_HAMMING, 6.76))
		self.wxgui_scopesink2_0_0.set_sample_rate(self.samp_rate)
		self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate)
		self.uhd_usrp_source.set_samp_rate(self.samp_rate)

	def get_lpf_interp(self):
		return self.lpf_interp

	def set_lpf_interp(self, lpf_interp):
		self.lpf_interp = lpf_interp

	def get_loop_bw(self):
		return self.loop_bw

	def set_loop_bw(self, loop_bw):
		self.loop_bw = loop_bw
		self.digital_costas_loop_cc_0.set_loop_bandwidth(self.loop_bw)

	def get_freq(self):
		return self.freq

	def set_freq(self, freq):
		self.freq = freq
		self.uhd_usrp_source.set_center_freq(self.freq, 0)

	def get_bp_transition_width(self):
		return self.bp_transition_width

	def set_bp_transition_width(self, bp_transition_width):
		self.bp_transition_width = bp_transition_width
		self.band_pass_filter_0.set_taps(firdes.band_pass(1, self.samp_rate, 1, self.bit_rate, self.bp_transition_width, firdes.WIN_HAMMING, 6.76))
		self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.bit_rate*1, self.bp_transition_width, firdes.WIN_HAMMING, 6.76))

	def get_Beta(self):
		return self.Beta

	def set_Beta(self, Beta):
		self.Beta = Beta


