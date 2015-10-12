/*
 * porting header file by I_A
 *
 * Porting Warning:
 *
 * If your system does NOT support memcpy() et al, you're between a rock
 * and a hard place here. The OpenSSL code is crawling with those calls
 * and I didn't put in the extra effort to ALSO 'wrap' any str* and mem*
 * function in there. Iff you want to support such odd embedded systems,
 * you'd better make sure those wrappers are added here.
 */

#ifndef __EBSNET_SSL_PORT_H__
#define __EBSNET_SSL_PORT_H__

#if defined(AN_EBSNET_PORT)

/* define this one if we're just testing this embedded port on Linux itself */
//#define EBSNET_FAKE_PORT

/*
  [i_a] determine which crypto algorithms we're going to include in the final
  binary.
 */


#ifndef INCLUDE_COMPLETE_SSL  
#define INCLUDE_COMPLETE_SSL   0  /* 0 if you want parts stripped... */
#endif
/* --------------------------------------- */

#if (!INCLUDE_COMPLETE_SSL)
#define NO_BF          1
#define NO_CAST        1
#undef  NO_DH					//#define NO_DH		   1	
#undef  NO_DES
#define NO_DSA         1
#undef  NO_HMAC
#define NO_IDEA        1
#define NO_MD2         1
#define NO_MD4         1
#undef  NO_MD5
#define NO_MDC2        1
#define NO_RC2         1
#define NO_RC4         1
#define NO_RC5         1
#define NO_RIPEMD      1
#undef  NO_RSA
#undef  NO_SHA
#undef  NO_SHA0
#undef  NO_SHA1
#define NO_FORZETTA    1
#define NO_DESCBCM     1

#define ONLY_DES_CBC   1    /* only DES CBC */
#define NO_EXPORT_CIPHERS	1 /* define if you don't wish to support export-only ciphers. */

#define NO_SSL2        1
#define NO_SSL3		   1
#undef  NO_SSL2
#undef  NO_SSL3
#undef  NO_TLS1

#undef  NO_FP_API			/* #define: you don't want 'FILE *' emulation/handling. */
#define NO_ERR		   1 	/* #define: don't display error descriptions and file loc's */
#undef  NO_OBJECT 			/* #define: all objects (OIDs) must be loaded by hand (see OBJ_DAT.C). */

/* --------------------------------------- */

/*
   #define if you don't want any 'password entry' code for access to 
   'protected' private keys 
 */
#define NO_PWD_ENTRY   1     

/* --------------------------------------- */

/*
   #define'd: remove all code that tries to help in a 'smart way', like
   detecting faulty HTTP requests on SSL connections, etc.
 */
#define NO_HELPFUL_HINTS

#endif /* INCLUDE_COMPLETE_SSL */

/* --------------------------------------- */

/*
   #undef for default behaviour: Nagle algorithm enabled 

   This is really a quick hack for something that you should
   do like this at application level:

	{
		// Turn NAGLE off to facilitate Telnet-oriented behaviour
		int val = 0;
		int fd = BIO_get_fd(ssl_bio);
#if defined(SO_NAGLE)
		i = setsockopt(fd, SOL_SOCKET, SO_NAGLE,
		 	       (PFCCHAR)&val, sizeof(val));
#else
		i = setsockopt(fd, SOL_SOCKET, TCP_NODELAY,
		 	       (void *)&val, sizeof(val));
#endif
		if (i < 0) 
		{ 
			perror("Nagle OFF"); 
			...
		}
	}

*/
#undef NO_SSL_TCP_NAGLE	

/*
   #undef to let OpenSSL ERR_* routines diplay full paths for source files.
   #define to let OpenSSL ERR_* routines show the path within the OpenSSL directory only.

   I.e. compare

     p:\projects\rtip.base\dev\rtip.new\source\openssl\crypto\err\err.h

   versus

     crypto\err\err.h
 */
#define REDUCED_FILEPATH_IN_ERRMSG   1

/* --------------------------------------- */

/* DEBUG DIAGNOSTICS */

/* show which ciphers are available & compared by client/server */
#undef CIPHER_DEBUG

/* --------------------------------------- */

/* sanity checks */
#if defined(NO_RIPEMD)
#define NO_RIPEMD160   1  /* the same thing as NO_RIPEMD. Used in unpatched speed.c */
#endif

/* --------------------------------------- */

#if defined(EBSNET_FAKE_PORT)

