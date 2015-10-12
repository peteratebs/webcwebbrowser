/*
|  RTPWCS.C -
| 
|  EBSnet -
| 
|   $Author: tony $
|   $Date: 2005/06/30 00:42:02 $
|   $Name:  $
|   $Revision: 1.5 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "rtpwcs.h"
#ifndef rtp_wcstol
#include "rtpwchar.h"
#endif

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

#ifndef rtp_wcsicmp
/*----------------------------------------------------------------------*
                             rtp_wcsicmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one unicode buffer with another.

    @doc    Compare characters of s1 to characters of s2 ignoring case.

    @return Less than 0 if s1 < s2, greater than 0 if s1 > s2, and 
    0 if s1 == s2.
 */
int rtp_wcsicmp (
  const unsigned short * s1,               /** Pointer to unicode buffer to be compared. */
  const unsigned short * s2                /** Pointer to unicode buffer to be compared. */
  )
{
unsigned short c1, c2;
int index=0;

	do
	{
		c1 = s1[index];
		c2 = s2[index];

		/* convert chars to lower case if they are letters */
		if ( (c1 >= 'A') && (c1 <= 'Z') )
		{
			c1 = (unsigned short)('a' + (c1 - 'A'));
		}
		if ( (c2 >= 'A') && (c2 <= 'Z') )
		{
			c2 = (unsigned short)('a' + (c2 - 'A'));
		}

		index++;
	} while (c1 == c2 && c1 && c2);

	if (!c1 && !c2)
	{
		return (0);
	}
	if (c1 < c2)
	{
		return (-1);
	}
    return (1);
}
#endif


#ifndef rtp_wcsistr
/*----------------------------------------------------------------------*
                             rtp_wcsistr
 *----------------------------------------------------------------------*/
/** @memo   Find a unicode string within a string.

    @doc    Find find_str in str ignoring case.

    @return The address of the first place that find_str 
    is found in str, 0 if not found.
 */
unsigned short * rtp_wcsistr (
  const unsigned short * str,                     /** Unicode string to search. */
  const unsigned short * find_str                 /** Unicode string to find. */
  )
{
unsigned short * ptr_str;
int find_len;

	ptr_str = (unsigned short *)0;
	find_len = rtp_wcslen(find_str);

	while (*str)	/* loop until end of string */
	{
		if (!rtp_wcsnicmp(str, find_str, find_len))
		{
			ptr_str = (unsigned short *)str;
			break;
		}
		str++;
	}
	return (ptr_str);
}
#endif


#ifndef rtp_wcsncmp
/*----------------------------------------------------------------------*
                             rtp_wcsncmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one unicode buffer with another.

    @doc    Compare a number of characters of s1 to a number of
    characters of s2.

    @return Less than 0 if s1 < s2, greater than 0 if s1 > s2, and 
    0 if s1 == s2.
 */
int rtp_wcsncmp (
  const unsigned short * s1,                      /** Pointer to unicode buffer to be compared. */
  const unsigned short * s2,                      /** Pointer to unicodebuffer to be compared. */
  unsigned int n                                  /** Number of characters to compare. */
  )
{
	unsigned int index=0;

	if (n == 0)
	{
		return (0);
	}

	for (index=0; index < n; )
	{
	    if (s1[index] == s2[index] && s1[index] && s2[index])
	    {
	        index++;
	        if (index == n)
	        {
	            return (0);
	        }
	    }
	    else
	    {
	        break;
	    }
	}

	if (!s1[index] && !s2[index])
	{
		return (0);
	}
	if (s1[index] < s2[index])
	{
		return (-1);
	}
    return (1);
}
#endif


#ifndef rtp_wcsnicmp
/*----------------------------------------------------------------------*
                             rtp_wcsnicmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one unicode buffer with another.

    @doc    Compare a number of characters of s1 to a number of 
    characters of s2, ignoring case.

    @return Less than 0 if s1 < s2, greater than 0 if s1 > s2, and 
    0 if s1 == s2.
 */
int rtp_wcsnicmp (
  const unsigned short * s1,            /** Pointer to unicode buffer to be compared. */
  const unsigned short * s2,            /** Pointer to buffer to be compared. */
  unsigned int n                        /** Number of characters to compare. */
  )
{
unsigned short c1, c2;
unsigned int index=0;

	c1 = '\0';
	c2 = '\0';

	for (index=0; index < n; )
	{
	    c1 = s1[index];
		c2 = s2[index];

		/* convert chars to lower case if they are letters */
		if ( (c1 >= 'A') && (c1 <= 'Z') )
			c1 = (unsigned short)('a' + (c1 - 'A'));
		if ( (c2 >= 'A') && (c2 <= 'Z') )
			c2 = (unsigned short)('a' + (c2 - 'A'));
	    
	    
	    if ((c1 == c2) && c1 && c2)
	    {
	        index++;
	        if (index == n)
	        {
	            return (0);
	        }
	    }
	    else
	    {
	        break;
	    }
	}

	if (!c1 && !c2)
	{
		return (0);
	}
	if (c1 < c2)
	{
		return (-1);
	}
    return (1);
}
#endif

