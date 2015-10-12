/*
|  wecLog.h -
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
#pragma once

#ifdef WEBC_DEBUG

#include "webcfg.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_DEBUG_LOG_JSCRIPT)
// this isn't a great place for this
#include "wjsbrowser.h" /* For WebcJSBrowserContext::SetJscriptTrace */
extern "C" WebcJSBrowserContext* gJSBrowserContext;
#endif
#endif

// file names used for various log output
#define WEBC_CFG_GLOBAL_LOG_FILE            "webclog.txt"
#define WEBC_CFG_PARSE_LOG_FILE             "parselog.txt"
#define WEBC_CFG_CAFTURE_STREAM_LOG_FILE    "streamcapture.txt"
#define WEBC_CFG_JSCRIPT_LOG_FILE           "jscriptlog.txt"
#define WEBC_CFG_JS_LOG_FILE                "jslog.txt"
#define WEBC_CFG_HTML_DOM_LOG_FILE			"htmlDomLog.txt"


class CLoggingUtil
{
public:
	enum { LOG_SYBSYSTEM_GLOBAL, LOG_SUBSYSTEM_HTMLPARSE, LOG_SUBSYSTEM_STREAM, LOG_SUBSYSTEM_JAVASCRIPT, LOG_SUBSYSTEM_HTML_DOM};

	static void InitLogging() 
	{
        printf("HTML progress logging is disabled, returning \n");
        return;
		// open up all of the files
        m_pGlobalLogFile = fopen(WEBC_CFG_GLOBAL_LOG_FILE, "w");
        m_pParseLogFile = fopen(WEBC_CFG_PARSE_LOG_FILE, "w");
        m_pStreamCaptureFile = fopen(WEBC_CFG_CAFTURE_STREAM_LOG_FILE, "w");
        m_pJscriptFile = fopen(WEBC_CFG_JSCRIPT_LOG_FILE, "w");
		m_pHTMLDomFile = fopen(WEBC_CFG_HTML_DOM_LOG_FILE, "w");
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_DEBUG_LOG_JSCRIPT)
        if (gJSBrowserContext)
            gJSBrowserContext->SetJscriptTrace(m_pJscriptFile);
#endif
#endif
    }

	static void CloseLogging()
	{
	    if (m_pGlobalLogFile)
    {
        fclose(m_pGlobalLogFile);
        m_pGlobalLogFile = 0;
    }
	    if (m_pParseLogFile)
    {
        fclose(m_pParseLogFile);
        m_pParseLogFile = 0;
    }
    if (m_pStreamCaptureFile)
    {
        fclose(m_pStreamCaptureFile);
        m_pStreamCaptureFile = 0;
    }
    if (m_pJscriptFile)
    {
        fclose(m_pJscriptFile);
        m_pJscriptFile = 0;
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_DEBUG_LOG_JSCRIPT)
        if (gJSBrowserContext)
            gJSBrowserContext->SetJscriptTrace(m_pJscriptFile);
#endif
#endif
    }
	}

	// central logging routine
	static int DbgLogPrintf(int subSystem, const char * format, va_list argptr)
	{
		int retVal = 0;
		switch(subSystem)
		{
		case LOG_SYBSYSTEM_GLOBAL:
		default:
			if(m_bUseConsole)
			{
				outputTime(0);
				retVal = vprintf( format, argptr); // print to console
			}
			else
			{
				if(m_pGlobalLogFile)
				{
					outputTime(m_pGlobalLogFile);
					retVal = vfprintf(m_pGlobalLogFile, format, argptr); // print to file
					fflush(m_pGlobalLogFile);
				}
			}
			break;
		case LOG_SUBSYSTEM_HTMLPARSE:
			if(m_pParseLogFile)
			{
				retVal = vfprintf(m_pParseLogFile, format, argptr);
				fflush(m_pParseLogFile);
			}
			break;
		case LOG_SUBSYSTEM_STREAM:
			if(m_pStreamCaptureFile)
			{
				retVal = vfprintf(m_pStreamCaptureFile, format, argptr);
				fflush(m_pStreamCaptureFile);
			}
			break;
		case LOG_SUBSYSTEM_JAVASCRIPT:
			if(m_pJscriptFile)
			{
				retVal = vfprintf(m_pJscriptFile, format, argptr);
				fflush(m_pJscriptFile);
			}
			break;
		case LOG_SUBSYSTEM_HTML_DOM:
			if(m_pHTMLDomFile)
			{
				retVal = vfprintf(m_pHTMLDomFile, format, argptr);
				fflush(m_pHTMLDomFile);
			}
			break;
		}

		return retVal;
	}
	static int DbgLogGlobalPrintf(const char * format, ...)
	{
		va_list argptr;
		va_start(argptr, format);

		return DbgLogPrintf(LOG_SYBSYSTEM_GLOBAL, format, argptr);
	}
	static int DbgLogHtmlParse(const char * format, ...)
	{
		va_list argptr;
		va_start(argptr, format);

		return DbgLogPrintf(LOG_SUBSYSTEM_HTMLPARSE, format, argptr);
	}
	static int DbgLogStream(const char * format, ...)
	{
		va_list argptr;
		va_start(argptr, format);

		return DbgLogPrintf(LOG_SUBSYSTEM_STREAM, format, argptr);
	}
	static int DbgLogJavascript(const char * format, ...)
	{
		va_list argptr;
		va_start(argptr, format);

		return DbgLogPrintf(LOG_SUBSYSTEM_JAVASCRIPT, format, argptr);
	}
	static int DbgLogHTMLDom(const char * format, ...)
	{
		va_list argptr;
		va_start(argptr, format);

		return DbgLogPrintf(LOG_SUBSYSTEM_HTML_DOM, format, argptr);
	}

protected:

	static void outputTime(FILE *pFile)
	{
       struct tm *timeStruct;
       time_t timeVal;

        time( &timeVal );
        timeStruct = localtime( &timeVal );

		if(0 == timeStruct)
			return;

	   if(0 == pFile)
		   printf("%d:%d:%d : ", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec );
	   else
		   fprintf(pFile, "%d:%d:%d : ", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec );
	}

	static bool m_bUseConsole;
	static FILE *m_pGlobalLogFile;
	static FILE *m_pParseLogFile;
	static FILE *m_pStreamCaptureFile;
	static FILE *m_pJscriptFile;
	static FILE *m_pHTMLDomFile;
};

#endif /* WEBC_DEBUG */

#ifdef WEBC_DEBUG

#define LOG_PRINTF CLoggingUtil::DbgLogGlobalPrintf
#define LOG_PARSER CLoggingUtil::DbgLogHtmlParse
#define LOG_STREAM CLoggingUtil::DbgLogStream
#define LOG_JAVASCRIPT CLoggingUtil::DbgLogJavascript
#define LOG_HTMLDOM CLoggingUtil::DbgLogHTMLDom

#else

#define LOG_PRINTF
#define LOG_PARSER
#define LOG_STREAM
#define LOG_JAVASCRIPT
#define LOG_HTMLDOM

#endif /* WEBC_DEBUG (again)*/


