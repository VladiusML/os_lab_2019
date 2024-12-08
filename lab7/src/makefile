CC = gcc
CFLAGS = -Wall -g

all : tcpserver tcpclient udpclient udpserver

tcpclient : tcpclient.c
	$(CC) $(CFLAGS) -o tcpclient tcpclient.c

tcpserver : tcpserver.c
	$(CC) $(CFLAGS) -o tcpserver tcpserver.c

udpclient : udpclient.c
	$(CC) $(CFLAGS) -o udpclient udpclient.c

udpserver : udpserver.c
	$(CC) $(CFLAGS) -o udpserver udpserver.c

clean :
	rm tcpclient tcpserver udpclient udpserver