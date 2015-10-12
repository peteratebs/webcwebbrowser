/*
|  hXMLHttpRequest.hpp -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#ifndef __HXMLHTTPREQUEST_HPP__
#define __HXMLHTTPREQUEST_HPP__

#include "webcfg.h"
#include "helement.hpp"
#include "webload.hpp"

/*************************************************************************
* class XMLHttpRequest
* 
*************************************************************************/
#if (WEBC_SUPPORT_AJAX)

class XMLHttpRequest : public HTMLElement
{
public:
	XMLHttpRequest(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0, JSObject *pJSObj = 0);
	virtual ~XMLHttpRequest();

	// XMLHttpRequest-Specific Methods ///
	void open(WEBC_CHAR *strMethod, WEBC_CHAR *strUrl, WEBC_BOOL bAsync=WEBC_TRUE, WEBC_CHAR *strUser=0, WEBC_CHAR *strPassword=0);
	void send(WEBC_CHAR *strVarBody);
	void send(JSObject *pXMLDOMObj);
	void abort();
	void getResponseHeader(WEBC_CHAR *strHeaderName, WEBC_CHAR *strReturnValue);
	void setRequestHeader(WEBC_CHAR *strHeaderName, WEBC_CHAR *strHeaderValue);
	// not used by jQuery
	void getAllResponseHeaders(WEBC_CHAR *strReturnValue); 
	////////////////////////////////////////

	// XMLHttpRequest Properties ///
	void get_readyState(WEBC_INT *state);
	void get_responseText(WEBC_CHAR **pStrBody);
	void get_responseXML(JSObject **ppXMLDOMObj);
	void get_status(WEBC_INT *staus);
	void get_timeout(WEBC_INT *timeout);
	void put_timeout(WEBC_INT timeout);
	// not used by jQuery
	void get_statusText(WEBC_CHAR *strStatusText);
	void get_responseBody(WEBC_CHAR *strBody); 
	////////////////////////////////////////

protected:

	// allowable values to the open method (first parameter)
	typedef enum { httpRequestType_GET, httpRequestType_POST, httpRequestType_HEAD, httpRequestType_PUT, httpRequestType_DELETE, httpRequestType_MOVE,
		httpRequestType_PROPFIND, httpRequestType_PROPPATCH , httpRequestType_MKCOL, httpRequestType_COPY, httpRequestType_LOCK, httpRequestType_UNLOCK, httpRequestType_OPTIONS } httpRequestType_t;

	typedef enum { 
		readyState_Uninitialized=0, // The object has been created, but not initialized (the open method has not been called).
		readyState_Open, // The object has been created, but the send method has not been called. 
		readyState_Sent, // The send method has been called. responseText is not available. responseBody is not available. 
		readyState_Receiving, // Some data has been received. responseText is not available. responseBody is not available. 
		readyState_Loaded } readyState_t;

	UrlStreamCtx *m_pStreamCtx;
	URLDescriptor m_URL;
	char *m_pDataBuffer;
	httpRequestType_t m_httpRequestType;
	readyState_t m_readyState;
	WEBC_INT m_status;
	WEBC_INT m_timeout;

	void changeState(readyState_t newState);
};
#endif // WEBC_SUPPORT_AJAX

#endif // __HXMLHTTPREQUEST_HPP__