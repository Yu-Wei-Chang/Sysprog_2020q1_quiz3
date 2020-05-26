reset
set term png enhanced font 'Verdana,10'
set output 'Divide_approach_comp.png'
set title 'Sorting time cost - Compare two different approach of divide node'
set xlabel "Data numbers of XOR linked list"
set ylabel "Sorting time cost(ns)"
set grid

plot [0:1000][0:6000000] 'sort_time_orig.txt' using 1:2 with linespoints linewidth 2 title 'Divide \
list 1 node by 1 node', \
'sort_time_new.txt' using 1:2 with linespoints linewidth 2 title 'Divide into two equal length list'
