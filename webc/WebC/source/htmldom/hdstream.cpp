//
// HDSTREAM.CPP - Write stream for parsing to HTML Documents
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains code to download, process, and display web pages,
//		as well as process browser messages, etc.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcjhconfig.h"
#include "htmldom.hpp"
#include "hdstream.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "webcLog.h"

/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local Types
/*****************************************************************************/

/*****************************************************************************/
// Methods for class HTMLDocumentStream
/*****************************************************************************/


#if(WEBC_DEBUG_PARSER)
#include "strbld.hpp" /* For StringBuilder */

#if (WEBC_SUPPORT_JSCRIPT)

#include "wjsbrowser.h" /* For WebcJSBrowserContext::SetJscriptTrace */

extern "C" WebcJSBrowserContext* gJSBrowserContext;
#endif

void StartParseLog()
{
	LOG_PARSER(" ------------------------ StartParseLog --------------------------------\n\n");
}

void StopParseLog()
{

}

int BreakPackedJavascript(char *str, int *indent_level)
{
int offset, ntowrite, nchars;

    ntowrite = nchars = 0;

    for (offset = 0; offset < 256; offset++, str++)
    {
        if (!*str)
            break;
        nchars++;
        if (*str == '\n')
        {
            ntowrite = offset + 1;
            break;
        }
        else if (*str == '{')
        {
            *indent_level += 1;
            if (offset == 0)
                ntowrite = 1;
            else
                ntowrite = offset;
            if (ntowrite)
                break;
        }
        else if (*str == '}')
        {
            *indent_level -= 1;
            if (offset == 0)
                ntowrite = 1;
            else
                ntowrite = offset;
            if (ntowrite)
                break;
        }
        else if (*str == ';')
        {
            ntowrite = offset + 1;
            break;
        }
    }
    if (!ntowrite)
        ntowrite = nchars;
    return (ntowrite);
}

void WriteParseLog(char *comment, const WEBC_CHAR* pString)
{
    if (comment)
    {
	    LOG_PARSER("%s", comment);
    }
    if (pString)
    {
        // Log content
        StringCopyBuilder ubld(pString);
        WEBC_CHAR *log_str = ubld.MallocString(__FILE__, __LINE__);
        if (log_str)
        {
		    char *str = webc_arg_to_char(log_str);
#if(WEBC_DEBUG_PARSER_BREAK_LINES)
            /* Break befor the last { or after the last newline ) or ; */
		    char *currstr = str;
            int ntowrite,indent_level;
            indent_level = 0;
            do
            {
                ntowrite = BreakPackedJavascript(currstr,&indent_level);
                if (ntowrite)
                    fwrite(currstr, 1, ntowrite, pFile);
                if (*(currstr + ntowrite-1) != '\n')
                    fwrite("\n", 1, 1, pFile);
                indent_level = 0;
                if (indent_level)
                {
                    for (int i = 0; i < indent_level; i++)
                        fwrite("  ", 1, 2, pFile);
                }
                currstr += ntowrite;
            } while (ntowrite);

#else
		    LOG_PARSER("%S", str);
#endif
            WEBC_FREE(str);
		    ubld.FreeString(log_str, __FILE__, __LINE__);
		}
	}

}
#endif

HTMLDocumentStream::HTMLDocumentStream(HTMLElement* rootElement)
	: mBuilder(rootElement)
{
	mInsideWrite = 0;
	mbFlags = 0;

	HTML_InitParse(&mParseContext);
#if(WEBC_DEBUG_PARSER)
    StartParseLog();
#endif
}

HTMLDocumentStream::~HTMLDocumentStream()
{
#if(WEBC_DEBUG_PARSER)
    StopParseLog();
    mParseInDataBuffer.discardAll();
	mParseOutDataBuffer.discardAll();
#endif
}

