//
// HFORM.CPP - Methods for HTML DOM class HTMLForm
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

#include "htmldoc.hpp"
#include "hform.hpp"
#include "hinput.hpp"
#include "hsubmit.hpp"
#include "util.h"
#include "webload.hpp"
#include "htmlbrow.hpp"
#include "htmlfind.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"


/*****************************************************************************/
// Local Constants
/*****************************************************************************/

#define FORM_ENCODING_TYPE_NAME_SIZE 2

// this list mirrors e_HTMLFormEncodingType
static const char *form_encoding_type_names[] =
{
	"application/x-www-form-urlencoded",
	"multipart/form-data",
};

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

class HTMLFormFieldEncoder
{
private:
	WebcCharset mAcceptCharset;
	long miBufferSize;
	WEBC_CHAR *mpInput;
	WEBC_CHAR *mpBuffer;

public:
	HTMLFormFieldEncoder (WebcCharset acceptCharset, long initialBufferSize);
	~HTMLFormFieldEncoder (void);

	void SetInput (WEBC_CHAR *str);
	long GetLength (void);
	long WriteTo (WEBC_CHAR *buffer);
};


/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// HTMLForm Constructors
/*****************************************************************************/

HTMLForm::HTMLForm(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLSpan(td, pDocument)
{
	InitObjectList(&mFieldVector);

	mMethod = HTML_FORM_METHOD_GET;
	mAction = 0;
	mName = 0;
	mEncType = HTML_ENCTYPE_URLENCODED;
	mTarget = 0;
	mAcceptCharset = WEBC_CHARSET_UNKNOWN;
}


/*****************************************************************************/
// HTMLForm Destructor
/*****************************************************************************/

HTMLForm::~HTMLForm(void)
{
	ObjectListClear(&mFieldVector);
	webc_free_string_copy(mAction, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	webc_free_string_copy(mTarget, __FILE__, __LINE__);
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

/*
void HTMLForm::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

HTMLEventStatus HTMLForm::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_SUBMIT:
		{
			DISPLAY_INT x,y;

			x = pEvent->data.position.x;
			y = pEvent->data.position.y;

			if (Submit((HTMLElement*) pEvent->elem, x, y) == 1)
			{
				return (HTML_EVENT_STATUS_HALT);
			}
			return (HTML_EVENT_STATUS_DONE);
		}

		case HTML_EVENT_RESET:
			Reset();
			return (HTML_EVENT_STATUS_DONE);

		default:
			break;
	}

	return (HTMLSpan::DefaultEvent(pEvent));
}

/*
HTMLElement *HTMLForm::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

void HTMLForm::Reset(void)
{
vector_iterator vi;
HTMLInput *pElement;

	pElement = (HTMLInput*) ObjectListGetFirst(&mFieldVector, &vi);
	while (pElement)
	{
		pElement->Reset();
		pElement = (HTMLInput*) ObjectListGetNext(&vi);
	}
}


int HTMLForm::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ACCEPT_CHARSET:
			SetAcceptCharset(value);
			return (1);

		case HTML_ATTRIB_ENCTYPE:
			SetEncType(value);
			return(1);

		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		case HTML_ATTRIB_METHOD:
			SetMethod(value);
			return (1);

		case HTML_ATTRIB_ACTION:
			SetAction(value);
			return (1);

		case HTML_ATTRIB_TARGET:
			SetTarget(value);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


void HTMLForm::SetMethod(const WEBC_CHAR *pMethod)
{
	if (!pMethod)
		return;

	if (!webc_c_stricmp(pMethod, "get"))
	{
		mMethod = HTML_FORM_METHOD_GET;
	}
	else if (!webc_c_stricmp(pMethod, "post"))
	{
		mMethod = HTML_FORM_METHOD_POST;
	}
}


void HTMLForm::SetAction(const WEBC_CHAR *pAction)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pAction, __FILE__, __LINE__);
	webc_free_string_copy(mAction, __FILE__, __LINE__);
	mAction = pStr;
}


void HTMLForm::SetName(const WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mName, __FILE__, __LINE__);
	mName = pStr;
}


void HTMLForm::SetTarget(const WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);

	webc_free_string_copy(mTarget, __FILE__, __LINE__);
	mTarget = pStr;
}

void HTMLForm::SetEncType (WEBC_CHAR *strType)
{
	int i;

	for (i=0; i<FORM_ENCODING_TYPE_NAME_SIZE; i++)
	{
		if (!webc_c_strnicmp(strType, form_encoding_type_names[i], tc_strlen(form_encoding_type_names[i])))
		{
			mEncType = (HTMLFormEncodingType) i;
			break;
		}
	}
}

const char* HTMLForm::EncType ()
{
	if((mEncType >= 0) && (mEncType < FORM_ENCODING_TYPE_NAME_SIZE))
		return (form_encoding_type_names[mEncType]);
	else
		return (form_encoding_type_names[0]);
}

/*****************************************************************************/
// HTMLForm-Specific Methods
/*****************************************************************************/

void HTMLForm::AddField(HTMLElement *pField)
{
	ObjectListAddRear(&mFieldVector, (WEBC_PFBYTE) pField);
}


void HTMLForm::RemoveField(HTMLElement *pField)
{
	ObjectListRemove(&mFieldVector, (WEBC_PFBYTE) pField);
}


WEBC_BOOL HTMLForm::ContainsField(HTMLElement *pField)
{
vector_iterator vi;
HTMLElement *pElement;

	pElement = (HTMLElement*) ObjectListGetFirst(&mFieldVector, &vi);
	while (pElement)
	{
		if (pField == pElement)
		{
			return (1);
		}

		pElement = (HTMLElement*) ObjectListGetNext(&vi);
	}

	return (0);
}

HTMLElement *HTMLForm::FindField(HTMLElementFinder *finder)
{
	vector_iterator vi;
	HTMLElement *pElem = (HTMLElement *) ObjectListGetFirst(&mFieldVector, &vi);
	while (pElem)
	{
		if (finder->Check(pElem))
		{
			return (pElem);
		}
		pElem = (HTMLElement *) ObjectListGetNext(&vi);
	}

	return (0);
}

int HTMLForm::Submit(HTMLElement *pSubmit, int x, int y)
{
WEBC_CHAR nullStr[] = {0};
WEBC_CHAR *query;
long queryLen;
URLDescriptor desc;
HTMLDocument *pMyDoc = GetDocument();
HTMLDocument *pTargDoc = pMyDoc;
HTMLBrowser *pTargBrowser = 0;
int bDocDestroyed;


	if (!pMyDoc)
	{
		return (-1);
	}

	// Find the target frame of this form action
	pTargDoc = GetFrameDocument(Target());

	bDocDestroyed = (pMyDoc == pTargDoc)? 1:0;

	switch (mMethod)
	{
		case HTML_FORM_METHOD_GET:
			queryLen = AssembleQueryString(0, pSubmit, x, y);

			query = (WEBC_CHAR *) WEBC_MALLOC((queryLen + 1) * sizeof(WEBC_CHAR));
			if (!query)
			{
				return (-1);
			}

			// Now assemble the actual query string
			AssembleQueryString(query, pSubmit, x, y);

			if (mAction)
			{
				desc.parseURL(mAction, pMyDoc->BaseURL());
			}
			else
			{
				desc.set(pMyDoc->CurrentURL());
			}

			desc.setQuery(query);

			pTargBrowser = pTargDoc->GetBrowser();
			if (pTargBrowser)
			{
				// don't save the current url to the history if protocol is JavaScript,
				//  because javascript:// url's are not really page locations
				if (desc.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					pTargBrowser->Stop(pTargDoc);
					pTargDoc->SaveLocation();
				}

				LoadJob *pJob;
				WEBC_NEW_VERBOSE(pJob, LoadHTMLDocument(pTargDoc, &desc, WEBC_CHARSET_UNKNOWN),"LoadHTMLDocument");

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

				WEBC_FREE(query);
				return (bDocDestroyed);
			}
			WEBC_FREE(query);
			break;

		case HTML_FORM_METHOD_POST:
			if (mEncType == HTML_ENCTYPE_URLENCODED)
			{
				// First call AssembleQueryString will NULL destination buffer to get the estimated size
				queryLen = AssembleQueryString(0, pSubmit, x, y) + 1;
				query = (WEBC_CHAR *) WEBC_MALLOC(queryLen * sizeof(WEBC_CHAR));
				if (!query)
				{
					//webc_user_prompt_ok(wc, "Not enough memory for query string!", 0);
					return (-1);
				}

				// Now assemble the actual query string
				AssembleQueryString(query,pSubmit,x,y);
			}
			else
			{
				queryLen = 0;
				query = nullStr;
			}

			if (mAction)
			{
				desc.parseURL(mAction, pMyDoc->BaseURL());
			}
			else
			{
				desc.set(pMyDoc->CurrentURL());
			}

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

				long len = webc_strlen(query);
				char *postData = webc_arg_to_char(query);
				if (postData)
				{
					WEBC_NEW_VERBOSE(pJob, PostHTMLDocument(pTargDoc, &desc, temp, postData, len, WEBC_CHARSET_UNKNOWN),"PostHTMLDocument");
				}
				else
				{
					pJob = 0;
				}
				webc_arg_done(postData);

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

				if (queryLen > 0)
				{
					WEBC_FREE(query);
				}
				return (bDocDestroyed);
			}

			if (queryLen > 0)
			{
				WEBC_FREE(query);
			}
			break;

		default:
			//pBrowser->ErrorBox("Unrecognized form type!"); tbd
			break;
	}

	return(0);
}

#if(0)
int HTMLForm::post_UploadFile(HTMLBrowser *postBrowser, WEBC_CHAR *DestFieldName, WEBC_CHAR *SourceFieldName, char *pUrl)
{
WEBC_CHAR *query="someString";
long queryLen;
URLDescriptor desc;
HTMLDocument *pMyDoc = GetDocument();
HTMLDocument *pTargDoc = pMyDoc;
HTMLBrowser *pTargBrowser = 0;
int bDocDestroyed;

HTMLElement *pSourceFilenameElem = 0;
HTMLElement *pDestFilenameElem = 0;
HTMLElement *pSourceFileSizeElem = 0;

	if (!pMyDoc)
	{
		return (-1);
	}

	// Find the target frame of this form action
	pTargDoc = GetFrameDocument(Target());

	bDocDestroyed = (pMyDoc == pTargDoc)? 1:0;

#if (WEBC_SUPPORT_UNICODE)
WEBC_CHAR *FileSizeFieldName = (WEBC_CHAR *) L"SourceFileSize";
#else
WEBC_CHAR *FileSizeFieldName = "SourceFileSize";
#endif
			// try to find a field whose name is <name> in this form
//			HTMLElementByIdFinder fid(name);
			HTMLElementByNameFinder destf(DestFieldName);
			pDestFilenameElem = this->FindField(&destf);
			HTMLElementByNameFinder sourcef(SourceFieldName);
			pSourceFilenameElem = this->FindField(&sourcef);
			pDestFilenameElem = this->FindField(&destf);
			HTMLElementByNameFinder sizef(FileSizeFieldName);
			pSourceFileSizeElem = this->FindField(&sizef);
			if (pSourceFilenameElem && 	pDestFilenameElem && pSourceFileSizeElem)
			{
				pSourceFileSizeElem->SetValue((WEBC_CHAR *)L"1234");
				// pSourceFilenameElem->SetValue((WEBC_CHAR *)L"X");
			}
/////
			/*if (mEncType == HTML_ENCTYPE_URLENCODED)
			{
				// First call AssembleQueryString will NULL destination buffer to get the estimated size
				queryLen = AssembleQueryString(0, pSubmit, x, y) + 1;
				query = (WEBC_CHAR *) WEBC_MALLOC(queryLen * sizeof(WEBC_CHAR));
				if (!query)
				{
					//webc_user_prompt_ok(wc, "Not enough memory for query string!", 0);
					return (-1);
				}

				// Now assemble the actual query string
				AssembleQueryString(query,pSubmit,x,y);
			}
			else
			{
				queryLen = 0;
				query = nullStr;
			}*/

			if (mAction)
			{
				desc.parseURL(mAction, pMyDoc->BaseURL());
			}
			else
			{
				desc.set(pMyDoc->CurrentURL());
			}

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

				long len = webc_strlen(query);
				char *postData = webc_arg_to_char(query);
				if (postData)
				{
			//webc_post_webc_post_uploadfileASCII(pTargDoc, &desc, SourceFileName);
			if (pSourceFilenameElem && 	pDestFilenameElem && pSourceFileSizeElem)
			{
//				webc_c_strcpy(temp, "text/plain");
					char *SourceFileName = webc_arg_to_char(pSourceFilenameElem->Value());
					char *DestfileName = webc_arg_to_char(pDestFilenameElem->Value());
					WEBC_NEW(pJob, PostUploadRawFile(pTargDoc, &desc, temp, postData, len, SourceFileName, WEBC_CHARSET_UNKNOWN));
					webc_arg_done(SourceFileName);
			}
			else
					WEBC_NEW(pJob, PostHTMLDocument(pTargDoc, &desc, temp, postData, len, WEBC_CHARSET_UNKNOWN));
				}
				else
				{
					pJob = 0;
				}
				webc_arg_done(postData);

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

				if (queryLen > 0)
				{
					WEBC_FREE(query);
				}
				return (bDocDestroyed);
			}

			if (queryLen > 0)
			{
				WEBC_FREE(query);
			}

	return(0);
}
#endif

#if 0
int HTMLForm::PostQuery(WEBC_CHAR *query)
{
WEBC_UINT16 target_frame_number;
int sl;
webc_ctx *wc = mpDocument->GetWebcContext();
WEBC_CHAR *dest;

	//this is a post, we need a target. Make sure it is given.
	if (!mAction)
	{
		return(-1);
	}

	switch (mEncType)
	{
		case HTML_ENCTYPE_URLENCODED:
			if (query[0] == '\0')
				break;

			sl = AssemblePostQuery(query,0);
			if(sl<0)
			{
				return(-1);
			}
			// SPR
			dest = (WEBC_CHAR *) WEBC_MALLOC((sl + 1) * sizeof(WEBC_CHAR),__FILE__,__LINE__);
			if(!dest)
			{
				return(-1);
			}

			AssemblePostQuery(query,dest);
			break;

		case HTML_ENCTYPE_MULTIPART_FORM_DATA:
			dest = AssembleMultipartPostQuery();
			if (dest == WEBC_NULL)
			{//tbd: there has been an error. This shoud be caught somewhere
			 // so the user can be notified
				return(-1);
			}
			break;
	}

/*
	webc_set_cur_url(wc,mAction,WEBC_BASE_URL(wc));
	gwrap_start_url_cycle(wc);
	webc_url_cycle_init(wc,1,dest);
*/

	// dest should be WEBC_FREE'd later (tbd - check this)

	return(0);
}
#endif

#if 0
int HTMLForm::GenerateMultipartHeader(WEBC_CHAR *part_data)
{
webc_ctx *wc = mpDocument->GetWebcContext();
struct url_descriptor action_descriptor;
int sl=0;

	sl+=tc_strlen("POST ");
	webc_get_url_descriptor(mAction,&action_descriptor,WEBC_BASE_URL(wc));
	sl+=tc_strlen(action_descriptor.path);
	sl+=tc_strlen(" HTTP/1.0\r\n");
	/* tbd - add this back in
	sl+=tc_strlen("Referer: http://");
	sl+=tc_strlen(hd->url.host);
	sl+=tc_strlen(hd->url.path);
	*/
	sl+=tc_strlen("\r\n");
	sl+=tc_strlen("Connection: Keep-Alive\r\n");
	sl+=tc_strlen("User-Agent: Mozilla/4.0 (compatible; WebC " WEBC_VERSION_STR "; Win32)\r\n");
	sl+=tc_strlen("Host: ibm\r\n");
	sl+=tc_strlen("Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*\r\n");
	sl+=tc_strlen("Accept-Encoding: gzip\r\nAccept-Language:en\r\n");
	sl+=tc_strlen("Content-Type: multipart/form-data; boundary=");
	sl+=tc_strlen("---------------------------");//will have to insert boundary.

	if(part_data)
	{
		//tc_strcpy(part_data,form->method);
		tc_strcpy(part_data,"POST ");
		tc_strcat(part_data,action_descriptor.path);
		tc_strcat(part_data," HTTP/1.0\r\n");
		/* tbd - add this back in
		tc_strcat(part_data,"Referer: http://");
		tc_strcat(part_data,hd->url.host);
		tc_strcat(part_data,hd->url.path);
		*/
		tc_strcat(part_data,"\r\n");
		tc_strcat(part_data,"Connection: Keep-Alive\r\n");
		tc_strcat(part_data,"User-Agent: Mozilla/4.0 (compatible; WebC " WEBC_VERSION_STR "; Win98)\r\n");
		tc_strcat(part_data,"Host: ibm\r\n");
		tc_strcat(part_data,"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*\r\n");
		tc_strcat(part_data,"Accept-Encoding: gzip\r\nAccept-Language:en\r\n");
		tc_strcat(part_data,"Content-Type: multipart/form-data; boundary=");
		tc_strcat(part_data,"---------------------------");
	}

	return sl;
}

WEBC_CHAR *HTMLForm::AssembleMultipartPostQuery(void)
{
int lenpos,is_selected,header_size;
int n, sl;
int qsize=0;
long int data_size=0;
struct vector pPartQueue[1];
struct html_input *i;
WEBC_CHAR *field_string,packet_data;
WEBC_CHAR psize[10];
WEBC_CHAR delim[WEBC_MAX_DELIM_LENGTH];
struct webc_multipart_form_unit *sub_part;
struct webc_delimiter_context dc[1];
vector_iterator vi;

	InitQueue(pPartQueue);

	sub_part = (struct webc_multipart_form_unit *)WEBC_MALLOC(
				sizeof(struct webc_multipart_form_unit),__FILE__,__LINE__);

	//POST method requires an action
	if(!mAction || !sub_part)
	{
		return (WEBC_NULL);
	}

	//create the header data and check for errors
	sl=GenerateMultipartHeader(0);
	if(sl<1)
	{
		return (WEBC_NULL);
	}
	sub_part->header = (WEBC_CHAR *) WEBC_MALLOC((sl + 1) * sizeof(WEBC_CHAR),__FILE__,__LINE__);
	if(!sub_part->header)
	{
		return (WEBC_NULL);
	}
	sub_part->is_file=0;
	sub_part->filename=WEBC_NULL;
	GenerateMultipartHeader(sub_part->header);
	if(tc_strlen(sub_part->header)!=sl)
	{
		return (WEBC_NULL);
	}

	header_size=tc_strlen(sub_part->header);
	//enqueue header, and free string
	EnqueueObject(pPartQueue, (WEBC_PFBYTE) sub_part);
	qsize++;
	sub_part = (struct webc_multipart_form_unit *)WEBC_MALLOC(
				sizeof(struct webc_multipart_form_unit),__FILE__,__LINE__);


	//Get the first input vector in 'i'
    i=(struct html_input*)vector_get_first(&(form->input_vector),&vi);
    n=0;

	//cycle	the input vectors.
	while(i)
	{
		if(i->type==HTML_INPUT_SUBMIT)
			break;//we've processed all the data

		sl=0;

		//Radio/Checkboxes have default values. Other input types
		//depend on the user for their value.
		//If a radio/checkbox has no default value, or if it is not
		//selected, we skip it, and go to the next input vector.
		//These invalid/unchecked radio/checkboxes do not appear in
		//a multipart/form-data transmission at all.
		switch(i->type)
		{
			case HTML_INPUT_CHECKBOX:
			case HTML_INPUT_RADIO:
				if(i->type==HTML_INPUT_CHECKBOX)
					is_selected=gwrap_get_checkbox_status(wc,WORD_ID(hd,n+1));
				if(i->type==HTML_INPUT_RADIO)
					is_selected=gwrap_get_radio_status(wc,WORD_ID(hd,n+1));

				field_string="";
				if(!is_selected || !(i->defvalue))
				{
					n++;
					i=(struct html_input*)vector_get_next(&vi);
					continue;
				}
			break;

			case HTML_INPUT_HIDDEN:
				field_string="";
			break;

			case HTML_INPUT_TEXT:
			case HTML_INPUT_PASSWORD:
			case HTML_INPUT_FILE:
				field_string=gwrap_get_string(wc,WORD_ID(hd,n+1));
				if (field_string == WEBC_NULL)
				{
					field_string="";
				}
			break;
		}

		//generate each sub_header and check for errors.
		//if this input vector has TYPE=FILE, then allocated
		//space for the filename in the webc_multipart_form_unit
		//structure so we can get it later without reprocessing
		//the input vectors.
		sub_part->header=WEBC_NULL;
		sl+=webc_generate_multipart_sub_header(i,field_string,sub_part);
		if(sl<1)
		{
			return (WEBC_NULL);
		}
		sub_part->header = (WEBC_CHAR *) WEBC_MALLOC((sl+1) * sizeof(WEBC_CHAR));
		if(!sub_part->header)
		{
			return (WEBC_NULL);
		}
		if (sub_part->is_file)
		{
			sub_part->filename = (WEBC_CHAR *) WEBC_MALLOC((tc_strlen(field_string)+1) * sizeof(WEBC_CHAR),__FILE__,__LINE__);
			if(!sub_part->filename)
			{
				return (WEBC_NULL);
			}
		}
		else
		{
			sub_part->filename=WEBC_NULL;
		}
		webc_generate_multipart_sub_header(i,field_string,sub_part);
		if(tc_strlen(sub_part->header)!=sl)
		{
			return (WEBC_NULL);
		}

		//enqueue the subpart header, then free the
		//webc_multipart_form_unit strings
		EnqueueObject(pPartQueue, (WEBC_PFBYTE) sub_part);
		qsize++;
		sub_part = (struct webc_multipart_form_unit *)WEBC_MALLOC(
				sizeof(struct webc_multipart_form_unit),__FILE__,__LINE__);
		n++;
		i=(struct html_input*)vector_get_next(&vi);
	}


	//parse pPartQueue to create a delimiter that does not occur in
	//the packet.

	n=0;
 	webc_init_delim(dc,(int)&n);
	while (n<qsize)
	{
		sub_part = (struct webc_multipart_form_unit *) DequeueObject(pPartQueue);
		data_size+=webc_create_delim(dc,sub_part->header,TYPESTRING);
		if (sub_part->is_file)
		{
			data_size+=webc_create_delim(dc,sub_part->filename,TYPEFILE);
		}
		EnqueueObject(pPartQueue,(WEBC_PFBYTE) sub_part);
		n++;
	}
	n=tc_strlen(dc->delim);

	//what follows is a hack. The data should not be sent only as one packet
	//there should be some mechanism to sent the data to tcp/ip as needed.
	//Before we can do that, we need to restructure the main urlcycle.
	// This, however, will work for relatively small files.

	//calculate the size of the data between the first and last
	//delimiter included.
	n=(data_size - header_size) + ((qsize*(30+tc_strlen(dc->delim))) +4);

	//calculate the number of characters in all occurences of the delimiter
	//in the packet, including the \r\n that follow it
	sl=(qsize+1)*(tc_strlen(dc->delim)+2);
	//calculate the number space needed for the 30 '-' that lead up to the
	//delimiter.
	sl+=qsize*30;
	//add space for the "\r\n" that follow every sub_header
	sl+=(qsize-1)*tc_strlen("\r\n");
	//add the two '-' that mark the last delimiter
	sl+=2;
	//add the 'Content-Length' line
	sl+=tc_strlen("Content-Length: \r\n");
	//add the actual length
	tc_itoa(n,psize,10);
	sl+=tc_strlen(psize);
	sl+=tc_strlen("\r\n\r\n");
	if(sl+data_size+1>WEBC_MAX_PACKET_SIZE)
	{
		return (WEBC_NULL);
	}
	packet_data = (char *) WEBC_MALLOC(data_size+sl+1,__FILE__,__LINE__);


	sub_part = (struct webc_multipart_form_unit *) DequeueObject(pPartQueue);
	tc_strcpy(packet_data,sub_part->header);
	tc_strcat(packet_data,dc->delim);
	tc_strcat(packet_data,"\r\n");
	tc_strcat(packet_data,"Content-Length: ");
	tc_strcat(packet_data,psize);
	tc_strcat(packet_data,"\r\n\r\n");

	WEBC_FREE(sub_part->header,__FILE__,__LINE__);
	if(sub_part->filename)
	{
		WEBC_FREE(sub_part->filename,__FILE__,__LINE__);
	}
	WEBC_FREE(sub_part,__FILE__,__LINE__);

	n=0;
	while (n<qsize-1)
	{
		tc_strcat(packet_data,"------------------------------");
		tc_strcat(packet_data,dc->delim);
		tc_strcat(packet_data,"\r\n");
		sub_part = (struct webc_multipart_form_unit *) DequeueObject(pPartQueue);
		tc_strcat(packet_data,sub_part->header);
		if(sub_part->is_file)
		{
			webc_add_file_to_packet(packet_data,sub_part->filename);
		}
		tc_strcat(packet_data,"\r\n");
		WEBC_FREE(sub_part->header,__FILE__,__LINE__);
		if(sub_part->filename)
		{
			WEBC_FREE(sub_part->filename,__FILE__,__LINE__);
		}
		WEBC_FREE(sub_part,__FILE__,__LINE__);
		n++;
	}

	//add the last delimiter and the ending "--\r\n"
	tc_strcat(packet_data,"------------------------------");
	tc_strcat(packet_data,dc->delim);
	tc_strcat(packet_data,"--\r\n");

	return (packet_data);
}

int HTMLForm::AssemblePostQuery(WEBC_CHAR *query, WEBC_CHAR *dest)
{
webc_ctx *wc = mpDocument->GetWebcContext();
struct url_descriptor action_descriptor;
int lenpos;
int sl=0;

	if(!mAction)
	{
		return(-1);
	}

	sl+=5;
	if(dest)
	{
		tc_strcpy(dest,"POST ");
	}

	webc_get_url_descriptor(mAction, &action_descriptor, WEBC_BASE_URL(wc));

	sl+=tc_strlen(action_descriptor.path);
	if(dest)
	{
		tc_strcat(dest,action_descriptor.path);
	}

	sl+=9;
	sl+=35;
	if(dest)
	{
		tc_strcat(dest," HTTP/1.0");
		tc_strcat(dest,"\r\nAccept: text/*;\r\nContent-length: ");
	}

	sl+=8;
	sl+=tc_strlen(query);
	sl+=2;
	sl+=52;
	if(dest)
	{
		lenpos=tc_strlen(dest);
		tc_itoa(tc_strlen(query),&(dest[lenpos]),10);
		tc_strcat(dest,"\r\nContent-Type:application/x-www-form-urlencoded");
		tc_strcat(dest,"\r\n\r\n");
		tc_strcat(dest,query);
		tc_strcat(dest,"\r\n");
	}

	return(sl);
}
#endif




int HTMLForm::AssembleQueryString(WEBC_CHAR *dest, HTMLElement *pSubmit, int x, int y)
{
int firstField = 1;
HTMLElement *pField;
HTMLInput *pInput;
vector_iterator vi;
int length = 0;
WEBC_CHAR *field_name, *field_value;
HTMLFormFieldEncoder encode(AcceptCharset(), 0);

	if(dest)
	{
		dest[0]='\0';
	}

	pField = (HTMLElement *) ObjectListGetFirst(&mFieldVector, &vi);
	while (pField)
	{
		pInput = (HTMLInput *) pField;
		field_name = 0;
		field_value = 0;

		switch (pField->Type())
		{
			case HTML_EDITBOX_ELEMENT:
			case HTML_SELECT_ELEMENT:
			case HTML_EDIT_STR_ELEMENT:
			case HTML_HIDDEN_INPUT_ELEMENT:
			case HTML_INPUT_PASSWORD_ELEMENT:
				field_name = pInput->Name();
				field_value = pInput->Value();
				break;

			case HTML_RADIO_BUTTON_ELEMENT:
			case HTML_CHECKBOX_ELEMENT:
				field_name = pInput->Name();
				field_value = pInput->Checked()? pInput->Value() : 0;
				// only send valid checked items, so null out unchecked items
				if (!field_value)
				{
					field_name = 0;
				}
				break;

			default:
				// no special handling for other types
				break;
		}

		if (pField == pSubmit)
		{
			switch (pField->Type())
			{
				case HTML_IMAGE_ELEMENT:
					WEBC_CHAR numbuf[30];

					if (!firstField)
					{
						length++;
						if (dest)
						{
							webc_c_strcat(dest, "&");
						}
					}

					field_name = ((HTMLImage *) pField)->Name();

					if (field_name)
					{
						encode.SetInput(field_name);
						length += encode.WriteTo(dest? &(dest[webc_strlen(dest)]) : 0);

						length += 3;
						if (dest)
						{
							webc_c_strcat(dest, ".x=");
						}
					}
					else
					{
						length += 2;
						if (dest)
						{
							webc_c_strcat(dest, "x=");
						}
					}

					webc_print_dec(numbuf, x, 1, 30);

					length += webc_strlen(numbuf);
					length++;
					if (dest)
					{
						webc_strcat(dest, numbuf);
						webc_c_strcat(dest, "&");
					}

					if (field_name)
					{
						length += encode.WriteTo(dest? &(dest[webc_strlen(dest)]) : 0);

						length += 3;
						if (dest)
						{
							webc_c_strcat(dest, ".y=");
						}
					}
					else
					{
						length += 2;
						if (dest)
						{
							webc_c_strcat(dest, "y=");
						}
					}

					webc_print_dec(numbuf, y, 1, 30);

					length += webc_strlen(numbuf);

					if (dest)
					{
						webc_strcat(dest, numbuf);
					}

					field_name = 0;
					break;

#if	(WEBC_INPUT_BUTTON_ELEMENT_FIX)
				case HTML_INPUT_BUTTON_ELEMENT:
#endif
				case HTML_BUTTON_ELEMENT:
					field_name = ((HTMLInputSubmit *) pField)->Name();
					field_value = ((HTMLInputSubmit *) pField)->Value();
					break;

				default:
					// no special handling for other types
					break;

			}
		}

		if (field_name && *field_name) // && field_value && *field_value)
		{
			if (!firstField)
			{
				length++;
				if (dest)
				{
					webc_c_strcat(dest, "&");
				}
			}

			encode.SetInput(field_name);
			length += encode.WriteTo(dest? &(dest[webc_strlen(dest)]) : 0);
			length++;

			if (dest)
			{
				webc_c_strcat(dest, "=");
			}

			if (field_value && *field_value)
			{
				encode.SetInput(field_value);
				length += encode.WriteTo(dest? &(dest[webc_strlen(dest)]) : 0);
			}

		}

		firstField = 0;
		pField = (HTMLElement *) ObjectListGetNext(&vi);
	}

	return (length);
}

void HTMLForm::SetAcceptCharset(const WEBC_CHAR *value)
{
	WEBC_CHAR *next;

	// value is a comma and/or space separated list of charset names;
	//  go through this list until we find one we recognize
	while (value)
	{
		while (IS_WHITESPACE(value[0]))
		{
			value++;
		}

		mAcceptCharset = webc_charset_lookup(value);
		if (mAcceptCharset != WEBC_CHARSET_UNKNOWN)
		{
			break;
		}

		next = (WEBC_CHAR *) webc_c_strstr(value, ",");

		if (!next)
		{
			next = (WEBC_CHAR *) webc_c_strstr(value, " ");
		}

		value = next;
	}
}

WebcCharset HTMLForm::AcceptCharset(void)
{
	if (mAcceptCharset == WEBC_CHARSET_UNKNOWN && GetDocument())
	{
		return (GetDocument()->GetCharset());
	}

	return (mAcceptCharset);
}


/*****************************************************************************/
// HTMLFormFieldEncoder methods
/*****************************************************************************/

HTMLFormFieldEncoder::HTMLFormFieldEncoder (WebcCharset acceptCharset, long initialBufferSize)
{
	mpInput = 0;
	mAcceptCharset = acceptCharset;
	miBufferSize = initialBufferSize;
	if (miBufferSize > 0)
	{
		mpBuffer = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * miBufferSize);
	}
	else
	{
		mpBuffer = 0;
	}
}

