#NAME: Tanzeela Khan
#EMAIL: tanzeelakhan@g.ucla.edu
#ID: 204577214

CC=gcc
CFLAGS = -g

all:	client	server

client:	lab1b-client.c
	gcc lab1b-client.c -g -lmcrypt -std=c99 -o lab1b-client

server:	lab1b-server.c
	gcc lab1b-server.c -g -lmcrypt -std=c99 -o lab1b-server

clean:
	@rm -f lab1a *.txt *.tar.gz

dist: 
	@tar -czvf lab1b-204577214.tar.gz Makefile lab1b-server.c lab1b-client.c my.key README
