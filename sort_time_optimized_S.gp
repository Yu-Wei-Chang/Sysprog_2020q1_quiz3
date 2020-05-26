reset
set term png enhanced font 'Verdana,10'
set output 'Optimized_S_comp.png'
set title 'Sorting time cost - Finding optimized S'
set xlabel "S"
set ylabel "Sorting time cost(ns)"
set grid

plot [0:100][0:700000] 'sort_time_optimized_s.txt' using 1:2 with linespoints linewidth 2 title \
'Sorting time cost'
