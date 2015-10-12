/*
|	FB_List.cpp -
|
|	This class is a list that is used by the Frame Buffer graphics layer
|   It has varios methods to provide functionality for a queue like list, 
|
|  EBS -
|
|   $Author: Peter Lavallee $
|   $Date: 2008/08/01 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "webc.h"
#include "webcmem.h"
#include "FB_Window_Manager.h"


/**
*	Constructor for the list.  Initialized head.
*/
FB_List::FB_List(){
	head = WEBC_NULL;
}

/**
*	Destructor for the list. This does not get rid of the data that was contained 
*   just destroys the list.
*/
FB_List::~FB_List(){
	FB_List_Node* currentNode = head;
	if(currentNode != WEBC_NULL){
		while(currentNode->next != WEBC_NULL){
			head = currentNode->next;
			WEBC_FREE((void*)currentNode);
			currentNode = head;
		}
	
		WEBC_FREE((void*)currentNode);
		head = WEBC_NULL;
	}
}

/**
*	Method used to get the data from and remove the first node of a list.
*   if data comes back NULL this means the data was not found, the headnode was locked
*   or that the headnode was NULL.
*
*	Parameters: list-a pointer to the first node, data-the holder where the data will be put
*
*/
void* FB_List::pop(){
	void* data = WEBC_NULL;
	FB_List_Node* currentNode = head;
	if(currentNode != WEBC_NULL ){
		if(!(currentNode->locked)){
			data = currentNode->data;
			head = currentNode->next;
			WEBC_FREE((void*)currentNode);
		}
	}
	return data;
}

/**
*	Method used to get the data from first node of a list.
*   if data comes back NULL this means the data was not found, that the headnode was NULL.
*
*	return - the data
*
*/
void* FB_List::peek(){
	void* data = WEBC_NULL;
	FB_List_Node* currentNode = head;
	if(currentNode != WEBC_NULL ){
		data = currentNode->data;
	}
	return data;
}


/**
*	Adds data onto the end of a list in a node, if the list is null a 
*   starting node will be created
*	
*	Parameters: data- data to be put in the list
*/

void FB_List::push(void* data){
	FB_List_Node* currentNode = head;
	FB_List_Node* newNode = (FB_List_Node*)WEBC_MALLOC(sizeof(FB_List_Node));
	if(newNode){
		newNode->locked = false;
		newNode->data = data;
		newNode->next = WEBC_NULL;
		if(currentNode == WEBC_NULL){
			head = newNode;
		}
		else{
			while(currentNode->next != WEBC_NULL){
				currentNode = currentNode->next;
			}
			currentNode->next = newNode;
		}
	}
}

/**
*	Adds data onto the end of a list in a node, if the list is null a 
*   starting node will be created
*	
*	Parameters: data- data to be put in the list
*/

void FB_List::addToFront(void* data){
	FB_List_Node* currentNode = head;
	FB_List_Node* newNode = (FB_List_Node*)WEBC_MALLOC(sizeof(FB_List_Node));
	if(newNode){
		newNode->locked = false;
		newNode->data = data;
		newNode->next = WEBC_NULL;
		if(currentNode == WEBC_NULL){
			head = newNode;
		}
		else{
			newNode->next = currentNode;
			head = newNode;
		}
	}
}

/**
*	Allows for an index to be specified to get a certain data element.
*   this only returns the data at the index if the index exists and is non-NULL.
*   If the index cannot be found returns NULL.
* 
*	Parameters: index-where in the list the data is located.
*/
void* FB_List::getDataAt(int index){
	FB_List_Node* current = head;
	void* retVal = WEBC_NULL;
	for(int i = 0; i < index; i++){
		if(current != WEBC_NULL){
			current = current->next;
		}
		else{
			i = index;
		}
	}
	if(current != WEBC_NULL){
		retVal = (void*)current->data;
	}
	return retVal;
}

/**
*	Given a piece of data removes it from the list. If the data does not exist, nothing happens
*
*	Params: data - the data that needs to be removed.
*/
void FB_List::remove(void* data){
	FB_List_Node* currentNode = head;
	FB_List_Node* prevNode = head;	
	if(head != WEBC_NULL){
		if(head->data == data){
			head = head->next;
			WEBC_FREE(currentNode);
		}
		else{
			while(currentNode->next != WEBC_NULL){
				if(currentNode->data == data){
					prevNode->next = currentNode->next;
					WEBC_FREE((void*)currentNode);
				}
				else{
					prevNode = currentNode;
					currentNode = currentNode->next;
				}
			}	
		}
	}		
}