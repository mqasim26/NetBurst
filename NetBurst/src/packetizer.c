/*
 ============================================================================
 Name        : Net6.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "packetizer.h"



/////////////////////////////////////////////////////////////////////////////////////////
//							Global variable declaration								   //
/////////////////////////////////////////////////////////////////////////////////////////

	struct tcp_node* head_tcp = NULL;
	struct tcp_node* fire_tcp = NULL;
	struct option_node* head_tcp_options=NULL;
	struct option_node* fire_tcp_options=NULL;

	struct ip_node* head_ip = NULL;
	struct ip_node* fire_ip = NULL;
	struct option_node* head_ip_options=NULL;
	struct option_node* fire_ip_options=NULL;
	u_int8_t optList[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

	int MAX_LOAD_SIZE=500;
	int Rand_List_size=10;
    int Ran_payload_len[10];
    int Random_index =0;


    u_short head_type;

    u_int32_t dst_addr, src_addr;
	struct libnet_in6_addr ip6_src_add;
	struct libnet_in6_addr ip6_des_add;
	char ip_addr_str[16];
	char ip_addr2_str[16];
	time_t curr_time;
	time_t end_time;

	char *Config_Host=NULL;
	int Config_Ex_Time=10;
	int Config_Interval = 0;
	int Config_Count = 0;
	int Config_IP_Ver=4;
	int Config_Random=0;
	int Config_Cold=0;
	int Config_Proto=0;
	int IP_LIST_SIZE=0;
	int TCP_LIST_SIZE=0;
	int TCP_OPTION_LIST_SIZE=0;
	int IP_OPTION_LIST_SIZE=0;

	libnet_t *libnet_handle;
	libnet_ptag_t libnet_tag_handle;
    char *libnet_errbuf[LIBNET_ERRBUF_SIZE];
    char *Device="eth0";

    int Rpt_Data=0;
    int Rpt_Messages=0;

	int payload_size = 0;
	char *payload_data;
	char payload[1500];

	char config_file[FILENAME_MAX]		= "config/config.opt";
	char payload_file[FILENAME_MAX]		= "config/payload.txt";
	char tcp_file[FILENAME_MAX]			= "config/tcp.opt";
	char tcp_options_file[FILENAME_MAX]	= "config/tcp_options.opt";
	char ipv4_file[FILENAME_MAX]		= "config/ipv4.opt";
	char ip_options_file[FILENAME_MAX]	= "config/ip_options.opt";
	u_char  ip_ttos[90];				/* IP TOS string */
	u_char  	tcp_control_char[65];	/* TCP control flages */
    u_int8_t 	ip_ttos_val = 0;




/////////////////////////////////////////////////////////////////////////////////////////
//							Function Implementation									   //
/////////////////////////////////////////////////////////////////////////////////////////


u_short get_src_port(void)
{
	if(head_tcp == NULL)
		load_tcp_file();

	return head_tcp->tcp_src_port;
}


char* get_netburst_host(void)
{
	if(Config_Host == NULL)
		load_configuration();

	return Config_Host;
}

int get_execution_time(void)
{
	if(Config_Ex_Time == NULL)
			load_configuration();
		return Config_Ex_Time;

}




void destroy (){
	free(payload);
	free(payload_data);
	free(head_ip);
	free(fire_ip);

	free(head_ip_options);
	free(fire_ip_options);

	free(head_tcp);
	free(fire_tcp );

	free(head_tcp_options);
	free(fire_tcp_options);

	libnet_destroy(libnet_handle);
}







void burst_data() {

	int counter=0;

	fire_tcp=head_tcp;
	fire_tcp_options=head_tcp_options;
	fire_ip=head_ip;
	fire_ip_options=head_ip_options;
	init();
	curr_time = time(NULL);
	end_time=curr_time+Config_Ex_Time;
	printf("\n -----Test Secanrio------");
	char *hwsrc= "00:15:C5:D1:1B:B7";
    char *hwdst ="00:0D:56:F3:58:CA";

	if(Config_Cold==0) printf("\n Cache type: Warm Cache");
	else if (Config_Cold!=0) printf("\n Cache type: Cold Cache");
	printf("\n Execution Time %d Second \n Will Fire %d Message Every %d Second",Config_Ex_Time,Config_Count,Config_Interval);
	//printf("\n Will Fire %d Message Every %d Second",Config_Count,Config_Interval);
	while(curr_time<end_time)
		{
				for(counter=1;counter<=Config_Count;counter++)
					{
						write_packet();
						libnet_clear_packet(libnet_handle);
						if(Config_Cold!=0)
						{
						load_ipv4_file();
						load_tcp_file();
						}
						if(fire_tcp->next!=NULL)
								{
									fire_tcp=fire_tcp->next;
								}
							else
								{
									fire_tcp=head_tcp;
								}
							if(fire_ip->next!=NULL)
								{
									fire_ip=fire_ip->next;
								}
							else
								{
									fire_ip=head_ip;
								}
					}
				sleep(Config_Interval);
				curr_time = time(NULL);
		}
printf("\n ----------------Test Report---------------");
printf("\n Number of Packets :   %d",Rpt_Messages);
printf("\n Total Bytes Sent  :   %d",Rpt_Data);



}

