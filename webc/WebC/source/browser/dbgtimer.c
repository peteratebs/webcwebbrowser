//
// DBGTIMER.C - Functions for performance timing
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

#include "dbgtimer.h"
#if (WEBC_SUPPORT_DEBUG_TIME)
#include "windows.h"
#include "largeint.h"
#include "webcmem.h"


/*****************************************************************************/
// Macros/Defines
/*****************************************************************************/

#define TIMENODE_FLAG_WRITTEN   0x01
#define ONE_THOUSAND            1000
#define ONE_MILLION             1000000
#define TEN_MILLION             10000000
#define ONE_HUNDRED_MILLION     100000000
#define ONE_BILLION             1000000000
#define SMALL_DELAY             75


/*****************************************************************************/
// Data structures
/*****************************************************************************/

typedef WEBC_UINT8 TIMENODEFLAGS;

struct TimeNode
{
	TIMENODEFLAGS flags;
	long          id;
	LARGE_INTEGER startTime;
	LARGE_INTEGER totalTime;
	LARGE_INTEGER overhead;
	long          count;
	struct TimeNode *pFirstChild, *pLastChild, *pParent, *pPrev, *pNext;
};

struct TimeNodeType
{
	char *name;
	int   enabled;
};


/*****************************************************************************/
// Local function prototypes
/*****************************************************************************/

static void LargeIntAdd(LARGE_INTEGER *a, LARGE_INTEGER *b);
static void LargeIntSub(LARGE_INTEGER *a, LARGE_INTEGER *b);
static struct TimeNode * _FindNode (struct TimeNode *pParentNode, long id);
static struct TimeNode * _CreateNode (struct TimeNode *pParentNode, long id);
static void _WriteTimeNode(FILE *fp, struct TimeNode *node, int *count, int depth);
static void _WriteLargeInteger(FILE *fp, LARGE_INTEGER *i, int withCommas);
static void _WriteLongInteger(FILE *fp, long i, int withCommas);
static void _DeleteNode(struct TimeNode *pNode);
static struct TimeNode *_RootNode (void);
static void _CalibrateTimers(void);
static int _calibrate_OneTimer (long id);
static int _calibrate_WithoutTimers(void);
static int _calibrate_WithTimers(void);
static void _TotalTime(struct TimeNode *pParentNode, long id, LARGE_INTEGER *time, long *count);


/*****************************************************************************/
// Global data declarations
/*****************************************************************************/

static LARGE_INTEGER gTimerOverhead, gChildOverhead;
static LARGE_INTEGER gTimerOverheadError, gChildOverheadError;
struct TimeNode *gpCurrentTimeNode = 0;

