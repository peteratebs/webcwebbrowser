 /*
 | RTPFIO.C - Runtime Platform STDIO File Implementation
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |   This module makes use of the RTPlatform ported modules to 
 |   implement higher level file system functions.      __st__
 |
 | Functions implemented:
 |   fopen      fread       fprintf       fseek         feof
 |   fclose     fwrite      fgets         fileno
 |
 | Note:
 |   if (rtp_stdio_fileno (&handle, rtpfile) == 0)
 |	 {
 |      fflush == rtp_file_flush(handle)
 |   }
 |   ftell == rtp_stdio_fseek(handle, offset, 1)
 |   fputs of std(in/out/err) == rtp_term_puts()
 |   fgets of std(in/out/err) == rtp_term_gets()
*/


/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpfio.h"
#include "rtpfile.h"
#include "rtpprint.h"
#include "rtpterm.h"
#include "rtpmem.h"
#include "rtpstr.h"

/************************************************************************
* Defines
************************************************************************/
/* --------------------------------------- */
/*  Set the size of the buffer (per file)  */
/*  used to speed up reading 'text lines'  */
/*  A multiple of 512 is advised for best  */
/*  results.                               */
/* --------------------------------------- */
#define RTP_READLINE_BUFFER 512

/* --------------------------------------- */
/*  Set the size of the buffer (per file)  */
/*  used to write out data.  A multiple    */
/*  of 512 is advised for best results.    */
/*  This should take into account the      */
/*  worst case senerio for the application.*/
/* --------------------------------------- */
#define RTP_FPRINTF_BUFFER  512

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Structs
************************************************************************/
typedef struct
{
#define RTP_FILE_VERIFICATION_VALUE    0x495F4121
                        /* --------------------------------------- */
                        /*  used to be sure that a RTP_HANDLE is   */
                        /*  not being passed in place of an        */
                        /*  RTP_FILE. This would imply that there  */
                        /*  is mixing of STDIO calls with low      */
                        /*  level file calls, which is not allowed */
                        /* --------------------------------------- */
  unsigned long verify;

                        /* --------------------------------------- */
  RTP_HANDLE handle;    /*  file handle returned from low level    */
                        /*  ported RTPlatform file function.       */
                        /* --------------------------------------- */

                        /* --------------------------------------- */
  char *buffer;         /*  buffer used by rtp_stdio_fgets()       */
  int   bufsiz;         /*  allocated size of buffer               */
  int   bufpos;         /*  current position within buffer         */
  int   buffil;         /*  current 'fill' of buffer               */
                        /* --------------------------------------- */
                        
                        /* --------------------------------------- */
  unsigned int eofBool; /*  0 if not end of file, 1 if end of file */
                        /* --------------------------------------- */
} RTP_STDIO_FILE;

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
static unsigned short _rtp_mode_to_permission (const char * mode);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                           rtp_stdio_fopen
 *----------------------------------------------------------------------*/
/** @memo   Opens a file using the supplied name.

    @doc    Opens a file using the mode settings, and sets the 
    file information.

    @return 0 if successful, -1 on failure, and -2 if a non-fatal 
    error occurred.
 */
