 /*
 | RTPENV.C - Runtime Platform Environment Services
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
#include "rtpenv.h"

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
                                rtp_setenv
 *----------------------------------------------------------------------*/
/** @memo   Set an environment variable.

    @doc    Create an environment variable, modifiy an existing 
    environment variable, or remove an environment variable. To
    remove a variable, pass 0 for its value.

    @return 0 if successful, -1 otherwise.
 */
int rtp_setenv (
  const char * envname,                 /** Name of the environment variable. */
  char * value                          /** Value of the environment variable. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                                rtp_getenv
 *----------------------------------------------------------------------*/
/** @memo   Get an environment variable.

    @doc    Get an environment variable with the name specified and
    store its value in the parameter passed in.

    @return 0 if successful, -1 otherwise.
 */
int rtp_getenv (
  const char * envname,                 /** Name of the environment variable. */
  char ** value                         /** Storage for environment variable value. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
