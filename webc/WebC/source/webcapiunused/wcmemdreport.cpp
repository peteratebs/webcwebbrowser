//
// wcmemdreport.cppP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
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


#include "wcapi.h"
#include "webcassert.h"
#include "webcmem.h"
#include "rtpmem.h"



// Memory diagnostics.

// Generic statistics, total memory used, peak usage,current sequence number,
// Install peak memory usage handler and threshold value
// Install memory allocation/free callback handler ?
// Usage reported by number of calls, by file, by size, all, betwen sequence numbers



#ifdef RTP_TRACK_LOCAL_MEMORY
extern "C" void rtp_debug_get_mem_stats(MemDbStats *pStats);
#endif

#ifdef RTP_TRACK_LOCAL_MEMORY

/*****************************************************************************/
// wcMemDbGetStats
/**

 @memo  Retrieve a compact summary of memory allocation statistics.
 @doc   Retrieves a summary report of current memory usage.
 		The resources that accumulate in the context block are mainly string buffers used to return data that has
		been converted from internal unicode form to Ascii.

 <pre>
 Note: \Ref{RTP_TRACK_LOCAL_MEMORY} must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the stats structure is not updated.
 The stats structure contains these fields:
    typedef struct s_MemDbStats  {
    unsigned long MemAlloced;     // Total bytes allocated since startup
    unsigned long MemFreed;       // Total bytes freed since startup
    unsigned long MemUsed;        // Total bytes currently in use
    unsigned long MaxMemUsed;     // High water mark of maximum bytes ever used.
    unsigned long MemClipped;     // Number of times a memory allocation failed (in debug mode an assert is also generated).
    unsigned long MemNewed;       // Total number of bytes currently allocated by the new operator and not yet released by the free operator. Note: what about detail about newed data.
    unsigned long MallocSequence; // Current sequence number, keep this around if you with to later on report on a range of memory allocattion calls starting from this point.
    unsigned long MaxUsageSeq;	// Sequence number of the allocation when the maximum allocation was reached. (see RTPMemBlockHeader);
    #ifdef RTP_TRACK_LOCAL_MEMORY
    // This list provides access to all current oustanding allocations in a linked list.
    // see \Ref{wcMemDbGetChangesSinceWatchPoint} for a description of the RTPMemBlockHeader structure
    // MemBlockList is a pointer to the most recently added block.
    // follow the chain through the prev pointers to get to the other blocks.
    RTPMemBlockHeader *MemBlockList; // A list of the currently outstanding memory starting with the most recent allocation.
    #endif
    } MemDbStats;
  </pre>

 @param  MemDbStats *pStats    		-   Memory stats structure to fill in.

 @return		nothing

 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/

void wcMemDbGetStats(MemDbStats *pStats)
{
#ifdef RTP_TRACK_LOCAL_MEMORY
	rtp_debug_get_mem_stats(pStats);
#endif
}


/*****************************************************************************/
// wcMemDbInstallPeakUsageCallback
/**

 @memo  Install a function to be called when peak memory allocation is reached.
 @doc   Set a threshold value for activating the callback and install a function to be called when peak memory allocation is reached.
 		A callback function may be used to capture the state of the heap at the time it is at maximum usage, this my be used to identify leaks and,
 		along with other reporting modules, to characterize your application's worst case mekory usage.


 <pre>
 Note: \Ref{RTP_TRACK_LOCAL_MEMORY} must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.

 For example: We've called \Ref{wcMemDbGetStats} from our application and determined that the maximum ram usage of an application is a little over 2,000,000 bytes.
 We want to know where this memory is being used, so we create a callback function that will call \Ref{wcPrintMemDetailReport} to print details to the system console.
 (assuming we have a system console).

// This callback will print a report summarizing the number of allocations made from the specific file and line and the number of bytes allocated, displaying the list
   largest to smallest.
    static void PeakUsageCallback(const char *file, long line_num,void *ctx)
    {
	    wcPrintMemDetailReport(0, 0, DbSummarizeByLine, SortBySize);
    }
  ..
 // This line of code when executed instructs webC to call PeakUsageCallback when a new maximum heap allocation is reached and the total memory allocated is over 2 million bytes.

		wcMemDbInstallPeakUsageCallback(PeakUsageCallback,(void *)0, 2000000);


 // A report like the following is produced.


  fbabstraction.cpp              369  (47  ) 457404   {BitMapData}
  webjpeg.cpp                    119  (1   ) 412164   {New}
  windowsbitmapframebuffer.cpp   94   (1   ) 307200   { }
  windowsbitmapframebuffer.cpp   97   (1   ) 307200   { }
  fbabstractwindowmanager.cpp    615  (1   ) 192000   {SaveUnder}
  hashtbl.cpp                    99   (6   ) 93168    {HashTable}
  jsapi.c                        738  (1   ) 66648    { }
  ....
  Records returned == 110
  Bytes accounted for = 2081088

  And from it we see that of the 2081088 byte allocated at the peak that almost 2,000,000 of those bytes are allocated from only 7 seperate locations. the next 80 K or so that is used is allocated
  from calls in over 103 seperate locations. The first three reocrds indicated that 457404 bytes are allocated in 47 seperate calls made from fbabstraction.cpp to hold bit map data. Notice how the memo field contains
  says that the memory is for "BitMapData". If a memo fields is not filled and empty field{} is displayed. There is one allocation at  webjpeg.cpp , line 369 for 412164 bytes (this is unlabled so the source must be consulted) and
  two calls each, for 307200, inside windowsbitmapframebuffer.cpp (the raw frame buffer and background buffer).

  </pre>

 @param  void (*fn)(const char *file, long line_num,void *puserctx)	-   Function to call when memory reaches a high water mark.
 @param void *puserctx -  webC does not use this, it is a pointer passed to the callback function that may be used for any purpose. Pass zero if it is not needed.
 @param unsigned long threshold - Tells webC to call the function only if this amount of memory or more is allocated. If zero is provided the callback is performed
 every time a new maximum is reached, if a non-zero value is provided the callback is performed only when the heap usagage exceeds the threshold.

 @return		nothing

 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/

void wcMemDbInstallPeakUsageCallback(void (*fn)(const char *file, long line_num,void *puserctx),void *puserctx, unsigned long threshold)
{
	_rtp_debug_set_high_water_callback (fn, puserctx, threshold);
}


static int DbAddAppendIfNewFile(RTPMemBlockHeader *BlockListCategories,RTPMemBlockHeader *MemBlockToAdd);
static int DbAddAppendIfNewLine(RTPMemBlockHeader *BlockListCategories,RTPMemBlockHeader *MemBlockToAdd);
static void DbAddToByFileSummary(RTPMemBlockHeader *Rval,RTPMemBlockHeader *MemBlock);
static void DbAddToByLineSummary(RTPMemBlockHeader *Rval,RTPMemBlockHeader *MemBlock);
static RTPMemBlockHeader *DbSortBySize(RTPMemBlockHeader *List);
static RTPMemBlockHeader *DbSortByFile(RTPMemBlockHeader *List);
static int wcMemDbSortListLength(RTPMemBlockHeader *pL);

/*****************************************************************************/
// wcMemDbGetReport
/**

 @memo  Returns a linked list of report records detailing memory usage.
 @doc   The caller supplies parameters that instruct wcMemDbGetReport how data is to be summarized and sorted. The data that is returned is a linked list of records that may be analysed or
 displayed. The returned list must be freed by calling wcMemDbReleaseReport.<br>
 Note: \Ref{RTP_TRACK_LOCAL_MEMORY} must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.
 <br><br><i>
 <b>Note:</b> The function \Ref{wcPrintMemDetailReport}, calls wcMemDbGetReport to retrieve a report and then prints the records and frees them. If you have a console you may be able to use \Ref{wcPrintMemDetailReport}, if not,
 you may want to review its source code.</i>

 @param  long  startSequence -  You may use the MallocSequence field returned from \Ref{wcMemDbGetStats} to select the first record to process, pass zero to start at the first allocation.
 @param  long  endSequence -   You may use the MallocSequence field returned from \Ref{wcMemDbGetStats} to select the last record to process, pass zero to end at the last allocation.
 @param  MemDbReportSummaryType ReportType - Choose a report type from the list below
 <pre>
&nbsp;&nbsp;DbSummarizeByFile // Return a list of records containing file name and total memory allocations outstanding for the file.
&nbsp;&nbsp;DbSummarizeByLine // Return a list of records containing file name:line number and total memory allocations outstanding at that location the file.
&nbsp;&nbsp;DbSummarizeNone   // Return a record of every outstanding allocation in the range.
 </pre>
 @param  MemDbReportSortType SortBy - Choose a sort type from the list below
 <pre>
&nbsp;&nbsp;SortNone		// Do not sort the records, best method for DbSummarizeNone report type.
&nbsp;&nbsp;SortBySize		// Sort the descending order by size.
&nbsp;&nbsp;SortByFile		// Sort in ascending order by file name followed by line if DbSummarizeByLine is specified.
 </pre>
@return		RTPMemBlockHeader * - A list of report records.
<pre>
 The following RTPMemBlockHeader structure fields may be used
	struct __rtp_mem_block_header  *next;       // pointer to next record in the order specified by the SortBy argument
	long	                        size;       // Sum of bytes for the report type in bytes. If ReportType==DbSummarizeNone contains the bytes allocated from a single call.
	long                            seq;        // Number of calls summarized in this rrecord or the internal sequence number if ReportType==DbSummarizeNone
	const char                     *file;      // File being reported on.
	long                            line;      // Line number if ReportType==DbSummarizeNone or ReportType==DbSummarizeByLine
	const char                   *comment;     // If if ReportType==DbSummarizeNone or ReportType==DbSummarizeByLine contains a pointer to a string describing the allocation, or zero if no comment was passed when the memory was allocated.
<br>
<i>Note: To free the report memory you must pass it to \Ref{wcMemDbReleaseReport}. </i>
</pre>

 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/

RTPMemBlockHeader *wcMemDbGetReport(long  startSequence, long  endSequence, MemDbReportSummaryType ReportType, MemDbReportSortType SortBy)
{
#ifdef RTP_TRACK_LOCAL_MEMORY
MemDbStats dbStats;
RTPMemBlockHeader *MemBlockList,*pStartofSequence,*pEndofSequence;
int ReportRecordsNeeded=0;

	wcMemDbGetStats(&dbStats);
	// Get the start and end records, if the sequence numbers were not specified take the begining and end of the list
	pStartofSequence=pEndofSequence=0;
	MemBlockList=dbStats.MemBlockList;
	if (endSequence==0)
		pEndofSequence=MemBlockList;
	while (MemBlockList && pStartofSequence==0)
	{
		if (pEndofSequence==0 && MemBlockList->prev==0)
			pEndofSequence=MemBlockList;
		else if (endSequence)
		{
			if (MemBlockList->seq == endSequence)
				pEndofSequence=MemBlockList;
			else if  (MemBlockList->seq <= endSequence)
			{
				pEndofSequence=MemBlockList->next ? MemBlockList->next : MemBlockList;
			}
		}
		if (pStartofSequence==0 && MemBlockList->prev==0)
			pStartofSequence=MemBlockList;
		else if (startSequence)
		{
			if (MemBlockList->seq == startSequence)
				pStartofSequence=MemBlockList;
			else if (MemBlockList->seq < startSequence)
				pStartofSequence=MemBlockList->next ? MemBlockList->next : MemBlockList;
		}
		MemBlockList=MemBlockList->prev;
	}

	// Calculate how many records we'll need
	RTPMemBlockHeader *MemBlock,*BlockListCategories,*BlockListCategoriesEnd;
	BlockListCategories=BlockListCategoriesEnd=0;
	if (!pStartofSequence)
		return 0;
	MemBlock=pStartofSequence;
	do
	{
		if (MemBlock->flags & dbMFlagIgnore)
		{
			MemBlock=MemBlock->next;
			continue;
		}
		if (!BlockListCategories)
		{
			BlockListCategories=BlockListCategoriesEnd=MemBlock;
			BlockListCategoriesEnd->nextSort=0;
			ReportRecordsNeeded=1;
		}
		else if (ReportType==DbSummarizeNone)
		{
			BlockListCategoriesEnd->nextSort=MemBlock;
			MemBlock->nextSort=0;
			BlockListCategoriesEnd=MemBlock;
			ReportRecordsNeeded +=1;
		}
		else if (ReportType==DbSummarizeByFile && DbAddAppendIfNewFile(BlockListCategories,MemBlock))
			ReportRecordsNeeded +=1;
		else if (ReportType==DbSummarizeByLine && DbAddAppendIfNewLine(BlockListCategories,MemBlock))
			ReportRecordsNeeded +=1;
		if (MemBlock==pEndofSequence)
			break;
		MemBlock=MemBlock->next;
	} while (MemBlock);
	if (!ReportRecordsNeeded)
		return 0;
	// Allocate ReportRecordsNeeded * ReportRecordSize bytes
	RTPMemBlockHeader *Rval = (RTPMemBlockHeader *) WEBC_DEBUG_MALLOC(ReportRecordsNeeded*sizeof(RTPMemBlockHeader),__FILE__,__LINE__,"MemDiagnostic", dbMFlagIgnore);
	if (!Rval)
		return 0;
	RTPMemBlockHeader *ToMemBlock=Rval;
	MemBlock=pStartofSequence;
	// Link our report records together and copy the extracted records, zero the size if we are summarizing in the next step
	for (int i=0; i < ReportRecordsNeeded; i++)
	{
		while (MemBlock->flags & dbMFlagIgnore)
		{
			MemBlock=MemBlock->nextSort;
			WEBC_ASSERT(MemBlock);
		}
		*ToMemBlock=*MemBlock;
		// Store the address of the allocated memory in the nextSort field.
		// Only meaningful if DbSummarizeNone is specified. Useful for hunting down leaks.
		ToMemBlock->nextSort = MemBlock+1;
		if (ReportType!=DbSummarizeNone)
		{
			ToMemBlock->size=0;
			ToMemBlock->seq=0; // Zero the seq field, this will count occurrences in a summary report
		}
		if (i+1 == ReportRecordsNeeded)
		{
			ToMemBlock->next=0;
			break;
		}
		else
		{
			ToMemBlock->next=ToMemBlock+1;
			ToMemBlock += 1;
			ToMemBlock->next=0;
			ToMemBlock->prev = (ToMemBlock-1);
		}
		MemBlock=MemBlock->nextSort;
	}
	// Update summary reports
	MemBlock=pStartofSequence;
	while (MemBlock)
	{
		if (MemBlock->flags & dbMFlagIgnore)
			MemBlock=MemBlock->next;
		else
		{
			if (ReportType==DbSummarizeNone)
				;
			else if (ReportType==DbSummarizeByFile)
				DbAddToByFileSummary(Rval,MemBlock);
			else if (ReportType==DbSummarizeByLine)
				DbAddToByLineSummary(Rval,MemBlock);
			MemBlock=MemBlock->next;
		}
	}
	// Now sort the report
	if (SortBy==SortNone)
		return Rval;
	if (SortBy==SortBySize)
	{
		Rval = DbSortBySize(Rval);
	}
	else if (SortBy==SortByFile)
	{
		Rval = DbSortByFile(Rval);
	}
	return Rval;
#endif
	return 0;
}


/*****************************************************************************/
// wcMemDbGetWatchPoint
/**

 @memo  Returns a watchpoint list that may be used later to determine changes to the heap since this point.
 @doc   To determine how much memory is allocated and release during a specific interval you should first call wcMemDbGetWatchPoint and then at the end of the interval call
 \Ref{wcMemDbGetChangesSinceWatchPoint} or \Ref{wcPrintMemChangeReport} to respectively retrieve and display the changes made to the heap during the interval.

 When the returned list is no longer needed it must be freed by calling wcMemDbReleaseReport.<br>
 Note: \Ref{RTP_TRACK_LOCAL_MEMORY} must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.
@return		RTPMemBlockHeader * - A watch list made of report records.
<pre>
 Example: Print the changes made to the heap by a fictitious function named MyAppletInit().

{
	RTPMemBlockHeader *myWatch;
	myWatch = wcMemDbGetWatchPoint();
 	MyAppletInit();
	wcPrintMemChangeReport(myWatch, wcTRUE); // Print the report, the second argument instructs it to print only locations with outstanding memory allocations.
	wcMemDbReleaseReport(myWatch);
}
 </pre>
 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/

RTPMemBlockHeader *wcMemDbGetWatchPoint(void)
{
	return wcMemDbGetReport(0, 0, DbSummarizeNone, SortNone);

}

/*****************************************************************************/
// wcMemDbReleaseReport
/**

 @memo  Free a memory report list that was returned from a wcMemDb function.
 @doc   Frees the memory returned from one of the functions that return a list. Non-zero values returned from \Ref{wcMemDbGetWatchPoint}, \Ref{wcMemDbGetReport},
 \Ref{wcMemDbGetChangesSinceWatchPoint} must be freed by calling this function.

 @param  RTPMemBlockHeader *pReport - The report list to free.

 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/
void wcMemDbReleaseReport(RTPMemBlockHeader *pReport)
{
RTPMemBlockHeader *pReportBase=pReport;
	while(pReport)
	{
		if (pReportBase > pReport)
			pReportBase=pReport;
		pReport=pReport->next;
	}
	WEBC_FREE(pReportBase);
}


static int wcMemDbSortListLength(RTPMemBlockHeader *pL);

// ===
// Add a record to the list of records segregated by line.
static RTPMemBlockHeader *DbEnsureLineOnList(RTPMemBlockHeader *BlockList,RTPMemBlockHeader *MemBlockToAdd)
{
	RTPMemBlockHeader *MemBlock=BlockList;
	while(MemBlock)
	{
		if(rtp_strcmp(MemBlock->file, MemBlockToAdd->file)==0 && MemBlock->line == MemBlockToAdd->line)
			return MemBlock;
		if (MemBlock->nextSort)
			MemBlock=MemBlock->nextSort;
		else
		{
			MemBlock->nextSort=MemBlockToAdd;
			MemBlockToAdd->nextSort=0;
			MemBlock = 0;
		}
	}
	return MemBlockToAdd;
}
static RTPMemBlockHeader *wcMemDbGetReleasesSinceWatchPoint(RTPMemBlockHeader *DbWatchPoint);
static RTPMemBlockHeader *wcMemDbGetAllocationsSinceWatchPoint(RTPMemBlockHeader *DbWatchPoint);

/*****************************************************************************/
// wcMemDbGetChangesSinceWatchPoint
/**

 @memo  Returns a list of changes to the heap since the watchpoint was established.
 @doc   Returns a list of changes to the heap since the watchpoint was established.\Ref{wcMemDbGetWatchPoint}. The returned list must be freed by calling wcMemDbReleaseReport.<br>

 <i><b>Note:</b> Note: RTP_TRACK_LOCAL_MEMORY must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.<br>	</i>

 @param  RTPMemBlockHeader *PrevWatchPoint - Retrieve changes since this watch point was established by a call to \Ref{wcMemDbGetWatchPoint}.

@return		RTPMemBlockHeader * - A list of report records.

<pre>
 The following RTPMemBlockHeader structure fields may be used
	struct __rtp_mem_block_header  *next;       // pointer to next record in the list.
	long	                        size;       // Net sum of bytes allocated and free from this location. If < 0, more bytes were freed than allocated since the WatchPoint was established.
	long                            seq;        // Net sum of allocation and frees of memory from this location. If < 0, more free calls than allocation calls were made.
	const char                     *file;      // File being reported on.
	long                            line;      // Line number.
	const char                   *comment;     // Contains a pointer to a string describing the allocation, or zero if no comment was passed when the memory was allocated.
<br>
<i>Note: To free the report memory you must pass it to \Ref{wcMemDbReleaseReport}. </i>
</pre>

 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/


RTPMemBlockHeader *wcMemDbGetChangesSinceWatchPoint(RTPMemBlockHeader *DbWatchPoint)
{
#ifdef RTP_TRACK_LOCAL_MEMORY
RTPMemBlockHeader *Releases = wcMemDbGetReleasesSinceWatchPoint(DbWatchPoint);
RTPMemBlockHeader *Allocations = wcMemDbGetAllocationsSinceWatchPoint(DbWatchPoint);
RTPMemBlockHeader Dummy;
RTPMemBlockHeader *p;
int RecordCount=0;

	rtp_memset(&Dummy, 0, sizeof(Dummy));
	Dummy.file="None";
	Dummy.line = -1;
	Dummy.nextSort=0;
	p = Releases;
	while (p)
	{
		RTPMemBlockHeader *pRecord;
		pRecord = DbEnsureLineOnList(&Dummy, p);
		if (pRecord==p)
		{
			RecordCount+=1;
			pRecord->seq = -1;
			pRecord->size = -p->size;
		}
		else
		{

			pRecord->seq -= 1;
			pRecord->size -= p->size;
		}
		p=p->next;
	}
	p = Allocations;
	while (p)
	{
		RTPMemBlockHeader *pRecord;
		pRecord = DbEnsureLineOnList(&Dummy, p);
		if (pRecord==p)
		{
			RecordCount+=1;
			pRecord->seq = 1;
		}
		else
		{
			pRecord->seq += 1;
			pRecord->size += p->size;
		}
		p=p->next;
	}

	// Allocate ReportRecordsNeeded * ReportRecordSize bytes
	RTPMemBlockHeader *Rval = (RTPMemBlockHeader *) WEBC_DEBUG_MALLOC(RecordCount*sizeof(RTPMemBlockHeader),__FILE__,__LINE__,"MemDiagnostic", dbMFlagIgnore);
	if (!Rval)
		return 0;
	RTPMemBlockHeader *ToMemBlock=Rval;
	RTPMemBlockHeader *MemBlock=Dummy.nextSort;
	// Link our report records together and copy the extracted records
	for (int i=0; i < RecordCount; i++)
	{
		*ToMemBlock=*MemBlock;
		// Store the address of the allocated memory in the nextSort field.
		// Only meaningful if DbSummarizeNone is specified. Useful for hunting down leaks.
		if (i+1 == RecordCount)
		{
			ToMemBlock->nextSort=ToMemBlock->next=0;
			break;
		}
		else
		{
			ToMemBlock->nextSort=ToMemBlock->next=ToMemBlock+1;
			ToMemBlock += 1;
			ToMemBlock->nextSort=ToMemBlock->next=0;
		}
		MemBlock=MemBlock->nextSort;
	}
	if (Releases)
		wcMemDbReleaseReport(Releases);
	if (Allocations)
		wcMemDbReleaseReport(Allocations);
	return Rval;
#else
	return 0;
#endif
}

static RTPMemBlockHeader *wcMemDbListStart(RTPMemBlockHeader *List)
{
	while (List && List->prev) List = List->prev;
	return List;
}
static RTPMemBlockHeader *wcMemDbRecordsNotInBase(RTPMemBlockHeader *BaseList,RTPMemBlockHeader *CurrentList);

static RTPMemBlockHeader *wcMemDbGetReleasesSinceWatchPoint(RTPMemBlockHeader *DbWatchPoint)
{
#ifdef RTP_TRACK_LOCAL_MEMORY
MemDbStats dbStats;
RTPMemBlockHeader *BaseList,*CurrentList;
int ReportRecordsNeeded=0;
	wcMemDbGetStats(&dbStats);
	CurrentList = wcMemDbListStart(dbStats.MemBlockList);
	BaseList = DbWatchPoint;
	return wcMemDbRecordsNotInBase(CurrentList,BaseList);
#else
	return 0;
#endif
}

static RTPMemBlockHeader *wcMemDbGetAllocationsSinceWatchPoint(RTPMemBlockHeader *DbWatchPoint)
{
#ifdef RTP_TRACK_LOCAL_MEMORY
MemDbStats dbStats;
RTPMemBlockHeader *BaseList,*CurrentList;
int ReportRecordsNeeded=0;
	wcMemDbGetStats(&dbStats);
	CurrentList = wcMemDbListStart(dbStats.MemBlockList);
	BaseList = DbWatchPoint;
	return wcMemDbRecordsNotInBase(BaseList, CurrentList);
#else
	return 0;
#endif
}


static RTPMemBlockHeader *wcMemDbSortListEnd(RTPMemBlockHeader *pL)
{
	while (pL && pL->nextSort)
		pL=pL->nextSort;
	return pL;
}
static int wcMemDbSortListLength(RTPMemBlockHeader *pL)
{
	int l=0;
	while (pL && pL->nextSort)
	{
		l++;
		pL=pL->nextSort;
	}
	return l;
}
static int wcMemDbRecordsMatch(RTPMemBlockHeader *pA, RTPMemBlockHeader *pB)
{
	return (pA!=0 && pB != 0 && pA->seq==pB->seq);
}

static RTPMemBlockHeader *wcMemDbRecordsNotInBase(RTPMemBlockHeader *BaseList,RTPMemBlockHeader *CurrentList)
{
RTPMemBlockHeader *pMatch,*pCurrent,*pLastBaseHit,*pFirstKeeper;
int ReportRecordsNeeded=0;
	pLastBaseHit=pFirstKeeper=0;
	pCurrent=CurrentList;
	while (pCurrent)
	{
		if (pCurrent->flags & dbMFlagIgnore)
		{
			pCurrent = pCurrent->next;
			continue;
		}
		pMatch=0;
		if (pLastBaseHit && wcMemDbRecordsMatch(pCurrent,pLastBaseHit->next))
		{
			pMatch = pLastBaseHit->next;
		}
		else
		{
			RTPMemBlockHeader *pBl=BaseList;
			while (pBl)
			{
				if (wcMemDbRecordsMatch(pCurrent,pBl))
				{
					pMatch = pBl;
					break;
				}
				pBl=pBl->next;
			}
		}
		if (!pMatch)
		{
			ReportRecordsNeeded += 1;
			pCurrent->nextSort=0;
			if (!pFirstKeeper)
				pFirstKeeper=pCurrent;
			else
				wcMemDbSortListEnd(pFirstKeeper)->nextSort=pCurrent;
		}
		pCurrent = pCurrent->next;
	}

	if (ReportRecordsNeeded==0)
		return 0;
	RTPMemBlockHeader *Rval = (RTPMemBlockHeader *) WEBC_DEBUG_MALLOC(ReportRecordsNeeded*sizeof(RTPMemBlockHeader),__FILE__,__LINE__,"MemDiagnostic", dbMFlagIgnore);
	if (!Rval)
		return 0;
	RTPMemBlockHeader *ToMemBlock=Rval;
	RTPMemBlockHeader *MemBlock=pFirstKeeper;
	// Link our report records together and copy the extracted records, zero the size if we are summarizing in the next step
	for (int i=0; i < ReportRecordsNeeded; i++)
	{
		while (MemBlock->flags & dbMFlagIgnore)
		{
			MemBlock=MemBlock->nextSort;
		}
		*ToMemBlock=*MemBlock;
		// Store the address of the allocated memory in the nextSort field.
		// Only meaningful if DbSummarizeNone is specified. Useful for hunting down leaks.
		if (i+1 == ReportRecordsNeeded)
		{
			ToMemBlock->next=0;
			break;
		}
		else
		{
			ToMemBlock->next=ToMemBlock+1;
			ToMemBlock += 1;
			ToMemBlock->next=0;
			ToMemBlock->prev = (ToMemBlock-1);
		}
		MemBlock=MemBlock->nextSort;
	}
	return Rval;
}

static RTPMemBlockHeader *DbInsertBySize(RTPMemBlockHeader *InList,RTPMemBlockHeader *pRecord)
{
RTPMemBlockHeader *List=InList;
RTPMemBlockHeader *Prev=0;
	pRecord->next = 0;
	if (!List)
		return pRecord;
	while (List)
	{
		if (pRecord->size > List->size)
		{
			pRecord->next = List;
			if (Prev)
				Prev->next = pRecord;
			if (List == InList)
				return pRecord;
			else
				return InList;
		}
		Prev = List;
		List=List->next;
	}
	Prev->next=pRecord;
	return InList;
}
static RTPMemBlockHeader *DbSortBySize(RTPMemBlockHeader *List)
{
	RTPMemBlockHeader *pSortList;
	pSortList=0;
	while (List)
	{
		RTPMemBlockHeader *p = List;
		List=p->next;
		pSortList = DbInsertBySize(pSortList,p);
	}
	return pSortList;
}
static RTPMemBlockHeader *DbInsertByFile(RTPMemBlockHeader *InList,RTPMemBlockHeader *pRecord)
{
RTPMemBlockHeader *List=InList;
RTPMemBlockHeader *Prev=0;
	pRecord->next = 0;
	if (!List)
		return pRecord;
	while (List)
	{
		int cmpval = rtp_strcmp(pRecord->file,List->file);
		if (cmpval < 0 || cmpval==0 && pRecord->line < List->line)
		{
			pRecord->next = List;
			if (Prev)
				Prev->next = pRecord;
			if (List == InList)
				return pRecord;
			else
				return InList;
		}
		Prev = List;
		List=List->next;
	}
	Prev->next=pRecord;
	return InList;
}
static RTPMemBlockHeader *DbSortByFile(RTPMemBlockHeader *List)
{
	RTPMemBlockHeader *pSortList;
	pSortList=0;
	while (List)
	{
		RTPMemBlockHeader *p = List;
		List=p->next;
		pSortList = DbInsertByFile(pSortList,p);
	}
	return pSortList;
}

// Add the record to the list of memory records if it resides in a file that is not yest accounted for.
static int DbAddAppendIfNewFile(RTPMemBlockHeader *BlockListCategories,RTPMemBlockHeader *MemBlockToAdd)
{
	RTPMemBlockHeader *MemBlock=BlockListCategories;
	while(MemBlock)
	{
		if(rtp_strcmp(MemBlock->file, MemBlockToAdd->file)==0)
			return 0;
		if (MemBlock->nextSort)
			MemBlock=MemBlock->nextSort;
		else
		{
			MemBlock->nextSort=MemBlockToAdd;
			MemBlockToAdd->nextSort=0;
			MemBlock = 0;
		}
	}
	return 1;
}

// Add the record to the list of memory records if it resides in a file and line that is not yet accounted for.
static int DbAddAppendIfNewLine(RTPMemBlockHeader *BlockListCategories,RTPMemBlockHeader *MemBlockToAdd)
{
	RTPMemBlockHeader *MemBlock=BlockListCategories;
	while(MemBlock)
	{
		if(rtp_strcmp(MemBlock->file, MemBlockToAdd->file)==0 && MemBlock->line == MemBlockToAdd->line)
			return 0;
		if (MemBlock->nextSort)
			MemBlock=MemBlock->nextSort;
		else
		{
			MemBlock->nextSort=MemBlockToAdd;
			MemBlockToAdd->nextSort=0;
			MemBlock = 0;
		}
	}
	return 1;
}

// Find the file and line accumulator record for this entry and update the size count
static void DbAddToByLineSummary(RTPMemBlockHeader *Rval,RTPMemBlockHeader *MemBlockToSum)
{
	RTPMemBlockHeader *MemBlock=Rval;
	while(MemBlock)
	{
		if(rtp_strcmp(MemBlock->file, MemBlockToSum->file)==0 && MemBlock->line == MemBlockToSum->line)
		{
			MemBlock->size += MemBlockToSum->size;
			MemBlock->seq += 1; // The seq field counts occurrences in a summary report
			return;
		}
		MemBlock=MemBlock->next;
	}
}

// Find the file and accumulator record for this entry and update the size count
static void DbAddToByFileSummary(RTPMemBlockHeader *Rval,RTPMemBlockHeader *MemBlockToSum)
{
	RTPMemBlockHeader *MemBlock=Rval;
	while(MemBlock)
	{
		if(rtp_strcmp(MemBlock->file, MemBlockToSum->file)==0)
		{
			MemBlock->size += MemBlockToSum->size;
			MemBlock->seq += 1; // The seq field counts occurrences in a summary report
			return;
		}
		MemBlock=MemBlock->next;
	}
}

#endif