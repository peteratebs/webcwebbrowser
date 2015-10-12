
//
// WEBJHCONFIG.H - Configuration file for overriding the defines for overriding the WEBC_SUPPORT_JSCRIPT command line
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004-2010
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __WEBJHCONFIG_H__
#define __WEBJHCONFIG_H__


/* Define as one or zero to superced the command line, comment out the next line to use the command line */
/* #define WEBC_SUPPORT_JSCRIPT_OVERRIDE 1 */
#define WEBC_SUPPORT_JSCRIPT_OVERRIDE 1
#ifdef WEBC_SUPPORT_JSCRIPT_OVERRIDE
#ifdef WEBC_SUPPORT_JSCRIPT
#undef WEBC_SUPPORT_JSCRIPT
#endif
#define WEBC_SUPPORT_JSCRIPT WEBC_SUPPORT_JSCRIPT_OVERRIDE
#endif
#endif // __WEBJHCONFIG_H__
