#!/usr/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "green"   linewidth 1.0 pointtype 6 pointsize 1.0 pointinterval 0
set style line 3  linetype 1 linecolor rgb "purple"  linewidth 1.0
set style line 4  linetype 2 linecolor rgb "black"   linewidth 1.0
set style line 5  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 6 pointsize 1.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "yellow"  linewidth 1.0 pointtype 6 pointsize 1.0 pointinterval 0
set style line 7  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 6 pointsize 1.0 pointinterval 0
set style line 8  linetype 1 linecolor rgb "black"  linewidth 1.0 pointtype 6 pointsize 1.0 pointinterval 0

set datafile separator ','

plot	'/tmp/ccsds_dll_debug_tauh.dat' using 1:5 with linespoints ls 3 title 'mu', \
	'/tmp/ccsds_dll_debug_orig.dat' using 1:6 with steps ls 6 title 'toggle', \
	'/tmp/ccsds_dll_debug_orig.dat' using 1:2 with linespoints ls 1 title 'real(y)', \
	'/tmp/ccsds_dll_debug_intp.dat' using 1:2 with points ls 2 title 'real(y_hat)',\
	'/tmp/ccsds_dll_debug_symb.dat' using 1:2 with points ls 5 title 'real(y_k)', \
	'/tmp/ccsds_dll_debug_tauh.dat' using 1:2 with steps ls 7 title 'e_tau', \
	'/tmp/ccsds_dll_debug_tauh.dat' using 1:3 with steps ls 8 title 'tau'

#	'/tmp/ccsds_dll_debug_symb.dat' using 1:4 with linespoints ls 6 title 'abs(y_k)', \
#	'/tmp/ccsds_dll_debug_symb.dat' using 1:5 with linespoints ls 7 title 'arg(y_k)', \
#	'/tmp/ccsds_dll_debug_tauh.dat' using 1:4 with linespoints ls 4 title 'l', \

#	'/tmp/ccsds_dll_debug_orig.dat' using 1:3 with linespoints ls 1 lc rgb "orange" title 'imag(y)', \
#	'/tmp/ccsds_dll_debug_intp.dat' using 1:4 with points ls 2 title 'imag(y_hat)', \
#
#

