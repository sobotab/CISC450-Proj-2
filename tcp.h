#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */
#include <ctype.h>          /* for toupper */

#define STRING_SIZE 1024

// I don't know what the different phase port numbers are for yet, but they are stored here
#define SERV_TCP_PORT 46238
#define SERV_TCP_PORT_1 48238
//Client port number
#define CLNT_TCP_PORT 45238

typedef struct message {
	//Message transmitted between server and client
	unsigned short int step_no;
	unsigned short int client_port_no;
	unsigned short int server_port_no;
	unsigned short int server_secret_no;
	char text[80];
} message_t;


