/*
 * NetBurst.c
 *
 *  Created on: May 10, 2009
 *      Author: root
 */


#include <stdlib.h>

#include "packetizer.h"
#include "sniffer.h"
#include "socket_options.h"
#include "syncapp.h"
#include "IP_Fragmenter.h";



void print_usage(void) {

	printf("\nusage: NetBurst [operation mode]");
	printf("\noperation mode: dump, sniff, packet, socket ");

}

void invoke_sniff(void){

	load_configuration();
	pcap_initilization();
	init_sniff();
	sniff();

}

void invoke_packetizer(void){
	//// if u want to send time stamp in each msg then uncomment following lines

	//	time_t current_time;
	//	current_time=time(NULL);
	//	sprintf(payload_data,"%d",current_time);
	//	payload_size=strlen(payload_data);
	load_configuration();
	load_payload_file();
	load_tcp_file();
	load_tcp_option_file();
	load_ip_option_file();
	load_ipv4_file();
	burst_data();

//	print_data();
}

int main(int argc, char *argv[]) {

	int option = -1;
	if(argc < 2)
	{
		print_usage();
		exit(1);
	}
	else
	{
		if(strcmp(argv[1], 			"-packet") == 0)
			option = 0;
		else if(strcmp(argv[1], 	"-sniff") == 0)
			option = 1;
		else if(strcmp(argv[1], 	"-dump") == 0)
			option = 2;
		else if(strcmp(argv[1], 	"-socket") == 0)
			option = 3;
		else if(strcmp(argv[1], 	"-client") == 0)
			option = 4;
		else if(strcmp(argv[1], 	"-server") == 0)
					option = 5;
		else if(strcmp(argv[1], 	"-fragment") == 0)
							option = 6;
	}

	switch(option)
	{

	case 0:	// packetizer

			invoke_packetizer();
			break;


	case 1:	// sniff to console

			invoke_sniff();
			break;


	case 2:	// dump in file

			pcap_initilization();
			init_dump();
			dump();
			break;


	case 3:	// show socket options

			print_socket_option();
			break;


	case 4://Clent
			if(run_client())
			{
				char *buff= (char*)malloc(sizeof(char)*6);
				invoke_sniff();
				sprintf(buff, "%d", recived_packet_count());
				write_data_on_scoket(buff);
				close_client();
			}
			else
			{
				printf("\nError: client failed.");
				exit(1);
			}
			break;

	case 5://Server
			if(run_server())
			{

				invoke_packetizer();
				int rcv= atoi(read_data());
				printf("\nData from clent: %d",rcv);
				int fir_msg= get_fired_msgs();
				int Packet_loss=fir_msg-rcv;
				printf("\nMessage Loss: %d",Packet_loss  );
				close_server();
			}
			else
			{
				printf("\nError: server failed.");
				exit(1);
			}
			break;

	case 6://fragment
		Fragmenter(1880);
		break;
	default:

			printf("\n\nInvalid arguement\n");
			print_usage();
			break;
	}

	return EXIT_SUCCESS;
}

