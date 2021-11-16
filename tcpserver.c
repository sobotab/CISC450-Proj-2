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
   unsigned short int step_no=0;
   unsigned short int tmp_client_port=0;

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
      
      recv(sock_connection, rec_message, sizeof(message_t), 0); 

      /*Convert message from network to host*/
      rec_message->step_no=ntohs(rec_message->step_no);
      rec_message->client_port_no=ntohs(rec_message->client_port_no);
      rec_message->server_port_no=ntohs(rec_message->server_port_no);
      rec_message->server_secret_no=ntohs(rec_message->server_secret_no);
      for (int i=0; i<80; i++) {
              rec_message->text[i]=tolower(rec_message->text[i]);
      }

      printf("Rec step_no: %hu\n", rec_message->step_no);
      printf("Rec client_port_no: %hu\n", rec_message->client_port_no);
      printf("Rec server_port_no: %hu\n", rec_message->server_port_no);
      printf("Rec server_secret_no: %hu\n", rec_message->server_secret_no);
      printf("Rec text: %s\n", rec_message->text);

      message_t tmp_message;
      FILE* visitors_log = fopen("Visitors.txt", "r");
      FILE* tmp_visitors_log = fopen("tempVisitors.txt", "a");
      char line[STRING_SIZE];
      char newLine[STRING_SIZE];

      while (fgets(line, sizeof(line), visitors_log)) {
	      strtok(line, ",");
	      tmp_client_port=atoi(strok(NULL, ","));
	      if (tmp_client_port != rec_message->client_port_no) {
		      fprintf(tmp_visitors_log, "%s", line);
	      }
      }
      unsigned short int tmp_str[1];
      sprintf(tmp_str, "%d", rec_message->step_no);
      strcat(newLine, tmp_str);
      strcat(newLine, ",");
      unsigned short int tmp_str[5];
      sprintf(tmp_str, "%d", rec_message->client_port_no);
      strcat(newLine, ",");
      strcat(newLine, rec_message->text);
      strcat(newLine, "\n");
      fprintf(tmp_visitors_log, newLine);

      fclose(visitors_log);
      fclose(tmp_visitors_log);
      system("cp ./tempVisitors.txt ./Visitors.txt");
      system("> ./tempVisitors.txt");


     if(1) {
     	     ret_message->step_no=htons(1);
             ret_message->client_port_no=htons(rec_message->client_port_no);
             ret_message->server_port_no=htons(SERV_TCP_PORT);
             ret_message->server_secret_no=htons(secret_code);
             strcpy(ret_message->text, rec_message->text);
     } else if (step_no == 2) {
		
     } else if (step_no == 3) {
	
     } else {
	     printf("Error: \n");
     }

     /* send message */
     send(sock_connection, ret_message, sizeof(message_t), 0);

      /* close the socket */

      close(sock_connection);
   } 
}
