CC=gcc
CFLAGS=-pthread

all: tcp udp

tcp:
	$(CC) -o tcpclient tcpclient.c -pthread  
	$(CC) -o tcpserver tcpserver.c -pthread  

udp:
	$(CC) -o udpserver udpserver.c -pthread  
	$(CC) -o udpclient udpclient.c -pthread

clean:
	rm -rf udpclient udpserver tcpclient tcpserver
