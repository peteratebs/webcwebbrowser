/* PORTGNU.C - 
   Provides initialization and OS calls to implement GCC library
   functions. */

/* #include "stringx.h" */

/* This provides a function to do any board/port specific initialization before
   calling the main application */
void C_Entry(void)
{
#if defined(PPC860)
/*    init_mpc860(); */
#endif
}
/*
**********************************************************************
*
*             Memory functions not included in GNU libs
*
**********************************************************************
*/

//void bzero(char *s, unsigned long count)
void bzero(char *s, int count)
{
  while (count--) *s++ = 0;
}

#define ALIGN_MASK      (sizeof(unsigned long) - 1)

//void bcopy(char *from, char *to, unsigned long  count)
void bcopy(const char *from, char *to, int  count)
{
  char *        dst = to;
  char *        src = (char *)from;
  unsigned long            t;

  if (count && (dst != src))
  {
    if ((unsigned long )dst < (unsigned long )src)
    {
        /* Copy forward.
         */
      if (((unsigned long )src | (unsigned long )dst) & ALIGN_MASK)
      {
        if (((unsigned long )src ^ (unsigned long )dst) & ALIGN_MASK || count < sizeof(unsigned long ))
        {
          t = count;
        }
        else
        {
          t = sizeof(unsigned long ) - ((unsigned long )src & ALIGN_MASK);
        }
        count -= t;
        while (t--) *dst++ = *src++;
      }
      t = count / sizeof(unsigned long );
      if (t)
      {
        while (t--)
        {
          * (unsigned long  *)dst = * (unsigned long  *)src;
          dst += sizeof(unsigned long );
          src += sizeof(unsigned long );
        }
      }
      t = count & ALIGN_MASK;
      if (t)
      {
        while (t--) *dst++ = *src++;
      }
    }
    else
    {
      /* Copy backwards.
       */
      src += count;
      dst += count;
      if (((unsigned long )src | (unsigned long )dst) & ALIGN_MASK)
      {
        if (((unsigned long )src ^ (unsigned long )dst) & ALIGN_MASK || count < sizeof(unsigned long ))
        {
          t = count;
        }
        else
        {
          t = (unsigned long )src & ALIGN_MASK;
        }
        count -= t;
        while (t--) *--dst = *--src;
      }
      t = count / sizeof(unsigned long );
      if (t)
      {
        while (t--)
        {
          dst -= sizeof(unsigned long );
          src -= sizeof(unsigned long );
          * (unsigned long  *)dst = * (unsigned long  *)src;
        }
      }
      t = count & ALIGN_MASK;
      if (t)
      {
        while (t--) *--dst = *--src;
      }
    }
  }
}

#if 1
//void *memset(void *ps, int c, unsigned long n)
void *memset(void *ps, int c, int n)
{
  register unsigned char *s = (unsigned char *)ps;
  unsigned char *saved = s;

  while (n--) *s++ = c;

  return saved;
}

//int memcmp(const void *pcs, const void *pct, unsigned long n)
int memcmp(const void *pcs, const void *pct, int n)
{
  register unsigned char * cs = (unsigned char *)pcs;
  register unsigned char * ct = (unsigned char *)pct;

  while (n--) {
      if (*cs > *ct) return 1;
      if (*cs < *ct) return -1;
      cs++;
      ct++;
  }
  return 0;
}

//void *memcpy(void *ps, const void *pct, unsigned long n)
void *memcpy(void *ps, const void *pct, int n)
{
  bcopy((char *)pct, ps, n);
  return ps;
}
#endif

void zeromem(char *start,char *end)
{
  while (start < end) 
    {
      *start++ = 0;
    }
}

void fillmem(char *start,char *end, char ch)
{
  while (start < end) 
    {
      *start++ = ch;
    }
}

void initdata(char *start,char *end,char *to)
{
  while (start < end) 
    {
      *to++ = *start++;
    }
}

void	exit(void)
{
   while (1) {};
}

/*
**********************************************************************
*
*                    startc
*
**********************************************************************

This is called by assembler startup code (crt0.s) during initialization.
You must init C bss sections here. Depending on your system, you
may have to setup the bus interface first.

*/

//extern void __sbss_start();
//extern void __sbss_end();
//extern void __bss_start();
//extern void __end();
//extern void _bdata();
//extern void _edata();
//extern void _bsavedata();
//extern void _esavedata();

