CC=gcc

all: server client

server: server.c
	$(CC) -o server server.c -pthread 

client: client.c
	$(CC) -o client client.c
	
clean: 
	
	rm -f server client

