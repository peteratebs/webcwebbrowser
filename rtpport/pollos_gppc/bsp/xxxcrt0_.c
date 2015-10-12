#include "RTK32.h"
#include "cpu860.h"      /* 860 definitions and declarations */
void _exit(void);
void _dummy(void);

extern char __sbss_start;
extern char __sbss_end;
extern char __bss_start;
extern char end;
extern char __stack;

void zeroandrun();

  char *pinit = (char *)0xffffffff;
  char *pstart = (char *)0xffffffff;
  char *pend = (char *)0xffffffff;

void _start()
{
  zeroandrun();
}

void zeroandrun()
{
/*  char *p;   */
  pstart = (char *)&__sbss_start;
  pend = (char *)&__sbss_end;

  pinit = (char *)&__sbss_start;
  while (pinit < (char *)&__sbss_end) 
    {
      *pinit++ = 0;
    }

  pstart = (char *)&__bss_start;
  pend = (char *)&end;

  pinit = (char *)&__bss_start;
  while (pinit < (char *)&end) 
    {
      *pinit++ = 0;
    }

/*    init_ppc();*/
main();
exit(1);
}


void
_exit ()
{
  while(1) {};
}

/* Force linker to include modules needed bt stheap.o */
void 
_dummy()
{
static RTKSemaphore DummyLock = RTK_NO_SEMAPHORE;
      DummyLock = RTKCreateSemaphore(ST_RESOURCE, 1, "Dummy");
      RTKWait(DummyLock);
}

/*
extern DWORD _end;
static char *end_of_data = (char *) &_end;

char *
sbrk (int delta) {
	char *ptr = end_of_data;

	end_of_data += delta;
	return ptr;
}
*/

/* Provide a dummy __eabi in case main got compiled without -mcall-solaris.    */
void
__eabi ()
{
}

     /*
      * glue.c -- all the code to make GCC and the libraries run on
      *           a bare target board.
      */
     
     #ifndef NULL
     #define NULL 0
     #endif
     
#define TERMDELAY 10	/* delay count (ticks) if message transmitter is busy */
     
     
     extern DWORD _end;                /* _end is set in the linker command file */
     
     /* just in case, most boards have at least some memory */
     #ifndef RAMSIZE
     #  define RAMSIZE             (DWORD)0x100000
     #endif
     
     /*
      * read  -- read bytes from the serial port. Ignore fd, since
      *          we only have stdin.
      */
     int
     read(fd, buf, nbytes)
          int fd;
          char *buf;
          int nbytes;
     {
       int i = 0;
     
       for (i = 0; i < nbytes; i++) {
         *(buf + i) = RTKsmc_term_getc(SMC_TERM_PORT);
         if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
           (*(buf + i)) = 0;
           break;
         }
       }
       return (i);
     }
     
     /*
      * write -- write bytes to the serial port. Ignore fd, since
      *          stdout and stderr are the same. Since we have no filesystem,
      *          open will only return an error.
      */
     int
     write(fd, buf, nbytes)
          int fd;
          char *buf;
          int nbytes;
     {
       int i;
     
       for (i = 0; i < nbytes; i++) {
         if (*(buf + i) == '\n') {
  while( RTKsmc_term_send('\r',SMC_TERM_PORT)) RTKDelay(TERMDELAY);
         }
  while( RTKsmc_term_send(*(buf + i),SMC_TERM_PORT)) RTKDelay(TERMDELAY);
       }
       return (nbytes);
     }
     
     /*
      * open -- open a file descriptor. We don't have a filesystem, so
      *         we return an error.
      */
     int
     open(buf, flags, mode)
          char *buf;
          int flags;
          int mode;
     {
       return (-1);
     }
     
     /*
      * close -- close a file descriptor. We don't need
      *          to do anything, but pretend we did.
      */
     int
     close(fd)
          int fd;
     {
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
     
       if ((RAMSIZE - (DWORD)heap_ptr) >= 0) {
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
     isatty(fd)
          int fd;
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
     
     /*
      * fstat -- get status of a file. Since we have no file
      *          system, we just return an error.
      */
     int
     fstat(fd, buf)
          int fd;
          void *buf;
     {
       return (-1);
     }
     
     /*
      * getpid -- only one process, so just return 1.
      */
     #define __MYPID 1
     int
     getpid()
     {
       return __MYPID;
     }
     
     /*
      * kill -- go out via exit...
      */
     int
     kill(pid, sig)
          int pid;
          int sig;
     {
       if(pid == __MYPID)
         _exit();
       return 0;
     }
     
     /*
      * print -- do a raw print of a string
      */
     int
     print(ptr)
     char *ptr;
     {
       while (*ptr) {
  while( RTKsmc_term_send(*ptr++,SMC_TERM_PORT)) RTKDelay(TERMDELAY);
       }
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
       print (buffer);
       return;
     }

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
