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

set title "Scalability 1: Throughput of Synchronized Lists"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput (operations/sec)"
set logscale y 10
set output 'lab2b_1.png'
plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'list ins/lookup/delete w/ mutex' with linespoints lc rgb 'orange', \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'list ins/lookup/delete w/spin' with linespoints lc rgb 'purple'

set title "Scalability 2: Per-operation Times for Mutex-Protected List Operations"
set xlabel "Threads"
set logscale x 2
set ylabel "mean time/operation (ns)"
set logscale y 10
set output 'lab2b_2.png'
plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):($7) \
     	title 'completion time' with linespoints lc rgb 'orange', \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):($8) \
     	title 'wait for lock' with linespoints lc rgb 'red'



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

set title "Scalability 4: Throughput with Mutex-Synchronized Partitioned Lists"
set xlabel "Number of thraeds"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_4.png'
plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=1' with linespoints lc rgb 'purple', \
     "< grep 'list-none-m,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=4' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=8' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=16' with linespoints lc rgb 'orange', \

set title "Scalability 5: Throughput with Spin Lock Sync"
set xlabel "Number of thraeds"
set logscale x 2
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=1' with linespoints lc rgb 'purple', \
     "< grep 'list-none-s,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=4' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=8' with linespoints lc rgb 'blue', \
     "< grep 'list-none-s,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     	title 'lists=16' with linespoints lc rgb 'orange', \

