/*
|  RTPQSORT.C -
| 
|  EBSnet -
| 
|   $Author: shane $
|   $Date: 2004/10/08 19:28:43 $
|   $Name:  $
|   $Revision: 1.4 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "rtpqsort.h"
#ifndef rtp_qsort
#include "rtpstr.h"
#include "rtpmem.h"
#endif

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
*****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

#ifndef rtp_qsort
/*----------------------------------------------------------------------*
                             rtp_qsort
 *----------------------------------------------------------------------*/
/** @memo   Quick sort using the indicated comparison function.

    @doc    Quick sort using the comparison function supplied 
    'compfunc'. The comparison function must return:<br>
    <pre>    
|		<0  If node1 is less than node2.
|		0   If node1 is equivalent to node2.
|		>0  If node1 is greater than to node2.
	</pre>
             
    @return void
 */
void rtp_qsort (
  void *head,                           /** Head of array to sort. */
  unsigned long num,                    /** Number of nodes in array. */
  unsigned long size,                   /** Size of each node. */
  RTP_QSORT_COMPARISON_FN compfunc      /** Comparison function. */
  )
{
long   curRightPos;
long   curLeftPos;
void * medianNode;
void * tailNode;
void * tmpNode;
void * headNode      = head;
long   maxPartitions = num - 1;
    
    while (maxPartitions > 0)
    {
        curRightPos   = num - 1;
        curLeftPos    = 0;
        medianNode    = (void *) (((unsigned long)headNode) + (((num - 1) / 2) * size));
        tailNode      = (void *) (((unsigned long)headNode) + ((num - 1) * size));

        /* --------------------------- */
        /*  Find next median using the */
        /*  'median of three' method.  */
        /* --------------------------- */
        if (compfunc ((const void *)headNode, (const void *)medianNode) < 0)
        {
            if (compfunc ((const void *)medianNode, (const void *)tailNode) >= 0)
            {
                if (compfunc ((const void *)headNode, (const void *)tailNode) < 0)
                {
                    medianNode = tailNode;
                }
                else
                {
                    medianNode = headNode;
                }
            }
        }
        else
        {
            if (compfunc ((const void *)medianNode, (const void *)tailNode) <= 0)
            {
                if (compfunc ((const void *)headNode, (const void *)tailNode) < 0)
                {
                    medianNode = headNode;
                }
                else
                {
                    medianNode = tailNode;
                }
            }
        }
        
        tmpNode = rtp_malloc (size);
        if (!tmpNode)
        {
            /* --------------------------- */
            /*  Memory allocation failure. */
            /* --------------------------- */
            return;
        }

        /* --------------------------- */
        /*  Find two discrepancies on  */
        /*  either side of the         */
        /*  partition that should be   */
        /*  swapped.                   */
        /* --------------------------- */

        while (1)
        {
            /* --------------------------- */
            /*    From right to median.    */
            /* --------------------------- */
            while (compfunc ((const void *) (((long)head) + (curRightPos * size)), (const void *)medianNode) > 0)
			{
                curRightPos--;
            }
            
            /* --------------------------- */
            /*     From left to median.    */
            /* --------------------------- */
            while (compfunc ((const void *) (((long)head) + (curLeftPos * size)), (const void *)medianNode) < 0)
            {
                curLeftPos++;
            }
            
            if (curLeftPos >= curRightPos)
            {
                /* --------------------------- */
                /*     No more swaps needed.   */
                /* --------------------------- */
                break;
            }
            
            /* --------------------------- */
            /*  Swap the nodes found out   */
            /*  of order on either side of */
            /*  the median node.           */
            /* --------------------------- */
            rtp_memcpy (tmpNode,
					   (const void *) (((long)head) + (curLeftPos * size)),
					   size);
            rtp_memcpy ((void *) (((long)head) + (curLeftPos * size)),
					   (const void *) (((long)head) + (curRightPos * size)),
					   size);
            rtp_memcpy ((void *) (((long)head) + (curRightPos * size)),
					   (const void *) tmpNode,
					   size);
        }
        
        rtp_free (tmpNode);
        
        /* --------------------------- */
        /* Recursively sort left side. */
        /* --------------------------- */
        rtp_qsort (head, (curRightPos + 1), size, compfunc);
        
        /* --------------------------- */
        /* Iteratively sort right side */
        /* --------------------------- */
        headNode = (void *) (((long)head) + ((curRightPos + 1) * size));
        num = num - (curRightPos + 1);
        maxPartitions = num - 1;
    }
}
#endif
