#Name--: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214

CC=gcc
CFLAGS = -g


build:
	@gcc -pthread -g -lrt -std=gnu99 -o lab2_list SortedList.c lab2_list.c

lab2_list: lab2_list.c SortedList.c
	gcc -std=gnu99 -g -Og  -lpthread -o lab2_list -L ~/lib/ -lprofiler lab2_list.c SortedList.c

profile1: lab2_list Makefile
	-rm -rf ./raw.gperf
	pprof --text ./local.raw
	export LD_PRELOAD=/u/cs/ugrad/tanzeela/lib/libprofiler.so.0 
	export CPUPROFILE=./raw.gperf 
	./lab2_list --threads=12 --iterations=1000 
	pprof --text ./lab2_list ./raw.gperf > profile 
	pprof --list=listAdd ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf

profile: lab2_list Makefile
	-rm -rf ./raw.gperf
	LD_PRELOAD=/u/cs/ugrad/tanzeela/lib/libprofiler.so.0 CPUPROFILE=raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=listAdd ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf

tests:
	rm -f *.txt *.tar.gzw
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv

	-./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2--lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab2_list.csv

	./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m >> lab2_list.csv

	./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s >> lab2_list.csv

	./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=m >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=m >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=m >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=m >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m >> lab2_list.csv

	./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=s >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=s >> lab2_list.csv

	./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=s >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=s >> lab2_list.csv

	./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=s >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=s >> lab2_list.csv

	./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=s >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s >> lab2_list.csv



graphs:
	chmod +x lab2_list.gp
	/usr/local/cs/bin/gnuplot lab2_list.gp

clean:
	@rm -f lab2_list *.txt *.tar.gzw *.csv

dist: 
	@tar -zcvf lab2b-204577214.tar.gz lab2_list.c SortedList.c SortedList.h lab2b_list.csv lab2_list.gp profile.out *.png Makefile README
