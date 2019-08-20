/*
 * linkpack.h
 *
 *  Created on: Nov 28, 2017
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "random_gen.h"

#ifndef LINKPACK_H_
#define LINKPACK_H_

int merge_packet(struct ether_header eh, struct iphdr ih, struct tcphdr th, int random_data, void** output_data);
unsigned short short_checksum(unsigned short * buffer, int size);

#endif /* LINKPACK_H_ */
