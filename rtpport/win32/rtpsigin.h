 /*
 | RTPSIGIN.H - Runtime Platform Services
 |
 |   PORTED TO THE Microsoft Win32 Environment.
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/12/14 18:27:45 $
 |  $Name:  $
 |  $Revision: 1.1 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPSIGIN_H__
#define __RTPSIGIN_H__

/************************************************************************
 * Portable signalling services including semaphores (counting semaphores
 * or event queues) and mutexes (resource locks).
 *
 * NOTE: THIS HEADER IS TO BE INCLUDED AT THE APPLICATION LEVEL OR IN 
 *       A LOCALIZED DATA DISTRIBUTION MODULE LIMITING ITS NATIVE 
 *       SYSTEM HEADER INCLUSION.  THEREFORE IT IS THE RESPONSIBILITY 
 *       OF THE APPLICATION AND/OR THIS SEGREGATED MODULE TO CONSTRUCT 
 *       (ALLOCATE) AND DECONSTRUCT (FREE) ALL SEMAPHORES AND MUTEXES.
 *
 * EXAMPLE: 
 *       notice that the only type that should be public for 
 *       use by the overall system will be the RTP_SEMAPHORE
 *       requiring the inclusion of rtpsignl.h ONLY.
 *       
 *       int proprietary_alloc_semaphore (RTP_SEMAPHORE* sem)
 *       {
 *          RTP_SEMAPHORE_INSTANCE* newSem = malloc(sizeof(RTP_SEMAPHORE_INSTANCE));
 *          if (newSem)
 *          {
 *             if (rtp_sig_semaphore_alloc(newSem, "ProprietarySemaphore") < 0)
 *             {
 *                free(newSem);
 *             }
 *             else
 *             {
 *                *sem = rtp_sig_semaphore_get_handle(newSem);
 *                return (0);
 *             }
 *          }
 *          return (-1);
 *       }
 ************************************************************************/

#include "rtptypes.h"
#include "rtpsignl.h"

#include <windows.h>

/************************************************************************
 * If RTP_TRACK_LOCAL_SEMAPHORES or RTP_TRACK_LOCAL_MUTEXES is turned   *
 * on, the debug implementaion of the corresponding signalling system   *
 * will be implemented.  This provides debug information otherwise not  *
 * available on most platforms. These should be defined at the          *
 * compiler or in rtpsignl.h if desired.                                *
 ************************************************************************/

typedef HANDLE RTP_SEMAPHORE_INSTANCE;
typedef HANDLE RTP_MUTEX_INSTANCE;

/************************************************************************
 * Signaling System API which should be used via the macros that follow.*
 *                                                                      *
 *                 DO NOT ALTER ANYTHING BELOW.                         *
 *        THE TYPEDEFS ABOVE SHOULD BE THE ONLY PORTING NEEDED.         *
 ************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Semaphore (signalling) services                                      *
 ************************************************************************/

int                     _rtp_sig_semaphore_alloc        (RTP_SEMAPHORE_INSTANCE* newSem, const char *name);
RTP_SEMAPHORE           _rtp_sig_semaphore_get_handle   (RTP_SEMAPHORE_INSTANCE* sem);
RTP_SEMAPHORE_INSTANCE* _rtp_sig_semaphore_get_instance (RTP_SEMAPHORE           semHandle);
void                    _rtp_sig_semaphore_free         (RTP_SEMAPHORE_INSTANCE sem);

/************************************************************************
 * Mutex (lock) services                                                *
 ************************************************************************/

int                     _rtp_sig_mutex_alloc            (RTP_MUTEX_INSTANCE* newMutex, const char *name);
RTP_MUTEX               _rtp_sig_mutex_get_handle       (RTP_MUTEX_INSTANCE* mutex);
RTP_MUTEX_INSTANCE*     _rtp_sig_mutex_get_instance     (RTP_MUTEX           mutexHandle);
void                    _rtp_sig_mutex_free             (RTP_MUTEX_INSTANCE  mutex);



#ifdef RTP_TRACK_LOCAL_SEMAPHORES
#include "rtpsemdb.h"
int                     _rtp_debug_sig_semaphore_alloc        (RTP_SEMAPHORE_INSTANCE* newSem, const char* name, const char* file, long line_num);
RTP_SEMAPHORE           _rtp_debug_sig_semaphore_get_handle   (RTP_SEMAPHORE_INSTANCE* sem,    const char* file, long line_num);
RTP_SEMAPHORE_INSTANCE* _rtp_debug_sig_semaphore_get_instance (RTP_SEMAPHORE     semHandle,    const char* file, long line_num);
void                    _rtp_debug_sig_semaphore_free         (RTP_SEMAPHORE_INSTANCE* sem,    const char* file, long line_num);
#endif

  #ifndef RTP_TRACK_LOCAL_SEMAPHORES
	/************************************************************************
	 * If RTP_TRACK_LOCAL_SEMAPHORES is not defined, semaphore access will  *
	 * be directed to the rtpsignl.c porting file and the semaphore debug   *
	 * utility functions will be defined to nothing to remove their use.    *
	 ************************************************************************/
	#define rtp_sig_semaphore_alloc(newSem, name)           _rtp_sig_semaphore_alloc(newSem, name)
	#define rtp_sig_semaphore_get_handle(sem)               _rtp_sig_semaphore_get_handle(sem)
	#define rtp_sig_semaphore_get_instance(semHandle)       _rtp_sig_semaphore_get_instance(semHandle)
	#define rtp_sig_semaphore_free(sem)                     _rtp_sig_semaphore_free(sem)              
  #endif /* RTP_TRACK_LOCAL_SEMAPHORES */

#ifdef RTP_TRACK_LOCAL_MUTEXES
#include "rtpmtxdb.h"
int                 _rtp_debug_sig_mutex_alloc        (RTP_MUTEX_INSTANCE* newMutex, const char* name, const char* file, long line_num);
RTP_MUTEX           _rtp_debug_sig_mutex_get_handle   (RTP_MUTEX_INSTANCE* mutex,    const char* file, long line_num);
RTP_MUTEX_INSTANCE* _rtp_debug_sig_mutex_get_instance (RTP_MUTEX     mutexHandle,    const char* file, long line_num);
void                _rtp_debug_sig_mutex_free         (RTP_MUTEX_INSTANCE* mutex,    const char* file, long line_num);
#endif

  #ifndef RTP_TRACK_LOCAL_MUTEXES
	/************************************************************************
	 * If RTP_TRACK_LOCAL_MUTEXES is not defined, mutex access will         *
	 * be directed to the rtpsignl.c porting file and the mutex debug       *
	 * utility functions will be defined to nothing to remove their use.    *
	 ************************************************************************/
	#define rtp_sig_mutex_alloc(newMutex, name)             _rtp_sig_mutex_alloc(newMutex, name)
	#define rtp_sig_mutex_get_handle(mutex)                 _rtp_sig_mutex_get_handle(mutex)
	#define rtp_sig_mutex_get_instance(mutexHandle)         _rtp_sig_mutex_get_instance(mutexHandle)
	#define rtp_sig_mutex_free(mutex)                       _rtp_sig_mutex_free(mutex)
  #endif /* RTP_TRACK_LOCAL_MUTEXES */

#ifdef __cplusplus
}
#endif

#endif /* __RTPSIGIN_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
