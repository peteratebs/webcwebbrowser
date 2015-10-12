//
// XMLUTIL.H - XML parser utilities
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __XMLUTIL_H__	
#define __XMLUTIL_H__

#include "xml.h"

#ifdef __cplusplus__
extern "C" {
#endif

int XML_IsBaseChar      (XMLchar ch);
int XML_IsCombiningChar (XMLchar ch);
int XML_IsDigit         (XMLchar ch);

#ifdef __cplusplus__
}
#endif

#endif // __XMLUTIL_H__
