 /*
 | RTPUTIL.C - Runtime Platform Network Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: tony $
 |  $Date: 2005/06/30 00:42:02 $
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
#include "rtpprint.h"
#include "rtpstr.h"
#include "rtpscnv.h"
#include "rtpchar.h"
#if (RTP_INCLUDE_PRINTF)
#if (!defined (rtp_vprintf))
#include "rtpterm.h"
#endif
#endif /* RTP_INCLUDE_PRINTF */


/************************************************************************
* Defines
************************************************************************/

/* -------------------------------------------- */
/*      Add floating point printf support.      */
/* -------------------------------------------- */
#define RTP_INCLUDE_PRINTF_FLOAT                0

/* -------------------------------------------- */
/*   Add little endian address printf support.  */
/* -------------------------------------------- */
#define RTP_INCLUDE_PRINTF_LENDIAN_ADDRESS      0

/* -------------------------------------------- */
/*      Add 16bit address printf support.       */
/* -------------------------------------------- */
#define RTP_INCLUDE_PRINTF_16BIT_ADDRESS        0


#if (!defined (rtp_vprintf) || !defined (rtp_vsnprintf))
#define vval  _val._val
#define cp    _val._str._cp
#define slen  _val._str._len

#define OPTSIGN     0x00
#define SPCSIGN     0x01
#define MANSIGN     0x02
#define NEGSIGN     0x03
#define FILL        0x04
#define LEFT        0x08
#undef LONG
#define LONG        0x10
#define UPCASE      0x20
#define TEN         0x00
#define EIGHT       0x40
#define SIXTEEN     0x80
#define UNSIGN      0xC0
#define BASEM       0xC0
#define EFMT       0x100
#define GFMT       0x200
#define FFMT       0x400
#define ALTERN     0x800
#define SHORT     0x1000
#define POINTER   0x2000
#define THOUSANDS 0x4000
#define PREC_SET  0x8000U
#endif /* !defined (rtp_vprintf) || !defined (rtp_vsnprintf) */

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
#if (!defined (rtp_vprintf) || !defined (rtp_vsnprintf))
const unsigned long int dpowers[10] =
{
	1,
	10,
	100,
	1000,
	10000,
	100000l,
	1000000l,
	10000000l,
	100000000l,
	1000000000l
};

const unsigned long int hexpowers[8] =
{
	1,
	0x10,
	0x100,
	0x1000,
	0x10000l,
	0x100000l,
	0x1000000l,
	0x10000000l
};

const unsigned long int octpowers[11] =
{
	1,
	010,
	0100,
	01000,
	010000,
	0100000,
	01000000l,
	010000000l,
	0100000000l,
	01000000000l,
	010000000000l
/* too large  ,0100000000000ul   */
};

const char *upper_string = "0123456789ABCDEF";
const char *lower_string = "0123456789abcdef";
#endif /* !defined (rtp_vprintf) || !defined (rtp_vsnprintf) */

/************************************************************************
* Structures
************************************************************************/
#if (!defined (rtp_vprintf) || !defined (rtp_vsnprintf))
struct rtp_printf_io
{
    union
    {              
        struct
        {
            char *output_buffer;
            long buffer_size;
            long bufpos;
        } str;
    } channel;
  
    long (*write)(struct rtp_printf_io *io, const char * msg, long msglen);
};
#endif /* !defined (rtp_vprintf) || !defined (rtp_vsnprintf) */

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
#if (!defined (rtp_vprintf) || !defined (rtp_vsnprintf))
static long _vzprintf       (struct rtp_printf_io *io, const char * f, rtp_va_list ap);

#if (RTP_INCLUDE_PRINTF)
static long _printf_write   (struct rtp_printf_io *io, const char * msg, long msglen);
#endif

#if (RTP_INCLUDE_SPRINTF)
static long _snprintf_write (struct rtp_printf_io *io, const char * msg, long msglen);
#endif
#endif /* !defined (rtp_vprintf) || !defined (rtp_vsnprintf) */

/************************************************************************
* Function Bodies
************************************************************************/

