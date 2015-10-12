//
// RTPTERM.C - Runtime Platform Terminal I/O Services
//
//   PORTED TO THE Segger embOS simulation for Windows 32 bit system
//                 and Visual C++.
//
// EBSnet - RT-Platform 
//
//  $Author: tom $
//  $Date: 2004/10/22 18:11:57 $
//  $Name:  $
//  $Revision: 1.3 $
//
// Copyright EBSnet Inc. , 2003
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/************************************************************************
* Headers
************************************************************************/
#define RTP_INCLUDE_TERM
#include "rtpconfig.h"

/************************************************************************
* Defines
************************************************************************/
#define TERMINAL_UP_ARROW       1
#define TERMINAL_DOWN_ARROW     2
#define TERMINAL_RIGHT_ARROW    3
#define TERMINAL_LEFT_ARROW     4
#define TERMINAL_ESCAPE         27

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
#ifdef USE_RESOURCE_LOCKS
static OS_RSEMA rtpResourceLock;
#endif

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
                            rtp_term_init
 *----------------------------------------------------------------------*/
int     rtp_term_init           (void)
{
#ifdef USE_RESOURCE_LOCKS
        OS_CREATERSEMA(&rtpResourceLock);
#endif        
	return (0);
}

/*----------------------------------------------------------------------*
                            rtp_term_kbhit
 *----------------------------------------------------------------------*/
int rtp_term_kbhit (void)
{
    return(kbhit());
}


/*----------------------------------------------------------------------*
                            rtp_term_getch
 *----------------------------------------------------------------------*/
int rtp_term_getch (void)
{
int ch = 0;

    ch = getch();
    return (ch);
}


/*----------------------------------------------------------------------*
                             rtp_term_putc
 *----------------------------------------------------------------------*/
void rtp_term_putc (char ch)
{
    // Do ASCII translation.  Make sure that \n gets translated to \r\n.
	if (ch == '\n')
	{
		rtp_term_putc('\r');
	}
	
	putch(ch);
}



/************************************************************************/
/*      THERE IS NO NEED TO CHANGE ANYTHING BELOW THIS COMMENT         */
/************************************************************************/




/*----------------------------------------------------------------------*
                             rtp_term_puts
 *----------------------------------------------------------------------*/
void rtp_term_puts (const char * string)
{
#ifdef USE_RESOURCE_LOCKS
        OS_Use(&rtpResourceLock);
#endif        
	rtp_term_cputs(string);
	rtp_term_putc('\n');
#ifdef USE_RESOURCE_LOCKS
        OS_Unuse(&rtpResourceLock);
#endif        
}


/*----------------------------------------------------------------------*
                             rtp_term_cputs
 *----------------------------------------------------------------------*/
int rtp_term_cputs (const char * string)
{
    while (*string)
	{
		rtp_term_putc(*string++);
	}
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_term_gets
 *----------------------------------------------------------------------*/
int rtp_term_gets (char * string)
{
    *string = 0;
    return (rtp_term_promptstring(string,0));
}


/*----------------------------------------------------------------------*
                          rtp_term_promptstring
 *----------------------------------------------------------------------*/
int rtp_term_promptstring (char * string, unsigned int handle_arrows)
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

    while (!rtp_term_kbhit( ))
    {
        /* ----------------------------------- */
        /*  Free resources to time critical    */
        /*  events.                            */
        /* ----------------------------------- */
        OS_Delay(2);
    }
    
    ch = rtp_term_getch( );
	while (ch != -1)
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
			    }               /* ----------------------------------- */
			    goto getnext;   /*  Get next character.                */
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

getnext:
        while (!rtp_term_kbhit( ))
        {
            /* ----------------------------------- */
            /*  Free resources to time critical    */
            /*  events.                            */
            /* ----------------------------------- */
            OS_Delay(2);
        }
        
        ch = rtp_term_getch( );
	}
	return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_term_up_arrow
 *----------------------------------------------------------------------*/
int rtp_term_up_arrow (void)
{
    return ((int) TERMINAL_UP_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_down_arrow
 *----------------------------------------------------------------------*/
int rtp_term_down_arrow (void)
{
    return ((int) TERMINAL_DOWN_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_left_arrow
 *----------------------------------------------------------------------*/
int rtp_term_left_arrow (void)
{
    return ((int) TERMINAL_LEFT_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_right_arrow
 *----------------------------------------------------------------------*/
int rtp_term_right_arrow (void)
{
    return ((int) TERMINAL_RIGHT_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_escape_key
 *----------------------------------------------------------------------*/
int rtp_term_escape_key (void)
{
    return ((int) TERMINAL_ESCAPE);
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
