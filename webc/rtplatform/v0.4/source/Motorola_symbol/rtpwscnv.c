/*
|  RTPWSCNV.C -
| 
|  EBSnet -
| 
|   $Author: shane $
|   $Date: 2004/10/05 18:51:30 $
|   $Name:  $
|   $Revision: 1.2 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "rtpwscnv.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
*****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

#ifndef rtp_itow
/*----------------------------------------------------------------------*
                             rtp_itow
 *----------------------------------------------------------------------*/
/** @memo   Convert an integer to a unicode string.

    @doc    Convert num to a unicode string using base and store it in dest.

    @return Value of dest.
 */
unsigned short * rtp_itow (
  int num,                              /** Integer to convert. */
  unsigned short * dest,                /** Destination unicode string. */
  int  base                             /** Base of conversion. */
  )
{
	return (rtp_ltow((long) num, dest, base));
}
#endif


#ifndef rtp_itow
/*----------------------------------------------------------------------*
                             rtp_ltow
 *----------------------------------------------------------------------*/
/** @memo   Convert an long to a unicode string.

    @doc    Convert num to a uniocde string using base and store it in dest.

    @return Value of dest.
 */
unsigned short * rtp_ltow (
  long num,                             /** Long to convert. */
  unsigned short * dest,                /** Destination unicode string. */
  int base                              /** Base of conversion. */
  )
{
	if (num < 0)
	{
		num = -num;
		*dest = '-';
		rtp_ultow((unsigned long) num, dest+1, base); 
	}
	else
	{
		rtp_ultow((unsigned long) num, dest, base); 
	}
	return dest;
}
#endif


#ifndef rtp_itow
/*----------------------------------------------------------------------*
                             rtp_ultow
 *----------------------------------------------------------------------*/
/** @memo   Convert an unsigned long to a uniocde string.

    @doc    Convert num to a unicode string using base and store it in dest.

    @return Value of dest.
 */
unsigned short * rtp_ultow (
  unsigned long num,                    /** Unsigned long to convert. */
  unsigned short * dest,                /** Destination unicode string. */
  int base                              /** Base of conversion. */
  )
{
unsigned short buffer[33]; /* MAXINT can have 32 digits max, base 2 */
register int digit;
unsigned short * olddest = dest;
unsigned short * p;

	p = &(buffer[32]);	

	*p = '\0';

	/* Convert num to a string going from dest[31] backwards */	
	do
	{
    	digit = (int) (num % base);

#if (1)
        *(--p) =
          (unsigned short)(digit<10 ? (unsigned short)(digit + '0') : (unsigned short)((digit-10) + 'a'));
#else
        *(--p) =
          (unsigned short)(digit<10 ? (unsigned short)(digit + '0') : (unsigned short)((digit-10) + 'A'));
#endif
		num /= base;
	}
	while (num);

	/* Now put the converted string at the beginning of the buffer */
	while ((*dest++=*p++)!='\0');

	return (olddest);
}
#endif

#ifndef rtp_wtoi
/*----------------------------------------------------------------------*
                             rtp_wtoi
 *----------------------------------------------------------------------*/
/** @memo   Convert a unicode string to an integer.

    @doc    Convert s to an integer.

    @return Integer value of s.
 */
int rtp_wtoi (
  const unsigned short * s                /** Unicode string to be converted. */
  )
{
	return ((int) rtp_wtol(s));
}
#endif

#ifndef rtp_wtol
/*----------------------------------------------------------------------*
                             rtp_wtol
 *----------------------------------------------------------------------*/
/** @memo   Convert a unicode string to a long.

    @doc    Convert s to a long.

    @return Long value of s.
 */
long rtp_wtol (
  const unsigned short * s                /** String to be converted. */
  )
{
long n;
unsigned int neg;

	/* skip over tabs and spaces */
	while ( (*s == ' ') || (*s == '\t') )
	{
		s++;
	}

	n = 0;
	neg = 0;

	if (*s == '-')
	{
		neg = 1;
		s++;
	}

	while (*s && (*s >= '0' && *s <= '9'))
	{
		n = n * 10;
	 	n += (long) (*s - '0');  
		s++;
	}

	if (neg)
	{
		n = 0 - n;
	}

	return(n);
}
#endif


#ifndef rtp_wcstol
/*----------------------------------------------------------------------*
                             rtp_wcstoul
 *----------------------------------------------------------------------*/
/** @memo   Convert a string to an unsigned long.

    @doc    Determine the long value of the passed string.  If base 
    is 0 then the base is determined by the format of the string; 
    otherwise the base is assumed to be correct.
             
    @return Unsigned long representation of the passed string to
    the delimiter, 0 on error.
 */
unsigned long rtp_wcstoul (
  const unsigned short * str,                     /** String to be converted. */
  unsigned short ** delimiter,                    /** Character that stops the convertion. */
  int base                              /** Base of return value. */
  )
{
    return ((unsigned long) rtp_wcstol (str, delimiter, base));
}
#endif
