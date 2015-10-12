 /*
 | RTPTERM.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/11/09 22:03:36 $
 |  $Name:  $
 |  $Revision: 1.2 $
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
#include "rtpterm.h"
#include "rtpstr.h"

#include <conio.h>

#include "xnconf.h"

/************************************************************************
* Defines
************************************************************************/
#define TERMINAL_UP_ARROW       1
#define TERMINAL_DOWN_ARROW     2
#define TERMINAL_RIGHT_ARROW    3
#define TERMINAL_LEFT_ARROW     4
#define TERMINAL_ESCAPE         27

#include <windows.h>
#include <process.h>

#define TERMINAL_THREAD_SLEEP   Sleep

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
extern void        poll_os_cycle(void);
#define USE_EMGL_CONSOLE 0
#if (USE_EMGL_CONSOLE)
#include "..\EMGL\Controls\controls.h"
#endif
int rtp_term_kbhit (void)
{
#if (!TAHI_TEST)
#if(defined(POLMSP3))
    poll_os_cycle();
#endif
#endif
#if (USE_EMGL_CONSOLE)
	return EMGLConsoleKbHit(EMGLConsoleGetDefaultConsole());
#else
    return(kbhit());
#endif
}


/*----------------------------------------------------------------------*
                            rtp_term_getch
 *----------------------------------------------------------------------*/
int rtp_term_getch (void)
{
int ch = 0;
#if (USE_EMGL_CONSOLE)
	ch=EMGLConsoleGetc(EMGLConsoleGetDefaultConsole());
#else
    ch = getch();
#endif
    return (ch);
}


/*----------------------------------------------------------------------*
                             rtp_term_putc
 *----------------------------------------------------------------------*/
void rtp_term_putc (char ch)
{
#if (USE_EMGL_CONSOLE)
	EMGLConsolePutc(EMGLConsoleGetDefaultConsole(),ch);
#else
    /* Do ASCII translation.  Make sure that \n gets translated to \r\n. */
    if (ch == '\n')
    {
        rtp_term_putc('\r');
    }
    
    putch(ch);
#endif
}



/************************************************************************/
/*      THERE IS NO NEED TO CHANGE ANYTHING BELOW THIS COMMENT          */
/************************************************************************/




/*----------------------------------------------------------------------*
                             rtp_term_puts
 *----------------------------------------------------------------------*/
void rtp_term_puts (const char * string)
{
    rtp_term_cputs(string);
    rtp_term_putc('\n');
}


/*----------------------------------------------------------------------*
                             rtp_term_cputs
 *----------------------------------------------------------------------*/
int rtp_term_cputs (const char * string)
{
#if (USE_EMGL_CONSOLE)
	EMGLConsolePuts(EMGLConsoleGetDefaultConsole(),string);
#else
    while (*string)
    {
        rtp_term_putc(*string++);
    }
#endif
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

#if (USE_EMGL_CONSOLE)
	return EMGLConsolePromptString(EMGLConsoleGetDefaultConsole(),string);
#endif

	rtp_term_cputs(string);
    while (!rtp_term_kbhit( ))
    {
        /* ----------------------------------- */
        /*  Free resources to time critical    */
        /*  events.                            */
        /* ----------------------------------- */
 //       TERMINAL_THREAD_SLEEP(2);
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
#if (USE_EMGL_CONSOLE==0)
        rtp_term_putc((char)ch);
#endif
        *endptr++ = (char)ch;
        *endptr = 0;

getnext:
        while (!rtp_term_kbhit( ))
        {
            /* ----------------------------------- */
            /*  Free resources to time critical    */
            /*  events.                            */
            /* ----------------------------------- */
            TERMINAL_THREAD_SLEEP(2);
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