int rtp_stdio_fopen (
  void ** rtpfile,                      /** A pointer to the address of an RTP_FILE to store the file information. */
  const char * fname,                   /** The name of the file to open. */
  const char * mode                     /** For the mode argument:<br>
	<pre>
|		"r"     Open a file for reading only                RTP_FILE_O_RDONLY
|		"r+"    Open a file for reading and writing         RTP_FILE_O_RDWR
|		"w"     Open a file for writing only                RTP_FILE_O_WRONLY
|		        Create a file if it does not exist          RTP_FILE_O_CREAT
|		        Truncate a file to 0 bytes after opening    RTP_FILE_O_TRUNC
|		"w+"    Open a file for reading and writing         RTP_FILE_O_RDWR
|		        Create a file if it does not exist          RTP_FILE_O_CREAT
|		        Truncate a file to 0 bytes after opening    RTP_FILE_O_TRUNC
|		"a"     All writes will be appended to the file     RTP_FILE_O_APPEND
|		        Create a file if it does not exist          RTP_FILE_O_CREAT
|		"a+"    Open a file for reading and writing         RTP_FILE_O_RDWR
|		        All writes will be appended to the file     RTP_FILE_O_APPEND
|		        Create a file if it does not exist          RTP_FILE_O_CREAT
|		"b..."  b prepended to the mode in the open call    
|		        causes the file to be opened in binary mode RTP_FILE_O_BINARY
|		"t..."  t prepended to the mode in the open call    
|		        causes the file to be opened in text mode   RTP_FILE_O_TEXT
|		Note:   If neither the b or t are prepended, text   
|		        mode is the default implementation.         
	</pre> */
  )
{
RTP_STDIO_FILE * p;
int result;

    result = (-1);
    p = (RTP_STDIO_FILE *) rtp_malloc ((unsigned int) sizeof (*p));
    if (p)
    {
        rtp_memset(p, 0, sizeof(*p));
        p->verify = RTP_FILE_VERIFICATION_VALUE;
     
        if ((result = rtp_file_open (&(p->handle), fname, _rtp_mode_to_permission(mode), RTP_FILE_S_IWRITE | RTP_FILE_S_IREAD)) != 0)
        {
            /* --------------------------------------- */
            /*       ERROR: drop the handle block.     */
            /* --------------------------------------- */
            rtp_free ((void *)p);
        }
    }
    *rtpfile = (void *)p;
    return (result);
}


/*----------------------------------------------------------------------*
                           rtp_stdio_fclose
 *----------------------------------------------------------------------*/
/** @memo   Close a file.

    @doc    Closes a file that was opened with rtp_stdio_fopen.

    @return 0 if successful, -1 otherwise.
 */
int rtp_stdio_fclose (
  void * rtpfile                        /** File handle used to close the file. */
  )
{
int result = (-1);

    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_STDIO_FILE. Should   */
		/*  use low level rtp_file_close().        */
        /* --------------------------------------- */
        return (-1);
    }

    result = rtp_file_close (((RTP_STDIO_FILE *)rtpfile)->handle);
    if (((RTP_STDIO_FILE *)rtpfile)->buffer)
    {
        rtp_free ((void *)((RTP_STDIO_FILE *)rtpfile)->buffer);
    }
    rtp_free (rtpfile);
    rtpfile = (void *) 0;
    return (result);
}


/*----------------------------------------------------------------------*
                           rtp_stdio_fread
 *----------------------------------------------------------------------*/
/** @memo   Read from a file to buffer.

    @doc    Reads n items of size len from the current position in 
    the rtpfile and stores them in buffer.
    
    @precondition Should not mix rtp_stdio_fread and 
    rtp_stdio_fgets calls.

    @return Number of items read, 0 if end of file, -1 on error, 
    and -2 on non-fatal error.
 */
long rtp_stdio_fread (
  void * buffer,                        /** Buffer to store the read items. */
  unsigned long len,                    /** Size of items to read. */
  unsigned long n,                      /** Number of items to read. */
  void * rtpfile                        /** Handle of file to be read. */
  )
{
int result = (-1);

    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_STDIO_FILE. Should   */
		/*  use low level rtp_file_read().         */
        /* --------------------------------------- */
        return ((long) -1);
    }

    if (((RTP_STDIO_FILE *)rtpfile)->handle)
    {
        result = rtp_file_read (((RTP_STDIO_FILE *)rtpfile)->handle, (unsigned char *) buffer, (unsigned int) (len * n));
        if (result > 0)
        {
            result /= len;
        }
    }
    
    if (result == 0)
    {
        /* --------------------------------------- */
        /*  End of file reached, indicate this in  */
        /*  the RTP_STDIO_FILE and return.         */
        /* --------------------------------------- */
        ((RTP_STDIO_FILE *)rtpfile)->eofBool = 1;
    }
    
    return ((long) result);
}


