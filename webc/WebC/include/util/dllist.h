//
// DLLIST.H - 
//
// EBS - 
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
#ifndef __DLLIST_H__
#define __DLLIST_H__

typedef struct s_DLListNode
{
	struct s_DLListNode *next;
	struct s_DLListNode *prev;
}
DLListNode;

#define DLLIST_INIT(X)            { (X)->next = (X); (X)->prev = (X); }

/* insert Y after X */
#define DLLIST_INSERT_AFTER(X,Y)  { (X)->next->prev = (Y); \
                                    (Y)->next = (X)->next; \
                                    (Y)->prev = (X); \
                                    (X)->next = (Y); \
                                  }

/* insert Y before X */                                  
#define DLLIST_INSERT_BEFORE(X,Y) { (X)->prev->next = (Y); \
                                    (Y)->prev = (X)->prev; \
                                    (Y)->next = (X); \
                                    (X)->prev = (Y); \
                                  }
                                  
#define DLLIST_REMOVE(X)          { (X)->prev->next = (X)->next; \
                                    (X)->next->prev = (X)->prev; \
                                  }

/* replace X with R */                                  
#define DLLIST_REPLACE(X,R)       { (R)->next = (X)->next; \
                                    (R)->prev = (X)->prev; \
                                    (X)->next->prev = (R); \
                                    (X)->prev->next = (R); \
                                  }

#endif /* __DLLIST_H__ */
