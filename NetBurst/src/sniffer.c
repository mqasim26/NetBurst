/*
 ============================================================================
 Name        : SaveDump.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <time.h>
#include "sniffer.h"


char *device="eth0";
char pcap_errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
char filter_exp[]="ip";

pcap_if_t *DeviceList;
pcap_if_t *Selected_Device;

pcap_t *pcap_handel;				/* packet capture handle */
pcap_dumper_t *dumpfile;
bpf_u_int32 device_subnet_mask;			/* subnet mask */
bpf_u_int32 device_ip;				/* ip */
struct bpf_program fp;

char packet_dump_file[FILENAME_MAX]="config/NetBurst.pcap";
char latency_file[FILENAME_MAX]="config/latency.txt";

int latency[400000];
int num_packets=0;

static int count = 0;


time_t stop_snifing_time;
time_t now;
int sniffin_duration=10;


void pcap_initilization(void)
{

	int Interface_Number=0;
	int inum;
	if (pcap_findalldevs(&DeviceList, pcap_errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", pcap_errbuf);
		exit(1);
	}

    for(Selected_Device=DeviceList; Selected_Device; Selected_Device=Selected_Device->next)
    {
        printf("%d. %s", ++Interface_Number, DeviceList->name);
        if (Selected_Device->description)
            printf(" (%s)\n", Selected_Device->description);
        else
            printf(" (No description available)\n");
    }

    if(Interface_Number==0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return -1;
    }
    printf("Enter the interface number (1-%d):",Interface_Number);
    scanf("%d", &inum);

    if(inum < 1 || inum > Interface_Number)
    {
        printf("\nInterface number out of range.\n");
        /* Free the device list */
        pcap_freealldevs(DeviceList);
        return -1;
    }

	/* Jump to the selected adapter */
    for(Selected_Device=DeviceList, Interface_Number=0; Interface_Number< inum-1 ;Selected_Device=Selected_Device->next, Interface_Number++);


}

void init_sniff(void)
{
	if (pcap_lookupnet(Selected_Device->name,&device_subnet_mask,&device_ip,pcap_errbuf)==-1)
    	{
    		printf(stderr, "Couldn't get netmask for device %s: %s\n",Selected_Device->name, pcap_errbuf);
    				device_ip = 0;
    				device_subnet_mask = 0;
    	}

    	pcap_handel=pcap_open_live(Selected_Device->name,MAX_PAC_LEN,1,1000,pcap_errbuf);
    	if(pcap_handel==NULL)
    	{
    		fprintf(stderr, "Couldn't open device %s: %s\n", Selected_Device->name, pcap_errbuf);
    		exit(EXIT_FAILURE);
    	}

    	if(pcap_compile(pcap_handel,&fp,filter_exp,0,device_subnet_mask)==-1)

    	{
    		fprintf(stderr,"Could not parse filter %s: %s\n",filter_exp,pcap_geterr(pcap_handel));
    		exit(EXIT_FAILURE);
    	}

    	if(pcap_setfilter(pcap_handel,&fp)==-1)
    	{
    		fprintf(stderr,"Could not install filter %s: %s\n",filter_exp,pcap_geterr(pcap_handel));
    		exit(EXIT_FAILURE);
    	}

    	if(pcap_datalink(pcap_handel)!=DLT_EN10MB)
    	{
    		fprintf(stderr, "Error:  %s is not an Ethernet device \n", Selected_Device->name);
    		exit(EXIT_FAILURE);
    	}

}

void init_dump(void)
{
   	if ((pcap_handel= pcap_open_live(Selected_Device->name,
										 65536,			// portion of the packet to capture.
    													// 65536 grants that the whole packet will be captured on all the MACs.
    									 1,				// promiscuous mode (nonzero means promiscuous)
    									 1000,			// read timeout
    									 pcap_errbuf	// error buffer
    									 )) == NULL)
    			{
    				fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", Selected_Device->name);
    				/* Free the device list */
    				pcap_freealldevs(DeviceList);
    				return -1;
    			}
    	dumpfile = pcap_dump_open(pcap_handel, packet_dump_file);
    	if(dumpfile==NULL)
    	{
    		fprintf(stderr,"\nError opening output file\n");
    		return -1;
    	}

    	 printf("\nlistening on %s... Press Ctrl+C to stop...\n", Selected_Device->description);

    	    /* At this point, we no longer need the device list. Free it */
    	    pcap_freealldevs(DeviceList);


}

void dump(void)
{
		sniffin_duration=get_execution_time();
		now=time(NULL);
		stop_snifing_time=now+sniffin_duration;
		pcap_loop(pcap_handel, 0, packet_dumper, (unsigned char *)dumpfile);
		pcap_close(pcap_handel);
}


void sniff(void)
{
		printf("\nPacket#\t\tFrom:\t\tTo:\t\tProtocol:\tSize:\tSrc port:\tDst port:\t Capture");
		sniffin_duration=get_execution_time();
		now=time(NULL);
		stop_snifing_time=now+sniffin_duration;
		pcap_loop(pcap_handel,num_packets,packet_graber,NULL);
		pcap_close(pcap_handel);
		save_latency();
}

void packet_dumper(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	now=time(NULL);
		if(now>=stop_snifing_time)
		{
			pcap_breakloop(pcap_handel);
		}
	pcap_dump(dumpfile, header, pkt_data);
}


void packet_graber(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

	now=time(NULL);
	if(now>=stop_snifing_time)
	{
		pcap_breakloop(pcap_handel);
	}

	int diff;
	//static int count = 1; /* packet counter */
	printf("\n");

	const struct ethernet_header_format *ethernet;
	const struct ip_header_format *ip;
	const struct tcp_header_format *tcp;
	const char *payload;

	int size_ip;
	int size_tcp;
	int size_payload;


	// Print Packet Number
	printf("\n%d:\t", count);
	count++;

	/* define ethernet header */
	ethernet = (struct ethernet_header_format*)(packet);
	ip = (struct ip_header_format*)(packet + ETHERNET_HEADER_SIZE);
		size_ip = IP_HL(ip)*4;
		if (size_ip < 20) {
			printf("   * Invalid IP header length: %u bytes\n", size_ip);
			return;
		}
		/* print source and destination IP addresses */
			printf("\t%s", inet_ntoa(ip->ip_src));
			printf("\t%s", inet_ntoa(ip->ip_dst));
			switch(ip->ip_protocol)
			{
				case IPPROTO_TCP:
						printf("\t TCP");
						break;
					case IPPROTO_UDP:
						printf("\t UDP");
						return;
					case IPPROTO_ICMP:
					printf("\t ICMP");
					return;
				case IPPROTO_IP:
					printf("\t IP");
					return;
				default:
					printf("\t unknown");
					return;

		}
		tcp = (struct tcp_header_format*)(packet + ETHERNET_HEADER_SIZE + size_ip);
			size_tcp = TH_OFF(tcp)*4;

			//print Packet Size
			printf("\t\t%d",size_tcp);

			if (size_tcp < 20) {
				printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
				return;
			}
			//Print Src port
			printf("\t%d\t", ntohs(tcp->tcp_source_port));
			//Print Dst port
			printf(" \t%d", ntohs(tcp->tcp_destination_port));

			payload= (u_char *) (packet + ETHERNET_HEADER_SIZE +size_ip + size_tcp);

			size_payload=ntohs(ip->ip_header_len)-(size_ip+size_tcp);
			if (size_payload > 0)
			{
				diff= atoi(payload);
				diff= header->ts.tv_sec-diff;
				// Print Latency
				//printf("\t\t%d ",diff);
				//Print Time Stamsp
				printf("\t\t%d \n",header->ts.tv_sec);
				latency[count]=diff;
			}
			else
			{
				printf("\n");
			}
}


int recived_packet_count()
{
	return count;
}


void save_latency(void)
{
	int x;
	FILE *fp;

	fp=fopen(latency_file,"w");

	if(fp!=NULL)
	{
			printf("Printing Data in file");
			fprintf(fp,"--------------------- Latency ------------------------------------\n\n");
			fprintf(fp,"Packet Number: %d\t\t\t\t\t Latency: %d ");
			fprintf(fp,"\n");
			for (x=1; x<count; x++)
				fprintf(fp,"\n%d,%d ",x,latency[x]);

	}
	else
	{
		printf("File Open Fail");
	}
	printf("Now closing File");
	fclose(fp);
}
