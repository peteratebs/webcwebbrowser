/*
|  FB_List_Iterator.cpp -
|	
|	This class when given a list provides a way to access each element at a time without
|   altering the actual list.
|   
|  EBS -
|
|   $Author: Peter Lavallee $
|   $Date: 2008/08/01 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/
#include "FB_Window_Manager.h"
#include <stdlib.h>


/**
*	Constuctor for a new iterator, takes a list and initializes current to the head of the list
*/
FB_List_Iterator::FB_List_Iterator(FB_List* list){
	current = list->head;
}

/**
*	Destructor doesnt have to do anything
*/
FB_List_Iterator::~FB_List_Iterator(){}

/**
*	Gets the next data item in the list. Each time getNext will advance the internal curent pointer	
*/
void* FB_List_Iterator::getNext(){
	void* retVal = NULL;
	if(current != NULL){
		retVal = current->data;
		current = current->next;
	}
	return retVal;
}