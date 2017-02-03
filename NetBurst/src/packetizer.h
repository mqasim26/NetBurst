/*
 * packetizer.h
 *
 *  Created on: May 10, 2009
 *      Author: root
 */

#ifndef PACKETIZER_H_
#define PACKETIZER_H_


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libnet.h>


	struct tcp_node
	{
			u_short tcp_src_port;				/* TCP source port */
			u_short tcp_des_port;				/* TCP dest port */
			u_short tcp_win;					/* TCP window size */
			u_short tcp_urgent;					/* TCP urgent data pointer */
			u_long  tcp_ack;					/* TCP ack number */
			u_long  tcp_seq;					/* TCP sequence number */
			u_int8_t  tcp_control;				/* TCP control flages */
			struct tcp_node* next;
	};


	struct ip_node
	{
		u_long  ip_des_addr;			/* IP dest addr */
		u_long  ip_src_addr;			/* IP source addr */
		u_short ip_id;					/* IP id */
		u_short ip_frag;				/* IP frag */
		u_char  ip_tl;					/* IP TTL */
		u_int8_t  ip_ttos;
		struct ip_node* next;
	};


/////////////////////////////////////////////////////////////////////////////////////////
//							options for TCP/IP 										   //
/////////////////////////////////////////////////////////////////////////////////////////

		struct option_node
		{
				char optionsList[100];
				u_int32_t option_list_size;
				struct option_node* next;
		};


/////////////////////////////////////////////////////////////////////////////////////////
//							functions to load parameters from files					   //
/////////////////////////////////////////////////////////////////////////////////////////

	void load_configuration(void);
	void load_payload_file(void);
	void load_tcp_file();
	void load_tcp_option_file();
	void load_ipv4_file();
	void load_ipv4_option_file();
	void load_ip_option_file();


/////////////////////////////////////////////////////////////////////////////////////////
//							NetBurst packetizer functions							   //
/////////////////////////////////////////////////////////////////////////////////////////

	void init();
	void write_packet(void);
	void burst_data();

	u_int8_t convert_tcp_control(u_char  tcp_control[]);
	u_int8_t convert_ip_ttps(u_char  ip_ttos[]);


/////////////////////////////////////////////////////////////////////////////////////////
//							link list functions										   //
/////////////////////////////////////////////////////////////////////////////////////////

	void insert_ip_node (struct ip_node** head,u_long  des_addr,u_long  src_addr,u_short id,u_short frag,u_char  tl,u_int8_t  ttos);
	void insert_ip_options_node(struct option_node** head, char* optisonL, u_int32_t optionSize);
	void insert_tcp_node (struct tcp_node** head, u_short src_port, u_short des_port, u_short win, u_short urgent, u_long ack, u_long  seq, u_int8_t  control);
	void insert_tcp_options_node(struct option_node** head, u_int8_t[], u_int32_t optionSize);


/////////////////////////////////////////////////////////////////////////////////////////
//							miscellaneous functions									   //
/////////////////////////////////////////////////////////////////////////////////////////

	void destroy(void);
	void print_data();
	u_short get_src_port(void);
	char * get_netburst_host(void);
	int get_execution_time(void);
	int get_fired_msgs(void);
#endif /* PACKETIZER_H_ */