/*----------------------------------------------------------------------*
                           rtp_stdio_fwrite
 *----------------------------------------------------------------------*/
/** @memo   Write a buffer to a file.

    @doc    Write n items of size len from the buffer to the current
    position in the file.
    
    @precondition Should not mix rtp_stdio_fwrite() and 
    rtp_stdio_fgets() calls.
    
    @return Number of items written, -1 on error, and -2 on non-fatal 
    error.
 */
long rtp_stdio_fwrite (
  void * buffer,                        /** Buffer to write. */
  unsigned long len,                    /** Size of items to write. */
  unsigned long n,                      /** Number of items to write. */
  void * rtpfile                        /** Handle of the file to be written to. */
  )
{
int result = (-1);

    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_STDIO_FILE. Should   */
		/*  use low level rtp_file_write().        */
        /* --------------------------------------- */
        return ((long) -1);
    }

    if (((RTP_STDIO_FILE *)rtpfile)->handle)
    {
        result = rtp_file_write (((RTP_STDIO_FILE *)rtpfile)->handle, (unsigned char *) buffer, (unsigned int) (len * n));
        if (result > 0)
        {
            result /= len;
        }
    }
    return ((long) result);
}


/*----------------------------------------------------------------------*
                           rtp_stdio_fprintf
 *----------------------------------------------------------------------*/
/** @memo   Write a formatted buffer to a file.

    @doc    Write n formatted bytes from the buffer to the current
    position in the file.
    
    @return Number of bytes written, -1 on error, and -2 on 
    non-fatal error.
 */
long rtp_stdio_fprintf (
    void *rtpfile,                      /** Handle of the file to be written to. */
    const char * f,                     /** Formatted buffer. */
    ...                                 /** (optional) Arguments to be formatted. */
    )
{
char buffer[RTP_FPRINTF_BUFFER];
rtp_va_list ap;
long ret;

    rtp_va_start(ap, f);
    rtp_vsnprintf(buffer, RTP_FPRINTF_BUFFER, f, ap);
    rtp_va_end(ap);
    
    ret = rtp_stdio_fwrite ((void *)buffer, 
                            (unsigned long) 1,
                            (unsigned long) rtp_strlen((const char *)buffer),
                            rtpfile);
	return (ret);
}


/*----------------------------------------------------------------------*
                            rtp_stdio_fgets
 *----------------------------------------------------------------------*/
/** @memo   Read lines from a file to a buffer.

    @doc    Read lines from a file to buf or if the end of the file 
    or the line has not been reached yet, reads (buflen-1) of data.
    
    @precondition Should not mix rtp_stdio_fread and rtpstdio_fgets
    calls.
    
    @return Number of bytes written, -1 otherwise.
 */