long HTMLDocumentStream::Write(const WEBC_CHAR* pString, long lStrLen)
{
long bytesParsed = 0;
long totalParsed = 0;
unsigned done = 0;
#if(WEBC_DEBUG_PARSER)
long LogWriteLevel = 0;
#endif
	if (mbFlags & HDOC_STREAM_DELETED)
	{
		return (-1);
	}

	if (mInsideWrite >= MAX_WRITE_NESTING)
	{
		// don't allow nested write calls too deep
		return (-1);
	}

	mInsideWrite++;
#if(WEBC_DEBUG_PARSER)
    if (!LogWriteLevel || mInsideWrite == LogWriteLevel)
    {
        LOG_STREAM("%S", pString);
        WriteParseLog("\n<!-- >>>           =====   Raw Input    ==============                -->\n", pString);
        /* Save all raw input */
        mParseInDataBuffer.append(pString, lStrLen);
    }

#endif
	if (mWriteBuffer[mInsideWrite - 1].append(pString, lStrLen) < 0)
	{
		mInsideWrite--;
		return (-1);
	}
#if(WEBC_DEBUG_PARSER)
    if (!LogWriteLevel || mInsideWrite == LogWriteLevel)
        WriteParseLog("\n<!-- >>>           =====After Append     ============                -->\n", pString);
#endif

	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLDocumentStream::Write", " ")

	while (mWriteBuffer[mInsideWrite - 1].getLength() > 0 && !done)
	{
  	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
       	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLDocumentStream::Stringops", " ")
        mWriteBuffer[mInsideWrite - 1].removeTrailingNulls();

        if (mWriteBuffer[mInsideWrite - 1].getLength())
        {
	       	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocumentStream::Stringops", " ")
        	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLDocumentStream::ParseHtml", " ")
		    bytesParsed = mBuilder.ParseHtml (
				mWriteBuffer[mInsideWrite - 1].getChars(),
				mWriteBuffer[mInsideWrite - 1].getLength(),
				&mParseContext
			);
        	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocumentStream::ParseHtml", " ")
			if (bytesParsed == 0)
			{
			    done = 1;
			}
		    totalParsed += bytesParsed;
#if(WEBC_DEBUG_PARSER)
// <EDIT> 20080120 BUGBUG - These diagnostics are trying to resolve a bug in
// <EDIT> 20080120 BUGBUG - Parser code that inadvertantly discards content at the end of a buffer
            if (mInsideWrite == LogWriteLevel && bytesParsed)
			{
			    WEBC_CHAR *pinput, *poutput;
                mParseOutDataBuffer.append(mWriteBuffer[mInsideWrite - 1].getChars(), bytesParsed);
                pinput = mParseInDataBuffer.getChars();
                poutput = mParseOutDataBuffer.getChars();
                for (long i = 0; i < mParseOutDataBuffer.getLength(); i++, pinput++, poutput++)
                {
                    if (*pinput != *poutput)
                    {
		                WriteParseLog("\n<!-- >>>           =====They Are Different (input)    ============                -->\n", mParseInDataBuffer.getChars());
		                WriteParseLog("\n<!-- >>>           =====They Are Different (output)    ============               -->\n", mParseOutDataBuffer.getChars());
		                break;
                    }
                }
            }
#endif

		    mWriteBuffer[mInsideWrite - 1].discardPrefix(bytesParsed);
#if(WEBC_DEBUG_PARSER)
        if (!LogWriteLevel || mInsideWrite == LogWriteLevel)
        {
			if (bytesParsed == 0)
			    WriteParseLog("\n<!-- >>>           =====No Bytes Parsed      ============                -->\n", 0);
            else
			    WriteParseLog("\n<!-- >>>           =====After Discard Prefix ============                -->\n", mWriteBuffer[mInsideWrite - 1].getChars());
        }
#endif
// <EDIT> 20080120 BUGBUG - New code removes trailing NULLS from the buffer
// <EDIT> 20080120 BUGBUG - Not effective, leave for test purposes
//		    mWriteBuffer[mInsideWrite - 1].removeTrailingNulls();
		}
		else
		{
	  		WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocumentStream::Stringops", " ");
		}

		switch (mBuilder.GetState())
		{
			case HDBLD_DONE:    /* intentional fall-through */
			case HDBLD_STOPPED:
				done = 1;
				break;

			case HDBLD_RUN_SCRIPT:
#if(WEBC_DEBUG_PARSER)
                WriteParseLog("\n<!-- >>>           =====Running ===Script  ============                -->\n",0);
#endif
				if (mBuilder.RunScript() < 0)
				{
					done = 1;
				}
				else
				{
					if (mInsideWrite < MAX_WRITE_NESTING && mWriteBuffer[mInsideWrite].getLength() > 0)
					{
						// move any unparsed data down to this level
						if (mWriteBuffer[mInsideWrite - 1].prepend(&mWriteBuffer[mInsideWrite]) < 0)
						{
							done = 1;
						}
#if(WEBC_DEBUG_PARSER)
			            WriteParseLog("\n<!-- >>>           =====After Run ===Script ============                -->\n", mWriteBuffer[mInsideWrite - 1].getChars());
			            mParseInDataBuffer.discardAll();
			            mParseOutDataBuffer.discardAll();
#endif
						mWriteBuffer[mInsideWrite].discardAll();
					}
				}

				if (mbFlags & HDOC_STREAM_DELETED)
				{
#if(WEBC_DEBUG_PARSER)
			        WriteParseLog("\n<!-- >>>           =====Run Script Deleted Stream ======                -->\n", 0);
#endif
					done = 1;
				}
				break;

			case HDBLD_ERR_CHARSET_CHANGE:
#if(WEBC_DEBUG_PARSER)
		        WriteParseLog("\n<!-- >>>           =====Charset Change Deleted Stream ======                -->\n", 0);
	            mParseInDataBuffer.discardAll();
	            mParseOutDataBuffer.discardAll();
#endif
				mWriteBuffer[mInsideWrite - 1].discardAll();
				mInsideWrite--;
				WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocumentStream::Write", " ")
				return (totalParsed);

			case HDBLD_ERR_UNSPECIFIED:
#if(WEBC_DEBUG_PARSER)
		        WriteParseLog("\n<!-- >>>           =====Unspecified Error Deleted Stream ====                -->\n", 0);
	            mParseInDataBuffer.discardAll();
	            mParseOutDataBuffer.discardAll();
#endif
				WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocumentStream::Write", " ")

				mWriteBuffer[mInsideWrite - 1].discardAll();
				mInsideWrite--;
				return (-1);
		}
	}
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLDocumentStream::Write", " ")

	mInsideWrite--;

	if (mbFlags & HDOC_STREAM_DELETED)
	{
		Close();
		return (-1);
	}
//<EDIT HEREHERE
    //printf("Calling flush... \n");
	mBuilder.FlushText();
	return (lStrLen);
}