void init()
{

	if(Config_Random!=0)
	{
	srand(MAX_LOAD_SIZE);
	int index = 1;
		for (index = 0 ; index <= Rand_List_size; index ++)
		{

			Ran_payload_len[index]=rand()%MAX_LOAD_SIZE;
		//	printf("Cnt    %d   ,  Rnd   %d\n",index,Ran_payload_len[index]);
		}


	}



	if(Config_IP_Ver==4)
   	{
        libnet_handle = libnet_init(LIBNET_RAW4,Device,libnet_errbuf);

        if (libnet_handle == NULL)
    		    {
    		        fprintf(stderr, "libnet_init() failed: %s", libnet_errbuf);
    		        exit(EXIT_FAILURE);
    		    }

    	printf("\nPlease Enter Destination IPv4 address: ");
        scanf("%s",ip_addr_str);

    src_addr=libnet_get_ipaddr4(libnet_handle);
    			    if ( src_addr == -1 )
    			    {
    						fprintf(stderr, "Couldn't get own IP address: %s\n", \
    						libnet_geterror(libnet_handle));
    						libnet_destroy(libnet_handle);
    						exit(EXIT_FAILURE);
    			    }
    dst_addr = libnet_name2addr4(libnet_handle, ip_addr_str, LIBNET_DONT_RESOLVE);
    			    if ( dst_addr == -1 )
    			    {
    					fprintf(stderr, "Error converting IP address.\n");
    					libnet_destroy(libnet_handle);
    					exit(EXIT_FAILURE);
    			    }

   	}
    if(Config_IP_Ver==6)
    {
        libnet_handle=libnet_init(LIBNET_RAW6,Device,libnet_errbuf);

        if (libnet_handle == NULL)
    		    {
    		        fprintf(stderr, "libnet_init() failed: %s", libnet_errbuf);
    		        exit(EXIT_FAILURE);
    		    }

    	printf("\nPlease Enter Source IPv6 address: ");
        scanf("%s",ip_addr_str);

        printf("\nPlease Enter Destination IPv6 address: ");
        scanf("%s",ip_addr2_str);

        ip6_src_add=libnet_name2addr6(libnet_handle,ip_addr_str,LIBNET_DONT_RESOLVE);
				if (strncmp((char*)&ip6_src_add,
                           (char*)&in6addr_error,sizeof(in6addr_error))==0){
                            fprintf(stderr, "Error in IPv6 Source Address: %s\n", ip_addr_str);
                            libnet_destroy(libnet_handle);
                            exit(EXIT_FAILURE);
				}


       ip6_des_add=libnet_name2addr6(libnet_handle,ip_addr2_str,LIBNET_DONT_RESOLVE);
				if (strncmp((char*)&ip6_des_add,
                                   (char*)&in6addr_error,sizeof(in6addr_error))==0){
                                    fprintf(stderr, "Error in IPv6 Destination Address: %s\n", ip_addr2_str);
                                    libnet_destroy(libnet_handle);
                                    exit(EXIT_FAILURE);
				}
    }


}


