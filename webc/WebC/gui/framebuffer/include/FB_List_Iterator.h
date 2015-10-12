/*
|  FB_List_Iterator.h -
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
#include "FB_List.h"

class FB_List_Iterator{
	FB_List_Node* current;//the current pointer to the node in the list
	public:
		FB_List_Iterator(FB_List* list);//constructor
		~FB_List_Iterator();//destructor

		//gets the next item in the iteration
		void* getNext();
};