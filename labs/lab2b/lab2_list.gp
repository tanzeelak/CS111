#! /usr/bin/gnuplot
#
#1;95;0c purpose:
#  generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
# 1. test name
# 2. # threads
# 3. # iterations per thread
# 4. # lists
# 5. # operations performed (threads x iterations x (ins + lookup + delete))
# 6. run time (ns)
# 7. run time per operation (ns)
#
# output:
# lab2_list-1.png ... cost per operation vs threads and iterations
# lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
# lab2_list-3.png ... threads and iterations that run (protected) w/o failure
# lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
# Managing data is simplified by keeping all of the results in a single
# file.  But this means that the individual graphing commands have to
# grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

# how many threads/iterations we can run without failure (w/o yielding)

set title "List-1: throughput vs number of threads"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Length-adjusted cost per operation(ns)"
set logscale y
set output 'lab2b_1.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '(adjusted) list w/mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '(adjusted) list w/spin-lock' with linespoints lc rgb 'green'

set title "List-2: mean time per mutex wait and mean time per operation"
set xlabel "Threads"
set logscale x 2
unset xrange
set xrange [0.75:]
set ylabel "Length-adjusted cost per operation(ns)"
set logscale y
set output 'lab2b_2.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):($7) \
     title '(adjusted) list w/completion' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):($8) \
     title '(adjusted) list w/waitfor' with linespoints lc rgb 'green'


set title "Scalability 3: Correct Synchronization of Partioned Lists"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
     "< grep 'list-id-none,' lab2b_list.csv" using ($2):($3) \
     with points lc rgb "red" title "unprotected", \
     "< grep 'list-id-m,' lab2b_list.csv" using ($2):($3) \
     with points lc rgb "green" title "Mutex", \
     "< grep 'list-id-s,' lab2b_list.csv" using ($2):($3) \
     with points lc rgb "blue" title "Spin-Lock", \

set title "Scalability 4: Throughput with Mutex Sync"
set xlabel "Number of thraeds"
set logscale x 2
set ylabel "Througput"
set logscale y 10
set output 'lab2b_4.png'
plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '4 list' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '8 list' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '16 list' with linespoints lc rgb 'orange', \

set title "Scalability 5: Throughput with Spin Lock Sync"
set xlabel "Number of thraeds"
set logscale x 2
set ylabel "Througput"
set logscale y 10
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '1 list' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '4 list' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '8 list' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title '16 list' with linespoints lc rgb 'orange', \