#if (RTP_INCLUDE_PRINTF)
#ifndef rtp_printf
/*----------------------------------------------------------------------*
                             rtp_printf
 *----------------------------------------------------------------------*/
/** @memo   Print a formatted string to the terminal using 
    rtp_term_cputs.

    @doc    Print a formatted string to the terminal using 
    rtp_term_cputs.  Format the string using the variable 
    length argument list.

	@precondition If not using the native system implementation
	of these functions via the macros in rtpprint.h and this 
	funcitionality is desired, RTP_INCLUDE_PRINTF must 
	be set to 1 in rtpprint.h.

    @return Number of characters printed.
 */
int rtp_printf (
  const char * f,                       /** Formatted string. */
  ...                                   /** Optional arguments for the formatted string. */
  )
{
rtp_va_list ap;
int ret;

    rtp_va_start(ap, f);
    ret = rtp_vprintf(f, ap);
    rtp_va_end(ap);

    return (ret);
}
#endif


#ifndef rtp_vprintf
/*----------------------------------------------------------------------*
                             rtp_vprintf
 *----------------------------------------------------------------------*/
/** @memo   Print a formatted string to the terminal using 
    rtp_term_cputs.

    @doc    Print a formatted string to the terminal using 
    rtp_term_cputs.  Format the string using the list of 
    arguments passed in using rtp_va_list.

	@precondition If not using the native system implementation
	of these functions via the macros in rtpprint.h and this 
	funcitionality is desired, RTP_INCLUDE_PRINTF must 
	be set to 1 in rtpprint.h.

    @return Number of characters printed.
 */
int rtp_vprintf (
  const char * f,                       /** Formatted string. */
  rtp_va_list ap                        /** Arguments for the formatted string. */
  )
{
struct rtp_printf_io io = {{{0,0,0}},0};

    io.write = &_printf_write;
    return (_vzprintf (&io, f, ap));
}
#endif
#endif /* RTP_INCLUDE_PRINTF */


#if (RTP_INCLUDE_SPRINTF)
#ifndef rtp_sprintf
/*----------------------------------------------------------------------*
                             rtp_sprintf
 *----------------------------------------------------------------------*/
/** @memo   Print formatted data to a string.

    @doc    Print formatted data to a string.  Format the data
    using the variable length argument list.

	@precondition If not using the native system implementation
	of these functions via the macros in rtpprint.h and this 
	funcitionality is desired, RTP_INCLUDE_SPRINTF must 
	be set to 1 in rtpprint.h.
	
    @return Number of characters printed.
 */
int rtp_sprintf (
  char * wh,                            /** Storage location string for the formatted data. */
  const char * f,                       /** Formatted data. */
  ...                                   /** Optional arguments for the formatted data. */
  )
{
rtp_va_list ap;
int ret;

    rtp_va_start(ap, f);
    ret = rtp_vsnprintf(wh, 32767, f, ap);
    rtp_va_end(ap);

    return (ret);
}
#endif


#ifndef rtp_snprintf
/*----------------------------------------------------------------------*
                             rtp_snprintf
 *----------------------------------------------------------------------*/
/** @memo   Print formatted data to a string.

    @doc    Print formatted data to a string.  Format the data
    using the variable length argument list.  Limit the number
    of characters to be formatted to the bufflen.

	@precondition If not using the native system implementation
	of these functions via the macros in rtpprint.h and this 
	funcitionality is desired, RTP_INCLUDE_SPRINTF must 
	be set to 1 in rtpprint.h.

    @return Number of characters printed.
 */
int rtp_snprintf (
  char * wh,                            /** Storage location string for the formatted data. */
  int bufflen,                          /** Number of characters to store. */
  const char * f,                       /** Formatted data. */
  ...                                   /** Optional arguments for the formatted data. */
  )
{
rtp_va_list ap;
int ret;

    rtp_va_start(ap, f);
    ret = rtp_vsnprintf(wh, bufflen, f, ap);
    rtp_va_end(ap);

    return (ret);
}
#endif


#ifndef rtp_vsprintf
/*----------------------------------------------------------------------*
                             rtp_vsprintf
 *----------------------------------------------------------------------*/