extern unsigned long c__bss_start;
extern unsigned long c__end;
extern unsigned long c__stack;
extern unsigned long c__sbss_start;
extern unsigned long c__sbss_end;
extern unsigned long c_bdata;
extern unsigned long c_edata;
extern unsigned long c_bsavedata;
extern unsigned long c_esavedata;

extern void _halt();

char *savedata = 0;	 

void startc(void) {
unsigned long * start = 0;

  zeromem((char*)c__bss_start,(char*)c__end);
  zeromem((char*)c__sbss_start,(char*)c__sbss_end);
  /* Determine if we need to copy initialized data section in or out */
  if(savedata) /* If non-zero, we need to restore to the original init data */
  {
    initdata((char*)c_bsavedata,(char*)c_esavedata,(char*)c_bdata);
  }
  else /* Copy the init data off before we start */
  { 
    savedata = (char*)c_bsavedata; 
    initdata((char*)c_bdata,(char*)c_edata,(char*)c_bsavedata);
  }

  while (start < (unsigned long *)0x2000) 
  {
    *start++ = 0x48000000;
  }

  /* Do any platform or board initialization */ 
//  C_Entry();
  main();
  while (1) {}
  exit();
}

/*
      * -- all the OS code to make GCC and the libraries run on
      *           a  target board.
*/
//#include <errno.h> 
//#undef errno 
//extern int errno;

//#include <stdio.h> 
     
//#ifndef NULL
//#define NULL 0
//#endif
     
extern char *_end;                /* _end is set in the linker command file */
     
/* just in case, most boards have at least some memory */
#  define RAMSIZE             0x04000000
#ifndef RAMSIZE
#if defined(CDK238)
#  define RAMSIZE             0xc1000000
#elif defined(FADS860T)
#  define RAMSIZE             0x00400000
#else
#error - define ram size for your target
#endif
#endif
 
/* A pointer to a list of environment variables and their values.
   For a minimal environment, this empty list is adequate: */
  
//char *__env[1] = { 0 }; 
//char **environ = __env;
//FILE _iob;
 


#if 0
/* These are predefined with stdlib */ 
#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */

/* An arbitrary numbering system to keep track of devices */
#define TTY0 3
#define TTY1 4

/* Define file types */
#define IS_NOT_A_FILE 0
#define IS_A_TTY      1

/* Define file status */
#define IS_CLOSED 0
#define IS_OPEN   1

struct my_fd 
{
  int fd_type;
  union
    {
      struct
        {
		  int status;
		  int port;
        } tty;
      struct
        {
		  int status;
        } unused;
    } dev;
};

struct my_fd devstatus[FOPEN_MAX] = { {IS_NOT_A_FILE,IS_CLOSED}, /* reserved for stdin */
                                      {IS_NOT_A_FILE,IS_CLOSED}, /* reserved for stdout */
                                      {IS_NOT_A_FILE,IS_CLOSED}, /* reserved for stderr */
                                      {IS_A_TTY,IS_CLOSED},
                                      {IS_A_TTY,IS_CLOSED},
                                      {IS_NOT_A_FILE,IS_CLOSED}
                                    }; 


/*
  * open -- open a file descriptor. We don't have a filesystem, so
  *         we return an error if not a tty device.
*/
int
open(buf, flags, mode)
          char *buf;
          int flags;
          int mode;
  {
	if(strcmp(buf, "tty0") == 0)
	{
#if defined(CDK238)
// Default com port set to 96, 8 char, no prity, 1 stop, polled, no handhake
	cs89712_uart_device_open(1,96,8,0,1,1,0);
    devstatus[TTY0].dev.tty.status = IS_OPEN; 
    devstatus[TTY0].dev.tty.port = 1; 
	return (TTY0);
#endif
#if defined(PPC860)
    RTKsmc_term_open(B9600, 1);
    devstatus[TTY0].dev.tty.status = IS_OPEN; 
    devstatus[TTY0].dev.tty.port = 1; 
	return (TTY0);
#endif
	}
	else if(strcmp(buf, "tty1") == 0)
	{
#if defined(CDK238)
// Default com port set to 96, 8 char, no prity, 1 stop, polled, no handhake
	cs89712_uart_device_open(2,96,8,0,1,1,0);
    devstatus[TTY1].dev.tty.status = IS_OPEN; 
    devstatus[TTY0].dev.tty.port = 2; 
	return (TTY1);
#endif
#if defined(PPC860)
    RTKsmc_term_open(B9600, 2);
    devstatus[TTY0].dev.tty.status = IS_OPEN; 
    devstatus[TTY0].dev.tty.port = 2; 
	return (TTY0);
#endif
	}
	else
        return (-1);
  }
     