int rtp_stdio_fgets (
  char * buf,                           /** Buffer to store the read. */
  int buflen,                           /** Number of bytes to read. */
  void * rtpfile                        /** Handle of file to be read. */
  )
{
    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_STDIO_FILE. Should   */
		/*  use low level rtp_file_read().         */
        /* --------------------------------------- */
        return ((long) -1);
    }
    
    if (buflen <= 0)
    {
        return (-1);
    }
    buflen--;

    if ( !(((RTP_STDIO_FILE *)rtpfile)->buffer) )
    {
        /* --------------------------------------- */
        /* Allocate buffer. If successful, use it. */
        /* --------------------------------------- */

        /* --------------------------------------- */
        /* Assume no line is larger than           */
        /* RTP_READLINE_BUFFER characters. Then we */
        /* can use a buffer RTP_READLINE_BUFFER    */
        /* bytes and only read that many bytes at  */
        /* a time to speed up reading.             */
        /* --------------------------------------- */
        ((RTP_STDIO_FILE *)rtpfile)->buffer = (char *) rtp_malloc ((unsigned int)RTP_READLINE_BUFFER);
        if (((RTP_STDIO_FILE *)rtpfile)->buffer)
        {
            ((RTP_STDIO_FILE *)rtpfile)->bufsiz = RTP_READLINE_BUFFER;
        }
        /* --------------------------------------- */
        /* ((RTP_STDIO_FILE *)rtpfile)->bufpos = 0;*/
        /* ((RTP_STDIO_FILE *)rtpfile)->buffil = 0;*/
        /* --------------------------------------- */
    }

    while (((RTP_STDIO_FILE *)rtpfile)->buffer)
    {
        int i;
        int n;

        if (buflen == 0)
        {
            buf[0] = 0;
            return (0);
        }

        /* --------------------------------------- */
        /*  check if we still got a line worth in  */
        /*  there...                               */
        /* --------------------------------------- */
        for (i = ((RTP_STDIO_FILE *)rtpfile)->bufpos; i < ((RTP_STDIO_FILE *)rtpfile)->buffil; i++)
        {
            if (((RTP_STDIO_FILE *)rtpfile)->buffer[i] == '\n')
            {
                /* --------------------------------------- */
                /*                Have a line.             */
                /* --------------------------------------- */
                i = i + 1 - ((RTP_STDIO_FILE *)rtpfile)->bufpos;
                if (buflen < i)
                {
                    i = buflen;
                }
                rtp_memmove(buf, ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->bufpos, i);
                ((RTP_STDIO_FILE *)rtpfile)->bufpos += i;
                buf[i] = 0;
                return (i);
            }
        }

        /* --------------------------------------- */
        /*  no more lines in buffer: get new data  */
        /*  in there.  FIRST: move what we've got  */
        /*  to front.                              */
        /* --------------------------------------- */
        if (((RTP_STDIO_FILE *)rtpfile)->bufpos < ((RTP_STDIO_FILE *)rtpfile)->buffil)
        {
            rtp_memmove(((RTP_STDIO_FILE *)rtpfile)->buffer,
				       ((RTP_STDIO_FILE *)rtpfile)->buffer +
					   ((RTP_STDIO_FILE *)rtpfile)->bufpos,
					   ((RTP_STDIO_FILE *)rtpfile)->buffil -
					   ((RTP_STDIO_FILE *)rtpfile)->bufpos);
            ((RTP_STDIO_FILE *)rtpfile)->buffil -= ((RTP_STDIO_FILE *)rtpfile)->bufpos;
            ((RTP_STDIO_FILE *)rtpfile)->bufpos = 0;
        }
        else
        {
            ((RTP_STDIO_FILE *)rtpfile)->buffil = 0;
            ((RTP_STDIO_FILE *)rtpfile)->bufpos = 0;
        }

        /* --------------------------------------- */
        /*  Get more data. Assume we don't mix     */
        /*  fgets() with fread().                  */
        /* --------------------------------------- */
        if (((((RTP_STDIO_FILE *)rtpfile)->bufsiz - ((RTP_STDIO_FILE *)rtpfile)->buffil) & ~(RTP_READLINE_BUFFER-1)) > 0)
        {
            /* --------------------------------------- */
            /*         load alligned: sector           */
            /* --------------------------------------- */
            n = rtp_file_read (((RTP_STDIO_FILE *)rtpfile)->handle, 
                               (unsigned char *) ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->buffil, 
                               (unsigned int)    ((((RTP_STDIO_FILE *)rtpfile)->bufsiz - ((RTP_STDIO_FILE *)rtpfile)->buffil) & ~(RTP_READLINE_BUFFER-1)));

            if (n > 0)
            {
                ((RTP_STDIO_FILE *)rtpfile)->buffil += n;

                /* --------------------------------------- */
                /* check if we got a line worth in there.  */
                /* --------------------------------------- */
                for (i = ((RTP_STDIO_FILE *)rtpfile)->bufpos; i < ((RTP_STDIO_FILE *)rtpfile)->buffil; i++)
                {
                    if (((RTP_STDIO_FILE *)rtpfile)->buffer[i] == '\n')
                    {
                        /* --------------------------------------- */
                        /*                Have line.               */
                        /* --------------------------------------- */
                        i = i + 1 - ((RTP_STDIO_FILE *)rtpfile)->bufpos;
                        if (buflen < i)
                        {
                            i = buflen;
                        }
                        rtp_memmove(buf, ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->bufpos, i);
                        ((RTP_STDIO_FILE *)rtpfile)->bufpos += i;
                        buf[i] = 0;
                        return (i);
                    }
                }
            }
        }

        /* --------------------------------------- */
        /*         load as much as possible.       */
        /* --------------------------------------- */
        if (((RTP_STDIO_FILE *)rtpfile)->bufsiz > ((RTP_STDIO_FILE *)rtpfile)->buffil)
        {
            /* --------------------------------------- */
            /*              load UNalligned            */
            /* --------------------------------------- */
            n = rtp_file_read (((RTP_STDIO_FILE *)rtpfile)->handle, 
                               (unsigned char *) ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->buffil, 
                               (unsigned int)    (((RTP_STDIO_FILE *)rtpfile)->bufsiz - ((RTP_STDIO_FILE *)rtpfile)->buffil));

            if (n > 0)
            {
                ((RTP_STDIO_FILE *)rtpfile)->buffil += n;

                /* --------------------------------------- */
                /* check if we got a line worth in there.  */
                /* --------------------------------------- */
                for (i = ((RTP_STDIO_FILE *)rtpfile)->bufpos; i < ((RTP_STDIO_FILE *)rtpfile)->buffil; i++)
                {
                    if (((RTP_STDIO_FILE *)rtpfile)->buffer[i] == '\n')
                    {
                        /* --------------------------------------- */
                        /*               Have line.                */
                        /* --------------------------------------- */
                        i = i + 1 - ((RTP_STDIO_FILE *)rtpfile)->bufpos;
                        if (buflen < i)
                        {
                            i = buflen;
                        }
                        rtp_memmove(buf, ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->bufpos, i);
                        ((RTP_STDIO_FILE *)rtpfile)->bufpos += i;
                        buf[i] = 0;
                        return (i);
                    }
                }
            }
        }

        /* --------------------------------------- */
        /* If we get here, the line is too long or */
        /* we're stuck with the last line, which   */
        /* is NOT '\n' terminated.                 */
        /* --------------------------------------- */
        if (((RTP_STDIO_FILE *)rtpfile)->buffil > ((RTP_STDIO_FILE *)rtpfile)->bufpos)
        {
            i = ((RTP_STDIO_FILE *)rtpfile)->buffil - ((RTP_STDIO_FILE *)rtpfile)->bufpos;
            if (buflen < i)
            {
                i = buflen;
            }
            rtp_memmove(buf, ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->bufpos, i);
            ((RTP_STDIO_FILE *)rtpfile)->bufpos += i;
            buf[i] = 0;
            /* --------------------------------------- */
            /* This may be part of a very large line.  */
            /* So correct pointers and everything and  */
            /* loop.                                   */
            /* --------------------------------------- */
            buflen -= i;
            buf += i;
            if (buflen == 0)
            {
                return (0);
            }
        }

        /* --------------------------------------- */
        /* We just copied the remains of our       */
        /* buffer into the user buffer. Set pos to */
        /* 0 and try one last ALLIGNED load to see */
        /* if we still got data coming in. But     */
        /* shift remaining data, if any, to FRONT  */
        /* first.                                  */
        /* --------------------------------------- */
        if (((RTP_STDIO_FILE *)rtpfile)->bufpos < ((RTP_STDIO_FILE *)rtpfile)->buffil)
        {
            rtp_memmove(((RTP_STDIO_FILE *)rtpfile)->buffer, 
				       ((RTP_STDIO_FILE *)rtpfile)->buffer + 
					   ((RTP_STDIO_FILE *)rtpfile)->bufpos, 
					   ((RTP_STDIO_FILE *)rtpfile)->buffil - 
					   ((RTP_STDIO_FILE *)rtpfile)->bufpos);
            ((RTP_STDIO_FILE *)rtpfile)->buffil -= ((RTP_STDIO_FILE *)rtpfile)->bufpos;
            ((RTP_STDIO_FILE *)rtpfile)->bufpos = 0;
        }
        else
        {
            ((RTP_STDIO_FILE *)rtpfile)->buffil = 0;
            ((RTP_STDIO_FILE *)rtpfile)->bufpos = 0;
        }

        /* --------------------------------------- */
        /* Get more data. And check if there's any */
        /* left.                                   */
        /* --------------------------------------- */

        /* --------------------------------------- */
        /*           load alligned: sector         */
        /* --------------------------------------- */
        n = rtp_file_read (((RTP_STDIO_FILE *)rtpfile)->handle, 
                           (unsigned char *) ((RTP_STDIO_FILE *)rtpfile)->buffer + ((RTP_STDIO_FILE *)rtpfile)->buffil, 
                           (unsigned int)    ((((RTP_STDIO_FILE *)rtpfile)->bufsiz - ((RTP_STDIO_FILE *)rtpfile)->buffil) & ~(RTP_READLINE_BUFFER-1)));

        if (n > 0)
        {
            ((RTP_STDIO_FILE *)rtpfile)->buffil += n;
        }
        else
        {
            /* --------------------------------------- */
            /*       EOF reached and buffer empty.     */
            /* --------------------------------------- */
            ((RTP_STDIO_FILE *)rtpfile)->eofBool = 1;
            ((RTP_STDIO_FILE *)rtpfile)->buffer = 0;
            return (0);
        }

        /* --------------------------------------- */
        /* we get here, iff we are reading a very  */
        /* long line that is larger than our       */
        /* '((RTP_STDIO_FILE *)rtpfile)->buffer':  */
		/* add next piece to output 'buf'.         */
        /* --------------------------------------- */
    }
    return (-1);
}


