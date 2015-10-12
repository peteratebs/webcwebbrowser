 /*
 | RTPDLLIST.H -
 |
 | EBSnet -
 |
 |  $Author: vmalaiya $
 |  $Date: 2005/08/05 20:30:45 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBSnet Inc. , 2004
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
*/

#ifndef __RTPDLLIST_H__
#define __RTPDLLIST_H__

/* define this symbol to make all double-linked list manipulation functions
   expand inline as preprocessor macros */
#define RTP_DLLIST_INLINE

typedef struct s_RTP_DLLIST_NODE RTP_DLLIST_NODE;

struct s_RTP_DLLIST_NODE
{
	RTP_DLLIST_NODE* next;
	RTP_DLLIST_NODE* prev;
};

#define RTP_DLLIST_INIT(X)            { (X)->next = (X); (X)->prev = (X); }

/* insert Y after X */
#define RTP_DLLIST_INSERT_AFTER(X,Y)  { (X)->next->prev = (Y); \
                                        (Y)->next = (X)->next; \
                                        (Y)->prev = (X); \
                                        (X)->next = (Y); \
                                      }

/* insert Y before X */
#define RTP_DLLIST_INSERT_BEFORE(X,Y) { (X)->prev->next = (Y); \
                                        (Y)->prev = (X)->prev; \
                                        (Y)->next = (X); \
                                        (X)->prev = (Y); \
                                      }

#define RTP_DLLIST_REMOVE(X)          { (X)->prev->next = (X)->next; \
                                        (X)->next->prev = (X)->prev; \
                                      }

/* replace X with R */
#define RTP_DLLIST_REPLACE(X,R)       { (R)->next = (X)->next; \
                                        (R)->prev = (X)->prev; \
                                        (X)->next->prev = (R); \
                                        (X)->prev->next = (R); \
                                      }

/* for each node of type T in list L, do X, where I is a empty variable name
   which hold the name of each node of the list */ // ## is strcat
#define RTP_DLLIST_FOREACH(T,I,L,X)   { \
                                         T* I; \
                                         T* next ## I; \
                                         I = (T*) (L).next; \
                                         while (I != (T*) &(L)) \
                                         { \
                                            next ## I = (T*) ((RTP_DLLIST_NODE*)(I))->next; \
                                            X; \
                                            I = next ## I; \
                                         } \
                                      }

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RTP_DLLIST_INLINE

#define rtp_dllist_init(X)            RTP_DLLIST_INIT(X)
#define rtp_dllist_insert_after(X,Y)  RTP_DLLIST_INSERT_AFTER(X,Y)
#define rtp_dllist_insert_before(X,Y) RTP_DLLIST_INSERT_BEFORE(X,Y)
#define rtp_dllist_remove(X)          RTP_DLLIST_REMOVE(X)
#define rtp_dllist_replace(X,R)       RTP_DLLIST_REPLACE(X,R)

#else

void rtp_dllist_init (
		RTP_DLLIST_NODE* x
	);

void rtp_dllist_insert_after (
		RTP_DLLIST_NODE* x,
		RTP_DLLIST_NODE* y
	);

void rtp_dllist_insert_before (
		RTP_DLLIST_NODE* x,
		RTP_DLLIST_NODE* y
	);

void rtp_dllist_remove (
		RTP_DLLIST_NODE* x
	);

void rtp_dllist_replace (
		RTP_DLLIST_NODE* x,
		RTP_DLLIST_NODE* r
	);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTPDLLIST_H__ */
