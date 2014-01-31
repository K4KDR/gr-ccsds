#!/bin/gnuplot
set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 4  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 5  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "green"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 7  linetype 1 linecolor rgb "grey"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0

set datafile separator ','

plot	'/tmp/ccsds_ticp_frame_source_debug_timing.dat' using 1:($2-0.2*$1) with lines ls 1 title 'timing error'
#	'/tmp/ccsds_ticp_frame_source_debug_timing.dat' using 1:2 with lines ls 1 title 'TICP frame source', \
#	'/tmp/ccsds_ticp_frame_source_debug_timing.dat' using 1:(0.2*$1) with lines ls 2 title 'reference'