void write_packet(void)
{
	if(Config_Random!=0)
	{
		if(Ran_payload_len[Random_index]>0)
		{

		strncpy(payload,payload_data,Ran_payload_len[Random_index]);
		payload_size=Ran_payload_len[Random_index];
		}
		Random_index++;

		if(Rand_List_size == Random_index+1)
				{
					Random_index = 0;
				}
	}
	else
	{
		strcpy(payload,payload_data);
	}

	if(Config_Proto==1)
	{
			libnet_tag_handle = libnet_build_tcp_options(
						fire_tcp_options->optionsList,
						        fire_tcp_options->option_list_size,
						        libnet_handle,
						        0);

				if (libnet_tag_handle == -1)
						    {
					        fprintf(stderr, "Can't build TCP options: %s\n", libnet_geterror(libnet_handle));
						    }

			libnet_tag_handle = libnet_build_tcp
				(
							fire_tcp->tcp_src_port,
							fire_tcp->tcp_des_port,
							libnet_get_prand(LIBNET_PR32),//(Rpt_Messages+1),//fire_tcp->tcp_seq,                                         /* sequence number */
							libnet_get_prand(LIBNET_PRu16),          //1,//fire_tcp->tcp_ack,
							fire_tcp->tcp_control,
							fire_tcp->tcp_win,
					        0,
					        fire_tcp->tcp_urgent,
					        ( LIBNET_TCP_H + payload_size + 20 ),
					        payload,//Pay load
					        payload_size,//Pay load szie
					        libnet_handle,
					        0);
	if (libnet_tag_handle==-1)
				{
						 fprintf(stderr, "Can't build TCP Header: %s\n", libnet_geterror(libnet_handle));
						 exit(1);
				}
	head_type=LIBNET_TCP_H;

	}

	if(Config_Proto==0)
	{
        libnet_tag_handle = libnet_build_udp(
        fire_tcp->tcp_src_port,
        fire_tcp->tcp_des_port,
	    (LIBNET_UDP_H + payload_size),           /* packet length */
	    0,                                         /* checksum */
	    payload,                          /* payload */
	    payload_size,                          /* payload size */
	    libnet_handle,                                         /* libnet handle */
	    0);                                        /* libnet id */

	    if (libnet_tag_handle == -1)
	    {
	        fprintf(stderr, "Can't build UDP header: %s\n", libnet_geterror(libnet_handle));
	        exit(1);
	    }
	    head_type=LIBNET_UDP_H;
	}

	// Build TCP and IP V4

	if( (Config_IP_Ver==4) && (Config_Proto==1) )
	{
		libnet_tag_handle= libnet_build_ipv4(
								(LIBNET_IPV4_H +  LIBNET_TCP_H + payload_size + fire_tcp_options->option_list_size /* this 20 is 0-tion size*/),
								fire_ip->ip_ttos,
								Rpt_Messages,//fire_ip->ip_id,
								0,//htons(fire_ip->ip_frag),
								fire_ip->ip_tl,
								IPPROTO_TCP,
								0,
								src_addr,
								dst_addr,
								NULL,
								0,                                                     /* payload size */
								libnet_handle,                                                     /* libnet handle */
								0);

		if (libnet_tag_handle == -1)
			    {
		           fprintf(stderr, "Can't build IP header: %s\n", libnet_geterror(libnet_handle));
	           }
	}


	// Build UDP and IPV4
	if( (Config_IP_Ver==4) && (Config_Proto==0) )
	{
		libnet_tag_handle= libnet_build_ipv4(
								(LIBNET_IPV4_H +  LIBNET_UDP_H + payload_size/* this 20 is 0-tion size*/),
								fire_ip->ip_ttos,
								Rpt_Messages,//fire_ip->ip_id,
								0,//htons(fire_ip->ip_frag),
								fire_ip->ip_tl,
								IPPROTO_TCP,
								0,
								src_addr,
								dst_addr,
								NULL,
								0,                                                     /* payload size */
								libnet_handle,                                                     /* libnet handle */
								0);

		if (libnet_tag_handle == -1)
			    {
		           fprintf(stderr, "Can't build IP header: %s\n", libnet_geterror(libnet_handle));
	           }
	}
	if(Config_IP_Ver==6)
	{
		libnet_tag_handle=libnet_build_ipv6(
							0,				  		/*u_int8_t  traffic class */
							0,				  		/*u_int32_t flow label  */
							LIBNET_TCP_H,			/* total length of the IP packet*/
							IPPROTO_TCP,			/*u_int8_t next header  */
							255,					/*u_int8_t hop limit */
							ip6_src_add,			/* source IPv6 address */
							ip6_des_add,			/* destination IPv6 address*/
							NULL,					/* payload  */
							0,						/* payload*/
							libnet_handle,
							0);
		if (libnet_tag_handle == -1)
					    {
				           fprintf(stderr, "Can't build IPV6 header: %s\n", libnet_geterror(libnet_handle));
			           }
	}

        int byteCount;
		byteCount = libnet_write(libnet_handle);
				 if (byteCount == -1)
				    {
				        fprintf(stderr, "Write error: %s\n", libnet_geterror(libnet_handle));
				    }
				 else
				    {
				        Rpt_Data=Rpt_Data+byteCount;
				        Rpt_Messages++;
				    }



}



