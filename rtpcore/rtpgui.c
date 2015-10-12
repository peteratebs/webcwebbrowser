/*
 | RTPGUI.C - Runtime Platform GUI Implementation
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBS - RT-Platform
 |
 |  $Author: pvanoudenaren $
 |  $Date: 2009/07817 15:29:00 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBS Inc. , 2009
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |   This module makes use of the RTPlatform ported modules to
 |   implement higher level file system functions.      __st__
 |
 | Functions implemented:
 |
 |
 |
 | Note:
 |
 |
 |
 |
*/


/************************************************************************
* Headers
************************************************************************/
#include "rtpprint.h"
#include "rtpterm.h"
#include "rtpmem.h"
#include "rtpstr.h"
#include "rtpgui.h"
#include "rtpstdup.h"
#include "rtpscnv.h"
/************************************************************************
* Defines
************************************************************************/
#define MENU_LEFT_INDENT "  "

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Structs
************************************************************************/
struct rtp_gui_list_item {
	char *item;
	void *itemid;
	int indentlevel;
	int isgreyed;
	int style;
	int index_number;
	struct rtp_gui_list_item *pnext;
};
struct rtp_gui_list {
	void *pguictx;
	int style;
	char *title;
	char *subtitle;
	char *prompt;
	int itemlistcount;
	struct rtp_gui_list_item *itemlist;
};

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
static int _rtp_gui_prompt_int(char *indent, char *prompt);
static struct rtp_gui_list_item *_rtp_gui_list_item_struct(struct rtp_gui_list *plist, void * itemid);
static struct rtp_gui_list_item *_rtp_gui_list_item_by_index(struct rtp_gui_list *plist, int index);
static int rtp_gui_list_display(struct rtp_gui_list * plist, int start_index, int depth);

/************************************************************************
* Function Bodies
************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/** @memo   Create a list or menu object.

    @doc    Creates a list or menu object and returns a handle that may be passed
    to other functions that add to, access and execute user dialogs.
    storage location.

    @precondition None

    @return A non null void * handle on success. 0 otherwise.
*/
void *rtp_gui_list_init(
void *pguictx,				/** Hook for GUI context (parent window handle). Zero for generic implementation. */
int  style,					/**	<br>
     RTP_LIST_STYLE_MENU     - Format as a menu.<br>
     RTP_LIST_STYLE_HELP	 - Format as a help message<br>
     RTP_LIST_STYLE_LISTBOX  - Format as a list box.<br>
	 RTP_LIST_STYLE_NOFORMAT - Do not format greyed expandable etc.<br>
*/
char *title,				/** Title to display */
char *subtitle,				/** Sub-title to display */
char *prompt				/** Prompt to use when requesting input */
)
{
struct rtp_gui_list *plist;

	plist = (struct rtp_gui_list *) rtp_malloc(sizeof(*plist), 0);
	if (plist)
	{
		rtp_memset(plist, 0, sizeof(*plist));
		plist->pguictx = pguictx;
        plist->style = style;
        plist->title = title;
        plist->subtitle = subtitle;
        plist->prompt = prompt;
	}
	return(void *) plist;
}


/** @memo   Release all memory used by a list or menu object.

    @doc    Releases all memory used by the list or menu object. Must be called to release memory
	allocated while the object was created and populated.

    @precondition rtp_gui_list_init must be called first

    @return Nothing
*/
void rtp_gui_list_destroy(void *vplist /** Handle returned from rtp_gui_list_init() */)
{
	struct rtp_gui_list *plist;
	plist = (struct rtp_gui_list *) vplist;
	{
	struct rtp_gui_list_item *pnext, *pthis;
		pthis = plist->itemlist;
		while (pthis)
		{
			if (pthis->item)
				rtp_free(pthis->item);
			pnext = pthis->pnext;
			rtp_free(pthis);
			pthis = pnext;
		}
	}
	rtp_free(plist);
}

