/*
|  RTPBSEARCH.C -
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

#include "rtpbsearch.h"

/*****************************************************************************/
/* Macros */
/******************************************************************************/

/*****************************************************************************/
/* Types */
/*****************************************************************************/

/*****************************************************************************/
/* Function Prototypes */
/*****************************************************************************/

/*****************************************************************************/
/* Data */
/******************************************************************************/

/*****************************************************************************/
/* Function Definitions */
/*****************************************************************************/

#ifndef rtp_bsearch
/*----------------------------------------------------------------------*
                             rtp_bsearch
 *----------------------------------------------------------------------*/
/** @memo   Binary search of an already sorted array.

    @doc    Binary search using the comparison function supplied 
    'compfunc'. The comparison function must return:<br>
    <pre>    
		<0  If node1 is less than node2.
		0   If node1 is equivalent to node2.
		>0  If node1 is greater than to node2.
	</pre>
             
    @return Pointer to the found node within the array,
    0 if not found or error.
 */
void * rtp_bsearch (
  const void *obj,                      /** Object that is to be found. */
  const void *head,                     /** Head of array to search. */
  unsigned int num,                     /** Number of nodes in array. */
  unsigned int size,                    /** Size of each node. */
  RTP_BSEARCH_COMPARISON_FN compfunc    /** Comparison function. */
  )
{
long curRightPos = num - 1;
long curLeftPos  = 0;
long medianIndex = 0;
int  result      = 0;

    while (curLeftPos <= curRightPos)
    {
        medianIndex = (curRightPos + curLeftPos) / 2;
        result = compfunc ((const void *) (((long)head) + (medianIndex * size)), obj);
        
        if (result < 0)
        {
            curLeftPos = medianIndex + 1;
        }
        else if (result > 0)
        {
            curRightPos = medianIndex - 1;
        }
        else
        {
            return ((void *) (((long)head) + (medianIndex * size)));
        }
    }
    return (0);
}
#endif
