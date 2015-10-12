 /*
 | RTPTERM.C - Runtime Platform Network Services
 |
 |   PORTED TO THE WIN32 PLATFORM for Rtip POLLOS
 |
 | EBSnet - RT-Platform 
 |
 | Copyright EBSnet Inc. , 2008
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
#include "rtptermaux.h"
#include "rtpstr.h"
//#include "rtpterm.h"
#include "rtpprint.h"

#include "pollos.h"   /* for poll_os_cycle */

#include <windows.h>
#include <winbase.h>

/************************************************************************
* Defines
************************************************************************/
#define     RTP_TERM_AUX_PUT_BACKSLASH_N_AFTER_WRITE    0
#define     RTP_TERM_AUX_COMM_PORT                      "COM1"
#define     RTP_TERM_AUX_BAUD_RATE                      CBR_9600

#define     TERMINAL_AUX_UP_ARROW                       1
#define     TERMINAL_AUX_DOWN_ARROW                     2
#define     TERMINAL_AUX_RIGHT_ARROW                    3
#define     TERMINAL_AUX_LEFT_ARROW                     4
#define     TERMINAL_AUX_ESCAPE                         27
#define     TERMINAL_AUX_THREAD_SLEEP                   Sleep
/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
RTP_HANDLE auxComm;
//POLLOS_TASK  poll_comm_handle;
/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
/* A local function called by pollos_cycle to poll the aux comm port */
//void poll_comm(void);
    /* set up so pollos_cycle will call the receive polling loop   */
//    poll_comm_handle.func = poll_comm;
//    pollos_task_add((PPOLLOS_TASK)&poll_comm_handle);

/************************************************************************
* Function Bodies
************************************************************************/
//void poll_comm(void)
//{
//}

int rtp_term_aux_kbhit (void)
{
    return (0);
}

/* SPR - was RTP_HANDLE but didn't build */
long rtp_term_aux_open(void)
{
    HANDLE hComm;
    DCB dcb;

    /* JRT */
    COMMTIMEOUTS comm_timeouts;

    hComm = CreateFile( RTP_TERM_AUX_COMM_PORT,  
                    GENERIC_READ | GENERIC_WRITE, 
                    0, 
                    0, 
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
                    0);
    if (hComm == INVALID_HANDLE_VALUE)
    {
        rtp_term_puts("Error in opening serial comm\n");
        return -1;
    }
    if (!GetCommState(hComm, &dcb))     // get current DCB
          // Error in GetCommState
          return FALSE;

    FillMemory(&dcb, sizeof(dcb), 0);
    dcb.DCBlength = sizeof(dcb);
    if (!BuildCommDCB("baud=9600 parity=n data=8 stop=1 dtr=on rts=on", &dcb)) {   
          // Couldn't build the DCB. Usually a problem
          // with the communications specification string.
          return FALSE;
    }

    // Update DCB rate.
    //dcb.BaudRate =  RTP_TERM_AUX_BAUD_RATE;

    // Set new state.
    if (!SetCommState(hComm, &dcb))
    {
      // Error in SetCommState. Possibly a problem with the communications 
      // port handle or a problem with the DCB structure itself.
    } 

    /* JRT - Get the timeout and set it appropriately */
    rtp_memset(&comm_timeouts, 0x0, sizeof(comm_timeouts));

#define GET_COMM_TIMEOUTS 0
#if GET_COMM_TIMEOUTS
    GetCommTimeouts(hComm, &comm_timeouts);
#endif

    if (!SetCommTimeouts(hComm, &comm_timeouts))
    {
        char err_string[100];
        int error;
        
        error = GetLastError();
        rtp_sprintf(err_string,
                    "rtp_term_aux_open: SetCommTimeouts failed, error = %d\n",
                    error);
        rtp_term_puts(err_string);
        return (-1);
    }

    auxComm = (RTP_HANDLE) hComm;
    
    return (auxComm);
 
}


/*----------------------------------------------------------------------*
                            rtp_term_aux_getch
 *----------------------------------------------------------------------*/
int rtp_term_aux_getch ()
{

    unsigned long dwRead;
    unsigned char buffer;
    unsigned long ret;

    HANDLE hComm;
    OVERLAPPED osReader = {0};
    
    hComm = (HANDLE) auxComm;

    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osReader.hEvent == NULL)
    {
        rtp_term_puts("rtp_term_aux_getch: Error creating overlapped event; abort\n");
        return -1;
    }

    do
    {// Issue read operation.
        if (!ReadFile(hComm, &buffer, 1, &dwRead, &osReader)) 
        {
            if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
            {
                            rtp_term_puts("rtp_term_aux_getch: Error in communications; report it.\n");
                return (-1);
            }
            else
            {
                /* wait for 1ms */
                do
                {
                    ret = WaitForSingleObject(osReader.hEvent, 0);

                    poll_os_cycle();

                }while (ret == WAIT_TIMEOUT);
    
                switch(ret)
                {
                    // Read completed.
                    case WAIT_OBJECT_0:
                        if (!GetOverlappedResult(hComm, &osReader, &dwRead, FALSE))
                        {
                            // Error in communications; report it.
                            rtp_term_puts("rtp_term_aux_getch: Error in communications; report it.\n");
                        }
                        else
                        {
                            // Read completed successfully.
                            //HandleASuccessfulRead(buffer, dwRead);
                        }
                        break;
    
                    case WAIT_TIMEOUT:
                        break;                       
    
                    default:
                        // Error in the WaitForSingleObject; abort.
                        // This indicates a problem with the OVERLAPPED structure's
                        // event handle.
                        break;
                }
            }
        }
        else
        {
        // read completed immediately
        //HandleASuccessfulRead(buffer, dwRead);
        }
        poll_os_cycle();
//tvotvo        TERMINAL_AUX_THREAD_SLEEP(100);
    }while (dwRead == 0);
    
    CloseHandle(osReader.hEvent);
    
    return (int) buffer;
    
}


