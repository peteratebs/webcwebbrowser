/*
|  RTPCHAR.C -
| 
|  EBSnet -
| 
|   $Author: tony $
|   $Date: 2005/06/30 00:42:02 $
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

#include "rtpchar.h"

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

#ifndef rtp_isalnum
/*----------------------------------------------------------------------*
                             rtp_isalnum
 *----------------------------------------------------------------------*/
/** @memo   Check if a value is a character or digit.

    @doc    Determine if 'chr' is an alphanumaric character 
    (between 0 and 9, a and z, or A and Z).

    @return Non-zero if it is a hexidecimal digit, 0 if not 
    a digit.
 */
int rtp_isalnum (
  int chr                               /** Value to check. */
  )
{
int ret=0;

    if ( ((char)chr >= '0' && (char)chr <= '9') ||
         ((char)chr >= 'A' && (char)chr <= 'Z') ||
         ((char)chr >= 'a' && (char)chr <= 'z')    )
    {
        ret=1;
    }
    return (ret);
}
#endif

#ifndef rtp_iscntrl
/*----------------------------------------------------------------------*
                             rtp_iscntrl
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a control character.

    @doc    Determines if ch is a control character.

    @return 1 if a control character, 0 if not a control
    character.
 */
int rtp_iscntrl (
  int ch                                /** Character to check. */
  )
{
    return (((ch > 0) && (ch < 32)) || (ch == 127));
}
#endif

#ifndef rtp_isdigit
/*----------------------------------------------------------------------*
                             rtp_isdigit
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a digit.

    @doc    Determines if ch is a digit between 0 and 9.

    @return Greater than 0 if it is a digit, 0 if not a digit.
 */
int rtp_isdigit (
  int ch                                /** Character to check. */
  )
{
	return (((char)ch >= '0') && ((char)ch <= '9'));
}
#endif

#ifndef rtp_isprint
/*----------------------------------------------------------------------*
                             rtp_isprint
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is printable.

    @doc    Determines if ch is a printable character.

    @return 1 if printable, 0 if not printable.
 */
int rtp_isprint (
  int ch                                /** Character to check. */
  )
{
    return (((char)ch >= ' ') && ((char)ch < 127));
}
#endif

#ifndef rtp_isspace
/*----------------------------------------------------------------------*
                             rtp_isspace
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a space.

    @doc    Determines if ch is a space.

    @return 1 if a space, 0 if not a space.
 */
int rtp_isspace (
  int ch                                /** Character to check. */
  )
{
	switch (ch)
	{
    	case ' ':
    	case '\t':
    	case '\v':
    	case '\f':
    	case '\r':
    	case '\n':
		    return (1);
	}
	return (0);
}
#endif

#ifndef rtp_isupper
/*----------------------------------------------------------------------*
                             rtp_isupper
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is an uppercase character.

    @doc    Determines if ch is an uppercase character.

    @return 1 if an uppercase character, 0 if not an 
    uppercase character.
 */
int rtp_isupper (
  int ch                                /** Character to check. */
  )
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return (1);
	}
	return (0);
}
#endif

#ifndef rtp_isxdigit
/*----------------------------------------------------------------------*
                             rtp_isxdigit
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a hexidecimal digit.

    @doc    Determines if ch is a hexidecimal digit 
    between 0 and 9, a and f, or A and F.

    @return 1 if a hexidecimal digit, 0 if not a 
    hexidecimal digit.
 */
int rtp_isxdigit (
  int chr                                /** Character to check. */
  )
{
int ret=0;

    if ( ((char)chr >= '0' && (char)chr <= '9') ||
         ((char)chr >= 'A' && (char)chr <= 'F') ||
         ((char)chr >= 'a' && (char)chr <= 'f')    )
    {
        ret=1;
    }
    return (ret);
}
#endif

#ifndef rtp_tolower
/*----------------------------------------------------------------------*
                             rtp_tolower
 *----------------------------------------------------------------------*/
/** @memo   Convert character to lower case.

    @doc    Convert c to lower case.
             
    @return c in lower case if c is a character.
 */
int rtp_tolower (
  int c                                 /** Character to convert. */
  )
{
char cout;

    cout = (char)c;           
	if (c >= 'A' && c <= 'Z')
	{	
		cout = ((char) ((c-'A')+'a'));	
	}
	return ((int) cout);
}
#endif

#ifndef rtp_toupper
/*----------------------------------------------------------------------*
                             rtp_toupper
 *----------------------------------------------------------------------*/
/** @memo   Convert character to upper case.

    @doc    Convert c to upper case.
             
    @return c in upper case if c is a character.
 */
int rtp_toupper (
  int c                                 /** Character to convert. */
  )
{
	if ((char)c >= 'a' && (char)c <= 'z')
	{
		c = (int) ('A' + (char) (c - 'a'));
	}
	return (c);
}
#endif