#ifndef rtp_wcscat
/*----------------------------------------------------------------------*
                             rtp_wcscat
 *----------------------------------------------------------------------*/
/** @memo   Copies the source string to the end of the 
    destination string.

    @doc    Appends string b to string a.
             
    @return Value of a.
 */
unsigned short * rtp_wcscat (
  unsigned short * a,                             /** Destination buffer. */
  const unsigned short * b                        /** Source buffer. */
  )
{
int ret_val;

	ret_val = rtp_wcslen(a);
	rtp_wcscpy(a + ret_val, b);
	return (a);
}
#endif

#ifndef rtp_wcschr
/*----------------------------------------------------------------------*
                             rtp_wcschr
 *----------------------------------------------------------------------*/
/** @memo   Find a character within a string.

    @doc    Find find_chr in str.
             
    @return The first place find_chr is found in str, 0
    if not found.
 */
unsigned short * rtp_wcschr (
  const unsigned short * str,           /** String to search. */
  int find_chr                          /** Character to find. */
  )
{
	do
	{
		if (*str == find_chr)
		{
			return ((unsigned short *)str);
		}
	} while (*str++);

	return ((unsigned short *)0);
}
#endif

#ifndef rtp_wcscmp
/*----------------------------------------------------------------------*
                             rtp_wcscmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one string with another.

    @doc    Compare characters of s1 to characters of s2.
             
    @return Less than 0 if s1 < s2, greater than 0 if 
    s1 > s2, and 0 if s1 == s2.
 */
int rtp_wcscmp (
  const unsigned short * s1,                      /** String to be compared. */
  const unsigned short * s2                       /** String to be compared. */
  )
{
int index=0;

	while (s1[index] == s2[index] && s1[index] && s2[index])
	{
		index++;
	}

	if (!s1[index] && !s2[index])
	{
		return (0);
	}
	if (s1[index] < s2[index])
	{
		return (-1);
	}
    return (1);
}
#endif

#ifndef rtp_wcscpy
/*----------------------------------------------------------------------*
                             rtp_wcscpy
 *----------------------------------------------------------------------*/
/** @memo   Copies the source string to the destination.

    @doc    Copies string b to string a.
             
    @return Value of a.
 */
unsigned short * rtp_wcscpy (
  unsigned short * a,                             /** Destination buffer. */
  const unsigned short * b                        /** Source buffer. */
  )
{
int loop_cnt=0;

	do
	{
		a[loop_cnt] = b[loop_cnt];
	} while(b[loop_cnt++]);
	return (a);
}
#endif

#ifndef rtp_wcslen
/*----------------------------------------------------------------------*
                             rtp_wcslen
 *----------------------------------------------------------------------*/
/** @memo   Finds the length of a string.

    @doc    Determines the lenght of string.
             
    @return Length of the string.
 */
unsigned int rtp_wcslen (
  const unsigned short * string                   /** String to find the length of. */
  )
{
unsigned int len=0;

    while (string[len] != 0)
    {
        len++;
    }
    return (len);
}
#endif

#ifndef rtp_wcsncpy
/*----------------------------------------------------------------------*
                             rtp_wcsncpy
 *----------------------------------------------------------------------*/
/** @memo   Copies the source string to the destination.

    @doc    Copies n characters of string b to string a.
             
    @return Value of a.
 */
unsigned short * rtp_wcsncpy (
  unsigned short * a,                   /** Destination buffer. */
  const unsigned short * b,             /** Source buffer. */
  unsigned int n                        /** Number of characters to copy from. */
  )
{
unsigned int loop_cnt;

	for (loop_cnt=0; loop_cnt < n; loop_cnt++)
	{
		a[loop_cnt] = b[loop_cnt];
		if (!b[loop_cnt])
		{
			break;
		}
	} 
											
	return (a);
}
#endif

#ifndef rtp_wcsrchr
/*----------------------------------------------------------------------*
                             rtp_wcsrchr
 *----------------------------------------------------------------------*/
/** @memo   Find the last occurance of a character within a string.

    @doc    Find last occurance of find_chr in str.
             
    @return The last occurance of find_chr found in str, 0 if not
    found.
 */