void load_configuration(void)
{

	FILE *infile;

	char ex_time[15] = "";
	char Interval[15] = "";
	char Count[15] = "";
	char ip_ver[15] = "";
	char rand[15] = "";
	char cold[15] = "";
	char proto[15] = "";
	char host[25] = "";

	infile = fopen(config_file, "r");
	if(infile==NULL)
	{
		perror("\n can not open file : Config File");
		exit(0);
	}

	fgets(ex_time, 15, infile);
	fgets(Interval, 15, infile);
	fgets(Count, 15, infile);
	fgets(ip_ver, 15, infile);
	fgets(rand, 15, infile);
	fgets(cold, 15, infile);
	fgets(proto, 15, infile);
	fgets(host, 25, infile);

	int et;
	sscanf(ex_time, "time,%d", &et);
	Config_Ex_Time = et;
	int ival;
	sscanf(Interval, "interval,%d", &ival);
	Config_Interval = ival;
	int cnt;
	sscanf(Count, "count,%d", &cnt);
	Config_Count = cnt;
	int ipv;
	sscanf(ip_ver, "ip_Ver,%d", &ipv);
	Config_IP_Ver = ipv;
	int rnd;
	sscanf(rand, "random,%d", &rnd);
	Config_Random = rnd;
	int cache;
	sscanf(cold, "cache,%d", &cache);
	Config_Cold = cache;
	int prt;
	sscanf(proto, "protocol,%d", &prt);
	Config_Proto = prt;
	Config_Host = (char*)malloc(sizeof(char)*25);
	sscanf(host, "host,%s", Config_Host);

	fclose(infile);
}


void load_payload_file()
{
    FILE *infile;
    struct stat statbuf;
    int i = 0;
    int c = 0;

    /* get the file size so we can figure out how much memory to allocate */

    stat(payload_file, &statbuf);
    payload_size = statbuf.st_size;


    payload_data = (char *)malloc(payload_size * sizeof(char));
    if (payload_data == 0)
    {
        printf("Allocation of memory for payload failed.\n");
        exit(0);
    }

    /* open the file and read it into memory */

    infile = fopen(payload_file, "r");	/* open the payload file read only */

    if(infile==NULL)
    {
    	printf("Error: Payload File Can Not Be loaded");
    }
    while((c = getc(infile)) != EOF)
    {
        *(payload_data + i) = c;
        i++;
    }


    fclose(infile);
}

void load_tcp_file()
{
	FILE *infile;

	infile = fopen(tcp_file, "r");

	if( !infile ) {
		printf("\ncan't open file : %s \n", tcp_file);
		exit(1);
	}

	char record_line[20]="";
	char sport_line[20]="";
	char dport_line[20] = "";
	char seq_line[20] = "";
	char ack_line[20] = "";
	char control_line[65] = "";
	char win_line[20] = "";
	char urg_line[20] = "";
	int index = 1;


	u_short 	tcp_src_port;				/* TCP source port */
	u_short 	tcp_des_port;				/* TCP dest port */
	u_short 	tcp_win;					/* TCP window size */
	u_short 	tcp_urgent;					/* TCP urgent data pointer */
	u_long		tcp_ack;					/* TCP ack number */
	u_long  	tcp_seq;					/* TCP sequence number */
	u_int8_t  	tcp_control;				/* TCP control flages */



	fgets(record_line, 20, infile);	/*read the source port*/
	sscanf(record_line, "record,%d", &TCP_LIST_SIZE);

	for(index = 0; index<TCP_LIST_SIZE; index++)
	{
		fgets(sport_line, 20, infile);	/*read the source port*/
		fgets(dport_line, 20, infile); 	/*read the dest port*/
		fgets(win_line, 20, infile);	/*read the win num*/
		fgets(urg_line, 20, infile);	/*read the urg id*/
		char urg_line[20] = "";
		fgets(urg_line, 20, infile);	/*read the urg id*/
		fgets(seq_line, 20, infile);	/*read the seq num*/
		fgets(ack_line, 20, infile);	/*read the ack id*/
		fgets(control_line, 65, infile);	/*read the control flags*/

		/* parse the strings and throw the values into the variable */

		sscanf(sport_line, "sport,%d", &tcp_src_port);
		u_short sp = tcp_src_port;
		sscanf(dport_line, "dport,%d", &tcp_des_port);
		u_short dp = tcp_des_port;
		sscanf(win_line, "win,%d", &tcp_win);
		u_short win = tcp_win;
		sscanf(urg_line, "urg,%d", &tcp_urgent);
		u_short urg = tcp_urgent;
		sscanf(seq_line, "seq,%ld", &tcp_seq);
		u_short seq = tcp_seq;
		sscanf(ack_line, "ack,%ld", &tcp_ack);
		u_short ack = tcp_ack;
		sscanf(control_line, "control,%[^!]", &tcp_control_char);

		// Insert Node in TCP Link List
		tcp_control = convert_tcp_control(tcp_control_char);

		insert_tcp_node(&head_tcp,sp,dp,win,urg,ack,seq,tcp_control);

	}

	fclose(infile); /*close the file*/


}