/** @memo   Add an text entry and a void * value a list or menu object.

    @doc    Adds text and to a list or object. Also provides a void * value to attach to the item that will be returned
    when the item is selected. <br>
    If rtp_gui_list_execute() is called itemvalue will be returned. <br>
    If rtp_gui_list_execute_handle() is called the handle to the item is returned.

    @precondition rtp_gui_list_init must be called first.

    @return A handle to the menu or list entry object.
*/
void *rtp_gui_list_add_item(void *vplist, /** Handle returned from rtp_gui_list_init(). */
char *item, 							  /** Text to display for the choice. */
void *itemvalue,						  /** void *value to return when the item is selected. */
int indentlevel, 						  /** Spaces to indent when text is displayed. */
int isgreyed, 							  /** 1 if the item may not be selected. */
int style								  /** Additional style attributes. <br>
RTP_LIST_ITEM_STYLE_NOBREAK - Display this and the next list item side by side. <br>
RTP_LIST_ITEM_STYLE_SUBMENU - Itemvalue is a menu or list handle returned from rtp_gui_list_init(). <br>
RTP_LIST_ITEM_STYLE_COLLAPSED - Don't display the submenu/sublist.<br>
RTP_LIST_ITEM_STYLE_EXPANDED  - Expand and display the submenu/sublist. <br>
*/
)
{
	struct rtp_gui_list *plist;
	struct rtp_gui_list_item *newitem;

	plist = (struct rtp_gui_list *) vplist;

	newitem = (struct rtp_gui_list_item *) rtp_malloc(sizeof(*newitem), 0);
	if (!newitem)
		return(0);
	rtp_memset(newitem, 0, sizeof(*newitem));
	newitem->item = rtp_strdup(item);
	if (!newitem->item)
	{
		rtp_free(newitem);
		return(0);
	}
	newitem->itemid = itemvalue;
	newitem->indentlevel = indentlevel;
	newitem->isgreyed = isgreyed;
	newitem->style = style;
	{
	struct rtp_gui_list_item *pthis;
		pthis = plist->itemlist;
		if (!pthis)
			plist->itemlist = newitem;
		else
		{
			while (pthis->pnext)
			{
				pthis = pthis->pnext;
			}
			pthis->pnext = newitem;
		}
	}
	plist->itemlistcount += 1;
	return((void *) newitem);
}
/** @memo   Add an text entry and an integer value to a list or menu object.

    @doc    Adds text to a list or object. Also provides an integer value to attach to the item that will be returned
    when the item is selected. <br>
    If rtp_gui_list_execute_int() is called itemvalue will be returned. <br>
    If rtp_gui_list_execute_handle() is called the handle to the item is returned.

    @precondition rtp_gui_list_init must be called first.

    @return A handle to the menu or list entry object.
*/
void *rtp_gui_list_add_int_item(void *vplist, /** Handle returned from rtp_gui_list_init() */
char *item, 							  /** Text to display for the choice */
int itemid,   						      /** Integer value to return if the item is selected */
int indentlevel, 						  /** Spaces to indent when text is displayed */
int isgreyed, 							  /** 1 if the item may not be selected */
int style								  /** see rtp_gui_list_add_item() */
)
{
	return(rtp_gui_list_add_item(vplist, item, (void *) itemid, indentlevel, isgreyed, style));
}

static struct rtp_gui_list_item *_rtp_gui_list_item_struct(struct rtp_gui_list *plist, void * itemid)
{
	struct rtp_gui_list_item *pthis;
	pthis = plist->itemlist;
	while (pthis)
	{
		if (pthis->itemid == itemid)
		{
			return(pthis);
		}
		pthis = pthis->pnext;
	}
	return(0);
}


static struct rtp_gui_list_item *_rtp_gui_list_item_by_index(struct rtp_gui_list *plist, int index)
{
struct rtp_gui_list_item *rval = 0;
struct rtp_gui_list_item *pitem;
	pitem = plist->itemlist;
	while (pitem && !rval)
	{
		if (!pitem->isgreyed)
		{
			if (pitem->index_number == index)
				rval = pitem;
			else if (pitem->style & RTP_LIST_ITEM_STYLE_EXPANDED )
			{
				rval = _rtp_gui_list_item_by_index((struct rtp_gui_list *)pitem->itemid, index);
			}
		}
		pitem = pitem->pnext;
	}
	return (rval);
}



/** @memo   Retrieve the count of entries in a list or menu object.

    @doc    Returns the nuber of entries that have been added to the list or menu by calls to
	rtp_gui_list_add_item() and rtp_gui_list_add_int_item().

    @precondition rtp_gui_list_init must be called first.

    @return Count of entries in the list or menu.
*/
int rtp_gui_list_size(void *vplist /** Handle returned from rtp_gui_list_init() */)
{
	struct rtp_gui_list *plist;
	plist = (struct rtp_gui_list *) vplist;
	return(plist->itemlistcount);
}

