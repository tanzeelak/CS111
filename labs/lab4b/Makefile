#Name: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214


build:
	gcc -pthread -g -lrt -std=gnu99 -o lab4b lab4b.c
	gcc -pthread -lmraa -g -lrt -std=gnu99 -o test2 test2.c

clean:

	@rm -f lab2_list *.txt *.tar.gzw *.csv

dist:
	@tar -zcvf lab2b-204577214.tar.gz lab2_list.c SortedList.c SortedList.h lab2b_list.csv lab2_list.gp profile.out *.png Makefile README
