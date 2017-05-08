#NAME: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214

CC=gcc
CFLAGS = -g

subfiles = SortedList.h SortedList.c lab2_add.c lab2_list.c Makefile README *.png

all:
	@gcc -g -pthread lab2_add.c -o lab2_add
	@gcc -g -pthread -std=gnu99 lab2_list.c -o lab2_list
	@gcc -pthread -lrt  -std=gnu99 -o lab2_list SortedList.c lab2_list.c


clean:
	@rm -f lab2_add *.txt *.tar.gz

dist: 
	@tar -czvf lab2a-204577214.tar.gz $(subfiles)
