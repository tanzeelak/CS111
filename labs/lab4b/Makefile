#NAME: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214


default:
	gcc -lmraa -lm -g -o lab4b lab4b.c

check:
	chmod +x test.sh
	./test.sh 204577214

clean:
	@rm -f lab4b *.txt *.tar.gzw

dist:
	@tar -zcvf lab4b-204577214.tar.gz lab4b lab4b.c test.sh Makefile README
