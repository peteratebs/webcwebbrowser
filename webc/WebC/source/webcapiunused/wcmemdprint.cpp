//
// wcmemdprint.cpp -
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
#include "rtpprint.h"
static char *exMemFileName(char *fpath);

/*****************************************************************************/
// wcPrintMemStats
/**

 @memo  Calls wcMemDbGetStats and prints the results to the console.
 @doc   Calls wcMemDbGetStats and prints the results to the console.

 <i><b>Note:</b> Note: RTP_TRACK_LOCAL_MEMORY must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.<br>
 <b>Note:</b> rtplatform's rtp_printf function must be functional to see the output. rtp_printf can be enabled by either mapping it to your system's printf() or by using the version supplied with webC and routing the output to an output port through rtp_term_cputs().</i>
 <pre>
The output likes like this:
&nbsp;&nbsp;Total bytes currently in use 866542
&nbsp;&nbsp;Total number of these bytes allocated by the new operator and not yet released by the free operator. 67028
&nbsp;&nbsp;High water mark of maximum bytes ever used. 1164331
&nbsp;&nbsp;Sequence number of the allocation when the maximum allocation was reached. (see RTPMemBlockHeader); 539
&nbsp;&nbsp;Total bytes allocated since startup 2336634
&nbsp;&nbsp;Total bytes freed since startup 1403064
&nbsp;&nbsp;Number of times a memory allocation failed 0
&nbsp;&nbsp;Current sequence number. 564
 </pre>

@return		None

@see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/
#if (WEBC_MEMORY_RTPDEBUG)
void wcPrintMemStats(void)
{
MemDbStats Stats;

	wcMemDbGetStats(&Stats);
    rtp_printf("Total bytes currently in use %d\n",Stats.MemUsed);        // Total bytes currently in use
	rtp_printf("Total number of these bytes allocated by the new operator and not yet released by the free operator. %d\n",Stats.MemNewed);       // Total number of bytes currently allocated by the new operator and not yet released by the free operator. Note: what about detail about newed data.
    rtp_printf("High water mark of maximum bytes ever used. %d\n",Stats.MaxMemUsed);     // High water mark of maximum bytes ever used.
	rtp_printf("Sequence number of the allocation when the maximum allocation was reached. (see RTPMemBlockHeader); %d\n",Stats.MaxUsageSeq);	// Sequence number of the allocation when the maximum allocation was reached. (see RTPMemBlockHeader);
	rtp_printf("Total bytes allocated since startup %d\n",Stats.MemAlloced);     // Total bytes allocated since startup
	rtp_printf("Total bytes freed since startup %d\n",Stats.MemFreed);       // Total bytes freed since startup
	rtp_printf("Number of times a memory allocation failed %d\n",Stats.MemClipped);     // Number of times a memory allocation fails.  HEREHERE - Add failur diagnostic
	rtp_printf("Current sequence number. %d\n",Stats.MallocSequence); // Current sequence number, keep this around if you with to later on report on a range of memory allocattion calls starting from this point.
}

/*****************************************************************************/
// wcPrintMemChangeReport
/**

 @memo  Print changes to the heap since the watchpoint was established.
 @doc   Print changes to the heap since the watchpoint was established by calling \Ref{wcMemDbGetWatchPoint}.

 <i><b>Note:</b> Note: RTP_TRACK_LOCAL_MEMORY must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.<br>
 <b>Note:</b> rtplatform's rtp_printf function must be functional to see the output. rtp_printf can be enabled by either mapping it to your system's printf() or by using the version supplied with webC and routing the output to an output port through rtp_term_cputs().</i>

 @param  RTPMemBlockHeader *PrevWatchPoint - Print changes since this watch point was established by a call to \Ref{wcMemDbGetWatchPoint}.
 @param  wcBOOL ExcludeNetZeroResults - If wcTRUE only print records of locations that a net change.

<pre>
 Example: Print the changes made to the heap by a fictitious function named MyAppletInit().

{
	RTPMemBlockHeader *myWatch;
	myWatch = wcMemDbGetWatchPoint();
 	MyAppletInit();
	wcPrintMemChangeReport(myWatch, wcTRUE); // Print the report, the second argument instructs it to print only locations with outstanding memory allocations.
	wcMemDbReleaseReport(myWatch);
}

<i>Note:</i>
<ul>
<li>The first column contains the file name.</li>
<li>The second column contains line number.</li>
<li>The third column is the net change in count of allocation or free calls. frees are -1 allocations are plus one, so a negative number means more calls to release that to alloc were made since the watchpoint was established.</li>
<li>The fourth column is the net change in memory allocated from that location and frees of that memory. Frees are -subtracted, so a negative number means a net release of memory occurred since the watchpoint was established.</li>
<li>note: If ExcludeNetZeroResults is wcFALSE the fourth column may contain the value zero, indicating that all memory from that location was released. If it is wcTRUE then the fourth column </li>
<li>will never contain because reporting of net zero allocations (round trips) is suppressed.</li>
<li>The fifth column is the comment assoiated with the memory when it was allocated or just {} if the location has no label.</li>
</ul>

The above call would result in a report looking like the following:

wtedstr.cpp                    481  1    386      bytes { }
fbabstractwindowmanager.cpp    881  2    46       bytes {VectorAddNode}
htmlbrow.cpp                   1011 1    2        bytes {webc_malloc_string_copy}
htmldoc.cpp                    834  1    2        bytes {webc_malloc_string_copy}
formatctx.cpp                  97   1   60        bytes {FormatContextFactory}
...
Total bytes currently in use 1149957
Net allocations since watchpoint= 314959

 </pre>


 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/

void wcPrintMemChangeReport(RTPMemBlockHeader *PrevWatchPoint, wcBOOL ExcludeNetZeroResults)
{
MemDbStats Stats;
long size_accounted = 0;
	wcMemDbGetStats(&Stats);


	if (PrevWatchPoint)
	{
	RTPMemBlockHeader *ChangeRecords = wcMemDbGetChangesSinceWatchPoint(PrevWatchPoint);
	RTPMemBlockHeader *p;
		size_accounted = 0;
		rtp_printf("=======Changes Since Last Mark:\n          -File-               Net Calls/Bytes    Comment\n");
		p = ChangeRecords;
		while (p)
		{
			if (!ExcludeNetZeroResults || p->size)
			{
				rtp_printf("%-30.30s %-4d %-4d %-8d bytes {%s} \n", exMemFileName((char *)p->file), p->line,  p->seq, p->size,p->comment ? p->comment : " ");
				size_accounted += p->size;
			}
			p=p->next;
		}
		rtp_printf("Total bytes currently in use %d\n",Stats.MemUsed);
		rtp_printf("Net allocations since watchpoint= %d\n", size_accounted);
		wcMemDbReleaseReport(ChangeRecords);

	}

}


/*****************************************************************************/
// wcPrintMemDetailReport
/**

 @memo  Specify sorting and summary options and print a heap report to the console.
 @doc   The caller supplies parameters that instruct wcPrintMemDetailReport how data is to be summarized and sorted. The report is then printed to the console.<br>
 Note: \Ref{RTP_TRACK_LOCAL_MEMORY} must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.
 <i><b>Note:</b> Note: RTP_TRACK_LOCAL_MEMORY must be enabled in rtplatform/include/rtpmem.h for the function to operate properly, if it is not enabled the function simply return.<br>
 <b>Note:</b> rtplatform's rtp_printf function must be functional to see the output. rtp_printf can be enabled by either mapping it to your system's printf() or by using the version supplied with webC and routing the output to an output port through rtp_term_cputs().</i><br>

 @param  long  startSequence -  You may use the MallocSequence field returned from \Ref{wcMemDbGetStats} to select the first record to process, pass zero to start at the first allocation.
 @param  long  endSequence -   You may use the MallocSequence field returned from \Ref{wcMemDbGetStats} to select the last record to process, pass zero to end at the last allocation.
 @param  MemDbReportSummaryType ReportType - Choose a report type from the list below
 <pre>
&nbsp;&nbsp;DbSummarizeByFile // Print a report containing file name and total memory allocations outstanding for the named file.
&nbsp;&nbsp;DbSummarizeByLine // Print a report containing file name:line number and total memory allocations outstanding at that location the file.
&nbsp;&nbsp;DbSummarizeNone   // Print a report containing every outstanding allocation in the range.
 </pre>
 @param  MemDbReportSortType SortBy - Choose a sort type from the list below
 <pre>
&nbsp;&nbsp;SortNone		// Do not sort the records, best method for DbSummarizeNone report type.
&nbsp;&nbsp;SortBySize		// Sort in descending order by size.
&nbsp;&nbsp;SortByFile		// Sort in ascending order by file name followed by line if DbSummarizeByLine is specified.
 </pre>
@return		none
<pre>
 Example: Print memory allocations totaled by file and print the largest first:
 	wcPrintMemDetailReport(0, 0,  DbSummarizeByFile, SortBySize);
 Example: Print memory allocations total by line print in file:line order:
 	wcPrintMemDetailReport(0, 0,  DbSummarizeByLine, SortByFile);
 Example: Print all new memory allocations performed by a function named MyAppletInit.
 {
 	MemDbStats Stats;

	wcMemDbGetStats(&Stats);
 	MyAppletInit();

 	wcPrintMemDetailReport(Stats.MallocSequence, 0,  DbSummarizeNone, SortNone);
}

<i>Note:</i>
<ul>
<li>The first column contains the file name.</li>
<li>The second column contains line number.</li>
<li>If ReportType is DbSummarizeByFile the third column contains the number of outstanding allocation calls by file surrounded by ().</li>
<li>If ReportType is DbSummarizeByLine the third column contains the number of outstanding allocation calls by file:line surrounded by ().</li>
<li>If ReportType is not DbSummarizeNone the third column contains the sequence number of the individual allocation surrounded by {}.</li>
<li>The fourth column is total outstanding memory allocated from file, line or individual allocation call, for ReportType equal to DbSummarizeByFile, DbSummarizeByLine, or DbSummarizeByNone, repectively.</li>
<li>The fifth column is the comment assoiated with the memory when it was allocated or just {} if the location has no label.</li>
</ul>


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

</pre>


 @see wcMemDbGetStats wcMemDbInstallPeakUsageCallback wcMemDbGetReport wcMemDbGetWatchPoint wcMemDbReleaseReport wcMemDbGetReport wcMemDbGetChangesSinceWatchPoint wcPrintMemStats wcPrintMemChangeReport wcPrintMemDetailReport
 */
