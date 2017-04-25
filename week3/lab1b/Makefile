#NAME: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214

CC=gcc
CFLAGS = -g

all:
	gcc lab1b-client.c -std=c99 -o lab1b-client
	gcc lab1b-server.c -std=c99 -o lab1b-server


clean:
	@rm -f lab1a *.txt *.tar.gz

dist: 
	@tar -czvf lab1a-204577214.tar.gz Makefile lab1a.c README
