#!/usr/bin/octave

addpath('/home/ngs1/gnuradio-3.6.0/gnuradio-core/src/utils');

raw = read_float_binary('/home/ngs1/martin_luelf_test/sync_test/fll_raw_freq.out');
fil = read_float_binary('/home/ngs1/martin_luelf_test/sync_test/fll_fil_freq.out');

fig = figure();

plot(raw,'b-');
hold on;
plot(fil,'r-');
legend('raw frequency', 'filtered frequency');