/*----------------------------------------------------------------------*
                            rtp_stdio_fseek
 *----------------------------------------------------------------------*/
/** @memo   Moves the file pointer to the offset from the origin.

    @doc    Move the file pointer to the offset in the file 
    relative to the origin.
    
    @return 0 on success, -1 otherwise.
 */
long rtp_stdio_fseek (
  void * rtpfile,                       /** Handle of file to move its pointer. */
  long offset,                          /** The offset to move the pointer. */
  int origin                            /** The location that the offset is relative to:<br>
	<pre>	
|		0   To seek from the beginning of the file.
|		1   To seek from the current file pointer position.
|		2   To seek from the end of the file.
	</pre> */
  )
{
    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_STDIO_FILE. Should   */
		/*  use low level rtp_file_read().         */
        /* --------------------------------------- */
        return (-1);
    }

    if (rtp_file_lseek (((RTP_STDIO_FILE *)rtpfile)->handle, offset, origin) < 0)
    {
        return (-1);
    }
    
    /* --------------------------------------- */
    /*  Clear end of file indicator if seek    */
    /*  is successful.                         */
    /* --------------------------------------- */
    ((RTP_STDIO_FILE *)rtpfile)->eofBool = 0;
    
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_stdio_fileno
 *----------------------------------------------------------------------*/
