/*
|  RTPWCHAR.C -
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
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "rtpwchar.h"

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


#ifndef rtp_iswalnum
/*----------------------------------------------------------------------*
                             rtp_iswalnum
 *----------------------------------------------------------------------*/
/** @memo   Check if a value is a character or digit.

    @doc    Determine if 'chr' is an alphanumaric character 
    (between 0 and 9, a and z, or A and Z).

    @return Non-zero if it is a hexidecimal digit, 0 if not 
    a digit.
 */
int rtp_iswalnum (
  int chr                               /** Value to check. */
  )
{
int ret=0;

    if ( ((unsigned short)chr >= '0' && (unsigned short)chr <= '9') ||
         ((unsigned short)chr >= 'A' && (unsigned short)chr <= 'Z') ||
         ((unsigned short)chr >= 'a' && (unsigned short)chr <= 'z')    )
    {
        ret=1;
    }
    return (ret);
}
#endif

#ifndef rtp_iswcntrl
/*----------------------------------------------------------------------*
                             rtp_iswcntrl
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a control character.

    @doc    Determines if ch is a control character.

    @return 1 if a control character, 0 if not a control
    character.
 */
int rtp_iswcntrl (
  int ch                                /** Character to check. */
  )
{
    return (((ch > 0) && (ch < 32)) || (ch == 127));
}
#endif

#ifndef rtp_iswdigit
/*----------------------------------------------------------------------*
                             rtp_iswdigit
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a digit.

    @doc    Determines if ch is a digit between 0 and 9.

    @return Greater than 0 if it is a digit, 0 if not a digit.
 */
int rtp_iswdigit (
  int ch                                /** Character to check. */
  )
{
	return (((unsigned short)ch >= '0') && ((unsigned short)ch <= '9'));
}
#endif

#ifndef rtp_iswprint
/*----------------------------------------------------------------------*
                             rtp_iswprint
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is printable.

    @doc    Determines if ch is a printable character.

    @return 1 if printable, 0 if not printable.
 */
int rtp_iswprint (
  int ch                                /** Character to check. */
  )
{
    return (((unsigned short)ch >= ' ') && ((unsigned short)ch < 127));
}
#endif

#ifndef rtp_iswspace
/*----------------------------------------------------------------------*
                             rtp_iswspace
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a space.

    @doc    Determines if ch is a space.

    @return 1 if a space, 0 if not a space.
 */
int rtp_iswspace (
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

#ifndef rtp_iswupper
/*----------------------------------------------------------------------*
                             rtp_iswupper
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is an uppercase character.

    @doc    Determines if ch is an uppercase character.

    @return 1 if an uppercase character, 0 if not an 
    uppercase character.
 */
int rtp_iswupper (
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

#ifndef rtp_iswxdigit
/*----------------------------------------------------------------------*
                             rtp_iswxdigit
 *----------------------------------------------------------------------*/
/** @memo   Check if a character is a hexidecimal digit.

    @doc    Determines if ch is a hexidecimal digit 
    between 0 and 9, a and f, or A and F.

    @return 1 if a hexidecimal digit, 0 if not a 
    hexidecimal digit.
 */
int rtp_iswxdigit (
  int chr                                /** Character to check. */
  )
{
int ret=0;

    if ( ((unsigned short)chr >= '0' && (unsigned short)chr <= '9') ||
         ((unsigned short)chr >= 'A' && (unsigned short)chr <= 'F') ||
         ((unsigned short)chr >= 'a' && (unsigned short)chr <= 'f')    )
    {
        ret=1;
    }
    return (ret);
}
#endif

#ifndef rtp_towlower
/*----------------------------------------------------------------------*
                             rtp_towlower
 *----------------------------------------------------------------------*/
/** @memo   Convert character to lower case.

    @doc    Convert c to lower case.
             
    @return c in lower case if c is a character.
 */
int rtp_towlower (
  int c                                 /** Character to convert. */
  )
{
unsigned short cout;

    cout = (unsigned short)c;           
	if (c >= 'A' && c <= 'Z')
	{	
		cout = ((unsigned short) ((c-'A')+'a'));	
	}
	return ((int) cout);
}
#endif

#ifndef rtp_towupper
/*----------------------------------------------------------------------*
                             rtp_towupper
 *----------------------------------------------------------------------*/
/** @memo   Convert character to upper case.

    @doc    Convert c to upper case.
             
    @return c in upper case if c is a character.
 */
int rtp_towupper (
  int c                                 /** Character to convert. */
  )
{
	if ((unsigned short)c >= 'a' && (unsigned short)c <= 'z')
	{
		c = (int) ('A' + (unsigned short) (c - 'a'));
	}
	return (c);
}
#endif