void load_tcp_option_file(){
	FILE *infile;

	infile = fopen(tcp_options_file, "r");
	if( !infile ) {
		printf("\ncan't open file : %s \n", tcp_options_file);
		exit(1);
	}

    char option_line1[100]="";char option_line2[100]="";char option_line3[100]="";char option_line4[100]="";char option_line5[100]="";
    char option_line6[100]="";char option_line7[100]="";char option_line8[100]="";char option_line9[100]="";char option_line10[100]="";
    char option_line11[100]="";char option_line12[100]="";char option_line13[100]="";char option_line14[100]="";char option_line15[100]="";
    char option_line16[100]="";char option_line17[100]="";char option_line18[100]="";char option_line19[100]="";char option_line20[100]="";
    char size_line[20]="";
	char record_line[20]="";


	int index=0;
	u_int32_t list_size;


	if (infile!=NULL)
	{
		fgets(record_line, 20, infile);	/*read the source port*/
		sscanf(record_line, "record,%d", &TCP_OPTION_LIST_SIZE);

		for(index = 0; index<TCP_OPTION_LIST_SIZE; index++)
		{
			fgets(size_line, 20, infile);	/*read the source port*/
			sscanf(size_line, "size,%d", &list_size);

		fgets(option_line1, 100, infile);	/*read the source port*/
		sscanf(option_line1, "options,%d", &optList[0]);
		fgets(option_line2, 100, infile);	/*read the source port*/
		sscanf(option_line2, "options,%d", &optList[1]);
		fgets(option_line3, 100, infile);	/*read the source port*/
		sscanf(option_line3, "options,%d", &optList[2]);
		fgets(option_line4, 100, infile);	/*read the source port*/
		sscanf(option_line4, "options,%d", &optList[3]);
		fgets(option_line5, 100, infile);	/*read the source port*/
		sscanf(option_line5, "options,%d", &optList[4]);
		fgets(option_line6, 100, infile);	/*read the source port*/
		sscanf(option_line6, "options,%d", &optList[5]);
		fgets(option_line7, 100, infile);	/*read the source port*/
		sscanf(option_line7, "options,%d", &optList[6]);
		fgets(option_line8, 100, infile);	/*read the source port*/
		sscanf(option_line8, "options,%d", &optList[7]);
		fgets(option_line9, 100, infile);	/*read the source port*/
		sscanf(option_line9, "options,%d", &optList[8]);
		fgets(option_line10, 100, infile);	/*read the source port*/
		sscanf(option_line10, "options,%d", &optList[9]);
		fgets(option_line11, 100, infile);	/*read the source port*/
		sscanf(option_line11, "options,%d", &optList[10]);
		fgets(option_line12, 100, infile);	/*read the source port*/
		sscanf(option_line12, "options,%d", &optList[11]);
		fgets(option_line13, 100, infile);	/*read the source port*/
		sscanf(option_line13, "options,%d", &optList[12]);
		fgets(option_line14, 100, infile);	/*read the source port*/
		sscanf(option_line14, "options,%d", &optList[13]);
		fgets(option_line15, 100, infile);	/*read the source port*/
		sscanf(option_line15, "options,%d", &optList[14]);
		fgets(option_line16, 100, infile);	/*read the source port*/
		sscanf(option_line16, "options,%d", &optList[15]);
		fgets(option_line17, 100, infile);	/*read the source port*/
		sscanf(option_line17, "options,%d", &optList[16]);
		fgets(option_line18, 100, infile);	/*read the source port*/
		sscanf(option_line18, "options,%d", &optList[17]);
		fgets(option_line19, 100, infile);	/*read the source port*/
		sscanf(option_line19, "options,%d", &optList[18]);
		fgets(option_line20, 100, infile);	/*read the source port*/
		sscanf(option_line20, "options,%d", &optList[19]);

//		int xxx;
//    for (xxx=0; xxx<20;xxx++)
//    	printf(" \n %d",optList[xxx]);

//    	printf(" \n@   its options value @");

		        u_int32_t ls = list_size;
		insert_tcp_options_node(&head_tcp_options, optList, ls);
	//	free(op);

		}
	}

	fclose(infile);


}