/** @memo   Extract the RTP_HANDLE from the RTP_FILE returned
    by rtp_stdio_fopen.

    @doc    Extract the RTP_HANDLE from the RTP_FILE returned 
    by rtp_stdio_fopen.
    
    @return 0 on success, -1 otherwise.
 */
int rtp_stdio_fileno (
  RTP_HANDLE * handle,                  /** Storage location for the extracted RTP_HANDLE. */
  void * rtpfile                        /** Descriptor used to extract the underlying RTP_HANDLE. */
  )
{
    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_FILE. Should use     */
        /*  low level rtp_file_read().             */
        /* --------------------------------------- */
        return (-1);
    }

    *handle = ((RTP_STDIO_FILE *)rtpfile)->handle;
    return (0);
}


/*----------------------------------------------------------------------*
                              rtp_stdio_feof
 *----------------------------------------------------------------------*/
/** @memo   Determines if at the end of the file.

    @doc    Determines if the internal file pointer is at the end
    of the file.
    
    @return 0 if rtp_stdio_fread has not read to the end of the 
    file, -1 in the case of an error (invalid RTP_FILE *, or bad 
    position in the file), and 1 if reached the end of the file.
 */
int rtp_stdio_feof (
  void * rtpfile                        /** Handle of file to check eof. */
  )
{
    if( !rtpfile || (((RTP_STDIO_FILE *)rtpfile)->verify != RTP_FILE_VERIFICATION_VALUE) )
    {
        /* --------------------------------------- */
        /*  Application passed an RTP_HANDLE       */
        /*  instead of an RTP_FILE. Should use     */
        /*  low level rtp_file_read().             */
        /* --------------------------------------- */
        return (-1);
    }
    
    /* --------------------------------------- */
    /*  End of file reached, indicate this in  */
    /*  the RTP_FILE.                          */
    /* --------------------------------------- */
    return (((RTP_STDIO_FILE *)rtpfile)->eofBool);
}


