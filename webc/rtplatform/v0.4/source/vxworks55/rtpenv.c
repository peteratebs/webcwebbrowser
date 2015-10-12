 /*
 | RTPENV.C - Runtime Platform Environment Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: manish $
 |  $Date: 2006/10/17 06:08:51 $
 |  $Name:  $
 |  $Revision: 1.1 $
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

#include <envlib.h>

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
int setenv(const char *pEnvName, const char *pEnvVal, int iOverwrite);

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
    return setenv( envname, value, 1);
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
    if ((*value = getenv(envname)) == NULL)
    {
        return (-1);
    }
    return (0);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

int setenv(const char *pEnvName, const char *pEnvVal, int iOverwrite)
{
   int iReturnValue = -1 ;
   char *pEnvStr ;
   pEnvStr = (char *)malloc( strlen(pEnvName) + strlen(pEnvVal) + 2 ) ;
   if ( pEnvStr == NULL )
   {
      iReturnValue = -1 ;
   }
   else
   {
      strcpy( pEnvStr, pEnvName );
      strcat( pEnvStr, "=" );
      strcat( pEnvStr, pEnvVal );
      if ( iOverwrite == 0 )
      {
         if ( getenv( pEnvName ) == NULL )
         {
            if ( putenv( pEnvStr ) == OK )
            {
               iReturnValue = 0 ;
            }
            else
            {
               iReturnValue = -1 ;
            }
         }
         else
         {
            iReturnValue = 0 ;
         }
      }
      else
      {
         if ( putenv( pEnvStr ) == OK )
         {
            iReturnValue = 0 ;
         }
         else
         {
            iReturnValue = -1 ;
         }
      }
   }
   free( pEnvStr );
   return iReturnValue ;
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
