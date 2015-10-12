//
// VECTOR.C - Vector routines
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

#include "vector.h"
#include "webcstr.h"
#include "webcmem.h"

// API routines

/*************************************************************************
 vector_add_node_to_front() - Add node to front of vector

 vector - vector to which node is to be added
 data - data to be copied to new node

 Returns: 0 on success, -1 if node allocation fails.
*************************************************************************/

#ifdef WEBC_DEBUG
int vector_debug_add_node_to_front(struct vector *vector, WEBC_PFBYTE data, const char* file, int line)
#else
int vector_add_node_to_front(struct vector *vector, WEBC_PFBYTE data)
#endif
{
struct vector_node *new_node;

#ifdef WEBC_DEBUG
	new_node=(struct vector_node*) WEBC_DEBUG_MALLOC(sizeof(struct vector_node) - 1 + vector->data_size,file,line,"VectorAddNode",0);
#else
    new_node=(struct vector_node*) WEBC_DEBUG_MALLOC(sizeof(struct vector_node) - 1 + vector->data_size,0,0,"VectorAddNode",0);

#endif
	if(!new_node)
	{
		return(-1);
	}

	tc_movebytes(&(new_node->data),data,vector->data_size);

	new_node->next=vector->first;
#if (VECTOR_STORE_LAST_NODE)
	if (!vector->last)
	{
		vector->last = new_node;
	}
#endif // VECTOR_STORE_LAST_NODE
	vector->first=new_node;

	return(0);	
}

/*************************************************************************
 vector_add_node_to_rear() - Add node to rear of vector.

 vector - vector to which node is to be added
 data - data to be copied to new node

 Returns: 0 on success, -1 if node allocation fails.
*************************************************************************/
#ifdef WEBC_DEBUG
int vector_debug_add_node_to_rear(struct vector *vector, WEBC_PFBYTE data, const char* file, int line)
#else
int vector_add_node_to_rear(struct vector *vector, WEBC_PFBYTE data)
#endif
{
#if (VECTOR_STORE_LAST_NODE)

struct vector_node *new_node;

  #ifdef WEBC_DEBUG
	new_node = (struct vector_node*) WEBC_DEBUG_MALLOC(sizeof(struct vector_node) - 1 + vector->data_size, file, line,"VectorAddNode",0);
  #else
	new_node = (struct vector_node*) WEBC_MALLOC(sizeof(struct vector_node) - 1 + vector->data_size);
  #endif
  	
	if (!new_node)
	{
		return (-1);
	}

	tc_movebytes(&(new_node->data),data,vector->data_size);

	new_node->next = 0;
	if (vector->last)
	{
		vector->last->next = new_node;
	}
	else 
	{
		vector->first = new_node;
	}
	vector->last = new_node;

	return (0);	
	
#else

int size=1;
struct vector_node *cur_node,*new_node;

	// create new node
	// we take sizeof(struct vector_node)-1 since the structure already
	// includes a 1-byte_ 'data' array.
	new_node=(struct vector_node*)WEBC_MALLOC(sizeof(struct vector_node)-1
		+vector->data_size);
	if(!new_node)
	{
		return(-1);
	}
	tc_movebytes(&(new_node->data),data,vector->data_size);
	new_node->next=0;

	// add new node to end of list
	if(!vector->first)
	{
		vector->first=new_node;
		return(size);
	}

	cur_node=vector->first;
	while(cur_node->next)
	{
		cur_node=cur_node->next;
		size++;
	};
	cur_node->next=new_node;
	
	return(size);

#endif // VECTOR_STORE_LAST_NODE
}

/*************************************************************************
 vector_delete() - Delete all nodes within a vector.

 vector - vector to be deleted
*************************************************************************/

void vector_delete(struct vector *vector)
{
struct vector_node *cur_node,*temp_node;

	if(!vector)
	{
		return;
	}

	cur_node=vector->first;
	while(cur_node)
	{
		temp_node=cur_node;
		cur_node=cur_node->next;
		WEBC_FREE(temp_node);
	};

	vector->first = (struct vector_node *)0;
#if (VECTOR_STORE_LAST_NODE)
	vector->last = 0;
#endif // VECTOR_STORE_LAST_NODE
}