/************************************************************************
* Utility Function Bodies
************************************************************************/

/*----------------------------------------------------------------------
    mode  - for the mode argument:
      "r"     Open a file for reading only                RTP_FILE_O_RDONLY
      "r+"    Open a file for reading and writing         RTP_FILE_O_RDWR
      "w"     Open a file for writing only                RTP_FILE_O_WRONLY
              Create a file if it does not exist          RTP_FILE_O_CREAT
              Truncate a file to 0 bytes after opening    RTP_FILE_O_TRUNC
      "w+"    Open a file for reading and writing         RTP_FILE_O_RDWR
              Create a file if it does not exist          RTP_FILE_O_CREAT
              Truncate a file to 0 bytes after opening    RTP_FILE_O_TRUNC
      "a"     All writes will be appended to the file     RTP_FILE_O_APPEND
              Create a file if it does not exist          RTP_FILE_O_CREAT
      "a+"    Open a file for reading and writing         RTP_FILE_O_RDWR
              All writes will be appended to the file     RTP_FILE_O_APPEND
              Create a file if it does not exist          RTP_FILE_O_CREAT
      "b..."  b prepended to the mode in the open call
              causes the file to be opened in binary
              mode                                        RTP_FILE_O_BINARY
      "t..."  t prepended to the mode in the open call
              causes the file to be opened in text
              mode                                        RTP_FILE_O_TEXT
      Note:   If neither the b or t are prepended, text
              mode is the default implementation.
----------------------------------------------------------------------*/

static unsigned short _rtp_mode_to_permission (const char * mode)
{
char type;
int index = 0;
unsigned short result = 0;
    
    type = mode[index++];
    
    if (type == 'b')
    {
        result |= RTP_FILE_O_BINARY;
        type = mode[index++];
    }
    else if (type == 't')
    {
        result |= RTP_FILE_O_TEXT;
        type = mode[index++];
    }
    else
    {
        /* ----------------------------------- */
        /*        Default to text mode.        */
        /* ----------------------------------- */
        result |= RTP_FILE_O_TEXT;
    }
    
    switch (type)
    {
        case 'a':
            type    = mode[index++];
            result |= RTP_FILE_O_APPEND;
            result |= RTP_FILE_O_CREAT;
            if( type == '+' )
            {
                result |= RTP_FILE_O_RDWR;
            }
            break;
            
        case 'r':
            type    = mode[index++];
            result |= RTP_FILE_O_RDONLY;
            if( type == '+' )
            {
                result &= (~RTP_FILE_O_RDONLY);
                result |= RTP_FILE_O_RDWR;
            }
            break;
    
        case 'w':
            type    = mode[index++];
            result |= RTP_FILE_O_WRONLY;
            result |= RTP_FILE_O_CREAT;
            result |= RTP_FILE_O_TRUNC;
            if( type == '+' )
            {
                result &= (~RTP_FILE_O_WRONLY);
                result |= RTP_FILE_O_RDWR;
            }
            break;
        
        default:
            
            break;
    }
	return (result);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */