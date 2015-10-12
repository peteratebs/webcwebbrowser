//
// WEBCSDKPROTO.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
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

#include "webcjhconfig.h"
#include "htmlsdk.h"
#include "httpc.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif
#if (WEBC_SUPPORT_COOKIES)
 #include "cookie.h"
#endif
#include "hchkbox.hpp"
#include "helement.hpp"
#include "hframe.hpp"
#include "himage.hpp"
#include "hobject.hpp"
#include "hinput.hpp"
#include "hhidden.hpp"
#include "hedit.hpp"
#include "hpasswd.hpp"
#include "hselect.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "format.hpp"
#include "webctmo.h"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "prothttp.h"
#include "wgkeys.hpp"
#include "jhutils.h"
#include "tuenum.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "rtpprint.h"
#include "webcdefault.h"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif


/*****************************************************************************/
// Local types
/*****************************************************************************/
#include "webcsdk.hpp"


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// External Declarations
/*****************************************************************************/

/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/

/*****************************************************************************/
/**
 @memo Prototyped implementations go here until moved.

 @doc
 */
/*****************************************************************************/

// HEREHERE Beging websdkproto.cpp

#if (WEBC_SUPPORT_POSTFILE)
#define POST_FIX	1
int webc_post_UploadFileASCII(HBROWSER_HANDLE postBrowser, char *SourceFileName, char *pUrl, char* DestApplication, char *DestFileName)
{
	//WEBC_CHAR *query;
	long queryLen;
#if (!POST_FIX)
    URLDescriptor desc;
#endif
	HDOC_HANDLE pDoc;
	int bDocDestroyed;
	char postActionASCII[1024];
#if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR postActionUnicode[1024];
#endif
	rtp_strcpy(postActionASCII,DestApplication);
	rtp_strcat(postActionASCII,"?");
	rtp_strcat(postActionASCII,DestFileName);
	queryLen = rtp_strlen(postActionASCII);

	pDoc = webc_BrowserGetDocument(postBrowser);

	HTMLDocument *pMyDoc      =  (HTMLDocument *)pDoc;
	HTMLDocument *pTargDoc    =  pMyDoc;
	HTMLBrowser *pTargBrowser =  0;

	if(!pMyDoc)
	{
		return(-1);
	}

	bDocDestroyed = (pMyDoc == pTargDoc)? 1:0;
#if (POST_FIX)

	WEBC_CHAR *mpNewUrl;
	char *mpNewUrl_ASCII, *http_str="http://" ;
	long urldesc_len;
	urldesc_len = rtp_strlen(http_str)+rtp_strlen(pUrl)+rtp_strlen(postActionASCII)+1;
	mpNewUrl_ASCII= (char *) WEBC_MALLOC(urldesc_len*sizeof(char));
	mpNewUrl=(WEBC_CHAR *) WEBC_MALLOC(urldesc_len*sizeof(WEBC_CHAR));

	rtp_strcpy(mpNewUrl_ASCII,http_str);
	rtp_strcat(mpNewUrl_ASCII,pUrl);
	rtp_strcat(mpNewUrl_ASCII,postActionASCII);
	mpNewUrl_ASCII[urldesc_len]=0;
	webc_c_strcpy(mpNewUrl,mpNewUrl_ASCII);
	mpNewUrl[urldesc_len]=0;



	//URLDescriptor desc(mpNewUrl, &desc1);
	URLDescriptor desc(mpNewUrl, 0);
	//desc.setProtocol(WEBC_PROTOCOL_HTTP);
#else

#if (WEBC_SUPPORT_UNICODE)
		webc_c_strcpy(postActionUnicode,&postActionASCII[0]);
		desc.parseURL(&postActionUnicode[0], pMyDoc->BaseURL());

#else
		desc.parseURL(&postActionASCII[0], pMyDoc->BaseURL());
#endif

#endif
		pTargBrowser = pTargDoc->GetBrowser();
		if (pTargBrowser)
			{
				WEBC_CHAR temp[48];
				LoadJob *pJob;

				// don't save the current url to the history if protocol is JavaScript,
				//  because javascript:// url's are not really page locations
				if (desc.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					pTargBrowser->Stop(pTargDoc);
					pTargDoc->SaveLocation();
				}

				//webc_c_strcpy(temp, "application/x-www-form-urlencoded");
				webc_c_strcpy(temp, "application/pdf");
				//The LoadJob contructor PostUploadRawFile
				WEBC_NEW(pJob, PostUploadRawFile(pTargDoc, &desc, temp, &postActionASCII[0], queryLen, SourceFileName, WEBC_CHARSET_UNKNOWN));

				if (pJob)
				{
					pTargBrowser->ProcessLoadJob(pJob, &desc, pMyDoc->BaseURL(), pTargDoc);
				  #ifdef WEBC_OLD_LOAD_METHOD
					webc_thread_load_browser(pTargBrowser);
				  #endif // WEBC_OLD_LOAD_METHOD
				}
				else
				{
					bDocDestroyed = 0;
				}
				return(bDocDestroyed);
			}
	return 0;
}
#endif /* (WEBC_SUPPORT_POSTFILE) */

