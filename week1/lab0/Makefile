# I am a commeynt, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
CFLAGS = -g

all:
	gcc lab0.c -o lab0

check:	all goodinput goodoutput badoutput inandout noinandnoout canopen catchtest

goodinput:
	echo "haha haha" > test.txt
	./lab0 --input=test.txt > output2.txt
	([ $$? -eq 0 ] && echo "Success for input") || (echo "Fail";)

goodoutput:
	./lab0 --output=out.txt < test.txt
	([ $$? -eq 0 ] && echo "Success for output") || (echo "Fail";)

inandout:
	./lab0 --input=test.txt --output=output.txt
	if [ $$? -eq 0 ] ;\
	then \
		echo "Success, input and output" ;\
	fi

noinandnoout:
	./lab0 < test.txt > out.txt
	if [ $$? -eq 0 ] ;\
	then \
		echo "no input and no output test" ;\
	fi



badoutput:
	touch out.txt
	chmod 444 out.txt 
	./lab0 --output=out.txt < test.txt ;\
	if [ $$? -eq 3 ] ;\
	then \
		echo "bad output detected" ;\
	fi
	rm -f out.txt

canopen:
	./lab0 --input=test.txt
	if [ $$? -eq 1 ] ;\
	then \
		echo "Success, can open file" ;\
	fi

catchtest:
	./lab0 --segfault --catch;\
	if [ $$? -eq 4 ] ;\
	then \
		echo "caught error" ;\
	fi

clean:
	rm -f lab0 *.txt *.tar.gz

dist:
	tar -czvf lab0-204577214.tar.gz Makefile lab0.c backtrace.png breakpoint.png README