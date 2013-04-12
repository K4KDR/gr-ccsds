#!/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 4  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 5  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "green"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 7  linetype 1 linecolor rgb "yellow"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0

set datafile separator ','

plot	'/tmp/ccsds_pll_debug.dat' using 1:2 with linespoints ls 1 title 'phi_raw', \
	'/tmp/ccsds_pll_debug.dat' using 1:3 with linespoints ls 2 title 'phi'
#	'/tmp/ccsds_pll_debug_lo.dat' using 1:2 with steps ls 4 title 'raw freq est.'

#	'/tmp/ccsds_pll_debug.dat' using 1:6 with linespoints ls 5 title 'abs(in)', \
#	'/tmp/ccsds_pll_debug.dat' using 1:4 with linespoints ls 3 title 'arg(in)'


#	'/tmp/ccsds_pll_debug.dat' using 1:8 with linespoints ls 6 title 'imag(in)', \
#	'/tmp/ccsds_pll_debug.dat' using 1:7 with linespoints ls 7 title 'real(in)', \

