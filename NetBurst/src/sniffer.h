/*
 * sniffer.h
 *
 *  Created on: May 10, 2009
 *      Author: root
 */

#ifndef SNIFFER_H_
#define SNIFFER_H_




#include <pcap.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>


#define MAX_PAC_LEN 1500
#define ETHERNET_HEADER_SIZE 14
#define ETHERBET_ADDRESS_LENGTH	6

#define IP_HL(ip)	(((ip)->ip_version) & 0x0f)
#define IP_V(ip)    (((ip)->ip_version) >> 4)

typedef u_int tcp_seq;




//////////////////////////////////////////////////////////////////////////////////////
//						headers														//
//////////////////////////////////////////////////////////////////////////////////////

	struct ethernet_header_format {
			u_char  destination_mac[ETHERBET_ADDRESS_LENGTH];    /* destination host address */
			u_char  source_mac[ETHERBET_ADDRESS_LENGTH];    /* source host address */
			u_short protocol_name;                     /* IP? ARP? RARP? etc */
	};

	struct ip_header_format {
			u_char  ip_version;                 /* version << 4 | header length >> 2 */
			u_char  ip_tos;                 /* type of service */
			u_short ip_header_len;                 /* total length */
			u_short ip_id;                  /* identification */
			u_short ip_off;                 /* fragment offset field */
			#define IP_RF 0x8000            /* reserved fragment flag */
			#define IP_DF 0x4000            /* dont fragment flag */
			#define IP_MF 0x2000            /* more fragments flag */
			#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
			u_char  ip_ttl;                 /* time to live */
			u_char  ip_protocol;                   /* protocol */
			u_short ip_checksum;                 /* checksum */
			struct  in_addr ip_src,ip_dst;  /* source and dest address */
	};


	struct tcp_header_format {
			u_short tcp_source_port;               /* source port */
			u_short tcp_destination_port;               /* destination port */
			tcp_seq tcp_seq_no;                 /* sequence number */
			tcp_seq tcp_ack_no;                 /* acknowledgement number */
			u_char  tcp_data_offset;               /* data offset, rsvd */
			#define TH_OFF(th)      (((th)->tcp_data_offset & 0xf0) >> 4)
			u_char  tcp_control_flags;
			#define TH_FIN  0x01
			#define TH_SYN  0x02
			#define TH_RST  0x04
			#define TH_PUSH 0x08
			#define TH_ACK  0x10
			#define TH_URG  0x20
			#define TH_ECE  0x40
			#define TH_CWR  0x80
			#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
			u_short tcp_window_size;                 /* window */
			u_short tcp_cheksum;                 /* checksum */
			u_short tcp_urgent;                 /* urgent pointer */
	};


//////////////////////////////////////////////////////////////////////////////////////
//						NetBurst sniffer functions									//														//
//////////////////////////////////////////////////////////////////////////////////////


	void packet_graber(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
	void pcap_initilization(void);
	void packet_dumper(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
	void dump(void);
	void init_dump(void);
	void sniff(void);
	void init_sniff(void);



//////////////////////////////////////////////////////////////////////////////////////
//						miscellaneous functions									//														//
//////////////////////////////////////////////////////////////////////////////////////


	void save_latency(void);
	int recived_packet_count();


#endif /* SNIFFER_H_ */