#if (WEBC_SUPPORT_POST_DATABUFFER)

int webc_post_UploadDataBuffer(HBROWSER_HANDLE postBrowser, char *Post_Buffer, long Post_Buffer_Size, char *pUrl, char* DestApplication, char *DestFileName)
{
	long queryLen;
#if (!POST_FIX)
    URLDescriptor desc;
#endif
	HDOC_HANDLE pDoc;
	int bDocDestroyed;
	char postActionASCII[1024];
#if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR postActionUnicode[1024];
#endif
	rtp_strcpy(postActionASCII,DestApplication);
	rtp_strcat(postActionASCII,"?");
	rtp_strcat(postActionASCII,DestFileName);
	queryLen = rtp_strlen(postActionASCII);

	pDoc = webc_BrowserGetDocument(postBrowser);

	HTMLDocument *pMyDoc      =  (HTMLDocument *)pDoc;
	HTMLDocument *pTargDoc    =  pMyDoc;
	HTMLBrowser *pTargBrowser =  0;

	if(!pMyDoc)
	{
		return(-1);
	}

	bDocDestroyed = (pMyDoc == pTargDoc)? 1:0;
#if (POST_FIX)

	WEBC_CHAR *mpNewUrl;
	char *mpNewUrl_ASCII, *http_str="http://" ;
	long urldesc_len;
	urldesc_len = rtp_strlen(http_str)+rtp_strlen(pUrl)+rtp_strlen(postActionASCII)+1;
	mpNewUrl_ASCII= (char *) WEBC_MALLOC(urldesc_len*sizeof(char));
	mpNewUrl=(WEBC_CHAR *) WEBC_MALLOC(urldesc_len*sizeof(WEBC_CHAR));

	rtp_strcpy(mpNewUrl_ASCII,http_str);
	rtp_strcat(mpNewUrl_ASCII,pUrl);
	rtp_strcat(mpNewUrl_ASCII,postActionASCII);
	mpNewUrl_ASCII[urldesc_len]=0;
	webc_c_strcpy(mpNewUrl,mpNewUrl_ASCII);
	mpNewUrl[urldesc_len]=0;



	//URLDescriptor desc(mpNewUrl, &desc1);
	URLDescriptor desc(mpNewUrl, 0);
	//desc.setProtocol(WEBC_PROTOCOL_HTTP);
#else
#if (WEBC_SUPPORT_UNICODE)
		webc_c_strcpy(postActionUnicode,&postActionASCII[0]);
		desc.parseURL(&postActionUnicode[0], pMyDoc->BaseURL());
#else   //WEBC_SUPPORT_UNICODE
		desc.parseURL(&postActionASCII[0], pMyDoc->BaseURL());
#endif //WEBC_SUPPORT_UNICODE

#endif  //POST_FIX
		pTargBrowser = pTargDoc->GetBrowser();
		if (pTargBrowser)
			{
				WEBC_CHAR temp[48];
				LoadJob *pJob;
				// don't save the current url to the history if protocol is JavaScript,
				//  because javascript:// url's are not really page locations
				if (desc.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					pTargBrowser->Stop(pTargDoc);
					pTargDoc->SaveLocation();
				}

				webc_c_strcpy(temp, "application/x-www-form-urlencoded");
				//The LoadJob contructor PostUploadRawFile
				WEBC_NEW(pJob, PostUploadDataBuffer(pTargDoc, &desc, temp, &postActionASCII[0], queryLen, Post_Buffer, Post_Buffer_Size ,WEBC_CHARSET_UNKNOWN));

				if (pJob)
				{
					pTargBrowser->ProcessLoadJob(pJob, &desc, pMyDoc->BaseURL(), pTargDoc);
				  #ifdef WEBC_OLD_LOAD_METHOD
					webc_thread_load_browser(pTargBrowser);
				  #endif // WEBC_OLD_LOAD_METHOD
				}
				else
				{
					bDocDestroyed = 0;
				}
				return(bDocDestroyed);
			}
	return 0;
}
#endif // WEBC_SUPPORT_POST_DATABUFFER





//MA
#if (WEBC_SUPPORT_TSD_TOKEN_EXCHANGE)

