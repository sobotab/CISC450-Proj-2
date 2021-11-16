#include "tcp.h"

int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   //Message information to be used in this application
   message_t *rec_message=(message_t *)malloc(sizeof(message_t));
   message_t *ret_message=(message_t *)malloc(sizeof(message_t));

   unsigned int secret_code = 39629;
   char travel_loc[80] = "London";

   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
    
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n"); 
         close(sock_server);
         exit(1);
      }
      
      rec_message->text=calloc(80, sizeof(char));
      printf("made it here\n");
      recv(sock_connection, rec_message, sizeof(rec_message)+sizeof(char)*80, 0);
      printf("did not make it here\n");

      if (1){
	 /*Convert message from network to host*/
        rec_message->step_no=ntohs(rec_message->step_no);
        rec_message->client_port_no=ntohs(rec_message->client_port_no);
        rec_message->server_port_no=ntohs(rec_message->server_port_no);
        rec_message->server_secret_no=ntohs(rec_message->server_secret_no);
        for (int i=0; rec_message->text[i] || i<80; i++) {
                rec_message->text[i]=tolower(rec_message->text[i]);
        }

	printf("Rec step_no: %hu\n", rec_message->step_no);
        printf("Rec client_port_no: %hu\n", rec_message->client_port_no);
        printf("Rec server_port_no: %hu\n", rec_message->server_port_no);
        printf("Rec server_secret_no: %hu\n", rec_message->server_secret_no);
        printf("Tec text: %s\n", rec_message->text);

	ret_message->step_no=htons(1);
        ret_message->client_port_no=htons(rec_message->client_port_no);
        ret_message->server_port_no=htons(SERV_TCP_PORT);
        ret_message->server_secret_no=htons(secret_code);
        ret_message->text=(char*)calloc(80,sizeof(char));
        ret_message->text=rec_message->text;


         /* send message */
         send(sock_connection, ret_message, sizeof(ret_message)+sizeof(char)*80, 0);
      }

      /* close the socket */

      close(sock_connection);
   } 
}