/*
  * close -- close a file descriptor. 
*/
int
close(fd)
          int fd;
  {
    if(devstatus[fd].fd_type == IS_A_TTY)
    {
      if(devstatus[fd].dev.tty.status == IS_OPEN)
      {
        devstatus[fd].dev.tty.status = IS_CLOSED; 
#if defined(CDK238)
        cs89712_uart_device_disable(devstatus[fd].dev.tty.port);
#endif
      }
	}
    return (0);
  }
     

/*
 * read  -- read bytes from the a device
*/    
int
read(fd, buf, nbytes)
          int fd;
          char *buf;
          int nbytes;
  {
    if(devstatus[fd].fd_type == IS_A_TTY)
    {
      if(devstatus[fd].dev.tty.status == IS_OPEN)
      {
#if defined(CDK238)
        char c = cs89712_uart_device_polled_getc(devstatus[fd].dev.tty.port);
	    while( c == 0xff) c = cs89712_uart_device_polled_getc(devstatus[fd].dev.tty.port);
		 *buf = c;
        return (1);
#endif
#if defined(PPC860)
        while(!RTKsmc_term_kbhit(devstatus[fd].dev.tty.port)) {};
		*buf = RTKsmc_term_getc(devstatus[fd].dev.tty.port);
        return (1);
#endif
	  }
	}
    return (0);
  }
     
/*
   * write -- write bytes to the device. 
   *          Since we have no filesystem,
   *          open will only return an error.
*/
int
write(fd, buf, nbytes)
          int fd;
          char *buf;
          int nbytes;
  {
    if(devstatus[fd].fd_type == IS_A_TTY)
    {
      if(devstatus[fd].dev.tty.status == IS_OPEN)
      {
#if defined(CDK238)
        cs89712_uart_device_polled_send(devstatus[fd].dev.tty.port, nbytes, buf);
        return (nbytes);
#endif
#if defined(PPC860)
        /* Wait until port is ready */
        while(RTKsmc_term_sendl( buf, nbytes, devstatus[fd].dev.tty.port) < 0){};
		/* Wait until port is finished */
        while(RTKsmc_term_send(0, devstatus[fd].dev.tty.port) < 0){};
        return (nbytes);
#endif
	  }
	}
    return (0);
  }
     
     /*
      * sbrk -- changes heap size size. Get nbytes more
      *         RAM. We just increment a pointer in what's
      *         left of memory on the board.
      */
char *
   sbrk(nbytes)
          int nbytes;
     {
       static char * heap_ptr = NULL;
       char * base;
     
       if (heap_ptr == NULL) {
         heap_ptr = (char *)&_end;
       }
     
       if ((RAMSIZE - (unsigned long)heap_ptr) >= 0) {
         base = heap_ptr;
         heap_ptr += nbytes;
         return (base);
       } else {
         return ((char *)-1);
       }
     }

     /*
      * isatty -- returns 1 if connected to a terminal device,
      *           returns 0 if not. Since we're hooked up to a
      *           serial port, we'll say yes and return a 1.
      */
     int
     isatty(int fd)
     {
       return (1);
     }
     
     /*
      * lseek -- move read/write pointer. Since a serial port
      *          is non-seekable, we return an error.
      */
     int
     lseek(fd,  offset, whence)
          int fd;
          int offset;
          int whence;
     {
       return ((int)-1);
     }
     
/* Status of an open file.
   For consistency with other minimal implementations ,
    all files are regarded as character special devices.
    The sys/stat.h header file required is distributed in the include
    subdirectory for this C library. */
  
#ifdef	_POSIX_SOURCE
#define	S_ISVTX		0001000	/* save swapped text even after use */
#define	S_IREAD		0000400	/* read permission, owner */
#define	S_IWRITE 	0000200	/* write permission, owner */
#define	S_IEXEC		0000100	/* execute/search permission, owner */

#define	S_ENFMT 	0002000	/* enforcement-mode locking */

#define	S_IFMT		_IFMT
#define	S_IFDIR		_IFDIR
#define	S_IFCHR		_IFCHR
#define	S_IFBLK		_IFBLK
#define	S_IFREG		_IFREG
#define	S_IFLNK		_IFLNK
#define	S_IFSOCK	_IFSOCK
#define	S_IFIFO		_IFIFO
#endif	/* _POSIX_SOURCE */
#include "sys/stat.h" 
  
