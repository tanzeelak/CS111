# I am a commeynt, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
CFLAGS = -g

all:
	gcc lab0.c -o lab0

check:	check1 check2 badoutput inandout canopen

check1:
	echo "haha haha" > test.txt
	./lab0 --input=test.txt > output2.txt
	([ $$? -eq 0 ] && echo "Success for input") || (echo "Fail";)

check2:
	./lab0 --output=out.txt < test.txt
	([ $$? -eq 0 ] && echo "Success for output") || (echo "Fail";)

#checkbado:
#	./lab0 < test.txt > output1.txt
#	([ $$? -eq 0 ] && echo "Success for bad output") || (echo "Fail";)


inandout:
	./lab0 --input=test.txt --output=output.txt
	if [ $$? -eq 0 ] ;\
	then \
		echo "Success, input and output" ;\
	fi

badoutput:
	touch out.txt
	chmod 444 out.txt 
	./lab0 --output=out.txt < test.txt ;\
	if [ $$? -ne 0 ] ;\
	then \
		echo "bad output detected" ;\
	fi

canopen:
	./lab0 --input=test.txt
	if [ $$? -eq 0 ] ;\
	then \
		echo "Success, can open file" ;\
	fi

clean:
	rm -f lab0 *.txt *.tar.gz

dist:
	tar -czvf lab0-204577214.tar.gz ~/cs111/week1/lab0/ Makefile lab0.c backtrace.png breakpoint.png README test.txt