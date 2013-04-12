#!/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 4  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 5  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "green"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 7  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0

set datafile separator ','

plot	'/tmp/ccsds_pll_debug_lo.dat' using 1:2 with steps ls 2 title 'PLL: nu', \
	'/tmp/ccsds_pll_debug.dat' using 1:9 with linespoints ls 5 title 'PLL: nu_raw unselected', \
	'/tmp/ccsds_lo_debug.dat' using 1:2 with lines ls 4 title 'LO rot: interp. freq', \
	'/tmp/ccsds_lo_debug.dat' using 1:3 with lines ls 3 title 'LO rot: freq', \
	'/tmp/ccsds_lo_debug_msg.dat' using 1:2 with steps ls 6 title'LO msg: freq increase', \
	'/tmp/ccsds_lo_debug_msg.dat' using 1:3 with steps ls 7 title'LO msg: freq'

#	'/tmp/ccsds_pll_debug_lo.dat' using 1:2 with steps ls 1 title 'PLL: nu_raw', \

#	'/tmp/ccsds_lo_debug.dat' using 1:($5*1) with steps ls 6 title 'LO: toggle'

#	'/tmp/ccsds_lo_debug.dat' using 1:6 with linespoints ls 7 title 'LO rot cos', \
#	'/tmp/ccsds_lo_debug.dat' using 1:7 with linespoints ls 8 title 'LO rot sin', \

#	'debug_fll_pow.dat' using 1:3 with linespoints ls 2 title 'abs(in^M)', \
#	'debug_fll_dif.dat' using 1:3 with linespoints ls 4 title 'abs(diff)', \

