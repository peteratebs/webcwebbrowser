//
// WEBCSTR.CPP - Functions and classes related to the CSS property database
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "webcstr.h"
#include "util.h"
#include "html.h"
#include "webc.h"
#include "webcmem.h"


/*****************************************************************************/
// Macros
/*****************************************************************************/

#if (WEBC_SUPPORT_UNICODE)
#define WEBCSTR_NEED_WEBC_C_FNS
//#define WEBCSTR_NEED_WEBC_FNS
#endif


/*****************************************************************************/
// Function bodies
/*****************************************************************************/

static WEBC_READONLY WEBC_CHAR parens[2] = {'(', ')'};

long webc_scan_string (WEBC_CHAR *dest, WEBC_CHAR *src, long maxLen)
{
	int inparen = 0;
	long len = 0;

	// Get the next token separated by whitespace
	// ignore whitespace terminator if surround by () (for rgb( rr, gg, bb))
	while (*src && (len < (maxLen-1)) )// && (!inparen && !IS_WHITESPACE(*src)))
	{
		if (!inparen && IS_WHITESPACE(*src))
			break;
		WEBC_CHAR c = *src;
		if (c == parens[0])
			inparen = 1;
		*dest = *src;
		dest++;
		src++;
		len++;
		if (c == parens[1] && inparen)
			break;
	}
	*dest = 0;

	return (len);
}

WEBC_CHAR * webc_scan_whitespace (WEBC_CHAR *src)
{
	while (IS_WHITESPACE(*src))
	{
		src++;
	}

	return (src);
}

// If a string ends with trailing zeroes clip it at the first one
// Used to trim trailing zeroes from fractions passed back from css to javascript
void webc_trim_zeroes(WEBC_CHAR *str)
{
WEBC_CHAR *pfirst = 0;
	while (*str)
	{
		if (*str == (WEBC_CHAR) '0')
		{
			if (!pfirst)
				pfirst = str;
		}
		else
			pfirst = 0;
		str++;
	}
	if (pfirst)
		*(pfirst) = 0;
}


void webc_print_dec(WEBC_CHAR *str, long num, int min, int max)
{
	int negative = 0;
	int pos = 14;
	WEBC_CHAR temp[16];

	if (num < 0)
	{
		negative = 1;
		num = -num;
	}

	temp[15] = 0;
	while (((num > 0) || (pos > (14-min))) && (pos > 0))
	{
		temp[pos] = '0' + (WEBC_CHAR) (num % 10);
		num /= 10;
		pos--;
	}

	if (negative)
	{
		temp[pos] = '-';
	}
	else
	{
		pos++;
	}

	webc_strncpy(str, &temp[pos], max);
	str[max] = 0;
}

