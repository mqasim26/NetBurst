/*
 * IP_Fragmenter.c
 *
 *  Created on: May 16, 2009
 *      Author: root
 */

#include "IP_Fragmenter.h";


#define MTU 1500

libnet_t *libnet_fragmneter;	/* libnet context */
char *Frag_Device="eth0";

void Write_Fragment(u_int8_t *payload, u_int32_t total_pload_size);


void Fragmenter(int fragmneter_payload_size)
{
	int i;
	char *errorbuff[LIBNET_ERRBUF_SIZE];

	u_int8_t fragmnet_payload[4000];
	libnet_fragmneter=libnet_init(LIBNET_RAW4,Frag_Device,errorbuff);

	if ( libnet_fragmneter == NULL ) {
		fprintf(stderr, "ERROR: libnet_init failed, Fragmenter termnated: %s\n", \
				errorbuff);
		exit(EXIT_FAILURE);
	}

	/* Generating random payload */
	libnet_seed_prand (libnet_fragmneter);

	for (i = 0; i < fragmneter_payload_size; i++) {
		fragmnet_payload[i] = libnet_get_prand(LIBNET_PR8);
	}

	Write_Fragment(fragmnet_payload,fragmneter_payload_size);
	libnet_destroy(libnet_fragmneter);

}

void Write_Fragment(u_int8_t *payload, u_int32_t total_pload_size)
{
	char dst_ip_addr_str[16];
	u_int32_t dst_addr, src_addr;
	u_int16_t id, seq, ip_id;
	int pload_offset, hdr_offset;
	int bytes_written, max_pload_size, packet_pload_size;
	libnet_ptag_t ip_tag;

	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);
	/* We need a non-zero id number for the IP headers, otherwise libnet
	 * will increase it after each build_ipv4, breaking the fragments */
	ip_id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

	seq = 1;

	src_addr = libnet_get_ipaddr4(libnet_fragmneter);
	if ( src_addr == -1 ) {
		fprintf(stderr, "Couldn't get own IP address: %s\n", \
				libnet_geterror(libnet_fragmneter));
		libnet_destroy(libnet_fragmneter);
		exit(EXIT_FAILURE);
	}

	printf("\n Please Enter Destination IP address: ");
	scanf("%s",dst_ip_addr_str);

	dst_addr=libnet_name2addr4(libnet_fragmneter, dst_ip_addr_str, LIBNET_DONT_RESOLVE);

	if ( dst_addr == -1 ) {
			fprintf(stderr, "Error converting IP address.\n");
			libnet_destroy(libnet_fragmneter);
			exit(EXIT_FAILURE);
		}

	max_pload_size = (MTU - LIBNET_IPV4_H);
	max_pload_size -= (max_pload_size % 8);
	pload_offset = 0;

	if ( total_pload_size > (max_pload_size - LIBNET_ICMPV4_ECHO_H) ) {
			hdr_offset = IP_MF;
			packet_pload_size = max_pload_size - LIBNET_ICMPV4_ECHO_H;
		}
		else {
			hdr_offset = 0;
			packet_pload_size = total_pload_size;
		}

	if (
			libnet_build_icmpv4_echo(ICMP_ECHO,
									0,
									Calculate_Checksum(payload,total_pload_size, id, seq),
									id,
									seq,
									payload,
									packet_pload_size,
									libnet_fragmneter,
									0) == -1 )
		{
			fprintf(stderr, "Error building ICMP header: %s\n", \
					libnet_geterror(libnet_fragmneter));
			libnet_destroy(libnet_fragmneter);
			exit(EXIT_FAILURE);
		}


	if ( libnet_build_ipv4(
							(LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + packet_pload_size),
							0,
							ip_id,
							hdr_offset,
							255,
							IPPROTO_ICMP,
							0,
							src_addr,
							dst_addr,
							NULL,
							0,
							libnet_fragmneter,
							0) == -1 )
		{
			fprintf(stderr, "Error building IP header: %s\n", \
					libnet_geterror(libnet_fragmneter));
			libnet_destroy(libnet_fragmneter);
			exit(EXIT_FAILURE);
		}

	bytes_written = libnet_write(libnet_fragmneter);
	if ( bytes_written != -1 )
		printf("%d bytes written.\n", bytes_written);
	else
		fprintf(stderr, "Error writing packet: %s\n", \
					libnet_geterror(libnet_fragmneter));

	/* Updating the offset */
	pload_offset += packet_pload_size;

	/* Clearing
	 *
	 * We need to get rid of the ICMP header to build the other
	 * fragments
	 */
	libnet_clear_packet(libnet_fragmneter);







	ip_tag = LIBNET_PTAG_INITIALIZER;
	while ( total_pload_size > pload_offset ) {
		if ( (total_pload_size - pload_offset) > max_pload_size ) {
			hdr_offset = IP_MF + (pload_offset + \
					LIBNET_ICMPV4_ECHO_H)/8;
			packet_pload_size = max_pload_size;
		}
		else {
			hdr_offset = (pload_offset +  LIBNET_ICMPV4_ECHO_H)/8;
			packet_pload_size = total_pload_size - pload_offset;
		}

		ip_tag = libnet_build_ipv4(
								(LIBNET_IPV4_H + max_pload_size),
								0,
								ip_id,
								hdr_offset,
								255,
								IPPROTO_ICMP,
								0,
								src_addr,
								dst_addr,
								(payload + pload_offset),
								packet_pload_size,
								libnet_fragmneter,
								ip_tag);

		if ( ip_tag == -1 ) {
			fprintf(stderr, "Error building IP header: %s\n", \
					libnet_geterror(libnet_fragmneter));
			libnet_destroy(libnet_fragmneter);
			exit(EXIT_FAILURE);
		}


		/* Writing packet */

		bytes_written = libnet_write(libnet_fragmneter);

		if ( bytes_written != -1 )
			printf("%d bytes written.\n", bytes_written);
		else
			fprintf(stderr, "Error writing packet: %s\n", \
					libnet_geterror(libnet_fragmneter));

		/* Updating the offset */
		pload_offset += packet_pload_size;
	}



}