struct TimeNodeType gTimeNodeTypes [] = 
{
	{"main", 1},
	{"DisplayManager::Refresh", 1},
	{"WebGraphics::Bitmap", 1},
	{"WebGraphics::ColorToRGB", 1},
	{"WebGraphics::CreateStretchedBitmap", 1},
	{"WebGraphics::DarkColor", 1},
	{"WebGraphics::DestroyStretchedBitmap", 1},
	{"WebGraphics::EndBuffer", 1},
	{"WebGraphics::FontHeight", 1},
	{"WebGraphics::Frame3D", 1},
	{"WebGraphics::GetCharOffset", 1},
	{"WebGraphics::GetClip", 1},
	{"WebGraphics::GetRect", 1},
	{"WebGraphics::HLine", 1},
	{"WebGraphics::Invalidate", 1},
	{"WebGraphics::LightColor", 1},
	{"WebGraphics::Rectangle", 1},
	{"WebGraphics::RGBToColor", 1},
	{"WebGraphics::RGBToDarkColor", 1},
	{"WebGraphics::RGBToLightColor", 1},
	{"WebGraphics::SetClip", 1},
	{"WebGraphics::StartBuffer", 1},
	{"WebGraphics::StretchBitmap", 1},
	{"WebGraphics::Text", 1},
	{"WebGraphics::TextHeight", 1},
	{"WebGraphics::TextWidth", 1},
	{"WebGraphics::TileBitmap", 1},
	{"WebGraphics::VLine", 1},
	{"HTMLFlowDisplay::DrawThisOnly", 1},
	{"HTMLElementDisplay::DrawThisOnly", 1},
	{"HTMLInputButtonDisplay::DrawThisOnly", 1},
	{"HTMLTableDisplay::DrawThisOnly", 1},
	{"HTMLStringDisplay::Draw", 1},
	{"HTMLElement::GetStyleFromCSS", 1},
	{"HTMLElement::GetFont", 1},
	{"HTMLElement::GetColor", 1},
	{"HTMLElement::GetVisibility", 1},
	{"HTMLElement::GetTextDecoration", 1},
	{"HTMLElement::GetStyleFromParent", 1},
	{"HTMLFlowDisplay::DrawBackground", 1},
	{"HTMLFlowDisplay::DrawBorder", 1},
	{"HTMLContentBox::DrawBorder", 1},
	{"HTMLContentBox::GetBorderColor", 1},       
	{"HTMLContentBox::GetBorderStyle", 1},  
	{"HTMLContentBox::GetMarginWidth", 1},  
	{"HTMLContentBox::GetPaddingWidth", 1},  
	{"HTMLContentBox::GetBorderWidth", 1}, 
	{"Timer Calibration (ignore)", 1}, 
	{"HTMLElement::GetBackgroundColor", 1},
	{"HTMLFlowDisplay::Format", 0},
	{"HTMLFlowDisplay::AddUnitToLine", 0},
	{"HTMLTableDisplay::Format", 0},
	{"DisplayTUEnumerator::GetNextUnit", 1},
	{"HTMLElement::GetLeft", 1},
	{"HTMLElement::GetTop", 1},
	{"HTMLElement::GetWidth", 1},
	{"HTMLElement::GetHeight", 1},
	{"HTMLElement::GetChildWrap", 1},
	{"HTMLElement::GetUnitType", 1},
	{"HTMLElement::GetClear", 1},
	{"HTMLElement::GetFloat", 1},
	{"HTMLElement::GetPosition", 1},
	{"HTMLElement::GetChildTextAlign", 1},
	{"HTMLElement::GetVAlign", 1},
	{"HTMLElement::GetZIndex", 1},
	{"HTMLStringDisplay::SetElementStyle", 1},
	{"", 1},
};

#define NUM_TIME_NODE_TYPES (sizeof(gTimeNodeTypes) / sizeof(struct TimeNodeType))


/*****************************************************************************/
// Functions
/*****************************************************************************/

