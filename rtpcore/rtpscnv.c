/*
|  RTPSCNV.C -
| 
|  EBSnet -
| 
|   $Author: sarah $
|   $Date: 2005/05/14 02:43:30 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
| Module description:
|  String to integers and integer to string conversion routines.
|  Integers can be ints, longs; strings can be decimal or hexidecimal
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "rtpscnv.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes */
/*****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

#ifndef rtp_hatoi
/*----------------------------------------------------------------------*
                             rtp_hatoi
 *----------------------------------------------------------------------*/
/** @memo   Convert a hex string to a decimal integer.

    @doc    Convert s to an integer.

    @return Decimal integer value of s.
 */
unsigned int rtp_hatoi (
  const char * s                        /** Hex string to convert. */
  )
{
unsigned int n;
char c;
int  cn;

	n = 0;

	if (*s == '0' && ( (*(s+1) == 'x') ||  (*(s+1) == 'X') ) )
		s += 2;

	while (*s)
	{
		c = *s;
		if (c >= 'a' && c <= 'f')
			c = (char) ((c - 'a') + 'A');
		if (c >= 'A' && c <= 'F')
			cn = (int) ((c - 'A') + 10);
		else if (c >= '0' && c <= '9')
			cn = (int) c - '0';
		else
			break;
		n = (unsigned short)(n*0x10);
	 	n += (unsigned short)cn;
		s++;
	}
	return(n);
}
#endif


#ifndef rtp_itoa
/*----------------------------------------------------------------------*
                             rtp_itoa
 *----------------------------------------------------------------------*/
/** @memo   Convert an integer to a string.

    @doc    Convert num to a string using base and store it in dest.

    @return Value of dest.
 */
char * rtp_itoa (
  int num,                              /** Integer to convert. */
  char * dest,                          /** Destination string. */
  int  base                             /** Base of conversion. */
  )
{
char tmpbuffer[33]; /* MAXINT can have 32 digits max, base 2 */

	if (!dest)
	{
		tmpbuffer[0] = '\0';
		dest = tmpbuffer;
	}
	rtp_ltoa((long) num, dest, base);
	return (dest);
}
#endif


#ifndef rtp_ltoa
/*----------------------------------------------------------------------*
                             rtp_ltoa
 *----------------------------------------------------------------------*/
/** @memo   Convert an long to a string.

    @doc    Convert num to a string using base and store it in dest.

    @return Value of dest.
 */
char * rtp_ltoa (
  long num,                             /** Long to convert. */
  char * dest,                          /** Destination string. */
  int base                              /** Base of conversion. */
  )
{
char tmpbuffer[33]; /* MAXINT can have 32 digits max, base 2 */

	if (!dest)
	{
		tmpbuffer[0] = '\0';
		dest = tmpbuffer;
	}
	if (num < 0)
	{
		num = -num;
		*dest = '-';
		rtp_ultoa((unsigned long) num, dest+1, base); 
	}
	else
	{
		rtp_ultoa((unsigned long) num, dest, base); 
	}
	return dest;
}
#endif


#ifndef rtp_ultoa
/*----------------------------------------------------------------------*
                             rtp_ultoa
 *----------------------------------------------------------------------*/
/** @memo   Convert an unsigned long to a string.

    @doc    Convert num to a string using base and store it in dest.

    @return Value of dest.
 */
char * rtp_ultoa (
  unsigned long num,                    /** Unsigned long to convert. */
  char * dest,                          /** Destination string. */
  int base                              /** Base of conversion. */
  )
{
char buffer[33]; /* MAXINT can have 32 digits max, base 2 */
char tmpbuffer[33]; /* MAXINT can have 32 digits max, base 2 */
register int digit;
char * olddest;
char * p;

	if (!dest)
	{
		tmpbuffer[0] = '\0';
		olddest = dest = tmpbuffer;
	}
	else
	{
		olddest = dest;
	}

	p = &(buffer[32]);	

	*p = '\0';

	/* Convert num to a string going from dest[31] backwards */	
	do
	{
    	digit = (int) (num % base);

#if (1)
        *(--p) =
          (char)(digit<10 ? (char)(digit + '0') : (char)((digit-10) + 'a'));
#else
        *(--p) =
          (char)(digit<10 ? (char)(digit + '0') : (char)((digit-10) + 'A'));
#endif
		num /= base;
	}
	while (num);

	/* Now put the converted string at the beginning of the buffer */
	while ((*dest++=*p++)!='\0') {;}

	return (olddest);
}
#endif


