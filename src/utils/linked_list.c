/*
 * linked_list.c
 *
 *  Created on: 24.05.2017
 *      Author: Tomek
 */

#ifndef SOURCE_LINKED_LIST_C_
#define SOURCE_LINKED_LIST_C_

#include "linked_list.h"

linked_list * create_list()
{
	//Allocating data for linked list struct
	linked_list * list = (linked_list *) malloc(sizeof(linked_list));

	//Default values
	list->firstNode = 0;
	list->lastNode = 0;
	list->size = 0;

	return list;
}

void remove_list(linked_list * list)
{
	while(list->size > 0)
		remove_last_element(list);

	free(list);
}

void add_element(linked_list * list, unsigned char * data, unsigned int size_p)
{
	//If list is
	if(list->size == 0)
	{
		//Setting up node
		node * first = malloc(sizeof(node));
		first->frame = data;
		first->size = size_p;
		first->nextNode = 0;
		first->previousNode = 0;

		//Setting up list
		list->firstNode = first;
		list->lastNode = first;
		list->size++;
	}
	else
	{
		node * created = malloc(sizeof(node));
		created->frame = data;
		created->nextNode = 0;
		created->size = size_p;
		created->previousNode = list->lastNode;

		//Setting up list
		list->lastNode->nextNode = created;
		list->lastNode = created;
		list->size++;
	}
}

void remove_last_element(linked_list * list)
{
	if(list->size <= 0)
		return;

	if(list->size == 1)
	{
		free(list->firstNode);

		list->firstNode = 0;
		list->lastNode = 0;
		list->size--;
	}
	else
	{
		node * last = list->lastNode;
		node * pre_last = last->previousNode;

		free(last);
		pre_last->nextNode = 0;

		list->lastNode = pre_last;
		list->size--;
	}
}

unsigned char * get_element(linked_list * list, int id, unsigned int* size)
{
	if(id > (list->size - 1))
		return 0;

	int i = 0;

	node * n = list->firstNode;
	while(n != 0)
	{
		if(id == i)
		{
			*size = n->size;
			return n->frame;
		}

		n = n->nextNode;
		i++;
	}
	*size = 0;
	return 0;
}

#endif /* SOURCE_LINKED_LIST_C_ */
