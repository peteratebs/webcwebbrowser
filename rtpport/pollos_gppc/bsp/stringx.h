#ifndef __Istring
#define __Istring

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef __size_t
#define	__size_t
typedef unsigned int size_t;		/* sizeof type			*/
#endif

extern void
	*memcpy(void *, const void *, size_t),
	*memmove(void *, const void *, size_t),
	*memchr(const void *, int, size_t),
	*memset(void *, int, size_t);
extern char
	*strcpy(char *, const char *),
	*strncpy(char *, const char *, size_t),
	*strcat(char *, const char *),
	*strncat(char *, const char *, size_t),
	*strchr(const char *, int),
	*strpbrk(const char *, const char *),
	*strrchr(const char *, int),
	*strstr(const char *, const char *),
	*strtok(char *, const char *),
	*strerror(int);
extern int
	memcmp(const void *, const void *, size_t),
	strcmp(const char *, const char *),
	strcoll(const char *, const char *),
	strncmp(const char *, const char *, size_t);
extern size_t
	strxfrm(char *, const char *, size_t),
	strcspn(const char *, const char *),
	strspn(const char *, const char *),
	strlen(const char *);

#if !defined(__STRICT_ANSI__)

extern void
	*memccpy(void *, const void *, int, size_t);
extern char
	*strdup(const char *);

#endif /* __STRICT_ANSI__ */
#ifdef	__cplusplus
}
#endif
#endif