/*************************************************************************
 vector_delete() - Delete all nodes within a vector; use callback to 
   free any internally used data

 vector - vector to be deleted
*************************************************************************/

void vector_delete_cb(struct vector *vector, void (*destructor)(WEBC_PFBYTE data))
{
struct vector_node *cur_node,*temp_node;

	if(!vector)
	{
		return;
	}

	cur_node=vector->first;
	while(cur_node)
	{
		destructor(cur_node->data);
		temp_node=cur_node;
		cur_node=cur_node->next;
		WEBC_FREE(temp_node);
	};

	vector->first = (struct vector_node *)0;
#if (VECTOR_STORE_LAST_NODE)
	vector->last = 0;
#endif // VECTOR_STORE_LAST_NODE	
}


/*************************************************************************
 vector_delete_index_to_end() - Delete nodes from index to end of vector.

 vector - vector from which to delete nodes
 node - first node to delete
*************************************************************************/

int vector_delete_index_to_end(struct vector *vector,int node)
{
int cur_index=0;
struct vector_node *cur_node,*temp_node,*last_node=0;

	cur_node=vector->first;
	while(cur_node)
	{
		if(cur_index==node)
		{
			break;
		}
		last_node=cur_node;
		cur_node=cur_node->next;
		cur_index++;
	}

#if (VECTOR_STORE_LAST_NODE)
	vector->last = last_node;
#endif // VECTOR_STORE_LAST_NODE

	// delete from 'cur_node' onwards
	while(cur_node)
	{
		temp_node=cur_node;
		cur_node=cur_node->next;
		WEBC_FREE(temp_node);
	}

	if(!last_node)
	{
		vector->first=0;
	}
	else
	{
		last_node->next=0;
	}

	return(0);
}

/*************************************************************************
 vector_delete_last_node() - Delete last node in vector.

 vector - vector from which to delete node
*************************************************************************/

int vector_delete_last_node(struct vector *vector)
{
struct vector_node *cur_node,*last_node=0;

	if(!vector)
	{
		return(-1);
	}

	cur_node=vector->first;
	if(!cur_node)
	{
		return(-1);
	}

	do
	{
		if(!cur_node->next)
		{
			if(!last_node)
			{
				vector->first=0;
			}
			else
			{
				last_node->next=0;
			}
			break;
		}
		else
		{
			last_node=cur_node;
			cur_node=cur_node->next;
		}
	}while(1);

#if (VECTOR_STORE_LAST_NODE)
	vector->last = last_node;
#endif // VECTOR_STORE_LAST_NODE

	WEBC_FREE(cur_node);

	return(0);
}

/*************************************************************************
 vector_delete_node() - Delete indexed node from vector

 vector - vector from which to delete node
 node - index of node to delete

 Returns: 0 on success, -1 if the index was out of range.
*************************************************************************/

int vector_delete_node(struct vector *vector,int node)
{
int index;
struct vector_node *cur_node,*last_node=0;

	index=0;
	cur_node=vector->first;
	while(cur_node)
	{
		if(index==node)
		{
#if (VECTOR_STORE_LAST_NODE)
			if (cur_node == vector->last)
			{
				vector->last = last_node;
			}
#endif // VECTOR_STORE_LAST_NODE			

			if(last_node)
			{
				last_node->next=cur_node->next;
			}
			else
			{
				vector->first=cur_node->next;
			}
			WEBC_FREE(cur_node);
			return(0);
		}
		last_node=cur_node;
		cur_node=cur_node->next;
		index++;
	}
	return(-1);
}

