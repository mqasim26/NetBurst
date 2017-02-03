/*
 * syncapp.c
 *
 *  Created on: May 10, 2009
 *      Author: root
 */


#include "syncapp.h"


int newsockfd;
int client_sockfd;
int server_sockfd;
int run_server(void)
{
	     int portno, clilen;
	     struct sockaddr_in serv_addr, cli_addr;

	     portno = (int) get_src_port();
	     server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	     if (server_sockfd < 0) {
	        printf("\nERROR: can't open socket\n Error Code: %d", server_sockfd);
	        return 0;
	     }

	     bzero((char *) &serv_addr, sizeof(serv_addr));

	     serv_addr.sin_family = AF_INET;
	     serv_addr.sin_addr.s_addr = INADDR_ANY;
	     serv_addr.sin_port = htons(portno);

	     if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	    	 printf("\nERROR: can't bind on port %d", portno);
	    	 return 0;
	     }

	     listen(server_sockfd, 1);

	     clilen = sizeof(cli_addr);

	     newsockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);

	     if (newsockfd < 0) {
	    	 printf("\nERROR: Connection Closed, can't accept client.");
	         return 0;
	     }

	     return 1;
}



char* read_data(void)
{

	char buffer[256];
	int n;

	 bzero(buffer,256);

     n = read(newsockfd, buffer, 255);

     if (n < 0)
     {
    	 printf("Warning: there is no data on socket.");
    	 return NULL;
     }

     return buffer;

}

void close_server(void)
{
	close(server_sockfd);
	close(newsockfd);
}
int run_client(void)
{


    struct sockaddr_in serv_addr;
    struct hostent *server;
    int  portno=(int)get_src_port();
    char *hostname=get_netburst_host();


    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_sockfd < 0)
    {
    	printf("ERROR opening socket");
    	return 0;
    }
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    	return 0;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(client_sockfd,&serv_addr,sizeof(serv_addr)) < 0)
    {
    	printf("ERROR: can not connect wirth host");
    	return 0;
    }

    return 1;

}

void write_data_on_scoket(char* buff)
{


		int n;
	    n = write(client_sockfd,buff,strlen(buff));
	    if (n < 0)
	    {
	         printf("ERROR writing to socket");
	         return 0;
	    }
}
void close_client(void)
{
	close(client_sockfd);
}