char *rtp_gui_list_item_value(void *vplist, void *itemid,int strdup_item)
{
	struct rtp_gui_list_item *pthis;

	pthis = _rtp_gui_list_item_struct((struct rtp_gui_list *)vplist, itemid);

	if (pthis)
	{
		if (strdup_item)
			return(rtp_strdup(pthis->item));
		else
			return(pthis->item);
	}
	return(0);
}

/** @memo   Check if an item is already contained in an item contained in a list or menu object.

    @doc    Checks if the string exactly matches a string that was passed in the item argument
    to one of the forms of rtp_gui_list_add_item(). The item argument is the text that is displayed
    when the list is displayed. This function may be used to identify and discard
    duplicate information like the device identifier information in rebroadcasts of SSDP advertisements.

    @precondition rtp_gui_list_init must be called first.

    @return 1 if the an exact match for item is already in the list. 0 otherwise.
*/
int rtp_gui_list_check_duplicate_item(
void *vplist, /** Handle returned from rtp_gui_list_init() */
char  *item   /** String to check for a match */)
{
	struct rtp_gui_list *plist;
	struct rtp_gui_list_item *pitem;

	plist = (struct rtp_gui_list *) vplist;
	pitem = plist->itemlist;
	while (pitem)
	{
		if (pitem->item && (rtp_strcmp(item, pitem->item) == 0))
			return(1);
		pitem = pitem->pnext;
	}
	return(0);
}


/** @memo   Display the content contained in a list or menu object. And retrieve user input.

    @doc    Displays the items added to the object by calls to rtp_gui_list_add_item() in its various forms
	and asks the user to select a choice. <br>
	rtp_gui_list_execute() uses the style, indent and isgreyed attribute to format the lists and to process
	the input with sub-menu support. If all options are greyed, like in a help screen or a status screen,
	a "Press Return" prompt is provided.<br>
	The generic version of rtp_gui_list_execute() is implemented using Rtplatfrom terminal IO primitives.
	If a GUI is available this function can be re-implemented in the target specific subdirectory using the GUI's native API.

    @precondition rtp_gui_list_init must be called first, rtp_gui_list_add_item() or rtp_gui_list_add_int_item() must
    have been called one or more times

    @return A handle to the item in the list. <br> See also rtp_gui_list_execute_int(), and rtp_gui_list_execute_void().
*/
void * rtp_gui_list_execute_item(void *vplist /** Handle returned from rtp_gui_list_init() */)
{
	struct rtp_gui_list *plist;
	struct rtp_gui_list_item *pitem;
	int numselections,selection;

	plist = (struct rtp_gui_list *) vplist;


	for (;;)
	{
    	rtp_printf("%s %s \n", MENU_LEFT_INDENT, plist->title);
    	rtp_printf("%s %s \n", MENU_LEFT_INDENT, plist->subtitle);

    	numselections = rtp_gui_list_display(plist, 0, 0);
    	if (numselections)
    	{
    		selection = _rtp_gui_prompt_int(MENU_LEFT_INDENT, plist->prompt);
    		{
    			pitem = _rtp_gui_list_item_by_index(plist, selection);
    			{
    				if (pitem->style & RTP_LIST_ITEM_STYLE_SUBMENU)
    				{  /* Toggle the collapsed versu expanded state and redraw */
    					if (pitem->style & RTP_LIST_ITEM_STYLE_EXPANDED)
    						pitem->style &= ~RTP_LIST_ITEM_STYLE_EXPANDED;
						else
    						pitem->style |= RTP_LIST_ITEM_STYLE_EXPANDED;
    				}
    				else
    					return((void *)pitem);
    			}
    		}
    	}
    	else
    	{
    		char inbuff[32];
    		rtp_gui_prompt_text(MENU_LEFT_INDENT, "Press Return..", inbuff);
    		rtp_printf("\n");
    		return(0);
    	}
	}
}

