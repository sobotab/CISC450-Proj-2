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
   int bytes_recd;
   unsigned short server_port=46238;  /* iPort number used by server (remote port) */
   unsigned short client_port=CLNT_TCP_PORT; /* Port number used by client (localort)*/
   unsigned short int step_no=1;
   unsigned short int server_port_no=48000;
   unsigned short int server_secret_no=0;

   struct timeval timeout;
   timeout.tv_sec=1;

   //Message information to be used in this application
   message_t *rec_message=(message_t *)malloc(sizeof(message_t));
   message_t *ret_message=(message_t *)malloc(sizeof(message_t));
  
   int done=0;

   /* open a socket */
   for(; server_port_no<48999; server_port_no++){
	FILE* travel_log1 = fopen("Travel.txt", "r");
        char line1[STRING_SIZE];

        while (fgets(line1, sizeof(line1), travel_log1)) {
              step_no=atoi(strtok(line1, ","));
              if(server_port_no == atoi(strtok(NULL, ",")) && step_no <= 3) {
		      if (step_no == 3) {
			      done=1;
		      }
		      step_no++;
		      server_secret_no = atoi(strtok(NULL, ","));
	      	      break;
		} else {
			step_no=1;
			server_secret_no=0;
		}
        }
	if(done) {
		done=0;
		continue;
	}
	fclose(travel_log1);

   	if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      		perror("Client: can't open stream socket");
      		exit(1);
   	}

	setsockopt(sock_client, SOL_SOCKET, SO_RCVTIMEO, (const void*) &timeout,
			sizeof(timeout));

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

	
   	/* connect to the server */
   	server_addr.sin_port = htons(server_port_no);

   	if (connect(sock_client, (struct sockaddr *) &server_addr, 
        	                            sizeof (server_addr)) < 0) {
      		close(sock_client);
      		continue;
   	}
		
   	/*Create message*/
	ret_message->client_port_no=htons(client_port);
	if(step_no == 1) {
		ret_message->step_no=1;
		ret_message->server_port_no=0;
		ret_message->server_secret_no=0;
		strcpy(ret_message->text, "*");
	} else if (step_no == 2) {
		ret_message->step_no=2;
		ret_message->server_port_no=server_port_no;
		ret_message->server_secret_no=0;
		strcpy(ret_message->text, "*");
	} else if (step_no == 3) {
		ret_message->step_no=3;
		ret_message->server_port_no=server_port_no;
		ret_message->server_secret_no=server_secret_no;
		strcpy(ret_message->text, "sobota-stahl");
	}
	ret_message->step_no=htons(ret_message->step_no);
	ret_message->server_port_no=htons(ret_message->server_port_no);
	ret_message->server_secret_no=htons(ret_message->server_secret_no);

   	/* send message */
   	send(sock_client, ret_message, sizeof(message_t), 0);

   	/* get response from server */
   	bytes_recd=recv(sock_client, rec_message, sizeof(message_t), 0); 

   	/*Convert message from network to host*/
	if(bytes_recd>0) {
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

	FILE* travel_log = fopen("Travel.txt", "r");
      	FILE* tmp_travel_log = fopen("tempTravel.txt", "a");
      	char line[STRING_SIZE];
      	char new_line[STRING_SIZE];
	char error_line[STRING_SIZE];
	unsigned short int tmp_server_port=0;
      	new_line[0]='\0';

      	//Saves visitor info and keeps old info as well
      	while (fgets(line, sizeof(line), travel_log)) {
	      strcat(new_line, line);
              strtok(new_line, ",");
              tmp_server_port=atoi(strtok(NULL, ","));
              if (tmp_server_port != rec_message->server_port_no) {
                      printf("server that already exists: %s\n", line);
		      fprintf(tmp_travel_log, "%s", line);
              } else {
		      strcpy(error_line, line);
	      }
      	}
	memset(new_line, '\0', STRING_SIZE);
	char tmp_str[STRING_SIZE];
	if (step_no == rec_message->step_no) {
             	memset(tmp_str, '\0', STRING_SIZE);
		printf("step no: %d\n", rec_message->step_no);
             	sprintf(tmp_str, "%d", rec_message->step_no);
             	strcat(new_line, tmp_str);
             	strcat(new_line, ",");
             	memset(tmp_str, '\0', STRING_SIZE);
             	sprintf(tmp_str, "%d", rec_message->server_port_no);
             	strcat(new_line, tmp_str);
		strcat(new_line, ",");
		printf("new_line half way: %s\n", new_line);
		memset(tmp_str, '\0', STRING_SIZE);
		sprintf(tmp_str, "%d", rec_message->server_secret_no);
		strcat(new_line, tmp_str);
             	strcat(new_line, ",");
             	strcat(new_line, rec_message->text);
             	strcat(new_line, "\n");
		printf("new_line: %s\n", new_line);
		fprintf(tmp_travel_log, "%s", new_line);
	} else {
		fprintf(tmp_travel_log, "%s", error_line);
	}
	fclose(travel_log);
     	fclose(tmp_travel_log);
     	system("cp ./tempTravel.txt ./Travel.txt");
     	system("> ./tempTravel.txt");
	}
	close(sock_client);
    }
}
