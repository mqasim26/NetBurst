/*
 * syncapp.h
 *
 *  Created on: May 10, 2009
 *      Author: root
 */

#ifndef SYNCAPP_H_
#define SYNCAPP_H_


#include <sys/socket.h>

#include "packetizer.h"


int run_server(void);
char* read_data(void);
int run_client(void);
void write_data_on_scoket(char* buff);
void close_server(void);
void close_client(void);



#endif /* SYNCAPP_H_ */
