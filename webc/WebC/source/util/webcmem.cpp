/*
|  WEBCMEM.CPP -
| 
|  EBS -
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "webcmem.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

#if (WEBC_MEMORY_RTPDEBUG)

char* gpWebcNewFile = 0;
long  giWebcNewLine = 0;
char* gpWebcDeleteFile = 0;
long  giWebcDeleteLine = 0;
void _mdbSetFile(const char *F)
{
	gpWebcNewFile=(char *) F;
}
void _mdbSetLine(long l)
{
	giWebcNewLine=l;
}
#endif // WEBC_MEMORY_DEBUG

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/


#if (0 && WEBC_MEMORY_RTPDEBUG)




#ifndef _EFI_

/*---------------------------------------------------------------------------*/
void * operator new(size_t s)
{
	return (WEBC_DEBUG_MALLOC(s, gpWebcNewFile, giWebcNewLine,"New Operator",dbMFlagNewEntry));
}

/*---------------------------------------------------------------------------*/
void * operator new[](size_t s)
{

	return ::operator new(s);
}

/*---------------------------------------------------------------------------*/
void operator delete(void * mem)
{
	WEBC_DEBUG_FREE(mem, gpWebcDeleteFile, giWebcDeleteLine);	
}

/*---------------------------------------------------------------------------*/
void operator delete[](void * mem)
{
	::operator delete(mem);
}

#endif // _EFI_

#endif // WEBC_MEMORY_DEBUG

