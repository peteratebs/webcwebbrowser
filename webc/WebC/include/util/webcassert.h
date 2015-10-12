/*
|  WEBCASSERT.H -
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#ifndef __WEBCASSERT_H__
#define __WEBCASSERT_H__

#include "webcfg.h"

#ifdef WEBC_DEBUG
 #ifdef _MSC_VER /* Microsoft C */
  #include "assert.h"
  #define WEBC_ASSERT    assert
 #else
#ifdef __TSDPROJECT__		/* Generic solution provided below */
#ifdef __cplusplus
extern "C" {
#endif
extern void locosto_assert_break(void);
#ifdef __cplusplus
}
#endif
		#define WEBC_ASSERT(X) if (!(X)) { locosto_assert_break(); }
	#else
	#include "assert.h"
  	#define WEBC_ASSERT    assert
  	#endif
 #endif
#else
 #define WEBC_ASSERT(X)
#endif

#endif /* __WEBCASSERT_H__ */