int fstat(int file, struct stat *st) { 
   st->st_mode = S_IFCHR; 
   return 0; 
}

/* Status of a file (by name). Minimal implementation: */
int _stat(const char *file, struct stat *st) { 
   st->st_mode = S_IFCHR; 
   return 0; 
}

/* Remove a file’s directory entry. Minimal implementation: */
int _unlink(char *name){ 
   errno=ENOENT; 
   return -1; 
}

     /*
      * getpid -- only one process, so just return 1.
      */
     #define __MYPID 1
     int
     _getpid()
     {
       return __MYPID;
     }
     
     /*
      * kill -- go out via exit...
      */
     int
     _kill(pid, sig)
          int pid;
          int sig;
     {
       if(pid == __MYPID)
         _exit();
       return 0;
     }

/* Timing information for current process. Minimal implementation: */
  
#include <sys/times.h> 
clock_t _times(struct tms *buf){ 
   return -1; 
}

/* Wait for a child process. Minimal implementation: */
  
int _wait(int *status) { 
   errno=ECHILD; 
   return -1; 
}

     
     /*
      * print -- do a raw print of a string
      */
     int
     _print(ptr)
     char *ptr;
     {
       return (RTK_term_sends(ptr));
     }
     
     /*
      * putnum -- print a 32 bit number in hex
      */
     int
     putnum (num)
     unsigned int num;
     {
       char  buffer[9];
       int   count;
       char  *bufptr = buffer;
       int   digit;
     
       for (count = 7 ; count >= 0 ; count--) {
         digit = (num >> (count * 4)) & 0xf;
     
         if (digit <= 9)
           *bufptr++ = (char) ('0' + digit);
         else
           *bufptr++ = (char) ('a' - 10 + digit);
       }
     
       *bufptr = (char) 0;
       _print (buffer);
       return;
     }

/* Transfer control to a new process.
   Minimal implementation (for a system without processes): */
  
int _execve(char *name, char **argv, char **env){ 
   errno=ENOMEM; 
   return -1; 
}

/* Create a new process.
   Minimal implementation (for a system without processes): */
  
int _fork() { 
   errno=EAGAIN; 
   return -1; 
}

int	_filbuf( FILE *p)
{p=p; }
int	_flsbuf( unsigned char x , FILE *p)
{}

/* KBHIT - This is not a stdio function but is needed by RTIP for console I/O.
   This function returns 1 if a kbhit occurred on the device, 0 otherwise.
*/    
int
kbhit(fp)
          FILE *fp;
  {
    int fd = fp->_file;
    if(devstatus[fd].fd_type == IS_A_TTY)
    {
      if(devstatus[fd].dev.tty.status == IS_OPEN)
      {
#if defined(CDK238)
        return(cs89712_uart_device_polled_kbhit(devstatus[fd].dev.tty.port));
#endif
#if defined(PPC860)
        return(RTKsmc_term_kbhit(devstatus[fd].dev.tty.port));
#endif
	  }
	}
    return (0);
  }
#endif

/* Include these functions (atexit, _exit) for a simple (no malloc) implementation.
   Otherwise, include/define the OS porting functions below */
/* Use a static array to save exit functions since mem alloc may not be up yet */
#if 0
#define N_EXIT_FUNCS 8
void (* exit_func[N_EXIT_FUNCS])(void)= {0,0,0,0,0,0,0,0};

int		atexit(void (* func)(void))
{
   int i;
   /* look for an empty slot */
   for (i=0; i< N_EXIT_FUNCS; i++)
     if (exit_func[i] == 0)
      { exit_func[i] = func; return (0);}
   return (1);   
}
void	exit(int status)
{
   int i;
   for (i=N_EXIT_FUNCS-1; i>0; i--)
     if (exit_func[i]) exit_func[i]();
   while (1) {};
}
#endif

#if 0
/* Define stubs to satisfy gcc libraries built for no floating point */
__eqdf2() { };
__nedf2() { };
__negdf2() { };
__gtdf2() { };
__ltdf2() { };
__ledf2() { };
__muldf3() { };
__divdf3() { };
__adddf3() { };
__subdf3() { };
__floatsidf() { };
__fixdfsi() { };
__udivdi3() { };
__udivsi3() { };
#endif
