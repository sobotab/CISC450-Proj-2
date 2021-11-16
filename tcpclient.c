#include "tcp.h"

int main(void) {
   
   //Variables defined to be used by the client backend
   int sock_client;  /* Socket used by client */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]="localhost"; /* Server's hostname */
   unsigned short server_port=46238;  /* Port number used by server (remote port) */
   unsigned short client_port=CLNT_TCP_PORT; /* Port number used by client (local port) */

   //Message information to be used in this application
   message_t *rec_message=(message_t *)malloc(sizeof(message_t));
   message_t *ret_message=(message_t *)malloc(sizeof(message_t));
  
   /* open a socket */
   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* clear client address structure and initialize with client address */
   memset(&client_addr, 0, sizeof(client_addr));
   client_addr.sin_family = AF_INET;
   client_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* This allows choice of
                                        any host interface, if more than one 
                                        are present */
   client_addr.sin_port = htons(client_port);
   
   /* bind the socket to the local client port */
   if (bind(sock_client, (struct sockaddr *) &client_addr,
                                    sizeof (client_addr)) < 0) {
      perror("Client: can't bind to local address\n");
      close(sock_client);
      exit(1);
   }

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
	   /*
	message_t tmp_message;
	FILE* file = fopen("Travel.txt", 'r');
	char line[STRING_SIZE];
	char *travel_info;
	
	while (fgets(line, sizeof(line), file)) {
		tmp_message.step_no=atoi(strtok(line, ' '));
		tmp_message.client_port_no=atoi(strtok(NULL, ' '));
		tmp_message.server_port_no=atoi(strtok(NULL, ' '));
		tmp_message.server_secret_no=atoi(strtok(NULL, ' '));
		tmp_message.server_secret_no=atoi(strtok(NULL, ' '));
	}
	fclose("Travel.txt");
	*/
	
	//step1	
   	/*Create message*/
	ret_message->step_no=htons(1);
	ret_message->client_port_no=htons(client_port);
	ret_message->server_port_no=htons(0);
	ret_message->server_secret_no=htons(0);
	ret_message->text=(char*)calloc(80,sizeof(char));
	ret_message->text="Message";
  
   	/* send message */
	int text_len=80;
   	send(sock_client, ret_message, sizeof(ret_message)+sizeof(char)*text_len, 0);

   	/* get response from server */
	rec_message->text=(char*)calloc(80,sizeof(char));
   	recv(sock_client, rec_message, sizeof(rec_message)+sizeof(char)*80, 0); 

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
	printf("Rec text: %s\n", rec_message->text);

   /* close the socket */
   close (sock_client);
}
