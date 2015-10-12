/*
|  BUFFER.H - 
| 
|  EBS - 
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __BUFFER_H__
#define __BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

int buffer_alloc_at_least (
		void** buffer,
		long* currentSize,
		long minSize,
		unsigned preserveContent,
		unsigned zeroNewSpace
	);

#ifdef __cplusplus
}
#endif

#endif /* __BUFFER_H__ */