/*****************************************************************************/

void wcPrintMemDetailReport(long  startSequence, long  endSequence, MemDbReportSummaryType ReportType, MemDbReportSortType SortBy)
{
MemDbStats Stats;
RTPMemBlockHeader *pReport,*p;
long size_accounted = 0;
int nRecords;
	wcMemDbGetStats(&Stats);

	pReport = p = wcMemDbGetReport(startSequence, startSequence, ReportType, SortBy);
	nRecords = 0;
	size_accounted = 0;
	while (p)
	{
		nRecords++;
		if (ReportType==DbSummarizeNone)
		{
			rtp_printf("%-30.30s %-4d {%-4d} %-8d {%s}\n", exMemFileName((char *)p->file), p->line, p->seq, p->size,p->comment ? p->comment : " ");
		}
		else
		{
			rtp_printf("%-30.30s %-4d (%-4d) %-8d {%s}\n", exMemFileName((char *)p->file), p->line, p->seq, p->size,p->comment ? p->comment : " ");
		}
		size_accounted += p->size;
		p=p->next;
	}
	rtp_printf("Records returned == %d\n", nRecords);
	rtp_printf("Bytes accounted for = %d\n", size_accounted);
	wcMemDbReleaseReport(pReport);
}
static char *exMemFileName(char *fpath)
{
char *plast = fpath;

	while (*fpath)
	{
		if(*fpath == '\\' || *fpath == '/')
		{
			plast = fpath + 1;
		}
		fpath++;
	}
	return(plast);
}
#endif // #if (WEBC_MEMORY_RTPDEBUG)