 /*
 | RTPTHRD.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tom $
 |  $Date: 2004/10/22 17:05:57 $
 |  $Name:  $
 |  $Revision: 1.4 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPTHRD_H__
#define __RTPTHRD_H__

/************************************************************************
 * Portable thread services including:
 *   initialization and reinitialization of thread services
 *   thread create(and start)/exit(and delete)
 *   thread info (thread handle/index, user data)
 *   thread control (sleep, yield)
 ************************************************************************/
#include "rtp.h"

/************************************************************************
 * Type definitions                                                     *
 ************************************************************************/
typedef RTP_HANDLE RTP_THREAD;
typedef RTP_HANDLE RTP_REENTRANT_THREAD;
typedef RTP_HANDLE RTP_THREADP;

typedef RTP_HANDLE RTP_STACK;
typedef RTP_HANDLE RTP_STACKP;

typedef void (* RTP_ENTRY_POINT_FN)(void *);
 
#define CFG_NUM_THREADS         32
#define CFG_NAME_SIZE           32

#define CFG_STACK_SIZE_HUGE     0
#define CFG_STACK_SIZE_BIG      1
#define CFG_STACK_SIZE_NORMAL   2
#define CFG_STACK_SIZE_SMALL    3
#define CFG_STACK_SIZE_TINY     4

#define rtp_thread_init  rtp_threads_init
#define rtp_thread_exit  rtp_threads_shutdown
#define rtp_threads_exit rtp_threads_shutdown

/************************************************************************
 * Thread services                                                      *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
 
int  rtp_threads_init               (void);
void rtp_threads_shutdown           (void);

int  rtp_threads_pool_init          (int nThread, RTP_THREADP resourceBlock);
int  rtp_threads_stacks_pool_init   (int nStack, int stackSizeIndex,
                                     int stackSize,
                                     RTP_STACKP resourceBlock);

int  rtp_thread_spawn               (RTP_THREAD  * newThread,
                                     RTP_ENTRY_POINT_FN entryPoint,
                                     const char  * name, 
                                     int           stackSizeIndex, 
                                     int           priorityIndex, 
                                     void        * userData);
                          
int  rtp_thread_handle              (RTP_THREAD  * currentThread);

int  rtp_thread_user_data           (void ** userData);
int  rtp_thread_user_data_by_handle (RTP_THREAD handle, 
                                     void ** userData);
                                     
int  rtp_thread_name                (char ** name);
int  rtp_thread_name_by_handle      (RTP_THREAD handle, 
                                     char ** name);

void rtp_thread_sleep               (long msecs);
void rtp_thread_yield               (void);

#ifdef __cplusplus
}
#endif

#endif /* __RTPTHRD_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
