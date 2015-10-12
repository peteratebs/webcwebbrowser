/*
|  FB_List.h -
|
|	This class is a list that is used by the Frame Buffer graphics layer
|   It has varios methods to provide functionality for a queue like list, 
|
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
#include "webc.h"

//list node, contains a void pointer for a data, contains a next node and can be locked
//so that no can access it

typedef struct fbnode {
	void* data;
	fbnode *next;
	WEBC_BOOL locked;
} FB_List_Node;


class FB_List{
	//declare the iterator as a friend class so that it may access the head and tail nodes
	friend class FB_List_Iterator;

	FB_List_Node* head;//head of the list
	FB_List_Node* tail;//last node in the list
	
	public:
		FB_List();//constructor
		~FB_List();//destructor
	
		//removes and returns data from the front of the list, see .cpp file for more info
		void* pop();

		//returns data fromt he front of the list without removing, see .cpp file for more info
		void* peek();

		//places data at the end of the list
		void push(void* data);

		//places datat at the front of the list
		void addToFront(void* data);

		//returns the data at a certain index within the 
		void* getDataAt(int index);

		//removes the node containing the data 
		void remove(void* data);
};