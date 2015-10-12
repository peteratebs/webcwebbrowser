//
// hXMLHttpRequest.CPP - Methods for AJAX XMLHttpRequest
//
// EBS - WebC
//
// Copyright EBS Inc. , 2009
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcfg.h"
#if (WEBC_SUPPORT_AJAX)
#include "htmldoc.hpp"
#include "hXMLHttpRequest.hpp"
#include "webcstr.h"
#include "webcmem.h"


/*****************************************************************************/
// Local Constants 
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/
static int  idle_proc(WEBC_PFBYTE data)
{
	return 0;
}

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// XMLHttpRequest Constructors
/*****************************************************************************/

XMLHttpRequest::XMLHttpRequest(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj)
	: HTMLElement(td, pDocument, pJSObj)
{
	m_pStreamCtx = 0;
	m_pDataBuffer = 0;
	m_httpRequestType = httpRequestType_GET;
	m_readyState = readyState_Uninitialized;
	m_timeout = 0;
}


/*****************************************************************************/
// XMLHttpRequest Destructor
/*****************************************************************************/

XMLHttpRequest::~XMLHttpRequest(void)
{	
	if(m_pStreamCtx)
	{
		wload_StreamFree(m_pStreamCtx, WEBC_TRUE);
		m_pStreamCtx = 0;
	}
	if(m_pDataBuffer)
	{
		WEBC_FREE(m_pDataBuffer);
		m_pDataBuffer = 0;
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

	
/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/


/*****************************************************************************/
// XMLHttpRequest-Specific Methods
/*****************************************************************************/


void XMLHttpRequest::open(WEBC_CHAR *strMethod, WEBC_CHAR *strUrl, WEBC_BOOL bAsync, WEBC_CHAR *strUser, WEBC_CHAR *strPassword)
{
	if(m_pStreamCtx)
	{
		wload_StreamFree(m_pStreamCtx, WEBC_TRUE);
		m_pStreamCtx = 0;
	}
	
	m_pStreamCtx = wload_StreamAlloc(WEBC_TRUE);
	wload_Init(m_pStreamCtx, idle_proc, (WEBC_PFBYTE) this);
	m_URL.parseURL(strUrl);

	// TODO we only support GET and POST for now
	if (!webc_strcmp(strMethod, WEBC_STR_GET))
	{
		m_httpRequestType = httpRequestType_GET;
	}
	if (!webc_strcmp(strMethod, WEBC_STR_POST))
	{
		m_httpRequestType = httpRequestType_POST;
	}
	else
	{
		m_httpRequestType = httpRequestType_GET;
	}

	changeState(readyState_Open);

}

#define MAX_TEMP_BUF_SIZE 1024

void XMLHttpRequest::send(WEBC_CHAR *strVarBody)
{
	RTP_TIMESTAMP ts;
	int tempBufferSize = MAX_TEMP_BUF_SIZE, bytesRead=0, status;
	int ContentLength;


	if(m_pDataBuffer)
	{
		WEBC_FREE(m_pDataBuffer);
		m_pDataBuffer = 0;
	}
	m_pDataBuffer = (char *) WEBC_MALLOC(tempBufferSize);

	switch(m_httpRequestType)
	{
		case httpRequestType_POST:
			ContentLength=webc_strlen(strVarBody);
			wload_Post(m_pStreamCtx, &m_URL, WEBC_NULL,ContentLength );
			webc_to_c_strcpy (m_pDataBuffer, strVarBody);
			wload_Write(m_pStreamCtx, (char *)m_pDataBuffer, ContentLength);
			wload_Read(m_pStreamCtx, m_pDataBuffer, MAX_TEMP_BUF_SIZE);
			wload_Close(m_pStreamCtx);
			changeState(readyState_Loaded);
			return;
			break;
		case httpRequestType_GET:
		default:
			wload_Get(m_pStreamCtx, &m_URL, &ts);
			break;
	}

	do
	{
		status = wload_Read(m_pStreamCtx, m_pDataBuffer + bytesRead, tempBufferSize);
		bytesRead += status;

		if(bytesRead == tempBufferSize)
			WEBC_REALLOC(m_pDataBuffer, tempBufferSize + MAX_TEMP_BUF_SIZE);
		else
			break;
	} while(status > 0);

	m_pDataBuffer[bytesRead] = 0;
	if (bytesRead)
		m_status = 200;
	wload_Close(m_pStreamCtx);

	changeState(readyState_Loaded);
}

void XMLHttpRequest::send(JSObject *pXMLDOMObj)
{
	// TODO need to figure out how to implement this method
	changeState(readyState_Loaded);
}

void XMLHttpRequest::abort()
{
	changeState(readyState_Open);
}

void XMLHttpRequest::getResponseHeader(WEBC_CHAR *strHeaderName, WEBC_CHAR *strReturnValue)
{
}

void XMLHttpRequest::setRequestHeader(WEBC_CHAR *strHeaderName, WEBC_CHAR *strHeaderValue)
{
}

void XMLHttpRequest::getAllResponseHeaders(WEBC_CHAR *strReturnValue)
{
}

void XMLHttpRequest::get_readyState(WEBC_INT *state)
{

	if(state)
		*state = (WEBC_INT) m_readyState;
}

void XMLHttpRequest::get_responseXML(JSObject **ppXMLDOMObj)
{
}

static WEBC_CHAR XMLHttpResponseText[MAX_TEMP_BUF_SIZE*sizeof(WEBC_CHAR)];

void XMLHttpRequest::get_responseText(WEBC_CHAR **pStrBody)
{
	webc_c_strncpy(XMLHttpResponseText, m_pDataBuffer,MAX_TEMP_BUF_SIZE);
	// *pStrBody = (WEBC_CHAR *)L"<sales vendor=\"John\"> <item type=\"peas\" price=\"4\" quantity=\"6\"/> <item type=\"carrot\" price=\"3\" quantity=\"10\"/> <item type=\"chips\" price=\"5\" quantity=\"3\"/> </sales>";
	*pStrBody = (WEBC_CHAR *)XMLHttpResponseText;
}

void XMLHttpRequest::get_status(WEBC_INT *status)
{
	*status = m_status;
}

void XMLHttpRequest::get_timeout(WEBC_INT *timeout)
{
	if(timeout)
		*timeout = m_timeout;
}

void XMLHttpRequest::put_timeout(WEBC_INT timeout)
{
	m_timeout = timeout;
}

// not used by jQuery
void XMLHttpRequest::get_statusText(WEBC_CHAR *strStatusText)
{
}

void XMLHttpRequest::get_responseBody(WEBC_CHAR *strBody)
{
}

/*****************************************************************************/
// XMLHttpRequest-Private/Protected Methods
/*****************************************************************************/

void XMLHttpRequest::changeState(readyState_t newState)
{
	// set our internal sense of the ready state
	m_readyState = newState;

	// trigger the onreadystatechange event handler
	HTMLEvent e;
	e.type = HTML_EVENT_ONREADYSTATECHANGE;
	Event(&e);
}



#endif // WEBC_SUPPORT_AJAX