/** @memo   Print formatted data to a string.

    @doc    Print formatted data to a string.  Format the data
    using the argument list.

	@precondition If not using the native system implementation
	of these functions via the macros in rtpprint.h and this 
	funcitionality is desired, RTP_INCLUDE_SPRINTF must 
	be set to 1 in rtpprint.h.

    @return Number of characters printed.
 */
int rtp_vsprintf (
  char * wh,                            /** Storage location string for the formatted data. */
  const char * f,                       /** Formatted data. */
  rtp_va_list ap                        /** Arguments for the formatted string. */
  )
{
    return (rtp_vsnprintf (wh, 32767, f, ap));
}
#endif


#ifndef rtp_vsnprintf
/*----------------------------------------------------------------------*
                             rtp_vsnprintf
 *----------------------------------------------------------------------*/
/** @memo   Print formatted data to a string.

    @doc    Print formatted data to a string.  Format the data
    using the argument list.  Limit the number of characters to 
    be formatted to the bufflen.

	@precondition If not using the native system implementation
	of these functions via the macros in rtpprint.h and this 
	funcitionality is desired, RTP_INCLUDE_SPRINTF must 
	be set to 1 in rtpprint.h.
	
    @return Number of characters printed.
 */
int rtp_vsnprintf (
  char * wh,                            /** Storage location string for the formatted data. */
  long bufflen,                         /** Number of characters to store. */
  const char * f,                       /** Formatted data. */
  rtp_va_list ap                        /** Arguments for the formatted string. */
  )
{
struct rtp_printf_io io = {{{0,0,0}},0};

    io.write = &_snprintf_write;
    
    io.channel.str.output_buffer = wh;
    io.channel.str.buffer_size   = bufflen;
    io.channel.str.bufpos        = 0;

    return (_vzprintf (&io, f, ap));
}
#endif
#endif /* RTP_INCLUDE_SPRINTF */


#if (!defined (rtp_vprintf) || !defined (rtp_vsnprintf))
/************************************************************************
* Helper Functions
************************************************************************/

/*----------------------------------------------------------------------
 NOTE: this software is almost completely rewritten by 
 Ger 'Insh_Allah' Hobbelt to facilitate 'output function callback', 
 additional format specifiers and modifiers and several other 
 formatting characteristic changes, additions and fixes.

 This software has been based on ideas as implemented in doprnt by 
 HiTech software [License to distribute granted to EBS 1996] and the 
 portable snprintf implementation by Mark Martinec
 (www.ijs.si/software/snprintf/)
----------------------------------------------------------------------*/

