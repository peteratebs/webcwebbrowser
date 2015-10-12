

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "webcfg.h"
#include "webctypes.h"

/* This switch speeds up the vector_add_node_to_rear operation */
#define VECTOR_STORE_LAST_NODE     1

struct vector
{
	int data_size;
	struct vector_node *first;
#if (VECTOR_STORE_LAST_NODE)	
	struct vector_node *last;
#endif // VECTOR_STORE_LAST_NODE	
};

struct vector_node
{
	struct vector_node *next;
	WEBC_UINT8 data[1]; // could be any size
};

typedef struct 
{
	struct vector*      v;
	struct vector_node* prev;
	struct vector_node* current;
}
vector_iterator;

#ifdef __cplusplus
extern "C" {
#endif

// API routines
int vector_add_node_to_front(struct vector *vector, WEBC_UINT8* data);

#ifdef WEBC_DEBUG
int vector_debug_add_node_to_rear(struct vector *vector, WEBC_PFBYTE data, const char* file, int line);
#define vector_add_node_to_rear(V,D) vector_debug_add_node_to_rear(V,D,__FILE__,__LINE__)

int vector_debug_add_node_to_front(struct vector *vector, WEBC_PFBYTE data, const char* file, int line);
#define vector_add_node_to_front(V,D) vector_debug_add_node_to_front(V,D,__FILE__,__LINE__)
#else
int vector_add_node_to_rear(struct vector *vector, WEBC_UINT8* data);
#define vector_debug_add_node_to_rear(V,D,F,L) vector_add_node_to_rear(V,D)
int vector_add_node_to_front(struct vector *vector, WEBC_UINT8* data);
#define vector_debug_add_node_to_front(V,D,F,L) vector_add_node_to_front(V,D)
#endif

void vector_delete(struct vector *vector);
void vector_delete_cb(struct vector *vector, void (*destructor)(WEBC_UINT8* data));
int vector_delete_index_to_end(struct vector *vector,int node);
int vector_delete_last_node(struct vector *vector);
int vector_delete_node(struct vector *vector,int node);
int vector_delete_node_cb(struct vector *vector,int node, void (*destructor)(WEBC_UINT8* data));
int vector_delete_cond(struct vector *vector,int (*match)(WEBC_UINT8* data, WEBC_UINT8* p), WEBC_UINT8* param);
int vector_delete_cond_cb(struct vector *vector,int (*match)(WEBC_UINT8* data, WEBC_UINT8* p), WEBC_UINT8* param, void (*destructor)(WEBC_UINT8* data));
WEBC_UINT8* vector_find_node(struct vector *vector,int (*match)(WEBC_UINT8* data, WEBC_UINT8* p), WEBC_UINT8* param);
WEBC_PFBYTE vector_find_nth_node(struct vector *vector,int (*match)(WEBC_PFBYTE data, WEBC_PFBYTE p), WEBC_PFBYTE param, int n);
WEBC_UINT8* vector_get_current(vector_iterator *pvi);
WEBC_UINT8* vector_get_first(struct vector *vector,vector_iterator *pvi);
WEBC_UINT8* vector_get_last_node(struct vector *vector);
WEBC_UINT8* vector_get_next(vector_iterator *pvi);
WEBC_UINT8* vector_delete_and_get_next(vector_iterator *pvi);
WEBC_UINT8* vector_get_node(struct vector *vector,int node);
int vector_get_size(struct vector *vector);
int vector_init(struct vector *vector,int size);
int vector_move_node_to_front(struct vector *vector,int node);

//////////////////////////////////////////////////////////////////////////////////////////
// VECTOR STACK FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////

void InitStack(struct vector *pStack);
int PushObject(struct vector *pStack, WEBC_UINT8* obj);
WEBC_UINT8* PopObject(struct vector *pStack);
void ClearStack(struct vector *pStack);

//////////////////////////////////////////////////////////////////////////////////////////
// VECTOR QUEUE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////

void InitQueue(struct vector *pQueue);
int EnqueueObject(struct vector *pQueue, WEBC_UINT8* obj);
WEBC_UINT8* DequeueObject(struct vector *pQueue);
void FlushQueue(struct vector *pQueue);

//////////////////////////////////////////////////////////////////////////////////////////
// VECTOR LIST FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////

void InitObjectList(struct vector *pList);
int ObjectListAddFront(struct vector *pList, WEBC_UINT8* obj);
int ObjectListAddRear(struct vector *pList, WEBC_UINT8* obj);
void ObjectListRemove(struct vector *pList, WEBC_UINT8* obj);
WEBC_UINT8* ObjectListGetLast(struct vector *pList);
WEBC_UINT8* ObjectListGetFirst(struct vector *pList, vector_iterator *pvi);
WEBC_UINT8* ObjectListGetNext(vector_iterator *pvi);
void ObjectListClear(struct vector *pList);


#ifdef __cplusplus
}
#endif

#endif // __VECTOR_H__

// END VECTOR.H