static int rtp_gui_list_display(struct rtp_gui_list * plist, int start_index, int depth)
{
	struct rtp_gui_list_item *pitem;
	int numselections, numliveselections,i, displayindex;

/*
First chracter
' ' - child
+   - Expanded submenu
-   - Closed submenu
x   - Greyed submenu
X   - Greyed child
*/
	displayindex = start_index;
	{
		pitem = plist->itemlist;
		numselections =  numliveselections = 0;
		while (pitem)
		{
		char first_char;

			if (pitem->isgreyed)
			{
				if (plist->style & RTP_LIST_STYLE_NOFORMAT)
					first_char = ' ';
				else if (pitem->style & RTP_LIST_ITEM_STYLE_SUBMENU)
					first_char = 'X';
				else
					first_char = 'x';
				pitem->index_number = -1;
				rtp_printf("%c     ",first_char);
			}
			else
			{
				if (pitem->style & RTP_LIST_ITEM_STYLE_SUBMENU)
				{
					if (pitem->style & RTP_LIST_ITEM_STYLE_EXPANDED)
						first_char = '-';
					else
						first_char = '+';
				}
				else
					first_char = ' ';
				rtp_printf("%c(%3d)",first_char, displayindex);
				pitem->index_number = displayindex;
				displayindex += 1;
			}
			rtp_printf("%s ",MENU_LEFT_INDENT);
			for (i = 0; i < pitem->indentlevel+depth;i++)
				rtp_printf(" ");
			if (pitem->style & RTP_LIST_ITEM_STYLE_NOBREAK)
				rtp_printf("%.60s", pitem->item);
			else
				rtp_printf("%.60s \n", pitem->item);
			if (pitem->style & RTP_LIST_ITEM_STYLE_EXPANDED )
			{
				displayindex += rtp_gui_list_display((struct rtp_gui_list *)pitem->itemid, displayindex, depth +1);
			}
			pitem = pitem->pnext;
		}
	}
	return (displayindex);
}

/** @memo   Execute a list and return the item value of the selected item.

    @doc    Displays the items in the list (see rtp_gui_list_execute_element()) and returns the itemvalue assigned
    by rtp_gui_list_add_item(). Call rtp_gui_list_execute_int() instead to process the menu if integer values were assigned
    by rtp_gui_list_add_int_item().

    @precondition rtp_gui_list_init must be called first, rtp_gui_list_add_item() or rtp_gui_list_add_int_item() must
    have been called one or more times

    @return A void pointer to the value passed in rtp_gui_list_add_item() or zero if none selected. (all greyed).
*/
void *rtp_gui_list_execute_void(void *plist/** Handle returned from rtp_gui_list_init() */)
{
	struct rtp_gui_list_item *pitem;
	pitem = (struct rtp_gui_list_item *) rtp_gui_list_execute_item(plist);
	if (pitem)
		return(pitem->itemid);
	else
		return(0);
}
/** @memo   Execute a list and return the item id of the selected item.

    @doc    Displays the items in the list (see rtp_gui_list_execute_element()) and returns the itemid assigned
    by rtp_gui_list_add_int_item(). Call rtp_gui_list_execute_void() instead to process the menu if void * values were assigned
    by rtp_gui_list_add_item().	<br>

    @precondition rtp_gui_list_init must be called first, rtp_gui_list_add_item() or rtp_gui_list_add_int_item() must
    have been called one or more times

    @return An item id value passed in rtp_gui_list_add_int_item() or zero if none selected. (all greyed).
*/
int  rtp_gui_list_execute_int(void *plist/** Handle returned from rtp_gui_list_init() */)
{
	return((int)rtp_gui_list_execute_void(plist));
}
static int _rtp_gui_prompt_int(char *indent, char *prompt)
{
char inbuffer[32];
int rval = -1;

	while (rval == -1)
	{
		rtp_gui_prompt_text(indent, prompt, inbuffer);
		if (inbuffer[0]  >= '0' && inbuffer[0] <= '9')
			rval = rtp_atoi(inbuffer);
	}
	return(rval);
}
/** @memo   Display a status line.

    @doc    Displays the status line in the UI's status display area. The generic version prints the status to the console.

    @return Nothing
*/
void rtp_gui_conout(char *status /** The text to display */)
{
    	rtp_printf("%s\n", status);
}
/** @memo   Display two status items on the same line.

    @doc    Displays the status lines in the UI's status display area. The generic version prints the two inputs
    side by side on the console.

    @return Nothing
*/
void rtp_gui_conout_two(char *alert1 /** First text to display */,char *alert2 /** Second text to display */)
{
   	rtp_printf("%s %s\n", alert1, alert2);
}


/** @memo   Prompt the user for input.

    @doc    Displays a prompt and retrieve keyboard input from the user.

    @return Nothing
*/
void rtp_gui_prompt_text(char *indent /** String of spaces used to indent before prompt. */,
char *prompt,/** Prompt to display */
char *inbuffer/** Input goes here. Not length checked so use large buffers. */)
{
	rtp_printf("%s %s :", indent, prompt);
	rtp_term_gets(inbuffer);
	rtp_printf("\n");
}
