#NAME: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214


default:
	gcc -lmraa -lm -g -o lab4c_tcp lab4c_tcp.c
	gcc -lmraa -lm -g -o lab4c_tls lab4c_tls.c
check:
	chmod +x test.sh
	./test.sh 204577214

clean:
	@rm -f lab4c *.txt *.tar.gzw

dist:
	@tar -zcvf lab4c-204577214.tar.gz lab4c_tcp lab4c_tcp.c test.sh Makefile README
