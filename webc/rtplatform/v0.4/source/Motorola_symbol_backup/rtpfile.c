 /*
 | RTPFILE.C - Runtime Platform Network Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/14 18:33:38 $
 |  $Name:  $
 |  $Revision: 1.5 $
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
#include "rtpfile.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Structures
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                               rtp_file_open
 *----------------------------------------------------------------------*/
/** @memo   Opens a file using the supplied name.

    @doc    Opens a file using the flag and mode settings, and sets 
    the file descriptor.  The number of files available at any one 
    time is dependant on the native File System.  When use of the 
    file handle is completed, rtp_file_close() should be called.  
    Default mode is RTP_FILE_O_RDONLY if flag is 0.

    @return 0 if successful, -1 on failure, -2 if a non-fatal
    error occurred.  If a -1 or -2 is returned the value of *fdPtr
    is undefined.  For debugging purposes; if the cause of the
    error is obtainable at the native File System layer, turn on
    RTP_DEBUG in rtpdebug.h to display the native error
    value.
 */
int rtp_file_open (
  RTP_HANDLE * fdPtr,                   /** Pointer to an RTP_HANDLE to store the file descriptor. */
  const char * name,                    /** The name of the file to open. */
  unsigned short flag,                  /** For the flag argument:<br>
	<pre>
|		RTP_FILE_O_APPEND   All writes will be appended to the file.
|		RTP_FILE_O_RDONLY   Open a file for reading only.
|		RTP_FILE_O_WRONLY   Open a file for writing only.
|		RTP_FILE_O_RDWR     Open a file for reading and writing.
|		RTP_FILE_O_CREAT    Create a file if it does not exist.
|		RTP_FILE_O_TRUNC    Truncate a file to 0 bytes after opening.
|		RTP_FILE_O_EXCL     If creating a file, fail if it already exists
|		RTP_FILE_O_BINARY   Create the file in binary mode.
|		RTP_FILE_O_TEXT     Create the file in text mode.
|		Note:               If neither the RTP_FILE_O_BINARY, or the 
|		                    RTP_FILE_O_TEXT are used, the default behavior 
|		                    is to open the file in text mode.
	</pre> */
  unsigned short mode                   /** For the mode argument:<br>
	<pre>
|		RTP_FILE_S_IWRITE   Create a file with write permissions.
|		RTP_FILE_S_IREAD    Create a file with read permissions.
	</pre> */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                              rtp_file_close
 *----------------------------------------------------------------------*/
/** @memo   Close a file.

    @doc    Close a file that was opened with rtp_file_open() and 
    update the disk by flushing the directory entry and file 
    allocation table, then free all core associated with the file 
    descriptor.

    @return 0 if successful, -1 otherwise.  For debugging purposes;
    if the cause of the error is obtainable at the native File System
    layer, turn on RTP_DEBUG in rtpdebug.h to display the
    native error value.
 */
int rtp_file_close (
  RTP_HANDLE fileHandle                 /** File descriptor used to close the file. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                              rtp_file_read
 *----------------------------------------------------------------------*/
/** @memo   Read from a file.

    @doc    Attempt to read a number of bytes from the current file 
    location of the file referred to by fileHandle and place the data 
    in the storage buffer.  The file location is updated accordingly.

    @return Number of bytes read, 0 if end of file, -1 on error,
    and -2 on a non-fatal error.  For debugging purposes; if the
    cause of the error is obtainable at the native File System layer,
    turn on RTP_DEBUG in rtpdebug.h to display the native
    error value.
 */
long rtp_file_read (
  RTP_HANDLE fileHandle,                /** File descriptor to read the file. */
  unsigned char * buffer,               /** Buffer to store the read bytes. */
  long count                            /** Maximum amount to read. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                              rtp_file_write
 *----------------------------------------------------------------------*/
/** @memo   Write to a file.

    @doc    Attempt to write a number of bytes from a buffer to the 
    current location in the file referred to by fileHandle.  The file 
    current location is updated accordingly.

    @return Number of bytes written, -1 on error, and -2 on a
    non-fatal error.  For debugging purposes; if the cause of the
    error is obtainable at the native File System layer, turn on
    RTP_DEBUG in rtpdebug.h to display the native error value.
 */
long rtp_file_write (
  RTP_HANDLE fileHandle,                /** File descriptor to write the file. */
  const unsigned char * buffer,         /** Buffer to write to the file. */
  long count                            /** Amount to write. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                              rtp_file_lseek
 *----------------------------------------------------------------------*/
/** @memo   Moves the file pointer to the offset from the 
    origin passed in.

    @doc    Move the file pointer offset bytes in the file relative to the
    origin.  Attempting to seek beyond the end of the file puts the
    file pointer one byte past the end of the file.  Seeking zero bytes
    from the origin set to 2 (end of the file) returns the file length.

    @return Offset from the beginning of the file, -1 on error.  For
    debugging purposes; if the cause of the error is obtainable
    at the native File System layer, turn on RTP_DEBUG
    in rtpdebug.h to display the native error value.
 */
long rtp_file_lseek (
  RTP_HANDLE fd,                        /** File descriptor to move its pointer. */
  long offset,                          /** The offset to move the pointer. */
  int origin                            /** The location that the offset is relative to:<br>
	<pre>
|		0 to seek from the beginning of the file.
|		1 to seek from the current file pointer position.
|		2 to seek from the end of the file.
	</pre> */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_truncate
 *----------------------------------------------------------------------*/
/** @memo   Truncate the file to the desired size.

    @doc    Given a file handle and a new file size, either extend the 
    file or truncate it.  If the current file pointer is still within 
    the range of the file, it is unmoved, otherwise it is moved to the
    end of the file.  If the original size of the file is smaller
    than the offset, the file is padded with '\0' until the offset.
    If the original size is larger than the offset, the extra bytes
    are removed and lost.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_truncate (
  RTP_HANDLE fd,                        /** File descriptor to the file to truncate. */
  long offset                           /** The new size of the file. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_flush
 *----------------------------------------------------------------------*/
/** @memo   Flushes a file stream.

    @doc    Flushes the file stream of any data.  After this call 
    completes, the on disk view of the file is completely consistent 
    with the in memory view.  It is a good idea to call this function
    periodically if a file is being extended.  If a file is not
    flushed or closed and a power down occurs, the file size will
    be wrong on disk and the FAT chains will be lost.

    @return 0 if successful and when nothing to flush, -1 on error.
    For debugging purposes; if the cause of the error is obtainable
    at the native File System layer, turn on RTP_DEBUG in
    rtpdebug.h to display the native error value.
 */
int rtp_file_flush (
  RTP_HANDLE fd                         /** File descriptor to the file to be flushed. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_rename
 *----------------------------------------------------------------------*/
/** @memo   Rename a file or directory.

    @doc    Moves the file or subdirectory named name to the new 
    name specified in newname.  name and newname must be on the 
    same drive but they may be in different directories.  Both 
    names must be fully qualified.  Fails if newname is invalid,
    already exists or name is not found.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_rename (
  const char * name,                          /** File name to be changed. */
  char * newname                        /** New name to be used. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_delete
 *----------------------------------------------------------------------*/
/** @memo   Delete a file.

    @doc    Deletes a file.  Fails if not a simple file, if it 
    is open, does not exist, or is read only.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_delete (
  const char * name                           /** Name of file to be deleted. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_mkdir
 *----------------------------------------------------------------------*/
/** @memo   Create a new subdirectory.

    @doc    Creates a subdirectory in the path specified by name.  
    Fails if a file or directory of the same name already exists 
    or if the directory component (if there is one) of the path 
    is not found.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_mkdir (
  const char * name                           /** Name of the new subdirectory. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_rmdir
 *----------------------------------------------------------------------*/
/** @memo   Remove a subdirectory.

    @doc    Remove a subdirectory specified by name.  Fails if 
    the path is not a directory, is read only, or is not empty.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_rmdir (
  const char * name                           /** Name of the new subdirectory. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_setcwd
 *----------------------------------------------------------------------*/
/** @memo   Set the current working directory.

    @doc    Sets the directory specified by name as the new current 
    working directory for this task.  The name must be a valid 
    directory name.  If a drive letter is used in the name parameter, 
    it will change the default drive letter to this new drive letter.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpnet.c to display
    the native error value.
 */
int rtp_file_setcwd (
  const char * name                           /** Name of the directory to become 
                                            the current working directory. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_file_pwd
 *----------------------------------------------------------------------*/
/** @memo   Print the name of the current working directory to a buffer.

    @doc    Fill name with the full path name of the current working
    directory for the current task in the default drive.  name
    must point to enough space to hold the full path without
    overriding user data.  The worst case possible is native
    File System dependant.

    @return 0 if successful, -1 on error.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_pwd (
  char * name,                          /** Storage location for the name 
                                            of the current working directory. */
  long size                             /** The size of the storage location. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_file_chmode
 *----------------------------------------------------------------------*/
/** @memo   Change file mode.

    @doc    Change the mode of a file to the desired attributes.  
    One or more of the attributes may be set by or'ing them together.

    @return 0 if successful, -1 otherwise.  For debugging purposes;
    if the cause of the error is obtainable at the native File
    System layer, turn on RTP_DEBUG in rtpdebug.h to display
    the native error value.
 */
int rtp_file_chmode (
  const char * name,                          /** The name of the file that needs its mode changed. */
  unsigned char attributes              /** The new attributes to be assigned to the file:<br>
	<pre>
|		RTP_FILE_ATTRIB_RDONLY      read only
|		RTP_FILE_ATTRIB_WRONLY      write only
|		RTP_FILE_ATTRIB_RDWR        read and write permissions
|		RTP_FILE_ATTRIB_HIDDEN      hidden
|		RTP_FILE_ATTRIB_SYSTEM      system file
|		RTP_FILE_ATTRIB_ARCHIVE     archived
	</pre> */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_file_get_free
 *----------------------------------------------------------------------*/
/** @memo   Retrieves disk free space information.

    @doc    Given a drive name, return the number of bytes on the 
    drive, the number of bytes free on the drive, the number of 
    sectors per unit, and the number of bytes per sector.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native File 
    System layer, turn on RTP_DEBUG in rtpdebug.h to display 
    the native error value.
 */
int rtp_file_get_free (
  const char * name,                          /** The name of the drive to obtain information on. */
  unsigned long *total,                 /** Total number of bytes on the drive. */
  unsigned long *free,                  /** Total number of bytes the drive has free. */
  unsigned long *sectors_per_unit,      /** Number of sectors per unit (cluster). */
  unsigned short *bytes_per_sector      /** Number of bytes per sector. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_file_set_time
 *----------------------------------------------------------------------*/
/** @memo   Changes the date of creation, access, and modification 
    of a file.

    @doc    Changes the date of creation, access, and modification 
    of a file using the file descriptor passed in.

    @return 0 if successful, -1 otherwise.  For debugging purposes; 
    if the cause of the error is obtainable at the native File 
    System layer, turn on RTP_DEBUG in rtpdebug.h to display 
    the native error value.
 */
int rtp_file_set_time (
  RTP_HANDLE fd,                        /** File descriptor of file that needs its dates changed. */
  RTP_DATE * adate,                     /** Date of last access. */
  RTP_DATE * wdate,                     /** Date of last write. */
  RTP_DATE * cdate,                     /** Date of creation. */
  RTP_DATE * hdate                      /** Date of last file mode change. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