void load_ipv4_file()
{

	FILE *infile;
    infile = fopen(ipv4_file, "r");
	if( !infile ) {
		printf("\ncan't open file : %s \n", ipv4_file);
		exit(1);
	}

    int index = 1;
    char record_line[20]="";
    char proto_line[40] = "";
    char id_line[40] = "";
    char frag_line[40] = "";
    char ttl_line[40] = "";
    char saddr_line[40] = "";
    char daddr_line[40] = "";
    char tos_line[90] = "";
    char z_zsaddr[40] = "";
    char z_zdaddr[40] = "";
    char inter_line[15]="";

     u_long  ip_des_addr;			/* IP dest addr */
     u_long  ip_src_addr;			/* IP source addr */
     u_short ip_id;					/* IP id */
     u_short ip_frag;				/* IP frag */
     u_char  ip_tl;					/* IP TTL */



    fgets(record_line, 20, infile);
    sscanf(record_line, "record,%d", &IP_LIST_SIZE);

    for(index = 1; index<=IP_LIST_SIZE;index++)
    {
			fgets(id_line, 11, infile);
    	    fgets(frag_line, 13, infile);
    	    fgets(ttl_line, 10, infile);
    	    fgets(saddr_line, 24, infile);
    	    fgets(daddr_line, 24, infile);
    	    fgets(tos_line, 78, infile);

    	    sscanf(id_line, "id,%d", &ip_id);
    	    u_short id = ip_id;
    	    sscanf(frag_line, "frag,%d", &ip_frag);
    	    u_short frag = ip_frag;
    	    sscanf(ttl_line, "ttl,%d", &ip_tl);
    	    u_short tl = ip_tl;
    	    sscanf(saddr_line, "saddr,%s", &z_zsaddr);
    	    char *zsadd = malloc(strlen(z_zsaddr)*sizeof(char));
    	    zsadd = z_zsaddr;
    	    sscanf(daddr_line, "daddr,%s", &z_zdaddr);
    	    char *zdadd = malloc(strlen(z_zdaddr)*sizeof(char));
    	    zdadd = z_zdaddr;
    	    sscanf(tos_line, "tos,%[^!]", &ip_ttos);
    	    char *ttos = malloc(strlen(ip_ttos)*sizeof(char));
    	    ttos = ip_ttos;

    	    ip_src_addr = libnet_name2addr4(libnet_handle, zsadd, LIBNET_RESOLVE);
    	    ip_des_addr = libnet_name2addr4(libnet_handle, zdadd, LIBNET_RESOLVE);

    	    ip_ttos_val=convert_ip_ttps(ip_ttos);
    	    insert_ip_node(&head_ip, ip_des_addr, ip_src_addr, id, frag, tl, ip_ttos_val);
    }

    fclose(infile);

}





void load_ip_option_file(){

	FILE *infile;

	infile = fopen(ip_options_file, "r");
	if( !infile ) {
		printf("\ncan't open file : %s \n", ip_options_file);
		exit(1);
	}

    char option_line[100]="";
    char size_line[20]="";
	char record_line[20]="";
	char optList[100];
	int index=0;
	u_int32_t list_size;


	if (infile!=NULL)
	{
		fgets(record_line, 20, infile);	/*read the source port*/
		sscanf(record_line, "record,%d", &IP_OPTION_LIST_SIZE);

		for(index = 0; index<IP_OPTION_LIST_SIZE; index++)
		{

		fgets(option_line, 100, infile);	/*read the source port*/
		sscanf(option_line, "options,%[^!]", &optList);
		char *op = malloc(strlen(optList)*sizeof(char));
		op = optList;
		fgets(size_line, 20, infile);	/*read the source port*/
		sscanf(size_line, "size,%d", &list_size);
		u_int32_t ls = list_size;
		insert_ip_options_node(&head_ip_options,op,ls);

		}
	}

	fclose(infile);

}







