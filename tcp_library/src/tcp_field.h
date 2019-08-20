/*
 * tcp_field.h
 *
 *  Created on: Nov 28, 2017
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#ifndef TCP_LIBRARY_SRC_TCP_FIELD_H_
#define TCP_LIBRARY_SRC_TCP_FIELD_H_

void set_tcp_field(void* hdr, unsigned int id, char* str);

#endif /* TCP_LIBRARY_SRC_TCP_FIELD_H_ */
