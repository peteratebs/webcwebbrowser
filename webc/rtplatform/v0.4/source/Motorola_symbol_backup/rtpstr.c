/*
|  RTPSTR.C -
| 
|  EBSnet -
| 
|   $Author: shane $
|   $Date: 2004/10/05 18:51:30 $
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

#include "rtpstr.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
/*****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/
#ifndef rtp_stricmp
/*----------------------------------------------------------------------*
                             rtp_stricmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one buffer with another.

    @doc    Compare characters of s1 to characters of s2 ignoring case.

    @return Less than 0 if s1 < s2, greater than 0 if s1 > s2, and 
    0 if s1 == s2.
 */
int rtp_stricmp (
  const char * s1,                      /** Pointer to buffer to be compared. */
  const char * s2                       /** Pointer to buffer to be compared. */
  )
{
char c1, c2;
int index=0;

	do
	{
		c1 = s1[index];
		c2 = s2[index];

		/* convert chars to lower case if they are letters */
		if ( (c1 >= 'A') && (c1 <= 'Z') )
		{
			c1 = (char)('a' + (c1 - 'A'));
		}
		if ( (c2 >= 'A') && (c2 <= 'Z') )
		{
			c2 = (char)('a' + (c2 - 'A'));
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


#ifndef rtp_stristr
/*----------------------------------------------------------------------*
                             rtp_stristr
 *----------------------------------------------------------------------*/
/** @memo   Find a string within a string.

    @doc    Find find_str in str ignoring case.

    @return The address of the first place that find_str 
    is found in str, 0 if not found.
 */
char * rtp_stristr (
  const char * str,                     /** String to search. */
  const char * find_str                 /** String to find. */
  )
{
char * ptr_str;
int find_len;

	ptr_str = (char *)0;
	find_len = rtp_strlen(find_str);

	while (*str)	/* loop until end of string */
	{
		if (!rtp_strnicmp(str, find_str, find_len))
		{
			ptr_str = (char *)str;
			break;
		}
		str++;
	}
	return (ptr_str);
}
#endif


#ifndef rtp_strncmp
/*----------------------------------------------------------------------*
                             rtp_strncmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one buffer with another.

    @doc    Compare a number of characters of s1 to a number of
    characters of s2.

    @return Less than 0 if s1 < s2, greater than 0 if s1 > s2, and 
    0 if s1 == s2.
 */
int rtp_strncmp (
  const char * s1,                      /** Pointer to buffer to be compared. */
  const char * s2,                      /** Pointer to buffer to be compared. */
  unsigned int n                        /** Number of characters to compare. */
  )
{
unsigned int index=0;

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


#ifndef rtp_strnicmp
/*----------------------------------------------------------------------*
                             rtp_strnicmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one buffer with another.

    @doc    Compare a number of characters of s1 to a number of 
    characters of s2, ignoring case.

    @return Less than 0 if s1 < s2, greater than 0 if s1 > s2, and 
    0 if s1 == s2.
 */
int rtp_strnicmp (
  const char * s1,                      /** Pointer to buffer to be compared. */
  const char * s2,                      /** Pointer to buffer to be compared. */
  unsigned int n                        /** Number of characters to compare. */
  )
{
char c1, c2;
unsigned int index=0;

	c1 = '\0';
	c2 = '\0';

	for (index=0; index < n; )
	{
	    c1 = s1[index];
		c2 = s2[index];

		/* convert chars to lower case if they are letters */
		if ( (c1 >= 'A') && (c1 <= 'Z') )
		{
			c1 = (char)('a' + (c1 - 'A'));
		}
		if ( (c2 >= 'A') && (c2 <= 'Z') )
		{
			c2 = (char)('a' + (c2 - 'A'));
		}
	    	    
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


#ifndef rtp_memchr
/*----------------------------------------------------------------------*
                             rtp_memchr
 *----------------------------------------------------------------------*/
/** @memo   Finds a character in a buffer.

    @doc    Finds the first occurance of chr in str.

    @return The location of chr in str, 0 if not
    found.
 */
void * rtp_memchr (
  const void * str,                     /** The buffer to search. */
  int chr,                              /** The character to find. */
  unsigned int n                        /** Number of characters to search in the buffer. */
  )
{
unsigned int i;
const unsigned char * p;
    
	p = (const unsigned char *)str;
    for (i = 0; i < n; i++)
    {
        if (*p++ == (unsigned char)chr)
        {
            return ((void *)(p-1));
        }
    }
    return (0);
}
#endif

#ifndef rtp_memcmp
/*----------------------------------------------------------------------*
                             rtp_memcmp
 *----------------------------------------------------------------------*/
/** @memo   Compare bytes of one buffer with another.

    @doc    Compare n bytes of a to n bytes of b.

    @return Less than 0 if a < b, greater than 0 if a > b, and 
    0 if a == b.
 */
int rtp_memcmp (
  const void * a,                       /** Pointer to a buffer to be compared. */
  const void * b,                       /** Pointer to a buffer to be compared. */
  unsigned int n                        /** Number of bytes to compare. */
  )
{
unsigned long i;
char *_a = (unsigned char *)a;
char *_b = (unsigned char *)b;

    for (i=0; i<n; i++)
    {
        if (*((unsigned char *)_a) < *((unsigned char *)_b))
        {
            return (-1);
        }
        if (*((unsigned char *)_a) > *((unsigned char *)_b))
        {
            return (1);
        }

        _a++;
        _b++;
    }
    return (0);
}
#endif

#ifndef rtp_memcpy
/*----------------------------------------------------------------------*
                             rtp_memcpy
 *----------------------------------------------------------------------*/
/** @memo   Copies bytes of one buffer to another.

    @doc    Copies n bytes of b to a.

    @return Value of a.
 */
void * rtp_memcpy (
  void *a,                              /** Pointer to destination buffer. */
  const void *b,                        /** Pointer to source buffer. */
  unsigned int n                        /** Number of bytes to copy. */
  )
{
char *_a = (unsigned char *)a;
char *_b = (unsigned char *)b;
 
	while (n--)
	{
	    *_a++ = *_b++;
	}
	return ((void *)_b);
}
#endif

#ifndef rtp_memmove
/*----------------------------------------------------------------------*
                             rtp_memmove
 *----------------------------------------------------------------------*/
/** @memo   Copies bytes of one buffer to another.

    @doc    Copies n bytes of b to a, and if they overlap, 
    b is copied over to a before it is lost.

    @return Value of a.
 */
void * rtp_memmove (
  void * a,                             /** Pointer to destination buffer. */
  const void * b,                       /** Pointer to source buffer. */
  unsigned int n                        /** Number of bytes to copy. */
  )
{
unsigned char * fr;
unsigned char * to;
unsigned int i;

	fr = (unsigned char *)b;
	to = (unsigned char *)a;
	if (b > a)
	{
		rtp_memcpy((void *)to, (const void *)fr, n);
	}
	else
	{
		fr += n;
		to += n;
		for (i=0; i<n; i++)
		{
			*(--to) =* (--fr);
		}
	}
	return (a);
}
#endif

#ifndef rtp_memset
/*----------------------------------------------------------------------*
                             rtp_memset
 *----------------------------------------------------------------------*/
/** @memo   Set a number of bytes in a buffer.

    @doc    Sets n bytes of p to the value of b.

    @return void
 */
void * rtp_memset (
  void * p,                             /** Bbuffer to be set. */
  int b,                                /** Value to set the buffer's bytes to. */
  unsigned int n                        /** Number of bytes to set. */
  )
{
char *_p = (unsigned char *)p;

    while (n--)
    {
        *_p++ = (unsigned char)b;
    }
    return (p);
}
#endif

#ifndef rtp_strcat
/*----------------------------------------------------------------------*
                             rtp_strcat
 *----------------------------------------------------------------------*/
/** @memo   Copies the source string to the end of the 
    destination string.

    @doc    Appends string b to string a.
             
    @return Value of a.
 */
char * rtp_strcat (
  char * a,                             /** Destination buffer. */
  const char * b                        /** Source buffer. */
  )
{
int ret_val;

	ret_val = rtp_strlen(a);
	rtp_strcpy(a + ret_val, b);
	return (a);
}
#endif

#ifndef rtp_strncat
/*----------------------------------------------------------------------*
                             rtp_strncat
 *----------------------------------------------------------------------*/
/** @memo   concatonate at most n char from src to end of targ;
            targ is null terminated

    @doc    Appends string b to string a.
             
    @return Value of a.
 */
char * rtp_strncat(char * targ, const char * src, int n)    /*__fn__*/
{
int len_targ;
int len_src;

	len_src = rtp_strlen(src);
	len_targ = rtp_strlen(targ);
	rtp_strncpy(targ+len_targ, src, n);

	if (len_src > n)
	{
		len_src = n;
	}

	targ[len_targ+len_src] = '\0';
	return(targ);
}
#endif


#ifndef rtp_strchr
/*----------------------------------------------------------------------*
                             rtp_strchr
 *----------------------------------------------------------------------*/
/** @memo   Find a character within a string.

    @doc    Find find_chr in str.
             
    @return The first place find_chr is found in str, 0
    if not found.
 */
char * rtp_strchr (
  const char * str,                     /** String to search. */
  int find_chr                          /** Character to find. */
  )
{
	do
	{
		if (*str == find_chr)
		{
			return ((char *)str);
		}
	} while (*str++);

	return ((char *)0);
}
#endif

#ifndef rtp_strcmp
/*----------------------------------------------------------------------*
                             rtp_strcmp
 *----------------------------------------------------------------------*/
/** @memo   Compare characters of one string with another.

    @doc    Compare characters of s1 to characters of s2.
             
    @return Less than 0 if s1 < s2, greater than 0 if 
    s1 > s2, and 0 if s1 == s2.
 */
int rtp_strcmp (
  const char * s1,                      /** String to be compared. */
  const char * s2                       /** String to be compared. */
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

#ifndef rtp_strcpy
/*----------------------------------------------------------------------*
                             rtp_strcpy
 *----------------------------------------------------------------------*/
/** @memo   Copies the source string to the destination.

    @doc    Copies string b to string a.
             
    @return Value of a.
 */
char * rtp_strcpy (
  char * a,                             /** Destination buffer. */
  const char * b                        /** Source buffer. */
  )
{
int loop_cnt=0;
	
	if (!b)
	{
		a[0] = '\0';
	}
	else
	{
		do
		{
			a[loop_cnt] = b[loop_cnt];
		} while(b[loop_cnt++]);
	}
	return (a);
}
#endif

#ifndef rtp_strlen
/*----------------------------------------------------------------------*
                             rtp_strlen
 *----------------------------------------------------------------------*/
/** @memo   Finds the length of a string.

    @doc    Determines the lenght of string.
             
    @return Length of the string.
 */
unsigned int rtp_strlen (
  const char * string                   /** String to find the length of. */
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

#ifndef rtp_strncpy
/*----------------------------------------------------------------------*
                             rtp_strncpy
 *----------------------------------------------------------------------*/
/** @memo   Copies the source string to the destination.

    @doc    Copies n characters of string b to string a.
             
    @return Value of a.
 */
char * rtp_strncpy (
  char * a,                             /** Destination buffer. */
  const char * b,                       /** Source buffer. */
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

#ifndef rtp_strrchr
/*----------------------------------------------------------------------*
                             rtp_strrchr
 *----------------------------------------------------------------------*/
/** @memo   Find the last occurance of a character within a string.

    @doc    Find last occurance of find_chr in str.
             
    @return The last occurance of find_chr found in str, 0 if not
    found.
 */
char * rtp_strrchr (
  const char * str,                     /** String to search. */
  int find_chr                          /** Character to find. */
  )
{
int i = rtp_strlen(str);

	/* loop from eos to beginning of string; */
	/* must start with \0                    */
	do
	{
		if (str[i] == find_chr)
		{
			return ((char *)(str + i));
		}
	} while(i--);

	return (0);
} 
#endif

#ifndef rtp_strstr
/*----------------------------------------------------------------------*
                             rtp_strstr
 *----------------------------------------------------------------------*/
/** @memo   Find a string within a string.

    @doc    Find last occurance of find_chr in str.
             
    @return The last occurance of find_chr found in str, 0 if not
    found.
 */
char * rtp_strstr (
  const char * str,                     /** String to search. */
  const char * find_str                 /** String to find. */
  )
{
char * ptr_str;
int find_len;

	ptr_str = (char *)0;
	find_len = rtp_strlen(find_str);

	while (*str)	/* loop until end of string */
	{
		if (!rtp_strncmp(str, find_str, find_len))
		{
			ptr_str = (char *)str;
			break;
		}
		str++;
	}
	return (ptr_str);
}
#endif

#ifndef rtp_memcat
/*----------------------------------------------------------------------*
                             rtp_memcat
 *----------------------------------------------------------------------*/
/** @memo   Write a data into the middle of a buffer, return total length
             
    @doc    copies size bytes from src to &dst[offset]
             
    @return offset + size
 */
long rtp_memcat(char * dst, long offset, const char * src, long size)
{
	if (dst)
	{
		rtp_memcpy((unsigned char*) (dst + offset), (unsigned char*) src, size);
	}
	
	return (offset + size);
}
#endif

#ifndef rtp_strpbrk
/*************************************************************************
 rtp_strpbrk() - strchr() with multiple characters.

 str - string in which to search
 find_chrs - string of characters to search for

 This routine is like strchr(), except it searches for multiple characters
 at once instead of a single character.

 Returns: Pointer to first occurrence of one of the characters in 
 'find_chrs'.
*************************************************************************/

char *rtp_strpbrk(char *str, const char *find_chrs)
{
int n;

	while (*str)
	{
		for(n=0; find_chrs[n]; n++)
		{
			if (*str == find_chrs[n])
			{
				return(str);
			}
		}
		str++;
	}
	return(0);
}
#endif

#ifndef rtp_strspn
/*----------------------------------------------------------------------*
                             rtp_strspn
 *----------------------------------------------------------------------*/
/** @memo   return length of prefix of str consisting of characters  
            in chrs
             
    @return length of prefix of str consisting of characters  
            in chrs
 */
unsigned long rtp_strspn (const char *str, const char *chrs)
{
	const char *ptr;
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

#ifndef rtp_strcspn
/*----------------------------------------------------------------------*
                             rtp_strcspn
 *----------------------------------------------------------------------*/
/** @memo   return length of prefix of str consisting of characters NOT 
            in chrs
             
    @return length of prefix of str consisting of characters NOT 
            in chrs
 */
unsigned long rtp_strcspn (const char *str, const char *chrs)
{
	const char *ptr;
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