struct s_token_table_entry
{
	void * ptr;
	WEBC_INT cLen;
};
//void *WEBC_TSD_TOKEN[WEBC_NUM_TOKENS];  //static because of Linker error "error LNK2005: _WEBC_TSD_TOKEN already defined in *.obj"
struct s_token_table_entry WEBC_TSD_TOKEN[WEBC_NUM_TOKENS]={0};
#define TSD_SERIALNO_LEN		20

int	set_webc_tsd_token(int token_name, void * token_value, int cLen)   // int cLen contain a string length (or array length) if it is a string, 0 otherwis
{
	switch (token_name)
	{
		case WEBC_TOKEN_MACADDR:
			if(cLen < 0)
				return (-1);
			if(cLen > 18)
				return (-2);
			if(!token_value)
				return (-3);

		if(WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr )
		{
			WEBC_FREE(WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr );
			WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr = 0;
		}
#if (WEBC_SUPPORT_UNICODE)
			WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr = (void *) WEBC_MALLOC(cLen * sizeof(WEBC_CHAR8) );
#else
			WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr = (void *) WEBC_MALLOC(cLen * sizeof(WEBC_CHAR) );
#endif //if WEBC_SUPPORT_UNICODE

			WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].cLen = cLen;
			rtp_memcpy(WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr,token_value,cLen);
			return (0);

		case WEBC_TOKEN_SERILANO:
			if(cLen < 0)
				return (-1);
			if(cLen > TSD_SERIALNO_LEN)   // we assume the serial number to be 20 characters in length
				return (-2);
			if(!token_value)
				return (-3);

			if(WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr)
			{
				WEBC_FREE(WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr);
				WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr=0;

			}
#if (WEBC_SUPPORT_UNICODE)
			//tsd_webc_exchange_token.device_serial_number=(WEBC_CHAR8*) WEBC_MALLOC(cLen*sizeof(WEBC_CHAR8));
			WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr= (void *) WEBC_MALLOC(cLen * sizeof(WEBC_CHAR8) );
#else
			WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr= (void *) WEBC_MALLOC(cLen * sizeof(WEBC_CHAR) );
#endif //if WEBC_SUPPORT_UNICODE

			WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].cLen=cLen;
			rtp_memcpy(WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr,token_value,cLen);
			return (0);

		case WEBC_TOKEN_ADDITIONAL_HEADER:
			if(WEBC_TSD_TOKEN[WEBC_TOKEN_ADDITIONAL_HEADER].ptr )
			{
				WEBC_FREE(WEBC_TSD_TOKEN[WEBC_TOKEN_ADDITIONAL_HEADER].ptr );
				WEBC_TSD_TOKEN[WEBC_TOKEN_ADDITIONAL_HEADER].ptr = 0;
			}

			if(cLen > 0)
			{

#if (WEBC_SUPPORT_UNICODE)
				WEBC_TSD_TOKEN[WEBC_TOKEN_ADDITIONAL_HEADER].ptr= (void *) WEBC_MALLOC(cLen * sizeof(WEBC_CHAR8) );
#else
				WEBC_TSD_TOKEN[WEBC_TOKEN_ADDITIONAL_HEADER].ptr= (void *) WEBC_MALLOC(cLen * sizeof(WEBC_CHAR) );
#endif //if WEBC_SUPPORT_UNICODE
				rtp_memcpy(WEBC_TSD_TOKEN[WEBC_TOKEN_ADDITIONAL_HEADER].ptr,token_value,cLen);
			}
			return (0);

		case WEBC_TOKEN_ROM_ADDR:
			WEBC_TSD_TOKEN[WEBC_TOKEN_ROM_ADDR].ptr= token_value;
			WEBC_TSD_TOKEN[WEBC_TOKEN_ROM_ADDR].cLen=0;
			//tsd_webc_exchange_token.rom_start_address= (WEBC_UINT8 *) token_value;
			return(0);
	}

	return (-1);
}

void* get_webc_tsd_token(int token_name,WEBC_INT *len)
{
	switch (token_name)
	{
		case WEBC_TOKEN_MACADDR:
			*len = WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].cLen;
			return (WEBC_TSD_TOKEN[WEBC_TOKEN_MACADDR].ptr);
		case WEBC_TOKEN_SERILANO:
			*len = WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].cLen;
			return (WEBC_TSD_TOKEN[WEBC_TOKEN_SERILANO].ptr);
		case WEBC_TOKEN_ROM_ADDR:
			*len = WEBC_TSD_TOKEN[WEBC_TOKEN_ROM_ADDR].cLen;
			return (WEBC_TSD_TOKEN[WEBC_TOKEN_ROM_ADDR].ptr);
	}

	return (0);
}

#endif  //if WEBC_SUPPORT_TSD_TOKEN_EXCHANGE