static long _vzprintf (struct rtp_printf_io *io, const char * f, rtp_va_list ap)
{
char  cbuf[82];
char  tmp[2];
int   prec;
int   c;
int   width;
int   dump_width;
unsigned  flag;

    union
    {
        unsigned long int _val;
        struct
        {
            const char *  _cp;
            unsigned  _len;
        } _str;
    } _val;
    
long ret = 0;
long wr_len = 0;
char *next;
#define cbuf_end   (cbuf + sizeof(cbuf))
#if (RTP_INCLUDE_PRINTF_FLOAT != 0)
const char *percent_location;
rtp_va_list ap_copy;
#endif
const char *mapping = ((char *)0);

    while (*f)
    {
        c = *f++;
        if (c != '%')
        {
            const char * f_end;

            /* fast: scan till f_end '%': write string part */
            for (f_end = f--; *f_end; f_end++)
            {
                if (*f_end == '%') break;
            }

            wr_len = (*io->write)(io, f, (int)(f_end - f));
            if (wr_len >= 0) ret += wr_len;
            f = f_end;
            continue;
        }
        width = 0;
        dump_width = 0;
        flag = 0;
#if (RTP_INCLUDE_PRINTF_FLOAT != 0)
        percent_location = f - 1;  /* remember position of the '%' */
        ap_copy = ap;
#endif
        for(;;)
        {
            switch(*f)
            {
            case '-':
                flag |= LEFT;
                f++;
                continue;

            case ' ':
                flag |= SPCSIGN;
                f++;
                continue;

            case '+':
                flag |= MANSIGN;
                f++;
                continue;

            case '#':
                flag |= ALTERN;
                f++;
                continue;

            case '0':
                flag |= FILL;
                f++;
                continue;

            case '\'':
                flag |= THOUSANDS;
                f++;
                continue;

            default:
                break;
            }
            break;
        }

        if (rtp_isdigit(*f))
        {
            width = 0;
            do
                width = width * 10 + *f++ - '0';
            while(rtp_isdigit(*f));
            dump_width = width;
        }
        else if (*f == '*')
        {
            width = rtp_va_arg(ap, int);
            dump_width = width;
            if (width < 0)
            {
                width = -width;
                flag |= LEFT;
            }
            f++;
        }

        prec = 0;
        if (*f == '.')
        {
            flag |= PREC_SET;  /* 'precision == 0' is quite different from 'unspecified precision' */
            if (*++f == '*')
            {
                prec = rtp_va_arg(ap, int);
                if (prec < 0)
                {
                    prec = 0;
                    flag &= ~PREC_SET;
                }
                f++;
            }
            else
            {
                while(rtp_isdigit(*f))
                    prec = prec*10 + *f++ - '0';
            }
        }
#if 0
        else
        {
            prec = 0;
        }
#endif

        if (flag & MANSIGN)
            flag &= ~SPCSIGN;
        if (flag & SPCSIGN)
            flag &= ~MANSIGN;
        if (flag & LEFT)
            flag &= ~FILL;

        /* modifier first! */
        switch (c = *f++)
        {
        case 0:
            f--;
            continue; /* !!! nasty, but no 'goto' no more. All optimizing compilers will work better now :-) !!! */

        case 'h':
            flag |= SHORT;
            c = *f++; /* SHORT */
            break;

        case 'l':
            flag |= LONG;
            c = *f++;
            /* don't support 'll' long-long */
            break;

        case 'O':
            flag |= LONG;
            c = 'o';
            break;

        case 'D':
            flag |= LONG;
            c = 'd';
            break;

        case 'U':
            flag |= LONG;
            c = 'u';
            break;
        
        case 'F':
            /* FAR pointer. We don't particularly care or support this. */
            c = *f++;
            break;
            
        case 'N':
            /* NEAR pointer. We don't particularly care or support this. */
            c = *f++;
            break;
            
        default:
            break;
        }

        /* type! 'hack' for i/o/u/x radixes&signs */
        switch (c)
        {
        case 'u':
        case 'o':
        case 'X':
        case 'x':
        case 'd':
        case 'i':
            switch (c)
            {
            case 'u':
                flag |= UNSIGN;
                break;

            case 'o':
                flag |= EIGHT;
                break;

            case 'X':
                flag |= UPCASE;
            case 'x':
                flag |= SIXTEEN;
                break;
            }

            if (flag & LONG)
                flag &= ~SHORT;
                
            /* format integer! */
            if ((flag & BASEM) == TEN)
            {
                if (flag & LONG)
                    vval = (unsigned long int)rtp_va_arg(ap, long int);
                else if (flag & SHORT)
                    vval = (unsigned long int)rtp_va_arg(ap, short int);
                else
                    vval = (unsigned long int)rtp_va_arg(ap, int);

                if ((long int)vval < 0)
                {
                    flag |= NEGSIGN;
                    vval = 0 - vval;
                }
            }
            else
            {
                if (flag & LONG)
                    vval = (unsigned long int)rtp_va_arg(ap, unsigned long int);
                else if (flag & SHORT)
                    vval = (unsigned long int)rtp_va_arg(ap, unsigned short int);
                else
                    vval = (unsigned long int)rtp_va_arg(ap, unsigned int);
            }

            if (prec == 0 && vval == 0 && !(flag & PREC_SET))
                prec++;

            switch ((unsigned char)(flag & BASEM))
            {
            default:
            case TEN:
            case UNSIGN:
                for(c = 1 ; c != sizeof(dpowers)/sizeof(dpowers[0]); c++)
                    if (vval < dpowers[c])
                        break;
                break;

            case SIXTEEN:
                for(c = 1 ; c != sizeof(hexpowers)/sizeof(hexpowers[0]); c++)
                    if (vval < hexpowers[c])
                        break;
                break;

            case EIGHT:
                for(c = 1 ; c != sizeof(octpowers)/sizeof(octpowers[0]); c++)
                    if (vval < octpowers[c])
                        break;
                break;
            }
            if (c < prec)
                c = prec;
            else if (prec < c)
                prec = c;
            if (width && (flag & NEGSIGN))
                width--;

            if (width > prec)
                width -= prec;
            else
                width = 0;

            if (vval && (flag & (FILL|BASEM|ALTERN)) == (EIGHT|ALTERN))
            {
                if (width)
                    width--;
            }
            else if ((flag & (BASEM|ALTERN)) == (SIXTEEN|ALTERN))
            {
                if (width > 2)
                    width -= 2;
                else
                    width = 0;
            }

            next = cbuf;
            if (flag & FILL)
            {
                if (flag & MANSIGN)
                {
                    if (flag & SPCSIGN)
                        *next++ = '-';    /* NEGSIGN ==> both MANSIGN          */
                                          /*             and SPCSIGN are 'on'! */
                    else
                        *next++ = '+';
                }
                else if (flag & SPCSIGN)
                {
                    *next++ = ' ';
                }

                if (vval && (flag & (BASEM|ALTERN)) == (EIGHT|ALTERN))
                {
                    *next++ = '0';
                }
                else if ((flag & (BASEM|ALTERN)) == (SIXTEEN|ALTERN))
                {
                    *next++ = '0';
                    if (flag & UPCASE)
                        *next++ = 'X';
                    else
                        *next++ = 'x';
                }

                if (width)
                {
                    do
                    {
                        *next++ = '0';

                        if (next == cbuf_end)
                        {
                            wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                            if (wr_len >= 0) ret += wr_len; else return (wr_len);
                            next = cbuf;
                        }
                    } while(--width);
                }
            }
            else
            {
                if (width && !(flag & LEFT))
                {
                    do
                    {
                        *next++ = ' ';

                        if (next == cbuf_end)
                        {
                            wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                            if (wr_len >= 0) ret += wr_len; else return wr_len;
                            next = cbuf;
                        }
                    } while(--width);
                }

                if (flag & MANSIGN)
                {
                    if (flag & SPCSIGN)
                        *next++ = '-';
                    else
                        *next++ = '+';
                }
                else if (flag & SPCSIGN)
                {
                    *next++ = ' ';
                }

                if (vval && (flag & (BASEM|ALTERN)) == (EIGHT|ALTERN))
                {
                    *next++ = '0';
                }
                else if ((flag & (BASEM|ALTERN)) == (SIXTEEN|ALTERN))
                {
                    *next++ = '0';
                    if (flag & UPCASE)
                        *next++ = 'X';
                    else
                        *next++ = 'x';
                }
            }

            while (prec > c)
            {
                *next++ = '0';
                prec--;

                if (next == cbuf_end)
                {
                    wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                    if (wr_len >= 0) ret += wr_len; else return wr_len;
                    next = cbuf;
                }
            }

            /* check if complete number will fit in space left in buffer now */
            if (next >= cbuf + sizeof(cbuf) - 2 - prec)
            {
                wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                if (wr_len >= 0) ret += wr_len; else return (wr_len);
                next = cbuf;
            }

            if (flag & UPCASE)
                mapping = upper_string;
            else
                mapping = lower_string;

            while (prec-- > 0)
            {
                switch ((unsigned char)(flag & BASEM))
                {
                default:
                case TEN:
                case UNSIGN:
                    c = (int)((vval / dpowers[prec]) % 10 + '0');
                    break;

                case SIXTEEN:
                    c = mapping[(int)((vval / hexpowers[prec]) & 0xF)];
                    break;

                case EIGHT:
                    c = (int)(((vval / octpowers[prec]) & 07) + '0');
                    break;
                }
                *next++ = (char)c;
            }

            if ((flag & LEFT) && width)
            {
                do
                {
                    *next++ = ' ';

                    if (next == cbuf_end)
                    {
                        wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                        if (wr_len >= 0) ret += wr_len; else return (wr_len);
                        next = cbuf;
                    }
                } while(--width);
            }

            if (next > cbuf)
            {
                wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                if (wr_len >= 0) ret += wr_len; else return (wr_len);
            }
            continue;
        
        case 'n':
            /* %n: write back the total number of characters written so far */
            {
                int *intp = rtp_va_arg(ap, int *);
                
                if (intp)
                    *intp = ret;
            }
            continue;   
            
        case 'p':
            /* %p: takes a void * and outputs it in hex - format: 0xPPPP[:]PPPP */
            {
                void * val = rtp_va_arg(ap, void *);
                unsigned char *ptr = (unsigned char *)&val;
                int i;

                next = cbuf;

                if ((flag & ALTERN) == ALTERN)
                {
                    *next++ = '0';
                    if (flag & UPCASE)
                    {
                        *next++ = 'X';
                    }
                    else
                    {
                        *next++ = 'x';
                    }
                }
                *next++ = '0';
                if (flag & UPCASE)
                {
                    mapping = upper_string;
                }
                else
                {
                    mapping = upper_string;  /* lower_string */
                }
#if (INCLUDE_LENDIAN_ADDRESS_PRINTF_SUPPORT)
                for(i = sizeof(void *) - 1; i >= 0; i--)
#else
                for(i = 0; i < sizeof(void *); i++)
#endif /* INCLUDE_LENDIAN_ADDRESS_PRINTF_SUPPORT */
                {
#if (INCLUDE_16BIT_ADDRESS_PRINTF_SUPPORT)
                    /* ----------------------------------- */
                    /*  Add ':' for 16bit address format.  */
                    /* ----------------------------------- */
                    if ((i == 4) && (sizeof(void *) > 4))
                    {
                        *next++ = ':';
                    }
#endif /* INCLUDE_16BIT_ADDRESS_PRINTF_SUPPORT */

                    *next++ = mapping[ptr[i] >> 4];
                    *next++ = mapping[ptr[i] & 0x0f];
                }

                wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                if (wr_len >= 0) ret += wr_len; else return (wr_len);
            }
            continue;

        case 'c':
            vval = rtp_va_arg(ap, int);
            c = (int)(vval >> 8);
            /* support double-byte characters! */
            if ((flag & LONG) && c && ((unsigned char)c != 0xFF))
            {
                tmp[0] = (char) c;
                tmp[1] = (char) vval;
                slen = 2;
            }
            else
            {
                tmp[0] = (char) vval;
                slen = 1;
            }
            cp = tmp;
            if (0)
            {
        case '%':
        default:     
                tmp[0] = (char) c;
                cp = tmp;
                slen = 1;
                if (0)
                {
#if (RTP_INCLUDE_PRINTF_FLOAT == 0)
        case 'L':
                    switch (*f)
                    {
                    case 'e':
                    case 'E':
                    case 'f':
                    case 'F':
                    case 'g':
                    case 'G':
                        f++; /* LONG DOUBLE FORMAT */
                        break;
                    default:
                        break;
                    }
                    /* fall through */
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
                    cp = "(non-float printf)";
                    prec = slen = 18;
                    if (0)
#endif
                    {
        case 's':
                        cp = rtp_va_arg (ap, char *);
                        if (!cp)
                            cp = "(null)";
                        /* slen = rtp_strlen(cp); */
                        slen = 0;
                        while (cp[slen])
                            slen++;
                    }
                }
            }
            if ((flag & PREC_SET) && (prec < (int)slen))
                slen = prec;
            if (width > (int)slen)
                width -= slen;
            else
                width = 0;

            next = cbuf;
            if (!(flag & LEFT))
            {
                while (width--)
                {
                    *next++ = ' ';

                    if (next == cbuf_end)
                    {
                        wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                        if (wr_len >= 0) ret += wr_len; else return (wr_len);
                        next = cbuf;
                    }
                }
                if (next > cbuf)
                {
                    wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                    if (wr_len >= 0) ret += wr_len; else return (wr_len);
                }
            }

            if (slen > 0)
            {
                wr_len = (*io->write)(io, cp, slen);
                if (wr_len >= 0) ret += wr_len; else return (wr_len);
            }

            next = cbuf;
            if (flag & LEFT)
            {
                while(width--)
                {
                    *next++ = ' ';

                    if (next == cbuf_end)
                    {
                        wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                        if (wr_len >= 0) ret += wr_len; else return (wr_len);
                        next = cbuf;
                    }
                }

                if (next > cbuf)
                {
                    wr_len = (*io->write)(io, cbuf, (int)(next - cbuf));
                    if (wr_len >= 0) ret += wr_len; else return (wr_len);
                }
            }
            continue;

#if (RTP_INCLUDE_PRINTF_FLOAT != 0)
        case 'L':
            switch (*f)
            {
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                f++; /* LONG DOUBLE FORMAT */
                break;
            default:
                break;
            }
            /* fall through */
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
            {
                char fmtstr[sizeof("(float printf format too complex)")];
                int fmtlen;
                /* use standard RTL sprintf() to format these elements! */

                fmtlen = (int)(f - percent_location);
                if (fmtlen >= sizeof(fmtstr)
                        /* will sprintf() output fit? rough 'conservative' estimate: */
                        || (width + 3 >= sizeof(cbuf))
                        || (prec + 3 + 5 >= sizeof(cbuf))
                     )
                {
                    /* oops... format string doesn't fit. This is crazy. so barf! */
                    rtp_strcpy(cbuf, "(float printf format too complex)");
                }
                else
                {
                    rtp_strncpy(fmtstr, percent_location, fmtlen);
                    fmtstr[fmtlen] = 0;

                    cbuf[0] = 0;      /* some vsprintf() versions don't write anything if something fails dramatically inside their bowels... */
                    vsprintf(cbuf, fmtstr, ap_copy);
                }

                if (cbuf[0] != 0)
                {
                    wr_len = (*io->write)(io, cbuf, (int)rtp_strlen(cbuf));
                    if (wr_len >= 0) ret += wr_len; else return (wr_len);
                }
            }
            continue;
#endif
        }
    }

    /* tmp[0] = 0;                                           */
    /* wr_len = (*io->write)(io, tmp, 1);                    */
    /* if (wr_len >= 0) ret += wr_len; else return (wr_len); */

    return (ret);
}