u_int16_t Calculate_Checksum(u_int8_t *payload, u_int32_t total_pload_size,u_int16_t id, u_int16_t seq)
{
	char *errbuf[LIBNET_ERRBUF_SIZE];
	libnet_ptag_t icmp_tag;
	u_int8_t *packet;
	u_int32_t packet_size;
	u_int16_t *sum_p, sum;
	u_int8_t dummy_dst[6] = {0, 0, 0, 0, 0, 0};
	icmp_tag = LIBNET_PTAG_INITIALIZER;

	/* Switching to advanced link mode */
	/* Nothing should be built yet and all random numbers should be already
	 * generated. */
	libnet_destroy(libnet_fragmneter);

	libnet_fragmneter = libnet_init(LIBNET_LINK_ADV, Frag_Device, errbuf);

	if ( libnet_fragmneter == NULL ) {
		fprintf(stderr, "libnet_init() failed: Error in calculating fragmnet checksum : %s\n",errbuf);
		exit(EXIT_FAILURE);
	}

	icmp_tag = libnet_build_icmpv4_echo(ICMP_ECHO,
										0,
										0,
										id,
										seq,
										payload,
										total_pload_size,
										libnet_fragmneter,
										icmp_tag);

	if ( icmp_tag == -1 ) {

		fprintf(stderr, "Error building ICMP header: %s\n",libnet_geterror(libnet_fragmneter));
		libnet_destroy(libnet_fragmneter);
		exit(EXIT_FAILURE);
	}

	if ( libnet_autobuild_ipv4(
							  (LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + total_pload_size),
							  IPPROTO_ICMP,
							  0,
							  libnet_fragmneter) == -1 ) {

		fprintf(stderr,	"Error building dummy IP header: %s\n", libnet_geterror(libnet_fragmneter));
		libnet_destroy(libnet_fragmneter);
		exit(EXIT_FAILURE);
	}


	if ( libnet_autobuild_ethernet (
			dummy_dst,
			ETHERTYPE_IP,
			libnet_fragmneter) == -1 ) {
		fprintf(stderr,	"Error building dummy Ethernet header: %s\n", \
				libnet_geterror(libnet_fragmneter));
		libnet_destroy(libnet_fragmneter);
		exit(EXIT_FAILURE);
	}


	if (libnet_adv_cull_packet(libnet_fragmneter, &packet, &packet_size) == -1) {
		fprintf(stderr, "Error pulling the packet: %s\n", libnet_geterror(libnet_fragmneter));
		libnet_destroy(libnet_fragmneter);
		exit(EXIT_FAILURE);
	}

	sum_p = (u_int16_t*)(packet + 36);
	sum = ntohs(*sum_p);
	libnet_adv_free_packet(libnet_fragmneter, packet);
	libnet_clear_packet(libnet_fragmneter);
	libnet_destroy(libnet_fragmneter);
	libnet_fragmneter = libnet_init(LIBNET_RAW4, Frag_Device, errbuf);
	if ( libnet_fragmneter == NULL ) {
		fprintf(stderr, "libnet_init() failed (raw4, 2nd call): %s\n", errbuf);
		exit(EXIT_FAILURE);
	}
	return sum;
}
