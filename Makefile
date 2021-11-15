C= /usr/bin/gcc

all:	tcpclient tcpserver

run-server: group20-tcpserver
	./group20-tcpserver

run-client: group20-tcpclient
	./group20-tcpclient

tcpclient: tcpclient.c;
	${CC} tcpclient.c tcp.c -o group20-tcpclient

tcpserver: tcpserver.c;
	${CC} tcpserver.c tcp.c -o group20-tcpserver

clean:
	rm group20-tcpclient group20-tcpserver
