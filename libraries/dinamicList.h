/*
   Copyright (C) 2016, Mariane Affonso Medeiros.

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

#include<stdio.h>
#include<stdlib.h>
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


List* initializeList();
void addLastList(List* list, ItemType element);
void addList(List* list, ItemType element, int index);
int removeList(List* list, ItemType* element, int index);
int removeElementList(List* list, ItemType element);
Node* getNodeByIndex(List* list, int index);

int getList(List* list, ItemType* element, int index);
int setList(List* list, ItemType element, int index);
int indexOfList(List* list, ItemType element);
int containsList(List* list, ItemType element);
int sizeList(List* list);
int isEmptyList(List* list);
void printList(List* list);
List* copyList(List* list, List* newList);
ItemType getElementByIndex(List* list, int index);
void freeList(List* list);
