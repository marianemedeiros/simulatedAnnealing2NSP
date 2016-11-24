/*
   Copyright (C) 2014, Guilherme Castro Diniz.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation (FSF); in version 2 of the
   license.

   This program is distributed in the hope that it can be useful,
   but WITHOUT ANY IMPLIED WARRANTY OF ADEQUATION TO ANY
   MARKET OR APPLICATION IN PARTICULAR. See the
   GNU General Public License for more details.
   <http://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdlib.h>
#define ItemType int

typedef struct node{
	ItemType data;
	struct node* next;
}Node;

typedef struct{
	Node* first;
	Node* last;
	int size;
}List;


/**
 * create and inicialize structure.
 *
 * @returns a pointer of type List, with first and last pointing to null and size zero.
 */
List* initializeList(){
	List* p;
	p = (List*) calloc(1, sizeof(List));
	p->first = NULL;
	p->last = NULL;
	p->size = 0;
	return p;
}

/**
 * Inserts elements at the end of the list.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, the element to be inserted in the end list.
 *
 */
void addLastList(List* list, ItemType element){
	if(list->first == NULL && list->last == NULL){
		list->first = (Node*) calloc(1,sizeof(Node));
		list->first->data = element;
		list->last = list->first;
		list->last->next = NULL;
		list->size++;
	}
	else{
		list->last->next = (Node*) calloc(1,sizeof(Node));
		list->last->next->data = element;
		list->last = list->last->next;
		//list->last->next == NULL;
		list->size++;		
	}
}


/**
 * Inserts elements by index.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, the element to be inserted in list.
 * @param int index, the index that will be done to insertion.
 *
 */
void addList(List* list, ItemType element, int index){
	int i;
	Node* newNode;
	Node* aux = list->first->next;
	Node* backNode = list->first;
	newNode = (Node*) calloc(1,sizeof(Node));
	newNode->data = element;

	if(index <= 0){
		newNode->next = list->first;
		list->first = newNode;
		list->size++;
	}
	else if(index > list->size){
		list->last->next = newNode;
		list->last = newNode;
		newNode->next = NULL;
		list->size++;
	}
	else{
		for(i = 1; i <= list->size; i++){
			if(i == index){
				backNode->next = newNode;
				newNode->next = aux;
				list->size++;
				break;
			}
			backNode = aux;
			aux = aux->next;
		}
	}
}

/**
 * Removes element by index.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, parameter used to send the element found..
 * @param int index, the index removal will be done.
 * And the removed element is returned by parameter.
 *
 * @returns 0 if element is successfully removed, or -1 if element was not removed.
 */
int removeList(List* list, ItemType* element, int index){
	int i;
	Node* aux = list->first;
	Node* backNode;

	if(index == 0){
		*element = list->first->data;
		aux = list->first->next;
		free(list->first);
		list->first = aux; 
		if(list->size == 1){
			list->last = list->first;		
		}
		list->size--;
		return 0;
	}
	if(index > list->size || index < 0){
		return -1;	
	}
	else{
		for(i = 0; i <= list->size; i++){
			if(i == index){
				*element = aux->data;
				backNode->next = aux->next;
				free (aux);
				list->size--;						
				return 0;
			}
			backNode = aux;
			aux = aux->next;
		}
	}
	return -2;
}

/**
 * Removes per element.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, the element to be removed in the List.
 *
 * @returns 0 if element is successfully removed, or -1 if element was not removed.
 */
int removeElementList(List* list, ItemType element){
	Node* aux = list->first;
	Node* backNode;	
	int i;
	if(element  == list->first->data){
		list->first = list->first->next;
		free(list->first);
		list->size--;
		return 0;
	}
	else{
		for(i = 0; i < list->size; i++){
			if(aux->data == element){
				backNode->next = aux->next;
				free(aux);
				list->size--;
				return 0;
			}
			backNode = aux;
			aux = aux->next;
		}
		return -1;
	}
}

/**
 * Search element by index.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, parameter used to send the element found.
 * @param int index, The index used to search for the element.
 * The element will be sent by parameter
 * 
 * @returns 0 if element is successfully found, or -1 if element was not found.
 */