int vector_delete_node_cb(struct vector *vector,int node, void (*destructor)(WEBC_PFBYTE data))
{
int index;
struct vector_node *cur_node,*last_node=0;

	index=0;
	cur_node=vector->first;
	while(cur_node)
	{
		if(index==node)
		{
#if (VECTOR_STORE_LAST_NODE)
			if (cur_node == vector->last)
			{
				vector->last = last_node;
			}
#endif // VECTOR_STORE_LAST_NODE			

			if(last_node)
			{
				last_node->next=cur_node->next;
			}
			else
			{
				vector->first=cur_node->next;
			}
			destructor(cur_node->data);
			WEBC_FREE(cur_node);
			return(0);
		}
		last_node=cur_node;
		cur_node=cur_node->next;
		index++;
	}
	return(-1);	
}


int vector_delete_cond(struct vector *vector,int (*match)(WEBC_PFBYTE data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
vector_iterator vi;
WEBC_PFBYTE p_data;
int index = 0;

	p_data = vector_get_first(vector, &vi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			vector_delete_node(vector, index);
			return (0);
		}
		index++;
		p_data = vector_get_next(&vi);
	}
	
	return (-1);	
}

int vector_delete_cond_cb(struct vector *vector,int (*match)(WEBC_PFBYTE data, WEBC_PFBYTE p), WEBC_PFBYTE param, void (*destructor)(WEBC_PFBYTE data))
{
vector_iterator vi;
WEBC_PFBYTE p_data;
int index = 0;

	p_data = vector_get_first(vector, &vi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			vector_delete_node_cb(vector, index, destructor);
			return (0);
		}
		index++;
		p_data = vector_get_next(&vi);
	}
	
	return (-1);
}


WEBC_PFBYTE vector_find_node(struct vector *vector,int (*match)(WEBC_PFBYTE data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
vector_iterator vi;
WEBC_PFBYTE p_data;

	p_data = vector_get_first(vector, &vi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			return (p_data);
		}
		p_data = vector_get_next(&vi);
	}
	
	return (0);
}

WEBC_PFBYTE vector_find_nth_node(struct vector *vector,int (*match)(WEBC_PFBYTE data, WEBC_PFBYTE p), WEBC_PFBYTE param, int n)
{
vector_iterator vi;
WEBC_PFBYTE p_data;

	p_data = vector_get_first(vector, &vi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			if(0 == n)
				return (p_data);
			else
				n--;
		}
		if(n < 0)
			break;
		p_data = vector_get_next(&vi);
	}
	
	return (0);
}

/*************************************************************************
 vector_get_current() - Return current node in iteration.

 pvi - pointer to vector iterator used in calls to vector_get_first()
       and vector_get_next()

 This function returns a pointer to the last node read via
 vector_get_first() or vector_get_next(), i.e. the "current" node.

 Returns: Pointer to current node's data.
*************************************************************************/

WEBC_PFBYTE vector_get_current(vector_iterator *pvi)
{
	return pvi->current? pvi->current->data : 0;
}

/*************************************************************************
 vector_get_first() - Return first node in vector.

 vector - vector from which to get node
 pvi - vector_iterator to be used in calls to vector_get_next()

 This function, in conjunction with vector_get_next(), allow you to 
 iteratively read the nodes of a vector.

 Returns: Pointer to first node's data.
*************************************************************************/

WEBC_PFBYTE vector_get_first(struct vector *vector,vector_iterator *pvi)
{
	pvi->current = vector->first;
	pvi->prev = 0;
	pvi->v = vector;
	return pvi->current? pvi->current->data : 0;
}

/*************************************************************************
 vector_get_last_node() - Return last node in vector.

 vector - vector from which to get node

 Returns: Pointer to last node's data.
*************************************************************************/

WEBC_PFBYTE vector_get_last_node(struct vector *vector)
{
#if (VECTOR_STORE_LAST_NODE)

	if (!vector)
	{
		return (0);
	}

	return (vector->last? (WEBC_PFBYTE)&(vector->last->data) : 0);

#else

struct vector_node *cur_node, *last_node=0;

	if(!vector)
	{
		return(0);
	}
	cur_node=vector->first;
	while(cur_node)
	{
		last_node=cur_node;
		cur_node=cur_node->next;
	};
	return(last_node?(WEBC_PFBYTE)&(last_node->data):0);

#endif // VECTOR_STORE_LAST_NODE
}

