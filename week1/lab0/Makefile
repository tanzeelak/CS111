# I am a commeynt, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
#CFLAGS=-c -g -Wall

default:
	$(CC) lab0.c -o lab0

check: checki

checki:
	./lab0 --input=in.txt > out.txt ; \
	if [ $$? -ne 0 ]; \
	then \
	echo "input test passed" \
	else \
	echo "input test failed" \
	fi

#checko:
#	./lab0 --output=out.txt < in.txt \
	if [ $$? -ne 0 ]; \
	then \
	     echo "output test passed" \
	fi

#checks:
#	./lab0 --segfault \
	if [ $$? -eq 4 ]; \
	then \
		echo "segfault test passed" \
	fi


#checkc:
#	./lab0 --catch \
	if [ $$? -eq 4 ]; \
	then \
		echo "catch test passed" \
	fi

#checkbadi:
#	./lab0 --input=furry.txt \
	if [ $$? -ne 2 ]; \
		echo "input file test failed" \
	fi

#checkbado:
#	touch furry.txt \
	chmod 444 furry.txt \
	./lab0 --output=furry.txt \
	if [ $$? -ne 3 ]; \
		echo "output file unable to be opened" \
	fi


#clean:
#	rm *o lab0

#dist: 