#if (RTP_INCLUDE_PRINTF)
/*----------------------------------------------------------------------
 Callback used to print formatted string on your display.
----------------------------------------------------------------------*/
static long _printf_write (struct rtp_printf_io *io, const char * msg, long msglen)
{
char str[80];// pmb i do not know why 33 (80 character terminals are very common */
long len = msglen;

    while (len >= sizeof (str))
    {
        rtp_memmove (str, msg, sizeof(str)-1);
        str[sizeof (str)-1] = 0;
        rtp_term_cputs (str);
        len -= sizeof (str)-1;
        msg += sizeof (str)-1;
    }

    if (len > 0)
    {
        rtp_memmove (str, msg, len);
        str[len] = 0;
        rtp_term_cputs (str);
    }

    return (msglen);
}
#endif /* RTP_INCLUDE_PRINTF */


#if (RTP_INCLUDE_SPRINTF)
/*----------------------------------------------------------------------
 Callback used to write the formatted printf string 
 particles to the destination buffer.
----------------------------------------------------------------------*/
static long _snprintf_write (struct rtp_printf_io *io, const char * msg, long msglen)
{
    if (io->channel.str.bufpos + msglen >= io->channel.str.buffer_size)
    {
        msglen = io->channel.str.buffer_size - io->channel.str.bufpos - 1;
    }
    if (msglen <= 0) return (0);

    rtp_memmove (io->channel.str.output_buffer + io->channel.str.bufpos, msg, msglen);
    io->channel.str.bufpos += msglen;
    io->channel.str.output_buffer[io->channel.str.bufpos] = 0;

    return (msglen);
}
#endif /* RTP_INCLUDE_SPRINTF */
#endif /* !defined (rtp_vprintf) || !defined (rtp_vsnprintf) */


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