/*************************************************************************
 vector_get_next() - Get next node in vector

 pvi - vector iterator passed to vector_get_first()

 After having called vector_get_first(), this routine may be called
 iteratively to read the nodes in a vector.

 Returns: Pointer to data in next node.
*************************************************************************/

WEBC_PFBYTE vector_get_next(vector_iterator* pvi)
{
	pvi->prev = pvi->current;
	if (pvi->current)
	{
		pvi->current = pvi->current->next;
		return pvi->current? pvi->current->data : 0;
	}

	return (0);
}

WEBC_UINT8* vector_delete_and_get_next(vector_iterator* pvi)
{
	if (pvi->current)
	{
		struct vector_node* toDelete = pvi->current;
		
		if (pvi->prev)
		{
			pvi->prev->next = pvi->current->next;			
		}
		else
		{
			pvi->v->first = pvi->current->next;
		}
		
	  #if (VECTOR_STORE_LAST_NODE)	
		if (pvi->v->last == toDelete)
		{
			pvi->v->last = pvi->prev;
		}
	  #endif 
		
		pvi->current = pvi->current->next;
		
		WEBC_FREE(toDelete);
		
		return pvi->current? pvi->current->data : 0;
	}
	
	return (0);
}


/*************************************************************************
 vector_get_node() - Get indexed node.

 vector - vector from which to retrieve node
 node - index of node to retrieve

 Returns: Pointer to indexed node's data.
*************************************************************************/

WEBC_PFBYTE vector_get_node(struct vector *vector,int node)
{
int index;
struct vector_node *cur_node;

	if(!vector)
	{
		return(0);
	}

	cur_node=vector->first;
	index=0;
	while(cur_node)
	{
		if(index==node)
		{
			return((WEBC_PFBYTE)&(cur_node->data));			
		}
		cur_node=cur_node->next;
		index++;
	};

	return(0);
}

/*************************************************************************
 vector_get_size() - Return # of nodes in vector.

 vector - vector to get size of

 Returns: Number of nodes in vector.
*************************************************************************/

int vector_get_size(struct vector *vector)
{
int size;
struct vector_node *cur_node;

	if(!vector)
	{
		return(-1);
	}
	cur_node=vector->first;
	size=0;
	while(cur_node)
	{
		cur_node=cur_node->next;
		size++;
	};
	return(size);
}

/*************************************************************************
 vector_init() - Initialize vector.

 vector - vector to initialize
 size - data size of elements in vector

 This routine must be called on a vector before using all other API calls
 in this module.  The 'size' parameter determines the size, in bytes, of
 the data in each node in the vector.  If you wanted to create a vector
 of struct xxx, for example, you would pass in sizeof(struct xxx).

 Returns: 0 on success, -1 if size is <= 0.
*************************************************************************/

int vector_init(struct vector *vector,int size)
{
	if(size<=0)
	{
		return(-1);
	}
	vector->data_size=size;
	vector->first=0;	
#if (VECTOR_STORE_LAST_NODE)
	vector->last = 0;
#endif // VECTOR_STORE_LAST_NODE
	
	return(0);
}

/*************************************************************************
 vector_move_node_to_front() - Move indexed node to front of vector.

 Moves indexed node to front of vector; for example, if node is 3,
 the fourth node is moved to the front, i.e. 0,1,2,3 becomes 3,0,1,2

 vector - vector in which to move nodes
 node - index of node to move

 Returns: 0 on success, -1 if index is invalid.
*************************************************************************/

