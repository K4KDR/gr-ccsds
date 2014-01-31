#!/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 4  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 5  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0

set datafile separator ','

plot	'debug_fll.dat' using 1:(1.2*$4) with steps ls 6 title 'block', \
	'debug_fll_pow.dat' using 1:2 with linespoints ls 2 title 'arg(in^M)', \
	'debug_fll_dif.dat' using 1:2 with linespoints ls 3 title 'arg(diff)', \
	'debug_fll_sum.dat' using 1:2 with linespoints ls 1 title 'arg(sum)', \
	'debug_fll.dat' using 1:2 with linespoints ls 4 title 'nu_raw', \
	'debug_fll.dat' using 1:3 with linespoints ls 5 title 'nu'


#	'debug_fll_pow.dat' using 1:3 with linespoints ls 2 title 'abs(in^M)', \
#	'debug_fll_dif.dat' using 1:3 with linespoints ls 4 title 'abs(diff)', \

