/*                                                                      */
/* RTPDB.C - DISPLAY functions                                          */
/*                                                                      */
/* EBS - RTIP                                                           */
/*                                                                      */
/* Copyright Peter Van Oudenaren , 1993                                 */
/* All rights reserved.                                                 */
/* This code may not be redistributed in source or linkable object form */
/* without the consent of its author.                                   */
/*                                                                      */

#include "rtp.h"
#if (0) /* SPRZ - exclude file */
#include "rtpdbapi.h"
#include "rtpdb.h"

#if (DEBUG_LEVEL > 0)       /* matches up with endif near end of file */

#define SAY_RADIX 16            /* 16 = integers written in hex */
                                /* 10 = integers written in decimal   */

/* *****************************************************   */
/* THE REST OF FILE PROBABLY DOES NOT NEED MODIFICATION:   */

#if (SAY_RADIX == 16)
#define MAX_INT_STR  8
#define MAX_DINT_STR 16
#elif (SAY_RADIX == 10)
#define MAX_INT_STR  10
#define MAX_DINT_STR 20
#elif (SAY_RADIX == 8)
#define MAX_INT_STR  16
#define MAX_DINT_STR 32
#else
#error: SAY_RADIX size not supported
#endif

#define TOTAL_STR_LEN 120

#define SAY_STR_LEN 120     /* length of string for formatting string */
                            /* to be printed; NOTE: string on stack   */
                            /* so don't make to too large             */

/* ********************************************************************   */
/* FUNCTION DECLARATIONS                                                  */
/* ********************************************************************   */

/* *****************************************************                */
/* EXTERNAL DATA                                                        */
/* ******************************************************************** */
extern int RTP_FAR in_irq;
/* *****************************************************                */
/* GLOBAL DATA                                                          */
/* ******************************************************************** */

char RTP_FAR hex_string[140];   /* must be at least 123 */
char RTP_FAR rtp_hmap[17] = "0123456789ABCDEF";

/* *****************************************************   */
#define ERROR_TYPE 1
#define LOG_TYPE   2    /* greater than 2 */

/* *****************************************************   */
/* FORMAT ROUTINES                                         */
/* *****************************************************   */

void say_out(int type, RTP_PFINT8 string)
{
	if (type == ERROR_TYPE)
	{
	   	CB_WR_ERROR_STRING(string, (RTP_BOOL)(in_irq > 0));
	}
	else
	{
	   	CB_WR_LOG_STRING(string, (RTP_BOOL)(in_irq > 0));
	}

}

/* *****************************************************   */
void say_hex(int error_type, RTP_PFINT8 comment, RTP_PFINT8 p, int len)     /* __fn__ */
{
int i;
int tot_len;
int curr_len;

    if (rtp_strlen(comment) > 0)
    {
        say_out(error_type, comment);
    }

    tot_len = len;

    len = 16;       /* amount that will fit on a line is 25 but */
                    /* in order to compare to sniffer use 16   */

    hex_string[0]='N';
    hex_string[1]='U';
    hex_string[2]='L';
    hex_string[3]='L';
    hex_string[4]='\0';

    curr_len = 0;
    len *= 3;

    while (curr_len < tot_len)
    {
        /* write one line worth of data   */
        for (i=0; i<len; i=i+3)
        {
            hex_string[i] = rtp_hmap[(int)((*p)>>4)];
            hex_string[i+1] = rtp_hmap[(int)((*p)&0x0f)];
            hex_string[i+2] = ' ';
            hex_string[i+3] = '\0';
            p++;
            curr_len++;
            if (curr_len >= tot_len)
                break;
        }
        say_out(error_type, hex_string);
    }
}

/* *****************************************************   */
void say(int error_type, RTP_PFINT8 comment)
{
    say_out(error_type, comment);
}


/* *****************************************************   */
void say_int(int error_type, RTP_PFINT8 comment, int val)
{
    say_lint(error_type, comment, (long)val);
}

/* *****************************************************   */
void say_int2(int error_type, RTP_PFINT8 comment, int val1, int val2)
{
    say_lint2(error_type, comment, (long)val1, (long)val2);
}

