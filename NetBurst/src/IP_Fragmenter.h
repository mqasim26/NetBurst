/*
 * IP_Fragmenter.h
 *
 *  Created on: May 16, 2009
 *      Author: root
 */

#ifndef IP_FRAGMENTER_H_
#define IP_FRAGMENTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

void Fragmenter(int payload_size);
void Write_Fragment(u_int8_t *payload, u_int32_t total_pload_size);
u_int16_t Calculate_Checksum(u_int8_t *payload, u_int32_t total_pload_size,u_int16_t id, u_int16_t seq);














#endif /* IP_FRAGMENTER_H_ */