HTMLFormFieldEncoder::~HTMLFormFieldEncoder (void)
{
	if (mpBuffer)
	{
		WEBC_FREE(mpBuffer);
	}
}

void HTMLFormFieldEncoder::SetInput (WEBC_CHAR *str)
{
	mpInput = str;

	if (mAcceptCharset != WEBC_DEFAULT_CHARSET)
	{
		// encode the string in the target charset
		if (str)
		{
			long size = webc_encode_string(0, str, 0, mAcceptCharset);
			if (miBufferSize < size + 1)
			{
				miBufferSize = (size + 0x10) & ~0xf;
				if (mpBuffer)
				{
					WEBC_FREE(mpBuffer);
				}
				mpBuffer = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * miBufferSize);
			}

			if (!mpBuffer)
			{
				miBufferSize = 0;
				return;
			}

			webc_encode_string(mpBuffer, str, 0, mAcceptCharset);
			mpBuffer[size] = (WEBC_CHAR) 0;
		}
	}
}

long HTMLFormFieldEncoder::GetLength (void)
{
	if ((mAcceptCharset == WEBC_DEFAULT_CHARSET) && mpInput)
	{
		return (webc_escape_form_fields(0, mpInput));
	}
	else if (mpBuffer)
	{
		return (webc_escape_form_fields(0, mpBuffer));
	}

	return (0);
}

long HTMLFormFieldEncoder::WriteTo (WEBC_CHAR *buffer)
{
	if (buffer)
	{
		if ((mAcceptCharset == WEBC_DEFAULT_CHARSET) && mpInput)
		{
			return (webc_escape_form_fields(buffer, mpInput));
		}
		else if (mpBuffer)
		{
			return (webc_escape_form_fields(buffer, mpBuffer));
		}
	}

	return (GetLength());
}