/* *****************************************************   */
void say_lint(int error_type, RTP_PFINT8 comment, long val)
{
char sint[MAX_DINT_STR];
char s2[TOTAL_STR_LEN];
int i;

    i = rtp_strlen(comment);
    rtp_ltoa(val, sint, SAY_RADIX);
    if (i + rtp_strlen(sint) + 2 > TOTAL_STR_LEN)    /* 1 for the spaces another for \0*/
    {
        say_out(error_type, comment);
        say_out(error_type, sint);
    }
    else
    {
        rtp_strcpy(s2, comment);
        s2[i] = ' ';
        rtp_strcpy(s2+i+1, sint);

        say_out(error_type, s2);
    }
}

/* *****************************************************   */
void say_lint2(int error_type, RTP_PFINT8 comment, long val1, long val2)
{
char s2[TOTAL_STR_LEN];
int i;
char sint1[MAX_DINT_STR];
char sint2[MAX_DINT_STR];

    i = rtp_strlen(comment);
    rtp_ltoa(val1, sint1, SAY_RADIX);
    rtp_ltoa(val2, sint2, SAY_RADIX);
    if (i + rtp_strlen(sint1) + rtp_strlen(sint2) + 3 > TOTAL_STR_LEN) /* 2 for the spaces, 1 for \0 */
    {
        say_out(error_type, comment);
        say_out(error_type, sint1);
        say_out(error_type, sint2);
    }
    else
    {
        rtp_strcpy(s2, comment);
        s2[i] = ' ';
        rtp_strcpy(s2+i+1, sint1);
        *(s2+rtp_strlen(s2)+1) = '\0';
        *(s2+rtp_strlen(s2)) = ' ';
        rtp_strcat(s2, sint2);

        say_out(error_type, s2);
    }

}

/* *****************************************************   */
void say_dint(int error_type, RTP_PFINT8 comment, RTP_INT32 val)
{
char sint[MAX_DINT_STR];
char s2[TOTAL_STR_LEN];
int i;

    i = rtp_strlen(comment);
    rtp_ultoa(val, sint, SAY_RADIX);
    if (i + rtp_strlen(sint) + 2 > TOTAL_STR_LEN)
    {
        say_out(error_type, comment);
        say_out(error_type, sint);
    }
    else
    {
        rtp_strcpy(s2, comment);
        s2[i] = ' ';
        rtp_strcpy(s2+i+1, sint);

        say_out(error_type, s2);
    }
}

/* *****************************************************   */
void say_dint2(int error_type, RTP_PFINT8 comment, RTP_INT32 val1, RTP_INT32 val2)
{
char s2[TOTAL_STR_LEN];
int i;
char sint1[MAX_DINT_STR];
char sint2[MAX_DINT_STR];

    i = rtp_strlen(comment);
    rtp_ultoa(val1, sint1, SAY_RADIX);
    rtp_ultoa(val2, sint2, SAY_RADIX);
    if (i + rtp_strlen(sint1) + rtp_strlen(sint2) + 3 > TOTAL_STR_LEN)
    {
        say_out(error_type, comment);
        say_out(error_type, sint1);
        say_out(error_type, sint2);
    }
    else
    {
        rtp_strcpy(s2, comment);
        s2[i] = ' ';
        rtp_strcpy(s2+i+1, sint1);
        *(s2+rtp_strlen(s2)+1) = '\0';
        *(s2+rtp_strlen(s2)) = ' ';
        rtp_strcat(s2, sint2);

        say_out(error_type, s2);
    }
}

/* *****************************************************   */
void say_ip_addr(int error_type, RTP_PFINT8 comment, RTP_UINT8 RTP_FAR *addr)
{
char say_string[TOTAL_STR_LEN];
int i;

    say_string[0] = '\0';

    /* max IP length is xxx.xxx.xxx.xxx = 15 chars   */
    i = rtp_strlen(comment);
    if (i + 15 + 1 > TOTAL_STR_LEN)   /* leave room for eos */
    {
        say_out(error_type, comment);
    }
    else
    {
        rtp_strcpy(say_string, comment);
    }
    rtp_itoa(addr[0], say_string+rtp_strlen(say_string), 10);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[1], say_string+rtp_strlen(say_string), 10);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[2], say_string+rtp_strlen(say_string), 10);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[3], say_string+rtp_strlen(say_string), 10);

    say_out(error_type, say_string);
}

