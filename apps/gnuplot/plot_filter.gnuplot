#!/bin/gnuplot

set style line 1  linetype 1 linecolor rgb "red"     linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 2  linetype 1 linecolor rgb "green"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 3  linetype 1 linecolor rgb "purple"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 4  linetype 2 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 5  linetype 1 linecolor rgb "blue"    linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 6  linetype 1 linecolor rgb "yellow"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 7  linetype 1 linecolor rgb "orange"  linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0
set style line 7  linetype 1 linecolor rgb "black"   linewidth 1.0 pointtype 7 pointsize 0.5 pointinterval 0

set datafile separator ','
plot '/tmp/ccsds_lpf2_debug.dat' using 1:2 with linespoints ls 1 title 'in', \
     '/tmp/ccsds_lpf2_debug.dat' using 1:3 with linespoints ls 2 title 'out', \
     '/tmp/ccsds_lpf2_debug.dat' using 1:4 with linespoints ls 3 title 'xi', \
     '/tmp/ccsds_lpf2_debug.dat' using 1:5 with linespoints ls 4 title 'ephi', \
     '/tmp/ccsds_lpf2_debug.dat' using 1:6 with lines ls 5 title 'wrap_map', \
     '/tmp/ccsds_lpf2_debug.dat' using 1:(-$6) with lines ls 5 title 'wrap_map', \
     '/tmp/ccsds_lo_debug.dat' using 1:($5*1) with steps ls 6 title 'LO: toggle'

#     '/tmp/ccsds_lpf2_debug.dat' using 1:5 with linespoints title 'freq', \

