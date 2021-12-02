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

      //printf("Rec step_no: %hu\n", rec_message->step_no);
      //printf("Rec client_port_no: %hu\n", rec_message->client_port_no);
      //printf("Rec server_port_no: %hu\n", rec_message->server_port_no);
      //printf("Rec server_secret_no: %hu\n", rec_message->server_secret_no);
      //printf("Rec text: %s\n", rec_message->text);

      FILE* visitors_log = fopen("Visitors.txt", "r");
      FILE* tmp_visitors_log = fopen("tempVisitors.txt", "a");
      char line[STRING_SIZE];
      char new_line[STRING_SIZE];
      new_line[0]='\0';

      //Saves visitor info and keeps old info as well
      while (fgets(line, sizeof(line), visitors_log)) {
	      strcat(new_line, line);
	      strtok(new_line, ",");
	      tmp_client_port=atoi(strtok(NULL, ","));
	      if (tmp_client_port != rec_message->client_port_no) {
		      fprintf(tmp_visitors_log, "%s", line);
	      }
     }
     memset(new_line, '\0', STRING_SIZE);
     
     //Steps for the different phases
     char tmp_str[STRING_SIZE];
     ret_message->client_port_no=rec_message->client_port_no;
     ret_message->server_port_no=SERV_TCP_PORT;
     if (rec_message->server_port_no == SERV_TCP_PORT) {
	     ret_message->server_secret_no = secret_code;
	     if (rec_message->server_secret_no == secret_code) {
	     	ret_message->step_no=3;
	     	strcpy(ret_message->text, travel_loc);
     	     } else if (rec_message->server_port_no == SERV_TCP_PORT) {
	     	ret_message->step_no=2;
	     	strcpy(ret_message->text, "*");
	     }
      	     memset(tmp_str, '\0', STRING_SIZE);
     	     sprintf(tmp_str, "%d", rec_message->step_no);
             strcat(new_line, tmp_str);
             strcat(new_line, ",");
             memset(tmp_str, '\0', STRING_SIZE);
             sprintf(tmp_str, "%d", rec_message->client_port_no);
             strcat(new_line, tmp_str);
             strcat(new_line, ",");
             strcat(new_line, rec_message->text);
             strcat(new_line, "\n");
     } else {
	     ret_message->step_no=1;
	     ret_message->server_secret_no=0;
	     ret_message->server_secret_no=0;
	     strcpy(ret_message->text, "*");
      	     memset(tmp_str, '\0', STRING_SIZE);
             sprintf(tmp_str, "%d", 1);
             strcat(new_line, tmp_str);
             strcat(new_line, ",");
             memset(tmp_str, '\0', STRING_SIZE);
             sprintf(tmp_str, "%d", rec_message->client_port_no);
             strcat(new_line, tmp_str);
             strcat(new_line, ",");
             strcat(new_line, "*");
             strcat(new_line, "\n");
     }

     fprintf(tmp_visitors_log, "%s", new_line);

     fclose(visitors_log);
     fclose(tmp_visitors_log);
     system("cp ./tempVisitors.txt ./Visitors.txt");
     system("> ./tempVisitors.txt");
     

     ret_message->step_no=htons(ret_message->step_no);
     ret_message->client_port_no=htons(ret_message->client_port_no);
     ret_message->server_port_no=htons(ret_message->server_port_no);
     ret_message->server_secret_no=htons(ret_message->server_secret_no);
     /* send message */
     send(sock_connection, ret_message, sizeof(message_t), 0);

      /* close the socket */

      close(sock_connection);
   } 
}