long HTMLDocumentStream::Write(const WEBC_CHAR* pString)
{
	return (Write(pString, webc_strlen(pString)));
}

long HTMLDocumentStream::WriteLine(const WEBC_CHAR* pString, long len)
{
long result;
WEBC_UINT8 bSaveFlags = mbFlags;

	mbFlags |= HDOC_STREAM_IN_WRITE;
	result = Write (pString, len) + Write ((WEBC_CHAR *) WEBC_STR_LF);

	if (!(bSaveFlags & HDOC_STREAM_IN_WRITE))
	{
		mbFlags &= ~HDOC_STREAM_IN_WRITE;
	}

	if (mbFlags & HDOC_STREAM_DELETED)
	{
		Close();
	}

	return (result);
}

long HTMLDocumentStream::WriteLine(const WEBC_CHAR* pString)
{
long result;
WEBC_UINT8 bSaveFlags = mbFlags;

	mbFlags |= HDOC_STREAM_IN_WRITE;
	result = Write (pString) + Write ((WEBC_CHAR *) WEBC_STR_LF);

	if (!(bSaveFlags & HDOC_STREAM_IN_WRITE))
	{
		mbFlags &= ~HDOC_STREAM_IN_WRITE;
	}

	if (mbFlags & HDOC_STREAM_DELETED)
	{
		Close();
	}

	return (result);
}

void HTMLDocumentStream::Close(void)
{
	if (!(mbFlags & HDOC_STREAM_IN_WRITE) && (mInsideWrite == 0))
	{
		if (!(mbFlags & HDOC_STREAM_DELETED))
		{
			mBuilder.FlushText();
		}
		WEBC_DELETE(this);
	}
	else
	{
		mbFlags |= HDOC_STREAM_DELETED;
		mBuilder.Stop();
	}
}