#if (INCLUDE_IPV6)
// *****************************************************
void say_ipv6_addr(int error_type, RTP_PFCCHAR comment, RTP_UINT8 RTP_FAR *addr)
{
char say_string[TOTAL_STR_LEN];
int i;

	say_string[0] = '\0';

	// max IP length is xxx.xxx.xxx.xxx = 15 chars
	i = rtp_strlen(comment);
    if (i + 15 + 1 > TOTAL_STR_LEN)   // leave room for eos
	{
		say_out(error_type, comment);
	}
	else
	{
		rtp_strcpy(say_string, comment);
	}
	for (i=0; i<IPV6_ALEN; i++)
	{
	    rtp_itoa(addr[i], say_string+rtp_strlen(say_string), 16);
	}
    say_out(error_type, say_string);
}
#endif
void say_ether_addr(int error_type, RTP_PFINT8 comment, RTP_UINT8 RTP_FAR *addr)
{
char say_string[TOTAL_STR_LEN];
int  i;

    say_string[0] = '\0';

    /* max ip length is xxx.xxx.xxx.xxx.xxx.xxx = 23 chars   */
    i = rtp_strlen(comment);
    if (i + 3 + 23 + 1 > TOTAL_STR_LEN)   /* leave room for eos */
    {
        say_out(error_type, comment);
    }
    else
    {
        rtp_strcpy(say_string, comment);
    }
    rtp_itoa(addr[0], say_string+rtp_strlen(say_string), 16);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[1], say_string+rtp_strlen(say_string), 16);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[2], say_string+rtp_strlen(say_string), 16);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[3], say_string+rtp_strlen(say_string), 16);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[4], say_string+rtp_strlen(say_string), 16);
    rtp_strcpy(say_string+rtp_strlen(say_string), ".");
    rtp_itoa(addr[5], say_string+rtp_strlen(say_string), 16);

    say_out(error_type, say_string);
}

/* *****************************************************   */
void say_str(int error_type, RTP_PFINT8 comment, RTP_PFINT8 str1)
{
char s2[SAY_STR_LEN];
int len;
int len_copy;

    if (!str1)
        str1 = "OOPS - str1 is 0";

    len = rtp_strlen(comment);
    if (str1)
        len += rtp_strlen(str1);

    rtp_strcpy(s2, comment);
    rtp_strcat((RTP_PFINT8)s2, " ");

    if ((len+1) > SAY_STR_LEN)
    {
        say_out(error_type, (RTP_PFINT8)"say_str: message below truncated");
        len_copy = SAY_STR_LEN - rtp_strlen(comment) - 2;
        rtp_strncpy((RTP_PFINT8)s2+rtp_strlen(comment), str1, len_copy);
        s2[SAY_STR_LEN-1] = '\0';
    }
    else
    {
        rtp_strcat((RTP_PFINT8)s2, str1);
    }
    say_out(error_type, s2);
}

/* *****************************************************   */
void say_str2(int error_type, RTP_PFINT8 comment, RTP_PFINT8 str1, RTP_PFINT8 str2)
{
char s2[SAY_STR_LEN];
int len;

    if (!str1)
        str1 = "OOPS - str1 is 0";
    if (!str2)
        str2 = "OOPS - str2 is 0";

    len = rtp_strlen(comment) + rtp_strlen(str1) + rtp_strlen(str2);
    if ((len+1) > SAY_STR_LEN)
    {
        say_str(error_type, comment, str1);
        say_str(error_type, (RTP_PFINT8)"     ", str2);
        return;
    }

    rtp_strcpy((RTP_PFINT8)s2, comment);
    rtp_strcat((RTP_PFINT8)s2, " ");
    rtp_strcat((RTP_PFINT8)s2, str1);
    say_str(error_type, s2, str2);
}