int vector_move_node_to_front(struct vector *vector,int node)
{
int index=0;
struct vector_node *cur_node,*last_node;

	// the 0th node is already first so nothing to do
	if(node==0)
	{
		return(0);
	}

	// loop thru until find the node
	// NOTE: last_node does not need to be initialized since we will
	//       never encounter the case where the first node in the
	//       list is the one we are looking for due to the check
	//       above; but must keep compiler happy
	last_node = cur_node=vector->first;
	while(cur_node)
	{
		if(index==node)
		{
			last_node->next=cur_node->next;
			cur_node->next=vector->first;
#if (VECTOR_STORE_LAST_NODE)
			if (cur_node == vector->last)
			{
				vector->last = last_node;
			}
#endif // VECTOR_STORE_LAST_NODE
			vector->first=cur_node;
			return(0);
		}
		last_node=cur_node;
		cur_node=cur_node->next;
		index++;
	}
	return(-1);
}


//////////////////////////////////////////////////////////////////////////////////////////
// VECTOR STACK FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////

void InitStack(struct vector *pStack)
{
	vector_init(pStack,sizeof(WEBC_PFBYTE));
}

int PushObject(struct vector *pStack, WEBC_PFBYTE obj)
{
	return vector_add_node_to_front(pStack, (WEBC_PFBYTE) &obj);
}

WEBC_PFBYTE PopObject(struct vector *pStack)
{
vector_iterator vi;
WEBC_PFBYTE *data;
WEBC_PFBYTE obj;

	data = (WEBC_PFBYTE *) vector_get_first(pStack,&vi);
	if (!data)
		return (0);

	obj = *data;
	vector_delete_node(pStack,0);	
		
	return (obj);
}

void ClearStack(struct vector *pStack)
{
	vector_delete(pStack);
}

//////////////////////////////////////////////////////////////////////////////////////////
// VECTOR QUEUE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////

void InitQueue(struct vector *pQueue)
{
	vector_init(pQueue,sizeof(WEBC_PFBYTE));
}

int EnqueueObject(struct vector *pQueue, WEBC_PFBYTE obj)
{
	return vector_add_node_to_rear(pQueue, (WEBC_PFBYTE) &obj);
}

WEBC_PFBYTE DequeueObject(struct vector *pQueue)
{
	return (PopObject(pQueue));
}

void FlushQueue(struct vector *pQueue)
{
	vector_delete(pQueue);
}


//////////////////////////////////////////////////////////////////////////////////////////
// VECTOR LIST FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////

struct VPointer
{
	WEBC_PFBYTE ptr;
};

void InitObjectList(struct vector *pList)
{
	vector_init(pList, sizeof(struct VPointer));
}

int ObjectListAddFront(struct vector *pList, WEBC_PFBYTE obj)
{
struct VPointer node;

	node.ptr = obj;
	return vector_add_node_to_front(pList, (WEBC_PFBYTE) &node);
}

int ObjectListAddRear(struct vector *pList, WEBC_PFBYTE obj)
{
struct VPointer node;

	node.ptr = obj;	
	return vector_add_node_to_rear(pList, (WEBC_PFBYTE) &node);
}

void ObjectListRemove(struct vector *pList, WEBC_PFBYTE obj)
{
	struct VPointer* current;
	vector_iterator vi;

	current = (struct VPointer*) vector_get_first(pList, &vi);
	while (current)
	{
		if (current->ptr == obj)
		{
			current = (struct VPointer*) vector_delete_and_get_next(&vi);
			continue;
		}
		current = (struct VPointer*) vector_get_next(&vi);
	}
}

WEBC_PFBYTE ObjectListGetLast(struct vector *pList)
{
struct VPointer *p_node = (struct VPointer *) vector_get_last_node(pList);

	if (!p_node)
		return 0;
	return (p_node->ptr);
}

WEBC_PFBYTE ObjectListGetFirst(struct vector *pList, vector_iterator *pvi)
{
struct VPointer *p_node = (struct VPointer *) vector_get_first(pList, pvi);

	if (!p_node)
		return 0;
	return (p_node->ptr);
}

WEBC_PFBYTE ObjectListGetNext(vector_iterator *pvi)
{
struct VPointer *p_node = (struct VPointer *) vector_get_next(pvi);

	if (!p_node)
		return 0;
	return (p_node->ptr);
}

void ObjectListClear(struct vector *pList)
{
	vector_delete(pList);
}


// END VECTOR.C
