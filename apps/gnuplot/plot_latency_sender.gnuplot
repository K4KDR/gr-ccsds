#!/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 4  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 5  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "green"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 7  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0

set datafile separator ','

set ylabel 'time [s]'
set xlabel 'frames'

#plot	'/tmp/ccsds_latency_sender_debug.dat' using 1:($2-(($1-1)*$3)) with lines ls 1 title 'timing error'

plot	'/tmp/ccsds_latency_sender_debug.dat' using 1:2 with linespoints ls 1 title 'TICP frame source', \
	'/tmp/ccsds_latency_sender_debug.dat' using 1:(($1-1)*$3) with linespoints ls 2 title 'reference', \
	'/tmp/ccsds_latency_sender_debug.dat' using 1:4 with linespoints ls 3 title 'calculated time'
