//
// DBGPROFILE.C - Functions for performance timing (replace dbgtime.x)
//
// EBS - WebC
//
// Copyright EBS Inc. , 2010
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webc.h"
#if (WEBC_SUPPORT_DEBUG_PROFILE)

#include "rtpmem.h"
#include "rtpstr.h"
#include "rtptime.h"
#include "rtpprint.h"

// Undefine the following to not print the entries as we run
// Used as a double test for windows because print times dominate.
// #define PRINT_ENTRIES
// Enable to print totals
#define PRINT_TOTALS


/*****************************************************************************/
// Macros/Defines
/*****************************************************************************/


/*****************************************************************************/
// Data structures
/*****************************************************************************/


/*****************************************************************************/
// Local function prototypes
/*****************************************************************************/
#define MINPRINTDEPTH 0
#define MAXPRINTDEPTH MAXPROFILEDEPTH
//#define MINPRINTDEPTH 0
//#define MAXPRINTDEPTH 0

#define MAXPROFILEDEPTH 100
static unsigned long zero;
static unsigned long last_reading;
static unsigned long timeInInterval[MAXPROFILEDEPTH];
static unsigned long timeBetweenInterval[MAXPROFILEDEPTH];
static int current_depth,max_depth;
static void _webc_debug_log_time_flush_enter(void);

static unsigned long saved_offset;
static unsigned long saved_Now;
static int saved_depth;
static char savedp1[256];
static char savedp2[256];
static unsigned long timer_stopped_at;
static unsigned long accumulated_error=0;

#if defined(_MSC_VER)
#include <windows.h>
static double freq_;
static unsigned __int64 baseTime_;
static void qpreset();
static double elapsed();
static unsigned long tenmicrosseconds();
#endif

static unsigned long _Profiler_get_msec(void);
static void _webc_debug_log_stop_timer(void);
static void _webc_debug_log_resume_timer();
static unsigned long Profiler_get_msec(void);
static unsigned long Profiler_get_msecOffset(void);
static void _webc_debug_log_time_flush_enter(void);
static int _times_are_the_same(unsigned long a, unsigned long b);


unsigned long total_time_blocked;
unsigned long total_bytes_received;

/*****************************************************************************/
// Global data declarations
/*****************************************************************************/

/*****************************************************************************/
// Functions
/*****************************************************************************/

unsigned long _webc_debug_log_time_getticks(void)
{
	unsigned long Now = _Profiler_get_msec();
	return(Now);
}
unsigned long _webc_debug_log_time_enter(char *p1, char *p2)
{
	unsigned long offset = Profiler_get_msecOffset();
	unsigned long Now = Profiler_get_msec();
	timeBetweenInterval[current_depth] += offset;

	_webc_debug_log_stop_timer();
	if (current_depth >= MINPRINTDEPTH && current_depth <= MAXPRINTDEPTH)
	{
		if (saved_Now)
			_webc_debug_log_time_flush_enter();
		saved_depth = current_depth;
		saved_offset = offset;
		saved_Now	 = Now;
		rtp_strcpy(savedp1, p1);
		rtp_strcpy(savedp2, p2);
	}
	current_depth += 1;
	if (current_depth > max_depth)
		max_depth = current_depth;
	_webc_debug_log_resume_timer();
	return(Now);
}
void _webc_debug_log_time_exit(unsigned long t,char *p1, char *p2)
{
	int print_enter_info_too = 0;
	unsigned long Now, offset;

	if (!current_depth)	/* We were cleared */
		return;
	offset = Profiler_get_msecOffset();
	Now = Profiler_get_msec();
	current_depth -= 1;
	_webc_debug_log_stop_timer();
	if (saved_Now)
	{
		if (saved_depth != current_depth)
			_webc_debug_log_time_flush_enter();
		else
			print_enter_info_too = 1;
	}
#ifdef PRINT_ENTRIES
	if (current_depth >= MINPRINTDEPTH && current_depth <= MAXPRINTDEPTH)
	{
		unsigned long myoffset, myNow;
		char *myp1, *myp2;
		char mymark;

		if (print_enter_info_too)
		{
			myoffset = saved_offset;
			myNow	 = saved_Now;
			myp1	 = savedp1;
			myp2	 = savedp2;
			mymark = '|';
		}
		else
		{
			myoffset = offset;
			myNow	 = Now;
			myp1	 = p1;
			myp2	 = p2;
			mymark = '<';

		}


		if (_times_are_the_same(myoffset,t))
		{
			rtp_printf("        (%-6d) %c%-25.25s %-25.25s (+%-4d)\r\n",       myNow, mymark, myp1, myp2,Now-t);
		}
		else
		{
		    rtp_printf("(+%-4d) (%-6d) %c%-25.25s %-25.25s (+%-4d)\r\n",  myoffset, myNow,mymark, myp1, myp2,Now-t);
		}
	}
#endif

	timeInInterval[current_depth] += Now-t;
	_webc_debug_log_resume_timer();
	saved_Now = 0;
}

