#include "tcp.h"

int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]="localhost"; /* Server's hostname */
   unsigned short server_port=46238;  /* Port number used by server (remote port) */
   //unsigned short int client_port=CLNT_TCP_PORT;

   char sentence[STRING_SIZE]="localhost";  /* send message */
   char modifiedSentence[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_recd; /* number of bytes sent or received */
  
   /* open a socket */

   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

    /* connect to the server */
 		
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
  
   /* user interface */

   printf("Please input a sentence:\n");
   scanf("%s", sentence);
   msg_len = strlen(sentence) + 1;

   /* send message */
   
   bytes_sent = send(sock_client, sentence, msg_len, 0);

   /* get response from server */
  
   bytes_recd = recv(sock_client, modifiedSentence, STRING_SIZE, 0); 

   printf("\nThe response from server is:\n");
   printf("%s\n\n", modifiedSentence);

   /* close the socket */

   close (sock_client);
}
