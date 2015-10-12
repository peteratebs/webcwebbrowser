#ifndef __DBGPROFILE_H__
#define __DBGPROFILE_H__

#include "webc.h"
#if(WEBC_SUPPORT_DEBUG_PROFILE)
struct ProfileAccumulator
{
unsigned long TimeIn;
unsigned long TimesCalled;
};


#ifdef __cplusplus
extern "C" {
#endif
unsigned long _webc_debug_log_time_enter(char *p1, char *p2);
void _webc_debug_log_time_exit(unsigned long t,char *p1, char *p2);
void Profiler_reset(char *m);
unsigned long _webc_debug_log_time_getticks(void);
void _webc_debug_log_time_printaccumulator(char *p1, struct ProfileAccumulator *pa);
#ifdef __cplusplus
}
#endif

#define WEBC_DEBUG_LOG_TIME_HANDLE(T) unsigned long T;
#define WEBC_DEBUG_LOG_TIME_RESET(M) Profiler_reset(M);
#define WEBC_DEBUG_LOG_TIME_ENTER(T,P,P2) T = _webc_debug_log_time_enter(P,P2);
#define WEBC_DEBUG_LOG_TIME_EXIT(T,P,P2)  _webc_debug_log_time_exit(T,P,P2);

#define WEBC_DEBUG_LOG_TIME_ACCUMULATOR(A) struct ProfileAccumulator A;
#define WEBC_DEBUG_LOG_TIME_CLEAR_ACCUMULATOR(A) A.TimeIn = 0,	A.TimesCalled = 0;
#define WEBC_DEBUG_LOG_TIME_ACCUMULATE_BEGIN(S) S=_webc_debug_log_time_getticks();
#define WEBC_DEBUG_LOG_TIME_ACCUMULATE(S,A)A.TimesCalled += 1, A.TimeIn = A.TimeIn + (_webc_debug_log_time_getticks()-S);
#define WEBC_DEBUG_LOG_TIME_PRINT_ACCUMULATOR(S,A) _webc_debug_log_time_printaccumulator(S, &A);


#else

#define WEBC_DEBUG_LOG_TIME_HANDLE(T)
#define WEBC_DEBUG_LOG_TIME_RESET(M)
#define WEBC_DEBUG_LOG_TIME_ENTER(T,P,P2)
#define WEBC_DEBUG_LOG_TIME_EXIT(T,P,P2)
#define WEBC_DEBUG_LOG_TIME_ACCUMULATOR(A)
#define WEBC_DEBUG_LOG_TIME_CLEAR_ACCUMULATOR(A)
#define WEBC_DEBUG_LOG_TIME_ACCUMULATE_BEGIN(S)
#define WEBC_DEBUG_LOG_TIME_ACCUMULATE(S,A)
#define WEBC_DEBUG_LOG_TIME_PRINT_ACCUMULATOR(S,A)
#endif // __ifdef WEBC_SUPPORT_DEBUG_PROFILE

#endif // __DBGPROFILE_H__