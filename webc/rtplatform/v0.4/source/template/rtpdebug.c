 /*
 | RTPDEBUG.C - Runtime Platform Debug Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/



/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_debug_output_str
 *----------------------------------------------------------------------*/
/** @memo   Outputs a debug string.

    @doc    Outputs a debug string.  This is not ISR safe.
    This should not be referenced from within an interrupt.
    
    @return void
 */
void _rtp_debug_output_str (
	char* msg,
	const char *file,
	long line_num
	)
{
    rtp_not_yet_implemented ();
    return (0);
}


/*----------------------------------------------------------------------*
                         rtp_debug_output_int
 *----------------------------------------------------------------------*/
/** @memo   Outputs a debug integer.

    @doc    Outputs a debug integer.  This is not ISR safe.
    This should not be referenced from within an interrupt.
    
    @return void
 */
void _rtp_debug_output_int (
	long val
	)
{
    rtp_not_yet_implemented ();
    return (0);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