u_int8_t convert_tcp_control(u_char  tcp_control[])
{

	u_int8_t  tcp_control_val = 0;

	if(strstr(tcp_control, "th_urg") != NULL)
	        tcp_control_val = tcp_control_val | TH_URG;
    if(strstr(tcp_control, "th_ack") != NULL)
        tcp_control_val = tcp_control_val | TH_ACK;

    if(strstr(tcp_control, "th_psh") != NULL)
        tcp_control_val = tcp_control_val | TH_PUSH;

    if(strstr(tcp_control, "th_rst") != NULL)
        tcp_control_val = tcp_control_val | TH_RST;

    if(strstr(tcp_control, "th_syn") != NULL)
        tcp_control_val = tcp_control_val | TH_SYN;

    if(strstr(tcp_control, "th_fin") != NULL)
        tcp_control_val = tcp_control_val | TH_FIN;

    return tcp_control_val;
}

u_int8_t convert_ip_ttps(u_char  ip_ttos[])
{
	u_int8_t  ip_ttos_val = 0;
	ip_ttos_val = 0;
    if(strstr(ip_ttos, "iptos_lowdelay") != NULL)
        ip_ttos_val = ip_ttos_val | IPTOS_LOWDELAY;

    if(strstr(ip_ttos, "iptos_throughput") != NULL)
        ip_ttos_val = ip_ttos_val | IPTOS_THROUGHPUT;

    if(strstr(ip_ttos, "iptos_reliability") != NULL)
        ip_ttos_val = ip_ttos_val | IPTOS_RELIABILITY;

    if(strstr(ip_ttos, "iptos_mincost") != NULL)
        ip_ttos_val = ip_ttos_val | IPTOS_MINCOST;
    return  ip_ttos_val;
}

void insert_tcp_node (struct tcp_node** head, u_short src_port, u_short des_port, u_short win, u_short urgent, u_long ack, u_long  seq, u_int8_t  control)
{



	struct tcp_node* newnode;
	struct tcp_node* crnt;

	// Create the node that will be inserted
	newnode = (struct node*)malloc(sizeof(struct tcp_node));
	newnode->tcp_src_port=src_port;
	newnode->tcp_des_port=des_port;
	newnode->tcp_win=win;
	newnode->tcp_urgent=urgent;
	newnode->tcp_seq=seq;
	newnode->tcp_ack=ack;
	newnode->tcp_control=control;
	// Insert into empty list
	if(*head == NULL)
	{
		newnode->next = NULL;
		*head = newnode;
		return;
	}
	else
	{
		crnt = *head;
		while(crnt->next != NULL)
		{
				crnt = crnt->next;
		}
	newnode->next = crnt->next;
	crnt->next = newnode;
	}
}

void insert_tcp_options_node(struct option_node** head, u_int8_t optisonL[], u_int32_t optionSize){


	struct option_node* newnode;
	struct option_node* crnt;
		newnode = (struct node*)malloc(sizeof(struct option_node));


		int opindex;

//		    	printf(" \n loading options in header");

		for(opindex=0;opindex<optionSize;opindex++)
		{
			newnode->optionsList[opindex]=optisonL[opindex];
			//printf(" \n %d",optisonL[opindex]);

		}
		newnode->option_list_size=optionSize;
		if(*head == NULL)
			{
				newnode->next = NULL;
				*head = newnode;
				return;
			}

		else
			{
				crnt = *head;
				while(crnt->next != NULL)
				{
						crnt = crnt->next;
				}


			newnode->next = crnt->next;
			crnt->next = newnode;
			}
}

