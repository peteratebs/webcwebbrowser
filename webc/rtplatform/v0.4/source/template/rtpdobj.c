/*
|  RTPDOBJ.C -
| 
|  EBSnet -
| 
|   $Author: shane $
|   $Date: 2004/10/14 18:33:38 $
|   $Name:  $
|   $Revision: 1.4 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/
#include "rtpdobj.h"

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

/*----------------------------------------------------------------------*
                             rtp_file_gfirst
 *----------------------------------------------------------------------*/
int rtp_file_gfirst (void ** dirobj, char * name)
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_file_gnext
 *----------------------------------------------------------------------*/
/** @memo   Get the next occurrence of a file.

    @doc    Get the next occurrence of a file indicated by the 
    dirobj, and store the related information, overwriting the 
    current information.
    
    @precondition <b>MUST</b> have called rtp_file_gfirst to obtain 
    the dirobj before referencing this function.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_gnext (
  void * dirobj                         /** File information to be altered. */
  )
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                             rtp_file_gdone
 *----------------------------------------------------------------------*/
/** @memo   Free the directory information originally allocated
    by rtp_file_gfirst.

    @doc    Given a pointer the file information that was setup by 
    a call to rtp_file_gfirst, free internal elements used by the 
    dirobj.
    
    @precondition This function <b>MUST</b> be called after rtp_file_gfirst, 
    and rtp_file_gnext are done being called so to avoid a memory
    leak.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
void rtp_file_gdone (
  void * dirobj                         /** File information to be freed. */
  )
{
	rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                            rtp_file_get_size
 *----------------------------------------------------------------------*/
/** @memo   Get the size of a file.

    @doc    Determines the size of the file referred to by the dirobj 
    handle returned by the call to rtp_file_gfirst and any successive 
    calls to rtp_file_gnext.

    @return 0 if successful, -1 otherwise.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_get_size (
  void * dirobj,                        /** The dirobj handle returned by
                                            rtp_file_gfirst, and rtp_file_gnext. */
  unsigned long * size                  /** The storage location for the
                                            acquired size. */
  )
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_attrib
 *----------------------------------------------------------------------*/
/** @memo   Retrieves the files associated attributes.

    @doc    Returns the attributes that have been assigned to the file.
    The attributes include:<br>
	<pre>
|		RTP_FILE_ATTRIB_ISDIR       if it is a directory
|		RTP_FILE_ATTRIB_ISVOL       if it is a volume
|		RTP_FILE_ATTRIB_RDONLY      if it is read only
|		RTP_FILE_ATTRIB_WRONLY      if it is write only
|		RTP_FILE_ATTRIB_RDWR        if it has read and write permissions
|		RTP_FILE_ATTRIB_HIDDEN      if it is hidden
|		RTP_FILE_ATTRIB_SYSTEM      if it is a system file
|		RTP_FILE_ATTRIB_ARCHIVE     if it is archived
	</pre>

    @return 0 if successful, -1 otherwise.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_get_attrib (
  void * dirobj,                        /** The dirobj handle returned by
                                            rtp_file_gfirst, and rtp_file_gnext. */
  unsigned char * attributes            /** The storage location to return the
                                            attributes that have been assigned
                                            to the file. */
  )
{
	rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_name
 *----------------------------------------------------------------------*/
/** @memo   Retrieves the name of the file.

    @doc    Returns the name of the file associated with the dirobj 
    in the storage location.  The native file system name will be 
    truncated if the storage location is not large enough to contain 
    it.

    @return 0 if successful, -1 otherwise.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_get_name (
  void * dirobj,                        /** The dirobj handle returned by 
                                            rtp_file_gfirst, and rtp_file_gnext. */
  char * name,                          /** The storage location to return the 
                                            name of the file. */
  int size                              /** The size of the storage location. */
  )
{
	rtp_not_yet_implemented ();
    return (-1);
}



/*----------------------------------------------------------------------*
                           rtp_file_get_time
 *----------------------------------------------------------------------*/
/** @memo   Retrieves the last access time, last write time, 
    creation time, and last time the mode was changed.

    @doc    Gets the time of last access, last write, creation and
    modification using the dirobj returned by rtp_file_gfirst
    and any successive calls to rtp_file_gnext.  If a time is
    not to be obtained, pass a NULL to indicate no storage
    location.

    @return 0 if successful, -1 otherwise.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_get_time (
  void * dirobj,                        /** The dirobj handle returned by 
                                            rtp_file_gfirst, and rtp_file_gnext. */
  RTP_DATE * adate,                     /** The storage location for date of last access. */
  RTP_DATE * wdate,                     /** The storage location for date of last write. */
  RTP_DATE * cdate,                     /** The storage location for date of creation. */
  RTP_DATE * hdate                      /** The storage location for date of last file mode change. */
  )
{
	rtp_not_yet_implemented ();
    return (-1);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