#if !defined(WIN32)
#include <arpa/inet.h>
#endif
#include <assert.h>
#if defined(WINDOWS) || defined(MSDOS) || defined(WIN32)
#include <conio.h>
#endif
#include <ctype.h>
#if defined(VMS) || defined(__VMS)
#include <descrip.h>
#include <lnmdef.h>
#include <starlet.h>
#endif
#if !defined(WIN32)
#include <dirent.h>
#endif
#include <errno.h>
//#include <Events.h>
#include <fcntl.h>
//#include <graph.h>
//#include <GUSIConfig.h>
//#include <iodef.h>
//#include <io.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#if !defined(WIN32)
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#endif
#include <setjmp.h>
#if !defined(WIN32)
#include <sgtty.h>
#endif
#include <signal.h>
//#include <stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(WIN32)
#include <strings.h>
#endif
//#include <synch.h>
#if !defined(WIN32)
#include <sys/cdefs.h>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/un.h>
#include <termio.h>
#include <termios.h>
#endif
#ifdef SOLARIS
#include <thread.h>
#endif
#include <time.h>
//#include <timer.h>
//#include <ttdef.h>
//#include <typedefs.h>
//#include <types.h>
//#include <ulocks.h>
#if !defined(WIN32)
#include <unistd.h>
#endif
//#include <unixio.h>
#if defined(__ultrix)
#include <sys/syslog.h>
#else /* Unix */
#if !defined(WIN32)
#include <syslog.h>
#endif
#endif
#if defined(WIN32)
#include <io.h>
#include <winsock2.h>
#include <wincrypt.h> /* HCRYPTPROV et al */
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/types.h>
#endif

/*
 * ACT like a bare bones RTIP: *no* floating point printf-like routines!
 */
#define INCLUDE_TC_PRINTF_FLOAT_SUPPORT   0

#ifdef  __cplusplus
extern "C" {
#endif

/* RTIP 'fake' proto's for all those functions that mimick ours */

int tm_vprintf(const char *msg, va_list args);
int tc_vsprintf(char *dst, const char *msg, va_list args);
int tm_printf(const char *msg, ...);
int tm_puts(const char *msg);
int tm_putc(int chr);
int tm_kbhit(void);
int tm_getch(void);
int tml_gets(char *buf, int size);
void tm_cputs(const char *str);

#if defined(WIN32)
#define tc_stricmp   stricmp
#else
#define tc_stricmp   strcasecmp
#endif

int ks_ticks_p_sec(void);

#ifdef  __cplusplus
}
#endif

#else /* EBSNET_FAKE_PORT */

#include "rtip.h"
#include "debug.h"      /* DIAG_ASSERT() */
#include "bget.h"
#include "socket.h"
#if (INCLUDE_VFS)
#include "vfile.h"
#elif (INCLUDE_MFS)
#include "memfile.h"
#endif

#include <limits.h>  /* INT_MAX, etc. */
#include <time.h>	 /* struct tm, time_t, ... */

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef BUFSIZ       /* should come from stdio.h */
/* Default buffer size use by "setbuf" function */
#define BUFSIZ  512         /* Buffer size for stdio */
#endif

#define NO_BN_LLONG /* we don't have 'long long' */

/* lots of non-UNIX-es *don't* support the 'signed size_t' typedef :-( */
typedef signed long int ssize_t;

/* Used by other time functions.  */
struct rtip_tm
{
  int tm_sec;           /* Seconds. [0-60] (1 leap second) */
  int tm_min;           /* Minutes. [0-59] */
  int tm_hour;          /* Hours.   [0-23] */
  int tm_mday;          /* Day.     [1-31] */
  int tm_mon;           /* Month.   [0-11] */
  int tm_year;          /* Year - 1900.  */
  int tm_wday;          /* Day of week. [0-6] */
  int tm_yday;          /* Days in year.[0-365] */
  int tm_isdst;         /* DST.     [-1/0/1]*/
};

#ifdef  __cplusplus
}
#endif

#endif