unsigned short * rtp_wcsrchr (
  const unsigned short * str,                     /** String to search. */
  int find_chr                          /** Character to find. */
  )
{
int i = rtp_wcslen(str);

	/* loop from eos to beginning of string; */
	/* must start with \0                    */
	do
	{
		if (str[i] == find_chr)
		{
			return ((unsigned short *)(str + i));
		}
	} while(i--);

	return (0);
} 
#endif

#ifndef rtp_wcsstr
/*----------------------------------------------------------------------*
                             rtp_wcsstr
 *----------------------------------------------------------------------*/
/** @memo   Find a string within a string.

    @doc    Find last occurance of find_chr in str.
             
    @return The last occurance of find_chr found in str, 0 if not
    found.
 */
unsigned short * rtp_wcsstr (
  const unsigned short * str,                     /** String to search. */
  const unsigned short * find_str                 /** String to find. */
  )
{
unsigned short * ptr_str;
int find_len;

	ptr_str = (unsigned short *)0;
	find_len = rtp_wcslen(find_str);

	while (*str)	/* loop until end of string */
	{
		if (!rtp_wcsncmp(str, find_str, find_len))
		{
			ptr_str = (unsigned short *)str;
			break;
		}
		str++;
	}
	return (ptr_str);
}
#endif

#ifndef rtp_wcstol
/*----------------------------------------------------------------------*
                             rtp_wcstol
 *----------------------------------------------------------------------*/
/** @memo   Find a string within a string.

    @doc    Find last occurance of find_chr in str.
             
    @return The last occurance of find_chr found in str, 0 if not
    found.
 */
long rtp_wcstol (
  const unsigned short * str,                     /** String to be converted. */
  unsigned short ** delimiter,                    /** Character that stops the convertion. */
  int base                              /** Base of return value. */
  )
{
const unsigned short *s = str;
unsigned short sign     = '+';
long result   = 0;
long temp     = 0;
long val      = 0;

	if (delimiter)
	{
		*delimiter = (unsigned short *) str;    
	}

    while (rtp_iswspace(*s))
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
        if (rtp_iswdigit(*s))
        {
            temp   = temp * base;
            val    = (*s - '0');
            if (val >= base)
            {
				if (delimiter)
				{
					*delimiter = (unsigned short *) s;
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
	                *delimiter = (unsigned short *) s;
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
	                *delimiter = (unsigned short *) s;
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
	            *delimiter = (unsigned short *) s;
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

#ifndef rtp_wcsncat
/*----------------------------------------------------------------------*
                             rtp_wcsncat
 *----------------------------------------------------------------------*/
/** @memo   concatonate at most n char from src to end of targ;
            targ is null terminated

    @doc    Appends string b to string a.
             
    @return Value of a.
 */
unsigned short * rtp_wcsncat(unsigned short * targ, const unsigned short * src, int n)    /*__fn__*/
{
int len_targ;

	len_targ = rtp_wcslen(targ);
	rtp_wcsncpy(targ+len_targ, src, n);
	targ[len_targ+n] = '\0';
	return(targ);
}
#endif

#ifndef rtp_wcsspn
/*----------------------------------------------------------------------*
                             rtp_wcsspn
 *----------------------------------------------------------------------*/
/** @memo   return length of prefix of str consisting of characters  
            in chrs
             
    @return length of prefix of str consisting of characters  
            in chrs
 */
unsigned long rtp_wcsspn (const unsigned short *str, const unsigned short *chrs)
{
	const unsigned short *ptr;
	unsigned long len = 0;
	
	while (str[0])
	{
		ptr = chrs;
		while (ptr[0])
		{
			if (str[0] == ptr[0])
			{
				break;
			}
			ptr++;
		}
		
		if (!ptr[0])
		{
			break;
		}
		
		len++;
		str++;
	}

	return (len);
}
#endif

#ifndef rtp_wcscspn
/*----------------------------------------------------------------------*
                             rtp_wcscspn
 *----------------------------------------------------------------------*/
/** @memo   return length of prefix of str consisting of characters NOT 
            in chrs
             
    @return length of prefix of str consisting of characters NOT 
            in chrs
 */
unsigned long rtp_wcscspn (const unsigned short *str, const unsigned short *chrs)
{
	const unsigned short *ptr;
	unsigned long len = 0;
	
	while (str[0])
	{
		ptr = chrs;
		while (ptr[0])
		{
			if (str[0] == ptr[0])
			{
				break;
			}
			ptr++;
		}
		
		if (ptr[0])
		{
			break;
		}
		
		len++;
		str++;
	}

	return (len);
}
#endif