int getList(List* list, ItemType* element, int index){
	int i;
	Node* aux = list->first;
	if(index > list->size || index < 0){
		return -1;
	}
	else{
		for(i = 0; i < list->size; i++){
			if(index == i){
				*element = aux->data;
				return 0;
			}
			aux = aux->next;
		}
	}
	return -2;
}

/**
 * Search element by index.
 *
 * @param List* list, pointer of type List.
 * @param int index, that the element stays.
 * 
 * @returns return the element or -1 if not exists.
 */
ItemType getElementByIndex(List* list, int index){
	int i;
	Node* aux = list->first;
	if(index > list->size || index < 0){
		return -1;
	}
	else{
		for(i = 0; i < list->size; i++){
			if(index == i){
				return aux->data;
			}
			aux = aux->next;
		}
	}
	return -2;
}

Node* getNodeByIndex(List* list, int index){
	int i;
	Node* aux = list->first;
	if(index > list->size || index < 0){
		return NULL;
	}
	else{
		for(i = 0; i < list->size; i++){
			if(index == i){
				return aux;
			}
			aux = aux->next;
		}
	}
	return NULL;
}

/**
 * Replaces the element by index.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, new element.
 * @param int index, index of the element to be replaced.
 * 
 * @returns 0 if element is successfully replaced, or -1 if element was not replaced.
 */
int setList(List* list, ItemType element, int index){
	int i;
	Node* aux = list->first;

	if(index > list->size || index < 0){
		return -1;
	}
	else{
		for(i = 0; i <= list->size; i++){
			if(index == i){
				aux->data = element;
				return 0;
			}
			aux = aux->next;
		}
	}
	return -2;
}

/**
 * Returns the index of the element.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, element.
 * 
 * @returns the index of the element, or -1 if element was not found.
 */
int indexOfList(List* list, ItemType element){
	int i;
	Node* aux = list->first;
	for(i = 0; i < list->size; i++){
		if(element == aux->data){
			return i;
		}
		aux = aux->next;
	}
	return -1;
}

/**
 * Search for an element in the list.
 *
 * @param List* list, pointer of type List.
 * @param ItemType element, the element to be searched.
 * 
 * @returns 0 if element is successfully found, or -1 if element was not found.
 */
int containsList(List* list, ItemType element){
	int i;
	Node* aux = list->first;
	for(i = 0; i < list->size; i++){
		if(element == aux->data){
			return 0;
		}
		aux = aux->next;
	}
	return -1;
}

/**
 * Size List.
 *
 * @param List* list, pointer of type List.
 * 
 * @returns size list.
 */
int sizeList(List* list){
	return list->size;
}


/**
 * Verifies that the list is empty.
 *
 * @param List* list, a pointer of type List.
 *
 * @returns 0 if the list is empty, or -1 if the list is not empty
 */
int isEmptyList(List* list){
	if(list->size == 0) return 1;
	return 0;
}

/**
 * Prints the elements of the list .
 *
 * @param List* list, a pointer of type List.
 *
 */
void printList(List* list){
	Node* newNode = list->first;
	while(newNode != NULL){
		printf("%d ", newNode->data);
		newNode = newNode->next;
	}
}

/**
 * Copy the elements of list to newList .
 *
 * @param List* list, a pointer of type List.
 * @param List* newList, a pointer to newList.
 *
 */
List* copyList(List* list, List* newList){
	Node* newNode = list->first;
	while(newNode != NULL){
		addLastList(newList,newNode->data);
		newNode = newNode->next;
	}
	return newList;
}

void freeList(List* list){
	Node* prev = list->first;
	Node* node = list->first->next;
	while(node != NULL){
		free(prev);
		prev = node;
		node = node->next;
	}
	free(prev);
	free(node);
}

List* invertList(List* list){
	List* rt = (List*) calloc(1, sizeof(List));

	for (int i = list->size-1; i >= 0 ; i--){
		int x = getElementByIndex(list,i);
		addLastList(rt, x);
	}
	return rt;
}