/* ********************************************************************   */
/* API                                                                    */
/* ********************************************************************   */

/* ********************************************************************   */
/* DEBUG ROUTINES - i.e. RTP_DEBUG_ERROR and RTP_DEBUG_LOG map to these routines  */
/* ********************************************************************   */

/* ********************************************************************            */
/* xn_rtp_debug_error() -  Log error information                                       */
/*                                                                                 */
/* Summary:                                                                        */
/*   #include "rtpdbapi.h"                                                         */
/*                                                                                 */
/*   void xn_rtp_debug_error(comment, type, val1, val2)                                */
/*       PFCCHAR comment - comment to be printed first                             */
/*       int     type    - type of parameters val1 and val2 (EBS_INT2 for example) */
/*       dword   val1    - value of first variable/structure etc. to be printed    */
/*       dword   val2    - value of second variable or possible lenght of val1 to  */
/*                         be printed                                              */
/*                                                                                 */
/* Description:                                                                    */
/*                                                                                 */
/*   Logs error information thru the callback routine CB_WR_SCREEN_STRING.         */
/*   The values for type are defined in rtpdbapi.h                                 */
/*                                                                                 */
/*   For more details see the RTIP Manual.                                         */
/*                                                                                 */
/* Returns:                                                                        */
/*   Nothing                                                                       */
/*                                                                                 */

void xn_rtp_debug_error(RTP_PFINT8 comment, int type, RTP_INT32 val1, RTP_INT32 val2)
{
    switch (type)
    {
    case NOVAR :
        say(ERROR_TYPE, comment);
        break;
    case EBS_INT1  :
        say_int(ERROR_TYPE, comment, (int)val1);
        break;
    case EBS_INT2  :
        say_int2(ERROR_TYPE, comment, (int)val1, (int)val2);
        break;
    case LINT1 :
        say_lint(ERROR_TYPE, comment, (long)val1);
        break;
    case LINT2 :
        say_lint2(ERROR_TYPE, comment, (long)val1, (long)val2);
        break;
    case DINT1 :
        say_dint(ERROR_TYPE, comment, (RTP_UINT32)val1);
        break;
    case DINT2 :
        say_dint2(ERROR_TYPE, comment, (RTP_UINT32)val1, (RTP_UINT32)val2);
        break;
    case STR1 :
        say_str(ERROR_TYPE, comment, (RTP_PFINT8)val1);
        break;
    case STR2 :
        say_str2(ERROR_TYPE, comment, (RTP_PFINT8)val1, (RTP_PFINT8)val2);
        break;
    case IPADDR:
        say_ip_addr(ERROR_TYPE, comment, (RTP_PFUINT8)val1);
        break;
    case ETHERADDR:
        say_ether_addr(ERROR_TYPE, comment, (RTP_PFUINT8)val1);
        break;
    case PKT:
        say_hex(ERROR_TYPE, comment, (char *)val1, (int)val2);
        break;
    default:
        say_out(ERROR_TYPE, "ERROR: debug_error() - invalid type for :");
        say_out(ERROR_TYPE, comment);
        break;
    }
}

/* ********************************************************************   */
/* DEBUG LOG                                                              */
/* ********************************************************************   */
#if (DEBUG_LEVEL > 1)
/* ********************************************************************            */
/* xn_rtp_debug_error() -  Log error information                                       */
/*                                                                                 */
/* Summary:                                                                        */
/*   #include "rtpdbapi.h"                                                         */
/*                                                                                 */
/*   void xn_rtp_debug_error(comment, type, val1, val2)                                */
/*       PFCCHAR comment - comment to be printed first                             */
/*       int     type    - type of parameters val1 and val2 (EBS_INT2 for example) */
/*       dword   val1    - value of first variable/structure etc. to be printed    */
/*       dword   val2    - value of second variable or possible lenght of val1 to  */
/*                         be printed                                              */
/*                                                                                 */
/* Description:                                                                    */
/*                                                                                 */
/*   Logs error information thru the callback routine CB_WR_LOG_STRING.            */
/*   The values for type are defined in rtpdbapi.h                                 */
/*                                                                                 */
/*   For more details see the RTIP Manual.                                         */
/*                                                                                 */
/* Returns:                                                                        */
/*   Nothing                                                                       */
/*                                                                                 */

