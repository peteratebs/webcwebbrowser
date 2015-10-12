/*
|  wecLog.cpp -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   Debug logging implementation
*/

#include "webcLog.h"

#ifdef WEBC_DEBUG
bool CLoggingUtil::m_bUseConsole = false;
FILE *CLoggingUtil::m_pGlobalLogFile = 0;
FILE *CLoggingUtil::m_pParseLogFile = 0;
FILE *CLoggingUtil::m_pStreamCaptureFile = 0;
FILE *CLoggingUtil::m_pJscriptFile = 0;
FILE *CLoggingUtil::m_pHTMLDomFile = 0;

#endif /* WEBC_DEBUG */