#ifdef  __cplusplus
extern "C" {
#endif

typedef unsigned long int   rtip_clock_t;
typedef unsigned long int   rtip_time_t;

/* EBSNET_FAKE_PORT macro hacks to bend all those fine UNIX calls to our own */

typedef struct
{
#define RTIP_FAKE_FILE_MAGIC   0x495F4121
  unsigned long int magic_value;
  long int handle;      /* VF_OPEN handle is stored here. */
  int flags;
  int special;		    /* !0: stdin/stderr/stdout */
  char *fname;          /* xn_malloc() allocated 'converted' filepath */

  /*
   * buffer used by rtip_fgets() as VFS doesn't support file buffering,
   * resulting in slooooow line reads...
   */
  char *buffer;
  int bufsiz;           /* allocated size of buffer */
  int bufpos;           /* current position within buffer */
  int buffil;           /* current 'fill' of buffer */

#if defined(EBSNET_FAKE_PORT)
  FILE *f;
  char *mode_str;
#endif
} RTIP_FAKE_FILE;

extern RTIP_FAKE_FILE rtip_stdin;
extern RTIP_FAKE_FILE rtip_stdout;
extern RTIP_FAKE_FILE rtip_stderr;

#if !defined(RTIP_IMPLEMENTATION) && defined(RTIP_SSL_SOURCECODE)

#if !defined(EBSNET_FAKE_PORT)

#undef assert
#define assert(expr)            DIAG_ASSERT(expr, DIAG_SECTION_OPENSSL, "OpenSSL assert()", NOVAR, 0, 0)

#endif

#undef clock_t
#define clock_t    rtip_clock_t
#undef time_t
#define time_t     rtip_time_t

#undef localtime
#define localtime(t)        rtip_localtime(t)
#undef gmtime
#define gmtime(t)           rtip_gmtime(t)
#undef time
#define time(t)             rtip_time(t)
#undef clock
#define clock()             rtip_clock()

#undef FILE
#define FILE		RTIP_FAKE_FILE
#undef stdout
#define stdout		(&rtip_stdout)
#undef stderr
#define stderr		(&rtip_stderr)
#undef stdin
#define stdin		(&rtip_stdin)

#define printf  	     	    rtip_printf
#define fprintf			        rtip_fprintf
#undef sprintf
#define sprintf			        rtip_sprintf
#undef putc	/* temporarily 'kill' the define in stdio.h */
#define putc(c, f)	        	rtip_putc(c, f)
#define gets(buf)	        	rtip_gets(buf)
#define fgets(buf, len, f)	    rtip_fgets(buf, len, f)
#undef fflush
#define fflush(f)	        	rtip_fflush(f)
#define fputs(buf, f)	    	rtip_fputs(buf, f)
#define fputc(c, f)	        	rtip_fputc(c, f)
#undef feof
#define feof(f)		        	rtip_feof(f)
#define ftell(f)	        	rtip_ftell(f)
#define fseek(f, pos, mode)	    rtip_fseek(f, pos, mode)
#define fopen(f, mode)	    	rtip_fopen(f, mode)
#define fclose(f)	        	rtip_fclose(f)
#define fread(buf, len, n, f)	rtip_fread(buf, len, n, f)
#define fwrite(buf, len, n, f)	rtip_fwrite(buf, len, n, f)
#undef ferror
#define ferror(f)		        rtip_ferror(f)
#define setvbuf(f, buf, mode, size)			\
				                rtip_setvbuf(f, buf, mode, size)
#define fdopen(handle, mode)	rtip_fdopen(handle, mode)
#define perror(str)             rtip_perror(str)
#define rename(from, to)        rtip_rename(from, to)

/* BIO_set_fp(), BIO_new_fp(), ... pass any 'FILE *' through to
 * BIO->ptr and call any fxxxx() RTL functions casting that
 * particular BIO->ptr back to a 'FILE *', so we can just
 * emulate the RTL routines using our additional wrapper
 * functions and VFS. So much wrapping going on... sigh...
 */

#define abort()		            rtip_abort(__LINE__)
#undef fileno
#define fileno(f)	            rtip_fileno(f)
#undef getenv
#define getenv(s)               rtip_getenv(s)

#if (0)
/*
 * only the PKCS7 and MAURICE* example sources use malloc+free directly and
 * those have been patched now.
 * Also patched: SPKIGEN.C
 */
#undef malloc
#define malloc(s)               rtip_malloc(s)
#undef realloc
#define realloc(p, l)           rtip_realloc(p, l)
#undef free
#define free(p)                 rtip_free(p)
#undef strdup
#define strdup(s)               rtip_strdup(s)
#endif

#endif /* RTIP_IMPLEMENTATION */


int                 rtip_printf(const char *msg, ...);
int                 rtip_fprintf(RTIP_FAKE_FILE *f, const char *msg, ...);
int                 rtip_sprintf(char *dst, const char *msg, ...);
int                 rtip_putc(int chr, RTIP_FAKE_FILE *f);
char *              rtip_gets(char *buf);
char *              rtip_fgets(char *buf, int buflen, RTIP_FAKE_FILE *f);
int                 rtip_fflush(RTIP_FAKE_FILE *f);
int                 rtip_fputs(const char *buf, RTIP_FAKE_FILE *f);
int                 rtip_fputc(int c, RTIP_FAKE_FILE *f);
int                 rtip_feof(RTIP_FAKE_FILE *f);
unsigned long int   rtip_ftell(RTIP_FAKE_FILE *f);
int                 rtip_fseek(RTIP_FAKE_FILE *f, long int pos, int mode);
RTIP_FAKE_FILE *    rtip_fopen(const char *fname, const char *mode);
int                 rtip_fclose(RTIP_FAKE_FILE *f);
long int            rtip_fread(void *buf, size_t len, size_t n, RTIP_FAKE_FILE *f);
long int            rtip_fwrite(void *buf, size_t len, size_t n, RTIP_FAKE_FILE *f);
int                 rtip_ferror(RTIP_FAKE_FILE *f);
int                 rtip_setvbuf(RTIP_FAKE_FILE *f, char *buf, int mode, size_t size);
RTIP_FAKE_FILE *    rtip_fdopen(int handle, const char *mode);
void                rtip_perror(const char *str);
char *              rtip_transmogrify_fname(const char *fname);
int 				rtip_rename(const char *from, const char *to);

void                rtip_abort(int line);
int                 rtip_fileno(RTIP_FAKE_FILE *f);
const char *        rtip_getenv(const char *envname);
int                 rtip_setenv(const char *envname, const char *envcontent);


struct tm *         rtip_localtime(const rtip_time_t *p);
struct tm *         rtip_gmtime(const rtip_time_t *p);
rtip_time_t         rtip_time(rtip_time_t *p);
rtip_clock_t        rtip_clock(void);

void *              rtip_malloc(size_t len);
void                rtip_free(void *p);
void *              rtip_realloc(void *p, size_t len);
char *              rtip_strdup(const char *str);


#if defined(MONOLITH)

/*
 * test programs that are converted to test _routines_ when compiled
 * for embedded environment, so we can put them all in a global test
 * routine, execute that one and see if we did succeed.
 *
 * Oh, many _Test() routines don't use their argc/argv,
 * but I advise strongly to fake it somehow anyway.
 *
 * E.g.:
 *
 * int argc_fake = 1;
 * char *argv_fake[2] = { "embedded", NULL };
 * // argv[argc] == NULL on almost any system. Make sure we're
 * // compliant _all_ the way...
 *
 * ret = RC5_Test(argc_fake, argv_fake);
 * ...
 */
int BigNumArith_Test(int argc, char *argv[]);
int Blowfish_Test(int argc, char *argv[]);
int Cast_Test(int argc, char *argv[]);
int DES_Test(int argc, char *argv[]);
int DSA_Test(int argc, char *argv[]);
int DiffieHellman_Test(int argc, char *argv[]);
int Exp_Test(int argc, char *argv[]);
int HMAC_Test(int argc, char *argv[]);
int IDEA_Test(int argc, char *argv[]);
int MD2_Test(int argc, char *argv[]);
int MD4_Test(int argc, char *argv[]);
int MD5_Test(int argc, char *argv[]);
int MDC2_Test(int argc, char *argv[]);
int RANDOM_Test(int argc, char *argv[]);
int RC2_Test(int argc, char *argv[]);
int RC4_Test(int argc, char *argv[]);
int RC5_Test(int argc, char *argv[]);
int RIPEMD160_Test(int argc, char *argv[]);
int RSA_Test(int argc, char *argv[]);
int SHA1_Test(int argc, char *argv[]);
int SHA_Test(int argc, char *argv[]);
int SSLtest_Test(int argc, char *argv[]);
int Sock4A_Connect_Test(int argc, char *argv[]);
int OpenSSL_Test(int argc, char *argv[]);
int s_server_main(int argc,char *argv[]);
int s_client_main(int argc,char *argv[]);
int SelfSignDemo_Test(int argc, char *argv[]);
int BasicSSLServer_Test(int argc, char *argv[]);
int BasicSSLClient_Test(int argc, char *argv[]);
int PKCS7Sign_Test(int argc, char *argv[]);
int X509v3PrintCertContents_Test(int argc, char *argv[]);
int X509v3AddExtFromFile_Test(int argc, char *argv[]);
int SSLThreads_Test(int argc, char *argv[]);

#endif /* MONOLITH */

#if (INCLUDE_TEST_CRYPT)
void test_crypto_stuff(void);
#endif

#ifdef  __cplusplus
}
#endif


#endif /* AN_EBSNET_PORT */

#endif /*  __EBSNET_SSL_PORT_H__ */