static WEBC_READONLY  WEBC_CHAR lowerHexDigit[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static WEBC_READONLY  WEBC_CHAR upperHexDigit[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void webc_print_hex(WEBC_CHAR *str, long num, int min, int max, int upperCase)
{
	WEBC_READONLY WEBC_CHAR *hexDigit = (upperCase)? upperHexDigit : lowerHexDigit;
	int negative = 0;
	int pos = 14;
	WEBC_CHAR temp[16];

	if (num < 0)
	{
		negative = 1;
		num = -num;
	}

	temp[15] = 0;
	while (((num > 0) || (pos > (14-min))) && (pos > 0))
	{
		temp[pos] = hexDigit[num & 0xf];
		num >>= 4;
		pos--;
	}

	if (negative)
	{
		temp[pos] = '-';
	}
	else
	{
		pos++;
	}

	webc_strncpy(str, &temp[pos], max);
	str[max] = 0;
}

WEBC_CHAR *webc_malloc_string_copy(const WEBC_CHAR *str, const char *file, unsigned int line)
{
WEBC_CHAR *newstr;

	if (!str)
	{
		return 0;
	}

	newstr = (WEBC_CHAR*) WEBC_DEBUG_MALLOC(sizeof(WEBC_CHAR) * (webc_strlen(str) + 1), (char *)file, line,"webc_malloc_string_copy", 0);
	if (newstr)
	{
		webc_strcpy(newstr,str);
	}

	return (newstr);
}

void webc_free_string_copy(WEBC_CHAR *str, const char *file, unsigned int line)
{
	if(str)
	{
		WEBC_DEBUG_FREE(str,(char *)file,line);
	}
}

#if (WEBC_SUPPORT_UNICODE)

char *webc_malloc_string_copy_8(const char *str, const char *file, unsigned int line)
{
char *newstr;

	if (!str)
	{
		return 0;
	}

	newstr = (char *) WEBC_DEBUG_MALLOC(sizeof(char) * (tc_strlen(str) + 1), (char*) file, line,"webc_malloc_string_copy_8",0);
	if (newstr)
	{
		tc_strcpy(newstr,str);
	}

	return (newstr);
}

void webc_free_string_copy_8(char *str, const char *file, unsigned int line)
{
	WEBC_DEBUG_FREE(str, (char*) file, line);
}

char * webc_arg_to_char (const WEBC_CHAR *str)
{
char *newstr;

	if (!str)
	{
		return 0;
	}

	newstr = (char *) WEBC_DEBUG_MALLOC(sizeof(char) * (webc_strlen(str) + 1), __FILE__, __LINE__,"webc_arg_to_char", 0);
	if (newstr)
	{
		int n;
		for (n=0; str[n]; n++)
		{
			newstr[n] = (char) (str[n] & 0xff);
		}
		newstr[n] = 0;
	}

	return (newstr);
}

void webc_arg_done (char *arg)
{
	WEBC_DEBUG_FREE(arg, __FILE__, __LINE__);
}

#endif

/*****************************************************************************/

#if (WEBC_SUPPORT_UNICODE)

#ifndef webc_stricmp

int webc_stricmp(const WEBC_CHAR * s1, const WEBC_CHAR * s2)
{
long index = 0;
WEBC_CHAR c1, c2;

	do
	{
		c1 = s1[index];
		c2 = s2[index];

		if (!c1 && !c2)
		{
			return 0;
		}

		// convert chars to lower case if they are letters
		if ( (c1 >= 'A') && (c1 <= 'Z') )
		{
			c1 = (WEBC_CHAR)('a' + (c1 - 'A'));
		}

		if ( (c2 >= 'A') && (c2 <= 'Z') )
		{
			c2 = (WEBC_CHAR)('a' + (c2 - 'A'));
		}

		index++;

	} while (c1 == c2);

	if (c1 < c2)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

#endif // webc_stricmp

#ifndef webc_strnicmp

BUGBUG;
int webc_strnicmp(const WEBC_CHAR * s1, const WEBC_CHAR * s2, long n)    /*__fn__*/
{
long index;
WEBC_CHAR c1, c2;

	for (index=0; index < n; index++)
	{
		c1 = s1[index];
		c2 = s2[index];

		if (!c1 && !c2)    // **NEW** check for simultaneous end of both strings
		{
			return (0);    // **NEW** return a match if we're at the end of both strings
		}

		// convert chars to lower case if they are letters
		if ( (c1 >= 'A') && (c1 <= 'Z') )
		{
			c1 = (WEBC_CHAR)('a' + (c1 - 'A'));
		}

		if ( (c2 >= 'A') && (c2 <= 'Z') )
		{
			c2 = (WEBC_CHAR)('a' + (c2 - 'A'));
		}


		if ((c1 < c2) || !c1)
		{
			return (-1);
		}
		else if ((c1 > c2) || !c2)
		{
			return (1);
		}
	}

	// if we make it to the end and all chars match, return difference == 0
	return(0);
}

#endif // webc_strnicmp

#endif // WEBC_SUPPORT_UNICODE

/*****************************************************************************/

#ifdef WEBCSTR_NEED_WEBC_C_FNS

void webc_c_strcpy (WEBC_CHAR *webcStr, const char *str)
{
	for (; *str; webcStr++, str++)
	{
		*webcStr = *str;
	}
	*webcStr = 0;
}

void webc_c_strncpy (WEBC_CHAR *webcStr, const char *str, long n)
{
	for (; n > 0; webcStr++, str++, n--)
	{
		*webcStr = *str;
		if (!*str)
		{
			break;
		}
	}
}

void webc_to_c_strcpy (char *dstCStr, const WEBC_CHAR *webcStr)
{
	// copy from WEBC_CHAR to ASCII
	int n;
	for (n=0; webcStr[n]; n++)
	{
		dstCStr[n] = (char) (webcStr[n] & 0x7f);
	}
	dstCStr[n] = 0;
}

void webc_c_strcat (WEBC_CHAR *webcStr, const char *str)
{
	while (*webcStr)
	{
		webcStr++;
	}

	webc_c_strcpy(webcStr, str);
}

void webc_c_strncat (WEBC_CHAR *webcStr, const char *str, long n)
{
	while (*webcStr)
	{
		webcStr++;
	}

	webc_c_strncpy(webcStr, str, n);
}

int webc_c_strcmp (const WEBC_CHAR *webcStr, const char *_str)
{
	const unsigned char *str = (const unsigned char *) _str;

	for (; *webcStr && *str && (*webcStr == *str); webcStr++, str++) {;}

	return ((int)*webcStr - (int)*str);
}

int webc_c_strncmp (const WEBC_CHAR *webcStr, const char *_str, long n)
{
	if (n <= 0)
	{
		return (0);
	}

	const unsigned char *str = (const unsigned char *) _str;

	for (; (n > 0) && *webcStr && *str && (*webcStr == *str); webcStr++, str++, n--) {;}

	if (n == 0)
	{
		return (0);
	}

	return ((int)*webcStr - (int)*str);
}

int webc_c_stricmp (const WEBC_CHAR *webcStr, const char *_str)
{
	const unsigned char *str = (const unsigned char *) _str;

	for (; *webcStr && *str && (UPPERCASE(*webcStr) == UPPERCASE(*str)); webcStr++, str++) {;}

	return ((int)UPPERCASE(*webcStr) - (int)UPPERCASE(*str));
}

int webc_c_strnicmp (const WEBC_CHAR *webcStr, const char *_str, long n)
{
	if (n <= 0)
	{
		return (0);
	}

	const unsigned char *str = (const unsigned char *) _str;

	for (; (n > 0) && *webcStr && *str && (UPPERCASE(*webcStr) == UPPERCASE(*str)); webcStr++, str++, n--) {;}

	if (n == 0)
	{
		return (0);
	}

	return ((int)UPPERCASE(*webcStr) - (int)UPPERCASE(*str));
}

WEBC_CHAR * webc_c_strstr (const WEBC_CHAR *str, const char *subStr)
{
	long len = tc_strlen(subStr);
	while (*str)
	{
		if (!webc_c_strncmp(str,subStr,len))
		{
			return ((WEBC_CHAR *) str);
		}
		str++;
	}

	return (0);
}

WEBC_CHAR * webc_c_stristr (const WEBC_CHAR *str, const char *subStr)
{
	long len = tc_strlen(subStr);
	while (*str)
	{
		if (!webc_c_strnicmp(str,subStr,len))
		{
			return ((WEBC_CHAR *) str);
		}
		str++;
	}

	return (0);
}

WEBC_CHAR * webc_c_strchrs (const WEBC_CHAR *str, const char *find_chrs)
{
int n;

	while (*str)
	{
		for(n=0; find_chrs[n]; n++)
		{
			if (*str == find_chrs[n])
			{
				return ((WEBC_CHAR *) str);
			}
		}
		str++;
	}
	return(0);
}

size_t webc_c_strcspn (const WEBC_CHAR *str, const char *find_chrs)
{
size_t len = 0;
int n;

	while (*str)
	{
		for(n=0; find_chrs[n]; n++)
		{
			if (*str == find_chrs[n])
			{
				return(len);
			}
		}
		str++;
		len++;
	}
	return(len);
}

size_t webc_c_strlen(const char *str)
{
	size_t len = 0;

	while (*str)
	{
		str++;
		len++;
	}
	return(len);
}


#endif // WEBCSTR_NEED_WEBC_C_FNS

/*****************************************************************************/

#ifdef WEBCSTR_NEED_WEBC_FNS
#endif // WEBCSTR_NEED_WEBC_FNS

#if (WEBC_SUPPORT_UNICODE)
#ifndef webc_atoi
long webc_atoi(const WEBC_CHAR* str)
{
	long i;
	HTML_ParseDecNumber(str, 12, &i);
	return (i);
}
#endif
#endif /* WEBC_SUPPORT_UNICODE */