#ifndef rtp_atoi
/*----------------------------------------------------------------------*
                             rtp_atoi
 *----------------------------------------------------------------------*/
/** @memo   Convert a string to an integer.

    @doc    Convert s to an integer.

    @return Integer value of s.
 */
int rtp_atoi (
  const char * s                        /** String to be converted. */
  )
{
	return ((int) rtp_atol(s));
}
#endif

#ifndef rtp_atol
/*----------------------------------------------------------------------*
                             rtp_atol
 *----------------------------------------------------------------------*/
/** @memo   Convert a string to a long.

    @doc    Convert s to a long.

    @return Long value of s.
 */
long rtp_atol (
  const char * s                        /** String to be converted. */
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

#ifndef rtp_strtol
/*----------------------------------------------------------------------*
                             rtp_strtol
 *----------------------------------------------------------------------*/
/** @memo   Find a string within a string.

    @doc    Find last occurance of find_chr in str.
             
    @return The last occurance of find_chr found in str, 0 if not
    found.
 */
long rtp_strtol (
  const char * str,                     /** String to be converted. */
  char ** delimiter,                    /** Character that stops the convertion. */
  int base                              /** Base of return value. */
  )
{
const char *s = str;
char sign     = '+';
long result   = 0;
long temp     = 0;
long val      = 0;

	if (delimiter)
	{
		*delimiter = (char *) str;
	}
    while ((*s == ' ')  ||
		   (*s == '\t') ||
		   (*s == '\v') ||
		   (*s == '\f') ||
		   (*s == '\r') ||
		   (*s == '\n'))
    {
        s++;
    }
    
    if ((*s == '-') || (*s == '+'))
    {
        sign = *s++;
    }
    
    if (*s == '0')
    {
        s++;
        if ((*s == 'x') || (*s == 'X'))
        {
            s++;
            base = 16;          /* hexidecimal */
        }
        else
        {
            base = 8;           /*    octal    */
        }
    }
    else if (base == 0)
    {
        base = 10;              /*   decimal   */
    }

    while (*s)
    {
        if ((*s >= '0') && (*s <= '9'))
        {
            temp   = temp * base;
            val    = (*s - '0');
            if (val >= base)
            {
				if (delimiter)
				{
					*delimiter = (char *) s;
				}
                break;
            }
            result = val + temp;
            temp   = result;
        }
        else if (*s >= 'A' && *s <= 'Z')
        {
            temp   = temp * base;
            val    = (*s - ('A' - 10));
            if (val >= base)
            {
				if (delimiter)
				{
					*delimiter = (char *) s;
				}
                break;
            }
            result = val + temp;
            temp   = result;
        }
        else if (*s >= 'a' && *s <= 'z')
        {
            temp   = temp * base;
            val    = (*s - ('a' - 10));
            if (val >= base)
            {
				if (delimiter)
				{
					*delimiter = (char *) s;
				}
                break;
            }
            result = val + temp;
            temp   = result;
        }
        else
        {
			if (delimiter)
			{
				*delimiter = (char *) s;
			}
            break;
        }
        *s++;
    }
    
    if (result)
    {
        if (sign == '-')
        {
            result = -result;
        }
    }
    
    return (result);
}
#endif

#ifndef rtp_strtoul
/*----------------------------------------------------------------------*
                             rtp_strtoul
 *----------------------------------------------------------------------*/
/** @memo   Convert a string to an unsigned long.

    @doc    Determine the long value of the passed string.  If base 
    is 0 then the base is determined by the format of the string; 
    otherwise the base is assumed to be correct.
             
    @return Unsigned long representation of the passed string to
    the delimiter, 0 on error.
 */
unsigned long rtp_strtoul (
  const char * str,                     /** String to be converted. */
  char ** delimiter,                    /** Character that stops the convertion. */
  int base                              /** Base of return value. */
  )
{
    return ((unsigned long) rtp_strtol (str, delimiter, base));
}
#endif