void insert_ip_node (struct ip_node** head,u_long  des_addr,u_long  src_addr,u_short id,u_short frag,u_char  tl,u_int8_t  ttos)
{

	struct ip_node* newnode;
	struct ip_node* crnt;
		newnode = (struct node*)malloc(sizeof(struct ip_node));
		newnode->ip_des_addr=des_addr;
		newnode->ip_src_addr=src_addr;
		newnode->ip_id=id;
		newnode->ip_frag=frag;
		newnode->ip_tl=tl;
		newnode->ip_ttos=ttos;
		if(*head == NULL)
			{
				newnode->next = NULL;
				*head = newnode;
				return;
			}

		else
			{
				crnt = *head;
				while(crnt->next != NULL)
				{
						crnt = crnt->next;
				}


			newnode->next = crnt->next;
			crnt->next = newnode;
			}

}

void insert_ip_options_node(struct option_node** head, char* optisonL, u_int32_t optionSize){


	struct option_node* newnode;
	struct option_node* crnt;
		newnode = (struct node*)malloc(sizeof(struct option_node));
		strcpy(newnode->optionsList,optisonL);
		newnode->option_list_size=optionSize;
		if(*head == NULL)
			{
				newnode->next = NULL;
				*head = newnode;
				return;
			}

		else
			{
				crnt = *head;
				while(crnt->next != NULL)
				{
						crnt = crnt->next;
				}


			newnode->next = crnt->next;
			crnt->next = newnode;
			}
}


int get_fired_msgs(void)
{
	return Rpt_Messages;
}

void print_data()
{
		int index = 1;
		struct tcp_node* tcp = NULL;
		struct ip_node* ip = NULL;
		struct option_node *tcp_options=NULL;
		struct option_node *ip_options=NULL;
		tcp=head_tcp;
		ip=head_ip;
		tcp_options =head_tcp_options;
		ip_options =head_ip_options;
	printf("\nFollowing data set is provided\n");

	printf("\n********** Config **********");
	printf("\n Execution Time      :%d",Config_Ex_Time);
	printf("\n Delay               :%d",Config_Interval);
	printf("\n Message             :%d",Config_Count);
	printf("\n IP Ver              :%d",Config_IP_Ver);
	printf("\n Random PayLoad      :%d",Config_Random);
	printf("\n Cold Cache          :%d",Config_Cold);

	printf("\n\n********** Payload **********");
	printf("\nPayload size is       :%d",payload_size);

	printf("\n\n---------- TCP ----------");
	printf("\nrecord		%d", TCP_LIST_SIZE);

		if(tcp==NULL)
		{
			printf("\TCP Node Is empty\n");
		}
		else
		{
			for(index = 0; index<TCP_LIST_SIZE; index++)
			{
			printf("\nsrc port	%d",tcp->tcp_src_port);;
			printf("\ndes port	%d",tcp->tcp_des_port);
			printf("\nwindow  	%d",tcp->tcp_win);
			printf("\nurgent  	%d",tcp->tcp_urgent);
			printf("\nseq     	%d",tcp->tcp_seq);
			printf("\nack     	%d",tcp->tcp_ack);
			printf("\ncont     	%d",tcp->tcp_control);
			printf("\n\n");
			tcp=tcp->next;
			}
		}
		printf("\n--------TCP Option -------------\n");
		if(tcp_options==NULL)
			{
				printf("\TCP Option Node Is empty\n");
			}
			else
			{
				for(index = 1; index<=TCP_OPTION_LIST_SIZE;index++)
				{
					printf("\n Number of options: %d",tcp_options->option_list_size);
					printf("\n Options : %s",tcp_options->optionsList);
					tcp_options=tcp_options->next;
				}
			}

		printf("\n\n__________ IP __________");
		if(head_ip==NULL)
			{
				printf("\IP Node Is empty\n");
			}
			else
			{
				for(index = 1; index<=IP_LIST_SIZE;index++)
				{
					printf("\n IP Data");
					printf("\n SRC: %d",&ip->ip_src_addr);
					printf("\n DES: %d",&ip->ip_des_addr);
					printf("\n ID:  %d",ip->ip_id);
					printf("\n IFrg: %d",ip->ip_frag);
					printf("\n TTL:  %d",ip->ip_tl);
					printf("\n TOS:  %d",ip->ip_ttos);
					printf("\n\n");
					ip=ip->next;
				}
			}
		printf("\n--------IP Option -------------\n");
		if(ip_options==NULL)
			{
				printf("\IP Option Node Is empty\n");
			}
			else
			{
				for(index = 1; index<=IP_OPTION_LIST_SIZE;index++)
				{
					printf("\n Number of options: %d",ip_options->option_list_size);
					printf("\n Options : %s",ip_options->optionsList);
					ip_options=ip_options->next;
				}
			}

}
