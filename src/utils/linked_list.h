/*
 * linked_list.h
 *
 *  Created on: 24.05.2017
 *      Author: root
 */

#ifndef HEADERS_LINKED_LIST_H_
#define HEADERS_LINKED_LIST_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct node_t {
	struct node_t * nextNode;
	struct node_t * previousNode;
	unsigned char * frame;
	unsigned int size;
} node;

typedef struct linked_list_t {
	struct node_t * firstNode;
	struct node_t * lastNode;

	unsigned int size;
} linked_list;

linked_list * create_list();
void remove_list(linked_list * list);
void add_element(linked_list * list, unsigned char * pointer, unsigned int size);
void remove_last_element(linked_list * list);
unsigned char * get_element(linked_list * list, int id, unsigned int* size);
void remove_list(linked_list * list);

#endif /* HEADERS_LINKED_LIST_H_ */