void Profiler_reset(char *m)
{

int i;
#ifdef PRINT_TOTALS
	_webc_debug_log_time_flush_enter();
	if (max_depth)
	{

		rtp_printf("    Depth report\r\n");
		rtp_printf("    %-6s %-6s %-6s %-6s\r\n", "Depth", "In", "Betwn" , "Sum");

		for (i = 0; i < max_depth; i++)
		{
			rtp_printf("    %-6d %-6d %-6d %-6d \r\n", i, timeInInterval[i], timeBetweenInterval[i], timeInInterval[i]+timeBetweenInterval[i]);
		}
		rtp_printf("    Measured Elapsed Time                  : %-6d\r\n", _Profiler_get_msec() - zero);
		rtp_printf("    Accumulated time  executing probe code : %-6d\r\n", accumulated_error);
		rtp_printf("    Corrected Elapsed Time                 : %-6d\r\n", _Profiler_get_msec() - zero-accumulated_error);
		rtp_printf("    Total bytes received                   : %-6d\r\n", total_bytes_received);
		rtp_printf("    Time blocked in receive                : %-6d\r\n", total_time_blocked);
		{
			unsigned long ll = (_Profiler_get_msec() - zero-accumulated_error);
			if (ll)
				rtp_printf("    Data Rate bytes/second                 : %-6d \r\n", total_bytes_received/(_Profiler_get_msec() - zero-accumulated_error));
			else
				rtp_printf("    Data Rate bytes/second    zero time    : %-6d \r\n", 0);
		}
	}


	rtp_printf("\r\n======Reset: %s\r\n",m);

#endif
	saved_Now = 0;
	zero = _Profiler_get_msec();// 0; //_Profiler_get_msec();
	last_reading = zero;
	for (i = 0; i < MAXPROFILEDEPTH; i++)
		timeInInterval[i] = timeBetweenInterval[i] = 0;
	current_depth = 0;
	max_depth = 0;
	accumulated_error = 0;
	total_time_blocked = 0;
	total_bytes_received = 0;

}
void _webc_debug_log_time_printaccumulator(char *p1, struct ProfileAccumulator *pa)
{
#ifdef PRINT_ENTRIES
	rtp_printf("%-25.25s Called %-4d Times Accumulated %-6d\r\n",p1, pa->TimesCalled, pa->TimeIn);
#endif
}


// ===

#if defined(_MSC_VER)
#include <windows.h>
/* Not a perfect solution but on Windows (FATS) we use a 10 microsecond perf counter,
  otherwise use milisecond clock */

/// Create a Timer, which will immediately begin counting
/// up from 0.0 seconds.
/// You can call reset() to make it start over.
static double freq_;
static unsigned __int64 baseTime_;
    /// reset() makes the timer start over counting from 0.0 seconds.
static void qpreset() {
      unsigned __int64 pf;
      QueryPerformanceFrequency( (LARGE_INTEGER *)&pf );
      freq_ = 0.0001; // Seems to scale right// 1.0; / (double)pf;
      QueryPerformanceCounter( (LARGE_INTEGER *)&baseTime_ );
    }


static double elapsed() {
      unsigned __int64 val;
      QueryPerformanceCounter( (LARGE_INTEGER *)&val );
      return ((val - baseTime_)/100.0); /*  * freq_; */
    }
    /// seconds() returns the number of milliseconds (to very high resolution)
    /// elapsed since the timer was last created or reset().

static unsigned long tenmicrosseconds(void) {
double dl;
unsigned long tenmicros;
	dl = (elapsed());

	tenmicros = (unsigned long) dl;
	return(tenmicros);
}
static unsigned long reset;
static unsigned long _Profiler_get_msec(void)
{
	if (!reset)
	{
		qpreset();
		reset = 1;
//		zero = 	tenmicrosseconds();
//		printf("Z == %d\n", zero);
	}
//	printf("read clock == %d\n", tenmicrosseconds());
	return(rtp_get_system_msec());
	return(tenmicrosseconds());
}
int _times_are_the_same(unsigned long a, unsigned long b)
{
#define SIGTHRESH 10
	if (a > b && a - b > SIGTHRESH) return 0;
	if (a < b && b - a > SIGTHRESH) return 0;
	return 1;

}

#else
static int _times_are_the_same(unsigned long a, unsigned long b)
{
	return((int) (a==b));

}

static unsigned long _Profiler_get_msec(void)
{
	return(rtp_get_system_msec());
}
#endif

static void _webc_debug_log_stop_timer(void)
{
	unsigned long Now =  _Profiler_get_msec();
	timer_stopped_at = Now;
}
static void _webc_debug_log_resume_timer()
{
	unsigned long Now =  _Profiler_get_msec();
	accumulated_error += (Now - timer_stopped_at);
}



static unsigned long Profiler_get_msec(void)
{
	if (!zero)
	{
		zero = _Profiler_get_msec();
		accumulated_error = 0;
	}
	last_reading =_Profiler_get_msec()-accumulated_error;
	return(last_reading-zero);
}
static unsigned long Profiler_get_msecOffset(void)
{
	if (!last_reading)
		return(0);
	return(_Profiler_get_msec()-accumulated_error-last_reading);
}


static void _webc_debug_log_time_flush_enter(void)
{
#ifdef PRINT_ENTRIES
	if (saved_Now && saved_depth >= MINPRINTDEPTH && saved_depth <= MAXPRINTDEPTH)
	   rtp_printf("(+%-4d) (%-6d) >%-25.25s %-25.25s \r\n", saved_offset, saved_Now, savedp1, savedp2);
#endif
	saved_Now = 0;
}

#endif // WEBC_SUPPORT_DEBUG_PROFILE