/*----------------------------------------------------------------------*
                             rtp_term_aux_putc
 *----------------------------------------------------------------------*/
void rtp_term_aux_putc (char ch)
{
    HANDLE hComm;
    OVERLAPPED osWrite = {0};
    unsigned long bytesWritten;
    unsigned long error;

    hComm = (HANDLE) auxComm;

    // Create this write operation's OVERLAPPED structure's hEvent.
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osWrite.hEvent == NULL)
    {
        rtp_term_puts("rtp_term_aux_putc: error creating overlapped event handle\n");
        return;
    }
    if (WriteFile( hComm, &ch, 1, &bytesWritten, &osWrite) == 0)
    {
        if ((error = GetLastError()) != ERROR_IO_PENDING) 
        { 
            char test[100];
            rtp_sprintf(test, "rtp_term_aux_putc: Error in Write! %d\n", error);
            rtp_term_puts(test);    
            // WriteFile failed, but it isn't delayed. Report error and abort.
        }
        else 
        {
            // Write is pending.
            GetOverlappedResult(hComm, &osWrite, &bytesWritten, TRUE);
        }
    }
    CloseHandle(osWrite.hEvent);
}



/************************************************************************/
/*      THERE IS NO NEED TO CHANGE ANYTHING BELOW THIS COMMENT          */
/************************************************************************/


/*----------------------------------------------------------------------*
                             rtp_term_aux_puts
 *----------------------------------------------------------------------*/
void rtp_term_aux_puts (const char * string)
{
    rtp_term_aux_cputs(string);

#if (RTP_TERM_AUX_PUT_BACKSLASH_N_AFTER_WRITE)
    rtp_term_aux_putc('\n');
#endif
}


/*----------------------------------------------------------------------*
                             rtp_term_aux_cputs
 *----------------------------------------------------------------------*/
int rtp_term_aux_cputs (const char * string)
{
    while (*string)
    {
        rtp_term_aux_putc(*string++);
    }
    return (0);
}


/*----------------------------------------------------------------------*
                             rtp_term_aux_gets
 *----------------------------------------------------------------------*/
int rtp_term_aux_gets (char * string)
{
    *string = 0;
    return (rtp_term_aux_promptstring(string,0));
}


/*----------------------------------------------------------------------*
                          rtp_term_aux_promptstring
 *----------------------------------------------------------------------*/
int rtp_term_aux_promptstring (char * string, unsigned int handle_arrows)
{
/* ----------------------------------- */
/*  Endptr always points to            */
/*  null-terminator.                   */
/* ----------------------------------- */
char * endptr = &string[rtp_strlen(string)];
int ch;
char clbuff[80];

    rtp_memset((unsigned char *)clbuff, ' ', 80);
    clbuff[0] = '\r';
//    clbuff[78] = '\r';
    clbuff[79] = 0;

#define CLEAR_LINE() rtp_term_aux_cputs(clbuff)

    /* ----------------------------------- */
    /*  Print out the default answer.      */
    /* ----------------------------------- */
    rtp_term_cputs(string);

    ch = rtp_term_aux_getch( );
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
                         
        }

        /* ----------------------------------- */
        /*  Display the editing.               */
        /* ----------------------------------- */
        rtp_term_putc((char)ch);
        *endptr++ = (char)ch;
        *endptr = 0;

getnext:
                
        ch = rtp_term_aux_getch( );
    }
    return (-1);
}


/*----------------------------------------------------------------------*
                           rtp_term_aux_up_arrow
 *----------------------------------------------------------------------*/
int rtp_term_aux_up_arrow (void)
{
    return ((int) TERMINAL_AUX_UP_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_aux_down_arrow
 *----------------------------------------------------------------------*/
int rtp_term_aux_down_arrow (void)
{
    return ((int) TERMINAL_AUX_DOWN_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_aux_left_arrow
 *----------------------------------------------------------------------*/
int rtp_term_aux_left_arrow (void)
{
    return ((int) TERMINAL_AUX_LEFT_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_aux_right_arrow
 *----------------------------------------------------------------------*/
int rtp_term_aux_right_arrow (void)
{
    return ((int) TERMINAL_AUX_RIGHT_ARROW);
}


/*----------------------------------------------------------------------*
                          rtp_term_aux_escape_key
 *----------------------------------------------------------------------*/
int rtp_term_aux_escape_key (void)
{
    return ((int) TERMINAL_AUX_ESCAPE);
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
