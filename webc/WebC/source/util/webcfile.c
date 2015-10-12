/*
|  WEBCFILE.C -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcfg.h"
#include "webcfile.h"

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

#if(WEBC_SUPPORT_FILE)

// ***********************************************************************
// File system porting routines
// ***********************************************************************

/*************************************************************************
 webc_fclose() - Wrapper for various implementations of fclose().

 Summary

   #include "webcfile.h"

   int webc_fclose(WEBC_FILE file_handle);

 Parameters

   WEBC_FILE file_handle - File handle filled in by fopen().
   PFCHAR drive (old VFILE only) - some sort of context buffer.

 Description

   This function closes an open file.

 Returns (int)

   0 on success, < 0 on failure.
*************************************************************************/

int webc_fclose(WEBC_FILE file_handle)
{
	return (rtp_file_close(file_handle));
}

/*************************************************************************
 webc_fopen() - Wrapper for various implementations of fopen().

 Summary

   #include "webcfile.h"

   int webc_fopen(PFCHAR path,WEBC_FILE *pfile_handle,int mode);

 Parameters

   PFCHAR path - pathname of file.
   WEBC_FILE *pfile_handle - pointer to file handle to be filled in.
   int mode - may be one of the following:

     WEBC_FILE_RDONLY - open the file for reading only
     WEBC_FILE_WRONLY - open the file for writing only
     WEBC_FILE_APPEND - open the file for writing (append the current data)
     WEBC_FILE_UPDATE - open the file for reading; update the file date

   PFCHAR drive (old VFILE only) - some sort of context buffer.

 Description

   This function opens a file for reading, writing or appending.

 Returns (int)

   0 on success, < 0 on failure.
*************************************************************************/

int webc_fopen(const char * path, WEBC_FILE *pfile_handle, int mode)
{
	switch (mode)
	{
		case WEBC_FILE_RDONLY:
			return (rtp_file_open(pfile_handle, path, RTP_FILE_O_RDONLY|RTP_FILE_O_BINARY, 0));

		case WEBC_FILE_WRONLY:
			return (rtp_file_open(pfile_handle, path, RTP_FILE_O_WRONLY|RTP_FILE_O_BINARY|RTP_FILE_O_CREAT, RTP_FILE_S_IWRITE|RTP_FILE_S_IREAD));

		case WEBC_FILE_APPEND:
			return (rtp_file_open(pfile_handle, path, RTP_FILE_O_APPEND|RTP_FILE_O_BINARY|RTP_FILE_O_CREAT, RTP_FILE_S_IWRITE|RTP_FILE_S_IREAD));

		case WEBC_FILE_UPDATE:
			return (rtp_file_open(pfile_handle, path, RTP_FILE_O_RDONLY|RTP_FILE_O_BINARY, 0));

		default:
			return (-1);
	}
}

/*************************************************************************
 webc_fread() - Wrapper for various implementations of fread().

 Summary

   #include "webcfile.h"

   long webc_fread(WEBC_FILE file_handle,WEBC_PFBYTE buf,long max_size);

 Parameters

   WEBC_FILE file_handle - File handle filled in by webc_fopen().
   WEBC_PFBYTE buf - Buffer to read into.
   long max_size - Maximum number of bytes to read.

 Description

   This function reads data from a file into a buffer.

 Returns (long)

   Number of bytes read, or < 0 on failure.
*************************************************************************/

long webc_fread (WEBC_FILE file_handle, char * buf, long max_size,
                 int (*idle_func)(WEBC_PFBYTE), WEBC_PFBYTE idle_data)
{
	return (rtp_file_read(file_handle, (unsigned char*) buf, max_size));
}


#if (WEBC_SUPPORT_POSTFILE)
/*************************************************************************
 webc_flength() - Wrapper for code to return the length of a file.

 Summary

   #include "webcfile.h"

   unsigned long webc_flength(WEBC_FILE file_handle,WEBC_PFBYTE buf,long max_size);

 Parameters

   WEBC_FILE file_handle - File handle filled in by webc_fopen().

 Description

   This function reads data from a file into a buffer.

 Returns (long)

   Length of the file 0xffff ffff on failure.
*************************************************************************/

unsigned long webc_flength (WEBC_FILE file_handle)
{
long current_pos = rtp_file_lseek (file_handle, 0, 1);
long end_pos;

	if (current_pos < 0)
		return(0xffffffff);
	end_pos = rtp_file_lseek (file_handle, 0, 2);

	rtp_file_lseek (file_handle, current_pos, 0);

	if (end_pos < 0)
		return(0xffffffff);
	else
		return((unsigned long)end_pos);
}

#endif

/*************************************************************************
 webc_fwrite() - Wrapper for various implementations of fputs().

 Summary

   #include "webcfile.h"

   long webc_fwrite(WEBC_FILE file_handle,PFCHAR buf,long len);

 Parameters

   WEBC_FILE file_handle - File handle filled in by webc_fopen().
   PFCHAR buf - Buffer to write to file.
   WEBC_UINT16 len - number of bytes to write

 Description

   This function writes data from a file into a buffer.

 Returns (long)

   Number of bytes written, or < 0 on failure.
*************************************************************************/

long webc_fwrite (WEBC_FILE file_handle, char * buf, long len,
                  int (*idle_func)(WEBC_PFBYTE), WEBC_PFBYTE idle_data)
{
	return (rtp_file_write(file_handle, (const unsigned char*) buf, len));
}

#endif // WEBC_SUPPORT_FILE
