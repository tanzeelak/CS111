# I am a commeynt, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
CFLAGS = -g

all:
	@gcc -g lab0.c -o lab0

check:	all goodinput goodoutput badoutput inandout noinandnoout canopen catchtest badinput

goodinput:
	@echo "haha haha" > test.txt
	@./lab0 --input=test.txt > output2.txt
	@if [ $$? -eq 0 ] ;\
	then \
		echo "Passed: Proper input.\n" ;\
	else \
		echo "Failed: Proper input.\n" ;\
	fi
	@rm output2.txt

goodoutput:
	@./lab0 --output=out.txt < test.txt
	@if [ $$? -eq 0 ] ;\
	then \
		echo "Passed: Proper output.\n" ;\
	else \
		echo "Failed: Proper output.\n" ;\
	fi
	@rm out.txt

inandout:
	@./lab0 --input=test.txt --output=out.txt
	@if [ $$? -eq 0 ] ;\
	then \
		echo "Passed: Proper input and output.\n" ;\
	else \
		echo "Failed: Proper input and output.\n" ;\
	fi ;\
	diff test.txt out.txt > changes.txt ;\
	if [ -s changes.txt ] ;\
	then \
		echo "Failed: Input and output are the same.\n" ;\
	else \
		echo "Passed: Input and output are different.\n" ;\
	fi
	@rm changes.txt out.txt


noinandnoout:
	@./lab0 < test.txt > out.txt
	@if [ $$? -eq 0 ] ;\
	then \
		echo "Passed: no input and no output test.\n" ;\
	else \
		echo "Failed: no input and no output test.\n" ;\
	fi
	@rm out.txt


badoutput:
	@touch out.txt
	@chmod 444 out.txt 
	@./lab0 --output=out.txt < test.txt ;\
	if [ $$? -eq 3 ] ;\
	then \
		echo "Passed: Bad output detected.\n" ;\
	else \
		echo "Failed: Bad output detected.\n" ;\
	fi
	@rm -f out.txt

canopen:
	@./lab0 --input=test.txt ;\
	if [ $$? -eq 0 ] ;\
	then \
		echo "Passed: Can open file.\n" ;\
	else \
		echo "Failed: Can open file.\n" ;\
	fi

badinput:
	@touch in.txt
	@chmod 000 in.txt
	@./lab0 --input=in.txt ;\
	if [ $$? -eq 2 ] ;\
	then \
		echo "Passed: Detected unopenable file.\n" ;\
	else \
		echo "Failed: Detected unopenable file.\n" ;\
	fi
	@rm -f in.txt test.txt

catchtest:
	@./lab0 --segfault --catch;\
	if [ $$? -eq 4 ] ;\
	then \
		echo "Passed: Caught segfault.\n" ;\
	else \
		echo "Failed: Caught segfault.\n" ;\
	fi

clean:
	@rm -f lab0 *.txt *.tar.gz

dist:
	@tar -czvf lab0-204577214.tar.gz Makefile lab0.c backtrace.png breakpoint.png README