/******************************************************************************
* webc_DebugTimerStart - start a debug profiling timer
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void webc_DebugTimerStart(long id)
{
LARGE_INTEGER beginOverhead, elapsed;
struct TimeNode *pChildNode;

	// looking up the child node with the correct id may introduce error to our
	//  results, so try to counter this by keeping track of overhead time
	QueryPerformanceCounter(&beginOverhead);

	if (!gpCurrentTimeNode || id < 0 || id >= NUM_TIME_NODE_TYPES || !gTimeNodeTypes[id].enabled)
	{
		return;
	}
	
	// find the child node; if it does not exist, create it
	pChildNode = _FindNode (gpCurrentTimeNode, id);
	if (!pChildNode)
	{
		pChildNode = _CreateNode(gpCurrentTimeNode, id);
	}
	
	// increment visited count
	pChildNode->count++;
	
	// reset overhead for the child node
	pChildNode->overhead = gChildOverhead;
	
	// Add the overhead associated with a timer call to the current time node
	LargeIntAdd(&gpCurrentTimeNode->overhead, &gTimerOverhead);

	// account for the overhead associated with looking up the child node
	QueryPerformanceCounter(&elapsed);
	LargeIntSub(&elapsed, &beginOverhead);
	elapsed = ExtendedIntegerMultiply(elapsed, ONE_MILLION);
	LargeIntAdd(&gpCurrentTimeNode->overhead, &elapsed);

	// step into the child node and set its start time	
	gpCurrentTimeNode = pChildNode;
	QueryPerformanceCounter(&gpCurrentTimeNode->startTime);
}


/******************************************************************************
* webc_DebugTimerStop - stop the currently nested debug profiling timer
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void webc_DebugTimerStop(long id)
{
LARGE_INTEGER elapsed;

	QueryPerformanceCounter(&elapsed);

	if (!gpCurrentTimeNode || id < 0 || id >= NUM_TIME_NODE_TYPES || !gTimeNodeTypes[id].enabled)
	{
		return;
	}

	LargeIntSub(&elapsed, &gpCurrentTimeNode->startTime);
	elapsed = ExtendedIntegerMultiply(elapsed, ONE_MILLION);
	if (LargeIntegerGreaterThan(elapsed, gpCurrentTimeNode->overhead))
	{
		LargeIntSub(&elapsed, &gpCurrentTimeNode->overhead);
		LargeIntAdd(&gpCurrentTimeNode->totalTime, &elapsed);
	}
	
	if (gpCurrentTimeNode->pParent)
	{
		LargeIntAdd(&gpCurrentTimeNode->pParent->overhead, &gpCurrentTimeNode->overhead);
	}
	
	gpCurrentTimeNode = gpCurrentTimeNode->pParent;
}


/******************************************************************************
* webc_DebugTimerReport - write a performance report to an HTML file
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

int webc_DebugTimerReport(char *fileName)
{
FILE *fp;
int c = 0, greatest, n;
struct TimeNode *root = _RootNode();
struct TimeNode *child = root->pFirstChild;
LARGE_INTEGER totalTimes[NUM_TIME_NODE_TYPES];
long totalCalls[NUM_TIME_NODE_TYPES];

	root->totalTime.LowPart = 0;
	root->totalTime.HighPart = 0;

	while (child)
	{
		LargeIntAdd(&root->totalTime, &child->totalTime);
		child = child->pNext;
	}

	fp = fopen(fileName, "w");

	fprintf(fp, "<script src=\"tree.js\"></script>\n");
	fprintf(fp, "<link href=\"tree.css\" type=\"text/css\" rel=stylesheet>\n");
	
	_WriteTimeNode(fp, root, &c, 0);
	
	/* Write a report of the total running time for each function */
	for (c=0; c<NUM_TIME_NODE_TYPES; c++)
	{
		_TotalTime(root, c, &totalTimes[c], &totalCalls[c]);
	}

	fprintf(fp, "<BR><BR>Total running times:<BR><TABLE cellpadding=0 cellspacing=0>\n");
	
	n=0;
	while (1)
	{
		for (c=0; c<NUM_TIME_NODE_TYPES && LargeIntegerEqualToZero(totalTimes[c]); c++);
		if (c == NUM_TIME_NODE_TYPES)
			break;
		
		greatest = c++;	
		for (; c<NUM_TIME_NODE_TYPES; c++)
		{
			if (LargeIntegerGreaterThan(totalTimes[c], totalTimes[greatest]))
			{
				greatest = c;
			}
		}
		
		fprintf(fp, "<TR>"
		              "<TD>"
		                "<div class=line%d>"
		                  "<span class=nameClass>"
		                    "%s"
		                  "</span>"
		                  " - "
		                "</div>"
		              "</TD>"
		              "<TD>"
		                "<div class=line%d>", 
			n%3, gTimeNodeTypes[greatest].name, n%3);
		_WriteLargeInteger(fp, &totalTimes[greatest], 1);
		fprintf(fp, "</div></TD><TD><div class=line%d>&nbsp;&nbsp;%d calls.</div></TD></TR>\n", n%3, totalCalls[greatest]);
		
		totalTimes[greatest].LowPart = 0;
		totalTimes[greatest].HighPart = 0;
		n++;
	}
	fprintf(fp, "</TABLE>\n");
	
	fclose(fp);

	return (0);
}


