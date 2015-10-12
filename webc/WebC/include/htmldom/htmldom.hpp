#ifndef __HTML_DOM_INCLUDED__
#define __HTML_DOM_INCLUDED__

#include "htmlsdk.h"
#include "textunit.hpp"
#include "css.h"
#include "webc.h"
#include "webcbmp.hpp"
#include "html.h"
#if (WEBC_SUPPORT_UNICODE)
 #include "webcutf.h"
 #include "wcconst.hpp"
#endif
#include "webfonts.hpp"
#include "enum.hpp"

/*****************************************************************************/

int HtmlAlignToTUTextAlign(int htmlAlign);
int HtmlAlignToTUVAlign(int htmlAlign);

/*****************************************************************************/
// HTML DOM typedefs/enums/structs 
//  (note: first letter of all simple (non-enum/union/struct/class) types are 
//  lower case; all HTML DOM types begin with HTML)
/*****************************************************************************/

typedef WEBC_BOOL (*MatchElementFn)(HTMLElement *pElement, void *ctx);


/*****************************************************************************/
// Externally Defined classes
/*****************************************************************************/

class LoadJob;
class HTMLDisplayList;
struct HTMLDisplayNode;


/*****************************************************************************/
// HTML DOM classes
/*****************************************************************************/

class HTMLBrowser;
class HTMLDocument;
class HTMLElement;
class HTMLForm;
class HTMLEventQueue;
class HTMLElementClass;

/*****************************************************************************/
// HTML DOM class definitions
/*****************************************************************************/

extern WEBC_READONLY WEBC_BOOL TextUnitIsContainer[NUM_HTML_ELEMENT_TYPES];
extern WEBC_READONLY HTMLElementType TagToHTMLElementType[HTML_NUM_TAGS];
extern HTMLEventQueue *gpEventQueue;
extern HTMLBrowser *gpFocusBrowser;

/*************************************************************************
* class HTMLEventHandler - interface
* 
*  Used to override default HTMLElement event handlers.  
*
*
*************************************************************************/

class HTMLEventHandler
{
public:
	virtual HTMLEventStatus Event(HTMLElement *pElement, HTMLEvent *pEvent, WEBC_CHAR *pParam = 0) = 0;
	virtual WEBC_CHAR *Name(void) = 0;
};

/*************************************************************************
* Meta name callback API
* 
*  Used to provide custom document-level processing through META tags
*
*
*************************************************************************/

typedef int (*MetaCallbackFn)(const WEBC_CHAR *pContent, HTMLDocument *pDocument);


/*********************************************************/
// support classes for HTMLElement

class HTMLElementClassEnumerator : public Enumerator
{
public:
	HTMLElementClassEnumerator (vector *classVector);
	virtual unsigned char * Next(void);
	virtual void Dispose (void);

	void Init (vector *classVector);
	
	static HTMLElementClassEnumerator *Alloc(vector *classVector, char *file, long line);
	static void CleanUp (void);
	
	HTMLElementClassEnumerator *mpPoolNext;
	
private:
	vector_iterator mIterator;
	const WEBC_CHAR *mpNext;
	
	static const int MAX_POOL_SIZE;
	static int miPoolSize;
	static HTMLElementClassEnumerator *mpPool;
};

class WebBrowserWindow;
class HtmlDocumentBuilder;


// HTMLElementClass - structure for class attribute of HTML elements
//  this is a seperate class because many elements can be in a single class,
//  so of course we would want to centralize class info storage
// hclass.cpp
class HTMLElementClass 
{
public:
	HTMLElementClass(WEBC_CHAR *pName, HTMLTagType tag = HTML_UNKNOWN_TAG);
	~HTMLElementClass(void);
	
	HTMLTagType Tag();
	WEBC_CHAR *Name();
	
	void   AddElement(HTMLElement *pElement);
	int    RemoveElement(HTMLElement *pElement);
	HTMLElement *GetElements(vector_iterator *vi);
	HTMLElement *NextElement(vector_iterator *vi);
	
protected:
	HTMLTagType mTag;
	WEBC_CHAR *mpName;
	vector mpElements[1]; // vector of struct HTMLElementPtr
};

struct EventQueueElement
{
	HTMLEvent    event;
	HTMLElement* element;
};

/**
 @memo This class queues events as they arrive
 @see HTMLElement::Event()
 */
class HTMLEventQueue
{
public:
	/**
	 @memo The default constructor
	 @see
	 */
	HTMLEventQueue();

	/**
	 @memo The Destructor
	 */
	virtual ~HTMLEventQueue();

	/**
	 @memo add an Event to the queue associated with element pElem
	 @param pEvent - the event to enqueue
	 @param pElem - the element to enqueue
	 @WEBC_UINT8 0 on success -1 on failure
	 @see Dequeue
	 */
	int Enqueue(HTMLEvent *pEvent, HTMLElement *pElem);

	/**
	 @memo remove the first element from the queue and store the event and 
	       element associated with it.
	 @param pEvent - a pointer to a pointer of the event to be used
	 @param pElem - a pointer to a pointer of the element to execute the event on.
	 @return -1 if there is nothing to dequeue 0 otherwise
	 @see Enqueue
	 */
	int Dequeue(HTMLEvent **pEvent, HTMLElement **pElem);

	/**
	 @memo This method searched through the queue and removes and event that has
	       pElem associated with it
     @param the key on which to remove a node from the queue
	 @return void
	 @see
	 */
	void Purge(HTMLElement *pElem);	

	/**
	 @memo This method searches through the queue and replaces a matching 
	       entry with the given event; calls enqueue if no match found
	 @param pEvent - the event to fold
	 @param pElem - the element to fold
	 @return void
	 @see
	 */
	void Fold(HTMLEvent *pEvent, HTMLElement *pElem);

private:
	void CopyNode(int start, int finish);

	WEBC_UINT8 mbHead;
	WEBC_UINT8 mbTail;	
	EventQueueElement mpQueue[WEBC_CFG_EVENT_QUEUE_SIZE];
};



/*****************************************************************************/
// HTML DOM Functions
/*****************************************************************************/

#ifdef __cplusplus

int               HTML_RegisterEventHandler (HTMLEventHandler *pHandler);
void              HTML_ResetEventHandlers (void);
void HTML_UnregisterEventHandler(HTMLEventHandler *pHandler);
HTMLEventHandler *HTML_FindEventHandler (WEBC_CHAR *pName, HTMLElement *pElem);
HTMLMetaStatus    HTML_ProcessMetaTag (const WEBC_CHAR *pName, const WEBC_CHAR *pContent, HTMLDocument *pDocument);

void HTML_GetElementManagerPosition  (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y);
void HTML_GetElementViewPortPosition (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y);
void HTML_GetElementDocumentPosition (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y);
void HTML_GetElementScreenPosition   (HTMLElement* elem, DISPLAY_INT* x, DISPLAY_INT* y);

//MA
int check_hash_entries(void);
#endif

#endif //__HTML_DOM_INCLUDED__
