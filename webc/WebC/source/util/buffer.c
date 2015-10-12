/*
|  BUFFER.C -
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

#include "buffer.h"
#include "webc.h"
#include "webcassert.h"
#include "rtpstr.h"
#include "rtptotc.h"
#include "webcmem.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

#define INCREASE_BUFFER_SIZE(X,T)    ((X < T)? (X << 1) : (X + T))
#define BUFFER_SIZE_THRESHOLD        4096

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
int buffer_alloc_at_least (
		void** buffer,
		long* currentSize,
		long minSize,
		unsigned preserveContent,
		unsigned zeroNewSpace
	)
{
	if (*buffer && *currentSize >= minSize)
	{
		return (0);
	}
	else
	{
		void* largerBuffer;
		long  largerSize;
		
		if (*buffer)
		{
			largerSize = INCREASE_BUFFER_SIZE(minSize, BUFFER_SIZE_THRESHOLD);
		}
		else
		{
			largerSize = minSize;
		}
		
		WEBC_ASSERT(largerSize >= minSize);
		
		largerBuffer = WEBC_MALLOC_VERBOSE(largerSize,__FUNCTION__);
		if (largerBuffer)
		{
			if (*buffer)
			{
				if (preserveContent)
				{
					tc_memcpy(largerBuffer, *buffer, *currentSize);					
					if (zeroNewSpace)
					{
						tc_memset((char*)largerBuffer + *currentSize, 0, largerSize - *currentSize);
					}
				}
				else
				{
					if (zeroNewSpace)
					{
						tc_memset(largerBuffer, 0, largerSize);
					}
				}				
				
				WEBC_FREE(*buffer);
			}
			else
			{
				if (zeroNewSpace)
				{
					tc_memset(largerBuffer, 0, largerSize);
				}
			}
			
			*buffer      = largerBuffer;
			*currentSize = largerSize;
			
			return (0);
		}
	}	
	
	return (-1);
}