/******************************************************************************
* webc_DebugTimerReset - reset the performance timer
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void webc_DebugTimerReset(void)
{
struct TimeNode *root = _RootNode();

	if (root)
	{
		_DeleteNode(root);
	}

	gpCurrentTimeNode = _CreateNode(0,0);
	
	// re-calibrate the timers
	_CalibrateTimers();
}


/******************************************************************************
* _WriteTimeNode - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void _WriteTimeNode(FILE *fp, struct TimeNode *node, int *count, int depth)
{
struct TimeNode *child = node->pFirstChild;
LARGE_INTEGER otherTime, rem;
struct TimeNode *highChild;

	otherTime = node->totalTime;
	while (child)
	{		
		LargeIntSub(&otherTime, &child->totalTime);
		child = child->pNext;
	}
	
	fprintf(fp, "<div onclick=\"ToggleImage(\'img%d\'); ToggleContents(\'div%d\');\" >", *count, *count);
	fprintf(fp, "<img id=img%d src=\"images/%s.gif\">\n", *count, node->pFirstChild? "closed" : "empty");
	fprintf(fp, "\t<span class=nameClass>%s</span> <span class=countClass>called ", gTimeNodeTypes[node->id].name);
	_WriteLongInteger(fp, node->count, 1);
	fprintf(fp, " times</span>");
	if (node->pParent && node->pParent->count != 0)
	{
		fprintf(fp," <span class=averageClass>(average of ");
		_WriteLongInteger(fp, node->count/node->pParent->count, 1);
		fprintf(fp," per parent call)</span>");
	}
	fprintf(fp, "; <span class=timeClass>");
	_WriteLargeInteger(fp, &node->totalTime, 1);
	fprintf(fp, " total running time</span>");
	if (node->pParent && node->pParent->totalTime.LowPart != 0)
	{
		LARGE_INTEGER temp;
		fprintf(fp, " <span class=percentClass>(");
		temp = ExtendedIntegerMultiply(node->totalTime, 100);
		temp = LargeIntegerDivide(temp, node->pParent->totalTime, &rem);
		_WriteLargeInteger(fp, &temp, 1);
		fprintf(fp, "%% of parent)</span>\n");
	}
	fprintf(fp, "</div>\n<div id=div%d class=divLevel%d>\n", *count, depth%3);

	fprintf(fp, "\t<span class=otherClass>Other</span> <span class=otherTime>(");
	_WriteLargeInteger(fp, &otherTime, 1);
	fprintf(fp, " total running time)</span>");
	if (node->totalTime.LowPart != 0)
	{
		LARGE_INTEGER temp;
		fprintf(fp, ", <span class=otherPercent>");
		temp = LargeIntegerDivide(ExtendedIntegerMultiply(otherTime, 100), node->totalTime, &rem);
		_WriteLargeInteger(fp, &temp, 1);
		fprintf(fp, "%% of total</span>\n");
	}

	// write children of this node in order from greatest totalTime to least
	while (1)
	{
		// highChild gets first child whose written flag is not set
		highChild = node->pFirstChild;
		while (highChild)
		{
			if (!(highChild->flags & TIMENODE_FLAG_WRITTEN))
			{
				break;
			}
			highChild = highChild->pNext;
		}
	
		// no child whose written flag is not set; we're done
		if (!highChild)
		{
			break;
		}
					 		
		// try to find a child (not written yet) with a higher totalTime
		child = highChild->pNext;
		while (child)
		{
			if (LargeIntegerGreaterThan(child->totalTime, highChild->totalTime) && 
			    !(child->flags & TIMENODE_FLAG_WRITTEN))
			{
				highChild = child;
			}
			child = child->pNext;
		}
	
		(*count)++;
		
		// write the child
		_WriteTimeNode(fp, highChild, count, depth+1);				
		highChild->flags |= TIMENODE_FLAG_WRITTEN;
	}
	
	child = node->pFirstChild;
	while (child)
	{
		child->flags &= ~TIMENODE_FLAG_WRITTEN;
		child = child->pNext;
	}
	
	fprintf(fp, "</div>\n\n");
	
}


/******************************************************************************
* _WriteLargeInteger - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void _WriteLargeInteger(FILE *fp, LARGE_INTEGER *i, int withCommas)
{
LARGE_INTEGER li;
ULONG parts[7];
int numParts = 0;

 	li = *i;
	
	for (numParts=0; numParts<7; numParts++)
	{		
		li = ExtendedLargeIntegerDivide(li, 1000, &parts[numParts]);
		if (!LargeIntegerGreaterThanZero(li))
		{
			break;
		}
	}
	
	fprintf(fp, "%d", parts[numParts--]);
	for (; numParts >= 2; numParts--)
	{
		if (withCommas)
		{
			fprintf(fp, ",%03d", parts[numParts]);
		}
		else
		{
			fprintf(fp, "%03d", parts[numParts]);
		}
	}
}


/******************************************************************************
* _WriteLongInteger - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void _WriteLongInteger(FILE *fp, long i, int withCommas)
{
LARGE_INTEGER li;

	li.LowPart = i;
	li.HighPart = 0;
	li = ExtendedIntegerMultiply(li, ONE_MILLION);
	_WriteLargeInteger(fp, &li, withCommas);
}

/******************************************************************************
* _FindNode - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

struct TimeNode * _FindNode (struct TimeNode *pParentNode, long id)
{
struct TimeNode *child = pParentNode->pFirstChild;

	while (child)
	{
		if (child->id == id)
		{
			return (child);
		}
		child = child->pNext;
	}
	
	return (0);
}


/******************************************************************************
* _CreateNode - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

struct TimeNode * _CreateNode (struct TimeNode *pParentNode, long id)
{
struct TimeNode *child = (struct TimeNode *) WEBC_MALLOC(sizeof(struct TimeNode), __FILE__, __LINE__);

	if (child)
	{
		child->flags = 0;
		child->id = id;
		child->count = 0;
		child->totalTime.LowPart = 0;
		child->totalTime.HighPart = 0;
	
		child->pFirstChild = 0;
		child->pLastChild = 0;
		child->pPrev = 0;
		child->pParent = pParentNode;

		if (pParentNode)
		{
			child->pNext = pParentNode->pFirstChild;
		
			if (pParentNode->pFirstChild)
			{
				pParentNode->pFirstChild->pPrev = child;
			}
			else
			{
				pParentNode->pLastChild = child;
			}
			pParentNode->pFirstChild = child;
		}
		else
		{
			child->pNext = 0;
		}
	}

	return (child);	
}


/******************************************************************************
* _DeleteNode - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void _DeleteNode(struct TimeNode *pNode)
{
struct TimeNode *child, *next;

	child = pNode->pFirstChild;
	while (child)
	{
		next = child->pNext;
		_DeleteNode(child);
		child = next;
	}
	
	WEBC_FREE(pNode, __FILE__, __LINE__);
}


/******************************************************************************
* _DeleteNode - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

struct TimeNode *_RootNode (void)
{
struct TimeNode *root = gpCurrentTimeNode;

	while (root && root->pParent)
	{
		root = root->pParent;
	}
	
	return (root);
}


/******************************************************************************
* LargeIntAdd - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void LargeIntAdd(LARGE_INTEGER *a, LARGE_INTEGER *b)
{
	*a = LargeIntegerAdd(*a, *b);
/*
	DWORD lowSum = a->LowPart + b->LowPart;
	a->HighPart += b->HighPart;
	if (lowSum < a->LowPart)
	{
		// carry bit
		a->HighPart++;
	}
	a->LowPart = lowSum;
*/
}


