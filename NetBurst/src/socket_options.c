/*
 * socket_options.c
 *
 *  Created on: May 10, 2009
 *      Author: root
 */
#include "socket_options.h"

void print_socket_option(void)
{
	int Err;
	printf("Options\t\t\tValue\t\tDescription\n");
	printf("____________________________________________________________________________________________________________________");
	struct sockaddr_in server_soc;
	int s;
	s=socket(AF_INET,SOCK_STREAM,0);


	int SO_DEBUG_valeu;
	int SO_DEBUG_len = sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_DEBUG,&SO_DEBUG_valeu,&SO_DEBUG_len);
	if(Err==0)
	{
		printf("\nSO_DEBUG:\t\t| %d\t\tReports whether debugging information is being recorded. A Boolean option. ",SO_DEBUG_valeu);
	}
	else
	{
		printf("\nError in Socket Option\t%d ",Err);
	}

	int SO_ACCEPTCONN_val;
	int SO_ACCEPTCONN_len = sizeof(int);;

	Err=getsockopt(s,SOL_SOCKET,SO_ACCEPTCONN,&SO_ACCEPTCONN_val,&SO_ACCEPTCONN_len);
	if(Err==0)
		{
		printf("\nSO_ACCEPTCONN:\t\t| %d\t\tReports whether socket listening is enabled. A Boolean option.",SO_ACCEPTCONN_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}


	int SO_BROADCAST_val;
	int SO_BROADCAST_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_BROADCAST,&SO_BROADCAST_val,&SO_BROADCAST_len);
	if(Err==0)
		{
		printf("\nSO_BROADCAST:\t\t| %d\t\tReports whether transmission of broadcast messages is supported.A Boolean option. ",SO_BROADCAST_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}


	int SO_REUSEADDR_val;
	int SO_REUSEADDR_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_REUSEADDR,&SO_REUSEADDR_val,&SO_REUSEADDR_len);
	if(Err==0)
		{
		printf("\nSO_REUSEADDR:\t\t| %d\t\tReports whether the rules used in validating addresses supplied to bind(). A Boolean option. ",SO_REUSEADDR_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}


	int SO_KEEPALIVE_val;
	int SO_KEEPALIVE_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_KEEPALIVE,&SO_KEEPALIVE_val,&SO_KEEPALIVE_len);
	if(Err==0)
		{
		printf("\nSO_KEEPALIVE:\t\t| %d\t\tReports whether connections are kept active with periodic transmission of messages.A Boolean option. ",SO_KEEPALIVE_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}




	struct linger SO_LINGER_val;
	int SO_LINGER_len=sizeof(SO_LINGER_val);
	Err=getsockopt(s,SOL_SOCKET,SO_LINGER,&SO_LINGER_val,&SO_LINGER_len);
	if(Err==0)
		{
		printf("\nSO_LINGER(l_linger):\t| %d\t\tReports whether the socket lingers on close().until it can transmit the data or until the end of the interval indicated by the value",SO_LINGER_val.l_linger);
		printf("\nSO_LINGER(l_onoff):\t| %d\t\tThis value of l_onoff variable",SO_LINGER_val.l_onoff);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}




	int SO_OOBINLINE_val;
	int SO_OOBINLINE_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_OOBINLINE,&SO_OOBINLINE_val,&SO_OOBINLINE_len);
	if(Err==0)
		{
		printf("\nSO_OOBINLINE:\t\t| %d\t\tReports whether the socket leaves received out-of-band data. A Boolean option.",SO_OOBINLINE_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}



	int SO_SNDBUF_val;
	int 	SO_SNDBUF_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_SNDBUF,&SO_SNDBUF_val,&SO_SNDBUF_len);
	if(Err==0)
		{
		printf("\nSO_SNDBUF:\t\t| %d\t\tReports send buffer size information.",SO_SNDBUF_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}


	int SO_RCVBUF_val;
	int SO_RCVBUF_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_RCVBUF,&SO_RCVBUF_val,&SO_RCVBUF_len);
	if(Err==0)
		{
		printf("\nSO_RCVBUF:\t\t| %d\t\tReports receive buffer size information.",SO_RCVBUF_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}

	int SO_ERROR_val;
	int SO_ERROR_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_ERROR,&SO_ERROR_val,&SO_ERROR_len);
	if(Err==0)
		{
		printf("\nSO_ERROR:\t\t| %d\t\tReports information about error status and clears it. Int option",SO_ERROR_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}



	int SO_TYPE_val;
	int SO_TYPE_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_TYPE,&SO_TYPE_val,&SO_TYPE_len);
	if(Err==0)
		{
		printf("\nSO_TYPE:\t\t| %d\t\tReports the socket type.",SO_TYPE_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}




	int SO_DONTROUTE_val;
	int SO_DONTROUTE_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_DONTROUTE,&SO_DONTROUTE_val,&SO_DONTROUTE_len);
	if(Err==0)
		{
		printf("\nSO_DONTROUTE:\t\t| %d\t\tReports whether outgoing messages bypass the standard routing facilities. Int Option",SO_DONTROUTE_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}


	int SO_RCVLOWAT_val;
	int SO_RCVLOWAT_len=sizeof(int);

	getsockopt(s,SOL_SOCKET,SO_RCVLOWAT,&SO_RCVLOWAT_val,&SO_RCVLOWAT_len);
	if(Err==0)
		{
		printf("\nSO_RCVLOWAT:\t\t| %d\t\tReports the minimum number of bytes to process for socket input operations. The default value for SO_RCVLOWAT is 1.",SO_RCVLOWAT_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}




	struct timeval SO_RCVTIMEO_val;
	int SO_RCVTIMEO_len=sizeof(SO_RCVTIMEO_val);

	Err=getsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&SO_RCVTIMEO_val,&SO_RCVTIMEO_len);
	if(Err==0)
		{
		printf("\nSO_RCVTIMEO:\t\t| %d\t\tReports the timeout value for input operations. The default for this option is zero",SO_RCVTIMEO_val.tv_usec);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}




	int SO_SNDLOWAT_val;
	int SO_SNDLOWAT_len=sizeof(int);

	Err=getsockopt(s,SOL_SOCKET,SO_SNDLOWAT,&SO_SNDLOWAT_val,&SO_SNDLOWAT_len);
	if(Err==0)
		{
		printf("\nSO_SNDLOWAT:\t\t| %d\t\tReports the minimum number of bytes to process for socket output operations.",SO_SNDLOWAT_val);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}


	struct timeval SO_SNDTIMEO_val;
	int SO_SNDTIMEO_len=sizeof(SO_SNDTIMEO_val);

	getsockopt(s,SOL_SOCKET,SO_SNDTIMEO,&SO_SNDTIMEO_val,&SO_SNDTIMEO_len);
	if(Err==0)
		{
		printf("\nSO_SNDTIMEO:\t\t| %d\t\tReports the timeout value specifying the amount of time that an output function blocks because flow control prevents data from being sent. ",SO_SNDTIMEO_val.tv_sec);
		}
		else
		{
			printf("\nError in Socket Option\t%d ",Err);
		}

	printf("\n");
}
