#!/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "green"   linewidth 1.0 pointtype 6 pointsize 1.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "blue"    linewidth 1.0
set style line 4  linetype 2 linecolor rgb "black"   linewidth 1.0

set datafile separator ','

plot	'/tmp/ccsds_dll_debug_tauh.dat' using 1:3 with steps ls 3 title 'tau_hat', \
	'/tmp/ccsds_dll_debug_tauh.dat' using 1:2 with steps ls 4 title 'e_tau'

#
#	'/tmp/ccsds_dll_debug_intp.dat' using 1:7 ls 4 lt 2 title 'mu'
#
#	'/tmp/ccsds_dll_debug_intp.dat' using 1:5 with lines ls 4 title 'l', \
#	'/tmp/ccsds_dll_debug_orig.dat' using 1:3 with lines title 'imag(y)', \
#	'/tmp/ccsds_dll_debug_intp.dat' using 1:4 title 'imag(y_hat)', \
