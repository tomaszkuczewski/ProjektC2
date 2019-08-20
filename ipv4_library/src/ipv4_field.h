/*
 * ipv4_field.h
 *
 *  Created on: Nov 28, 2017
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#ifndef IPV4_LIBRARY_SRC_IPV4_FIELD_H_
#define IPV4_LIBRARY_SRC_IPV4_FIELD_H_

void set_ipv4_field(void* hdr, unsigned int id, char* str);

#endif /* IPV4_LIBRARY_SRC_IPV4_FIELD_H_ */
