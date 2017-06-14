#NAME: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214


default: tcp tls

tcp:
	gcc -lmraa -lm -g -o lab4c_tcp lab4c_tcp.c

tls:
	gcc -lmraa -lm -lssl -lcrypto -g -o lab4c_tls lab4c_tls.c

check:
	chmod +x P4C_check.sh
	./P4C_check.sh 204577214

clean:
	@rm -f lab4c_tcp lab4c_tls *.txt *.tar.gz

dist:
	@tar -zcvf lab4c-204577214.tar.gz P4C_check.sh lab4c_tls.c lab4c_tcp.c Makefile README
