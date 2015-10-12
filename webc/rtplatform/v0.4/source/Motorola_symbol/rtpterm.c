 /*
 | RTPTERM.C - Runtime Platform Network Services
 |
 | EBSnet - RT-Platform
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
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
#include "rtpterm.h"
#include "rtpstr.h"

/************************************************************************
* Defines
************************************************************************/
#define TERMINAL_UP_ARROW       1
#define TERMINAL_DOWN_ARROW     2
#define TERMINAL_RIGHT_ARROW    3
#define TERMINAL_LEFT_ARROW     4
#define TERMINAL_ESCAPE         27

void do_trace_string_major(const char *);
void do_trace_char_major(char c);

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
                            rtp_term_kbhit
 *----------------------------------------------------------------------*/
/** @memo   Check and see if there are any characters waiting.

    @doc    Check and see if there are any characters waiting.

    @return Nonzero if a character is waiting, 0 otherwise.
 */
int rtp_term_kbhit (void)
{
    rtp_not_yet_implemented ();
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_term_getch
 *----------------------------------------------------------------------*/
/** @memo   Receive a single character from a terminal.

    @doc    Receive a single character from a terminal, and it
    should never echo the character back to the other side. This
    function will block if there are no characters available.

    @return The character received.
 */
int rtp_term_getch (void)
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                             rtp_term_putc
 *----------------------------------------------------------------------*/
/** @memo   Output a single character from a terminal.

    @doc    Output a single character to a terminal.  This
    function may block.

    @return void
 */
void rtp_term_putc (
  char ch                               /** The character to output. */
  )
{
	do_trace_char_major(ch);
}



/************************************************************************/
/*      THERE IS NO NEED TO CHANGE ANYTHING BELOW THIS COMMENT         */
/************************************************************************/




/*----------------------------------------------------------------------*
                             rtp_term_puts
 *----------------------------------------------------------------------*/
/** @memo   Output a string to a terminal, with a newline at the end.

    @doc    Output a string to a terminal, with a newline at the end.
    This function may block.

    @return void
 */
void rtp_term_puts (
  const char * string                   /** The string to output. */
  )
{
	do_trace_string_major(string);
    do_trace_string_major("\r\n");// the tsd use carriage return linefeed

}


/*----------------------------------------------------------------------*
                             rtp_term_cputs
 *----------------------------------------------------------------------*/
/** @memo   Output a string to a terminal.

    @doc    Output a string to a terminal.  This function may block.

    @return 0 if successful, -1 otherwise.
 */
int rtp_term_cputs (
  const char * string                   /** The string to output. */
  )
{
	do_trace_string_major(string);
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_term_gets
 *----------------------------------------------------------------------*/
/** @memo   Input a string from a terminal.

    @doc    Input a string from a terminal. This function may block.
    It reads a string from the terminal device, and stops reading
    when it encounters a return ('\r') from the data stream. It
    calls rtp_term_promptstring (a line editing function) with an
    empty string. In the future, this may be a duplicate
    implementation of rtp_term_promptstring, because
    rtp_term_promptstring is at a slightly higher layer than
    rtp_term_gets. This is because rtp_term_gets is a standard ANSI
    I/O library function, but rtp_term_promptstring is not.

    @return 0 if successful, -1 otherwise.
 */
int rtp_term_gets (
  char * string                         /** The string to received to. */
  )
{
    *string = 0;
    return (rtp_term_promptstring(string,0));
}


/*----------------------------------------------------------------------*
                          rtp_term_promptstring
 *----------------------------------------------------------------------*/
/** @memo   Allow the user to edit a string interactively.

    @doc    Allow the user to edit a string interactively. This
    function may block. The handle_arrows flag is mostly useful
    in interactive shells and other interactive prompts, where
    an application might have a history of commands that have
    been entered, or a list of possible choices for an answer that
    may be scrolled through with the arrow keys.

    @return 0 if successful, -1 otherwise, and if handle_arrows is
    set to 1 then it may return the result from rtp_term_up_arrow,
    rtp_term_down_arrow, or rtp_term_escape.
 */
int rtp_term_promptstring (
  char * string,                        /** The string to edit. */
  unsigned int handle_arrows            /** Boolean flag for interactivity level. */
  )
{
/* ----------------------------------- */
/*  Endptr always points to            */
/*  null-terminator.                   */
/* ----------------------------------- */
char * endptr = &string[rtp_strlen(string)];
int ch;
char clbuff[80];

	rtp_memset((unsigned char *)clbuff, ' ', 79);
	clbuff[0] = '\r';
	clbuff[78] = '\r';
	clbuff[79] = 0;

#define CLEAR_LINE() rtp_term_cputs(clbuff)

    /* ----------------------------------- */
	/*  Print out the default answer.      */
	/* ----------------------------------- */
	rtp_term_cputs(string);

	while ((ch = rtp_term_getch()) != -1)
	{
		switch(ch)
		{
		    /* ----------------------------------- */
			/*  Return.                            */
			/* ----------------------------------- */
		    case '\n':
		    case '\r':
			    rtp_term_putc('\n');
			    return (0);

            /* ----------------------------------- */
			/*  Backspace.                         */
			/* ----------------------------------- */
		    case '\b':
			    if(endptr > string)
			    {
				    rtp_term_cputs("\b \b");
				    *(--endptr) = 0;
			    }           /* ----------------------------------- */
			    continue;   /*  Go back to the while loop.         */
			                /* ----------------------------------- */

		    case TERMINAL_UP_ARROW:
			    if(handle_arrows)
			    {
			        /* ----------------------------------- */
				    /*  Erase the current line.            */
				    /* ----------------------------------- */
				    CLEAR_LINE();                       /* ----------------------------------- */
				    return (rtp_term_up_arrow ( ));     /*  TERMINAL_UP_ARROW                  */
			    }                                       /* ----------------------------------- */
			    break;

		    case TERMINAL_DOWN_ARROW:
			    if(handle_arrows)
			    {
			        /* ----------------------------------- */
				    /*  Erase the current line.            */
				    /* ----------------------------------- */
				    CLEAR_LINE();                       /* ----------------------------------- */
				    return (rtp_term_down_arrow ( ));   /*  TERMINAL_DOWN_ARROW                */
			    }                                       /* ----------------------------------- */
			    break;

		    case TERMINAL_ESCAPE:
			    if(handle_arrows)
			    {
				    /* ----------------------------------- */
				    /*  Erase the current line.            */
				    /* ----------------------------------- */
				    CLEAR_LINE();                       /* ----------------------------------- */
				    return (rtp_term_escape_key ( ));   /*  TERMINAL_ESCAPE                    */
			    }                                       /* ----------------------------------- */
			    break;
		}

        /* ----------------------------------- */
		/*  Display the editing.               */
		/* ----------------------------------- */
		rtp_term_putc((char)ch);
		*endptr++ = (char)ch;
		*endptr = 0;
	}
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_term_up_arrow
 *----------------------------------------------------------------------*/
/** @memo   Used to check the response from rtp_term_promptstring.

    @doc    Used to check the response from rtp_term_promptstring.
    If the response is equivalent to rtp_term_up_arrow, then an
    up arrow was entered at the terminal.

    @return Up arrow value.
 */
int rtp_term_up_arrow (void)
{
    return ((int) TERMINAL_UP_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_down_arrow
 *----------------------------------------------------------------------*/
/** @memo   Used to check the response from rtp_term_promptstring.

    @doc    Used to check the response from rtp_term_promptstring.
    If the response is equivalent to rtp_term_down_arrow, then a
    down arrow was entered at the terminal.

    @return Down arrow value.
 */
int rtp_term_down_arrow (void)
{
    return ((int) TERMINAL_DOWN_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_left_arrow
 *----------------------------------------------------------------------*/
/** @memo   Used to check the response from rtp_term_promptstring.

    @doc    Used to check the response from rtp_term_promptstring.
    If the response is equivalent to rtp_term_left_arrow, then a
    left arrow was entered at the terminal.

    @return Left arrow value.
 */
int rtp_term_left_arrow (void)
{
    return ((int) TERMINAL_LEFT_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_right_arrow
 *----------------------------------------------------------------------*/
/** @memo   Used to check the response from rtp_term_promptstring.

    @doc    Used to check the response from rtp_term_promptstring.
    If the response is equivalent to rtp_term_right_arrow, then a
    right arrow was entered at the terminal.

    @return Right arrow value.
 */
int rtp_term_right_arrow (void)
{
    return ((int) TERMINAL_RIGHT_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_escape_key
 *----------------------------------------------------------------------*/
/** @memo   Used to check the response from rtp_term_promptstring.

    @doc    Used to check the response from rtp_term_promptstring.
    If the response is equivalent to rtp_term_escape_key, then
    escape was entered at the terminal.

    @return Escape value.
 */
int rtp_term_escape_key (void)
{
    return ((int) TERMINAL_ESCAPE);
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