/******************************************************************************
* LargeIntSub - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void LargeIntSub(LARGE_INTEGER *a, LARGE_INTEGER *b)
{
	if (b->LowPart > a->LowPart)
	{
		a->HighPart--;
	}
	a->LowPart = a->LowPart - b->LowPart;
	a->HighPart = a->HighPart - b->HighPart;
}


/******************************************************************************
* _CalibrateTimers - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void _CalibrateTimers(void)
{
long i,n;
LARGE_INTEGER timerOverhead;
struct TimeNode *node;

	gTimerOverhead.LowPart = 0;
	gTimerOverhead.HighPart = 0;
	gChildOverhead.LowPart = 0;
	gChildOverhead.HighPart = 0;

	/* First find error associated with timing a single block of code */
	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (node)
	{
		node->totalTime.LowPart = 0;
		node->totalTime.HighPart = 0;		
	}
	for (i=0; i<ONE_MILLION; i++)
	{
		WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);
	}
	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (!node)
	{
		// error!
		return;
	}
	gChildOverhead = ExtendedLargeIntegerDivide(node->totalTime, ONE_MILLION, &i);	

	node->totalTime.LowPart = 0;
	node->totalTime.HighPart = 0;		

	/*************************************************************************/
	/* Now find error associated with timing a child call                    */
	/*************************************************************************/
 
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
	for (i=0; i<ONE_MILLION; i++)
	{
		WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
		for (n=0; n<SMALL_DELAY; n++); // create a small delay
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);
	}
	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);

	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (!node)
	{
		// error!
		return;
	}
	timerOverhead = node->totalTime;
	node->totalTime.LowPart = 0;
	node->totalTime.HighPart = 0;		

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
	for (i=0; i<ONE_MILLION; i++)
	{
		for (n=0; n<SMALL_DELAY; n++); // create a small delay
	}
	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);

	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (!node)
	{
		// error!
		return;
	}
	LargeIntSub(&timerOverhead, &node->totalTime);	
	gTimerOverhead = ExtendedLargeIntegerDivide(timerOverhead, ONE_MILLION, &i);	
	
	/*************************************************************************/
	/* Test our calibration settings                                         */
	/*************************************************************************/
	
	/* First find error associated with timing a single block of code */
	/*
	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (node)
	{
		node->totalTime.LowPart = 0;
		node->totalTime.HighPart = 0;		
	}
	for (i=0; i<ONE_MILLION; i++)
	{
		WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);
	}
	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (!node)
	{
		// error!
		return;
	}
	gChildOverheadError = ExtendedLargeIntegerDivide(node->totalTime, ONE_MILLION, &i);	
	node->totalTime.LowPart = 0;
	node->totalTime.HighPart = 0;		

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
	for (i=0; i<ONE_MILLION; i++)
	{
		WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
		for (n=0; n<SMALL_DELAY; n++); // create a small delay
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);
	}
	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);

	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (!node)
	{
		// error!
		return;
	}
	timerOverhead = node->totalTime;
	node->totalTime.LowPart = 0;
	node->totalTime.HighPart = 0;		

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_TIMER_CALIBRATE);
	for (i=0; i<ONE_MILLION; i++)
	{
		for (n=0; n<SMALL_DELAY; n++); // create a small delay
	}
	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_TIMER_CALIBRATE);

	node = _FindNode(gpCurrentTimeNode, WEBC_DEBUG_TIMER_CALIBRATE);
	if (!node)
	{
		// error!
		return;
	}
	LargeIntSub(&timerOverhead, &node->totalTime);	
	gTimerOverheadError = ExtendedLargeIntegerDivide(timerOverhead, ONE_MILLION, &i);	
	*/
}


/******************************************************************************
* _TotalTime - 
*   
* Parameters
*
* Description
*    
* Returns (void)
*   nothing
******************************************************************************/

void _TotalTime(struct TimeNode *pParentNode, long id, LARGE_INTEGER *time, long *count)
{
LARGE_INTEGER subTotal;
long subCount;
struct TimeNode *child;
	
	if (pParentNode->id == id)
	{
		*count = pParentNode->count;
		*time = pParentNode->totalTime;
		return;
	}
	
	*count = 0;
	time->LowPart = 0;
	time->HighPart = 0;
	child = pParentNode->pFirstChild;
	while (child)
	{
		_TotalTime(child, id, &subTotal, &subCount);
		LargeIntAdd(time, &subTotal);
		*count += subCount;
		child = child->pNext;
	}
}

#endif // WEBC_SUPPORT_DEBUG_TIME