void xn_rtp_debug_log(RTP_PFCCHAR comment, int level, int type, RTP_UINT32 val1, RTP_UINT32 val2)
{
#if (INCLUDE_ROUTING_TABLE)
PROUTE prt;
#endif

    if (level > DEBUG_LEVEL)
        return;

    switch (type)
    {
    case NOVAR :
        say_out(level, (RTP_PFCHAR)comment);
        break;
    case EBS_INT1  :
        say_int(level, (RTP_PFCHAR)comment, (int)val1);
        break;
    case EBS_INT2  :
        say_int2(level,(RTP_PFCHAR) comment, (int)val1, (int)val2);
        break;
    case LINT1 :
        say_lint(level,(RTP_PFCHAR) comment, (long)val1);
        break;
    case LINT2 :
        say_lint2(level, (RTP_PFCHAR)comment, (long)val1, (long)val2);
        break;
    case DINT1 :
        say_dint(level,(RTP_PFCHAR) comment, (RTP_UINT32)val1);
        break;
    case DINT2 :
        say_dint2(level, (RTP_PFCHAR)comment, (RTP_UINT32)val1, (RTP_UINT32)val2);
        break;
    case STR1 :
        say_str(level, (RTP_PFCHAR)comment, (RTP_PFCHAR)val1);
        break;
    case STR2 :
        say_str2(level, (RTP_PFCHAR)comment, (RTP_PFCHAR)val1, (RTP_PFCHAR)val2);
        break;
    case IPADDR:
        say_ip_addr(level, (RTP_PFCHAR)comment, (RTP_PFUINT8)val1);
        break;
    case ETHERADDR:
        say_ether_addr(level,(RTP_PFCHAR) comment, (RTP_PFUINT8)val1);
        break;
    case PKT:
        say_hex(level, (char *)comment, (char *)val1, val2);
        break;
    default:
        say_out(level, "ERROR: xn_rtp_debug_error() - invalid type for :");
        say_out(level,(RTP_PFCHAR) comment);
        break;
    }
}
#endif	/* DEBUG_LEVEL > 1 */

#endif  /* DEBUG_LEVEL > 0 */

#if (INCLUDE_PROFILING)
/* ********************************************************************     */

/* Profiling package -
   This is an execution profiling package. It is used internally to 
   profile code sections. 
   Two macros: PROFILE_ENTER(TOKEN) and PROFILE_EXIT(TOKEN) are defined. If 
   profiling is turned off they do nothing. If they are on they note the
   start time and end time for the section being profiled. The routine
   dump_profile() dumps the duration for each call. This profiling package
   does not do histogram analysis. It only logs the duration of the last
   pass through the code being profiled.. so it must be used carefully.
  
   Currently the only platform that supports profiling is the NET186 board.
*/

struct timerecord profile_time_array[PROF_N_RECORDS];
RTP_UINT32 tick_every_10000_tenth_micros =0;

struct namerecord profile_name_array[PROF_N_RECORDS] = {
    {PROF_TOKEN_IP_INTERPRET, "IP INTERPRET"},
};

static char *get_name(int token)
{
int i;
    for (i = 0; i < PROF_N_RECORDS; i++)
    {
        if (token == profile_name_array[i].token)
            return(profile_name_array[i].name);  
    }
    return("LOST");
}

void dump_profile(void)
{
int i;
RTP_UINT32 duration;

    for (i = 0; i < PROF_N_RECORDS; i++)
    {
        if (profile_time_array[i].start_micro)
        {
            duration = profile_time_array[i].end_micro 
                    - profile_time_array[i].start_micro;
            rtp_printf("%20s ---------------- %8l\n", get_name(i), duration/10);
        }
    }
}

#endif
#endif


