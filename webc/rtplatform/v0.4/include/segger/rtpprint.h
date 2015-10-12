
#ifndef __RTPPRINT_H__
#define __RTPPRINT_H__

#include "rtp.h"


/************************************************************************
* Configuration Define                                                  *
*************************************************************************/
/* ----------------------------------- */
/*  For optimization purposes, set to  */
/*  1 so that the native print         */
/*  functions will be macroed in. This */
/*  is warned against, because a       */
/*  number of print implementations    */
/*  are inconsistant. By enabling this */
/*  flag, it will also cause inclusion */
/*  of native header files throughout  */
/*  the products that make use of this */
/*  module.                            */
/* ----------------------------------- */
#define RTP_INCLUDE_NATIVE_PRINTF      1

/************************************************************************
 * Native print library utility functions includes                      *
 *      printf      vprintf      va_list    va_end                      *
 *      sprintf     vsprintf     va_arg                                 *
 *      snprintf    vsnprintf    va_start                               *
 ************************************************************************/

#if (RTP_INCLUDE_NATIVE_PRINTF)
/************************************************************************/
/* Include the compiler header file that contains the above             */
/* functions.  If not supplied by the compiler, or not implemented      */
/* properly; comment out the defines below to use the supplied         */
/* version in rtpprint.c (rtpprint.c does not need porting).            */
/************************************************************************/
#include <stdio.h>
#include <stdarg.h>


/************************************************************************
* Defines                                                               *
*************************************************************************/
#define rtp_printf     printf
#define rtp_sprintf    sprintf
#define rtp_snprintf   _snprintf
#define rtp_vprintf    vprintf
#define rtp_vsprintf   vsprintf
#define rtp_vsnprintf  _vsnprintf

#define rtp_va_list   va_list
#define rtp_va_arg    va_arg
#define rtp_va_start  va_start
#define rtp_va_end    va_end

#endif /* RTP_INCLUDE_NATIVE_PRINTF */

/************************************************************************
* If the above are undefined for the current implementation, default    *
* RTPlatform implementation will be included.  Set the following        *
* defines according to the protocol needs.                              *
*************************************************************************/
/* -------------------------------------------- */
/*              Add printf support.             */
/* -------------------------------------------- */
#define RTP_INCLUDE_PRINTF                      0

/* -------------------------------------------- */
/*              Add sprintf support.            */
/* -------------------------------------------- */
#define RTP_INCLUDE_SPRINTF                     0


/************************************************************************/
/*      THERE IS NO NEED TO CHANGE ANYTHING BELOW THIS COMMENT         */
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
* Typedefs                                                              *
*************************************************************************/

/************************************************************************
* Macros                                                                *
*************************************************************************/
#if (!defined (rtp_va_list) || !defined (rtp_va_arg) || !defined (rtp_va_start) || !defined (rtp_va_end))

#if defined (rtp_va_list)
#undef rtp_va_list
#endif
#if defined (rtp_va_arg)
#undef rtp_va_arg
#endif
#if defined (rtp_va_start)
#undef rtp_va_start
#endif
#if defined (rtp_va_end)
#undef rtp_va_end
#endif


#define _RTPINTSIZEOF(n)          ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

typedef void * rtp_va_list;
#define rtp_va_arg(ap, type)      ( (char*)ap += _RTPINTSIZEOF(type) , *((type*)((char*)ap - _RTPINTSIZEOF(type))) )
#define rtp_va_start(ap, lastarg) ( ap = (rtp_va_list)((char*)&(lastarg) + _RTPINTSIZEOF(lastarg)) )
#define rtp_va_end(ap)            ( ap = (rtp_va_list)0 )

#endif /* !defined (rtp_va_list) || !defined (rtp_va_arg) || !defined (rtp_va_start) || !defined (rtp_va_end) */

/************************************************************************
* Prototypes                                                            *
*************************************************************************/

#if (RTP_INCLUDE_PRINTF)
#ifndef rtp_printf
/*----------------------------------------------------------------------
 rtp_printf - print formatted output
----------------------------------------------------------------------*/
int             rtp_printf     (const char * f, ...);
#endif


#ifndef rtp_vprintf
/*----------------------------------------------------------------------
 rtp_vprintf - print a list of formatted data to a string
----------------------------------------------------------------------*/
int             rtp_vprintf    (const char * f, rtp_va_list ap);
#endif
#endif /* RTP_INCLUDE_PRINTF */


#if (RTP_INCLUDE_SPRINTF)
#ifndef rtp_sprintf
/*----------------------------------------------------------------------
 rtp_sprintf - print formatted data to a string
----------------------------------------------------------------------*/
int             rtp_sprintf    (char * wh, const char * f, ...);
#endif


#ifndef rtp_snprintf
/*----------------------------------------------------------------------
 rtp_snprintf - print formatted data to a string
----------------------------------------------------------------------*/
int             rtp_snprintf   (char * wh, int bufflen, const char * f, ...);
#endif


#ifndef rtp_vsprintf
/*----------------------------------------------------------------------
 rtp_vsprintf - print a list of formatted data to a string
----------------------------------------------------------------------*/
int             rtp_vsprintf   (char * wh, const char * f, rtp_va_list ap);
#endif


#ifndef rtp_vsnprintf
/*----------------------------------------------------------------------
 rtp_vsnprintf - print a list of formatted data to a string
----------------------------------------------------------------------*/
int             rtp_vsnprintf  (char * wh, long bufflen, const char * f, rtp_va_list ap);
#endif
#endif /* RTP_INCLUDE_SPRINTF */

#ifdef __cplusplus
}
#endif

#endif /*__RTPPRINT_H__*/

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
