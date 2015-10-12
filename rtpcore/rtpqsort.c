/*
|  RTPQSORT.C -
| 
|  EBSnet -
| 
|   $Author: shane $
|   $Date: 2004/11/01 17:26:15 $
|   $Name:  $
|   $Revision: 1.1 $
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
#include "stdio.h"
#ifndef rtp_qsort
#include "rtpstr.h"
#endif
#include "rtpmem.h"


/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/
int find_num_of_pivot_points(unsigned char* mapp, int num);
int find_next_pivot_point(unsigned char *mapp, int last_p_point, int num);
int check_mapp_to_finish(unsigned char *mapp, int num);

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
  void *head,                            /** Head of array to sort.		*/
  unsigned long num,                     /** Number of nodes in array.	*/
  unsigned long size,                    /** Size of each node.			*/
  RTP_QSORT_COMPARISON_FN compfunc		 /** Compare function			*/
  )
{
	unsigned char first_time;
	unsigned char *mapp;
	
	
	int pivot_point;

	void *sorted_head;
	void *pivot_value;
	void *val;
	
	int counter, counter1;
	int lessthan_index, greaterthan_index;
	int sections;
	int num_of_p;
	
	int last_p_point, next_p_point;
	
	if ((num <= 0) || (size <= 0))
	  {
		return;
	  }
	
	if ((pivot_value = rtp_malloc(size, 0)) == 0)  /* SPRZ */
	  {
		return;
	  }
	
	if ((val = rtp_malloc(size, 0)) == 0)  /* SPRZ */
	  {
		rtp_free(pivot_value);
		return;
	  }
	
	if ((mapp = (unsigned char *)rtp_malloc(num, 0)) == 0)  /* SPRZ */
	  {
		rtp_free(pivot_value);
		rtp_free(val);
		return;
	  }
	
	if ((sorted_head = rtp_malloc(num*size, 0)) == 0)  /* SPRZ */
	  {
		rtp_free(pivot_value);
		rtp_free(val);
		rtp_free(mapp);
		return;
	  }
	
	rtp_memset(mapp, 0, num*sizeof(unsigned char));
	rtp_memset(sorted_head, 0, size*num);
	
	pivot_point = (unsigned long) (num/2);
	first_time = 1;	
	
	while (!check_mapp_to_finish(mapp, num))
	  {
		num_of_p = find_num_of_pivot_points(mapp, num);
		
		last_p_point = -1;
		
		sections = num_of_p + 1;
		
		for (counter = 0; counter < sections; counter++)
		  {
			
			next_p_point = find_next_pivot_point(mapp, last_p_point+1, num);
			pivot_point = (int)((last_p_point+1)+(next_p_point))/2;
			if (last_p_point+1 < next_p_point) {        /* [Feb/21/2007]ADD */
			
//				if (pivot_point >= num)
//				{
//					pivot_point = num - 1; 
//				}
//				else if (pivot_point < 0)
//				{
//					pivot_point = 0;
//				}
			rtp_memcpy(pivot_value,
					   ((unsigned char*)head + (pivot_point*size)),
					   size);
			
			lessthan_index = last_p_point+1;
			greaterthan_index = 0;
			
			/* Take care of the left side of the array from the pivot point */
			for (counter1 = last_p_point+1; counter1 < next_p_point; counter1++)
			  {
//					if (lessthan_index >= num)
//						lessthan_index = num-1;
			  //val = ((int*)head)[counter1]; 
				rtp_memcpy(val,
						   ((unsigned char*)head + (counter1*size)),
						   size);
				
				if ( (compfunc(val,pivot_value) <= 0) && (counter1 != pivot_point))
				  {
				  //sorted_head[lessthan_index] = ((int*)head)[counter1];
					rtp_memcpy((unsigned char *)sorted_head + (lessthan_index*size), 
							   ((unsigned char *)head + (counter1*size)),
							   size);
					lessthan_index++;
				  }
			  }
			
		  /* Add the pivot point value permanently to the final array */
		  //sorted_head[lessthan_index] = pivot_value;
			rtp_memcpy((unsigned char *)sorted_head + (lessthan_index*size),
					   pivot_value,
					   size);
			
			/* Mark the index so that we know this is the final position of
			   the pivot point value */
			mapp[lessthan_index] = 1;
			greaterthan_index = lessthan_index + 1;
			
			/* Take care of the right side of the array from the pivot point */
			for (counter1 = last_p_point+1; counter1 < next_p_point; counter1++)
			{
//					if (greaterthan_index >= num)
//						greaterthan_index = num-1;
			  //*val = ((int*)head)[counter1];
			  rtp_memcpy(val,
						 ((unsigned char*)head + (counter1*size)),
						 size);
			  if ( (compfunc(val,pivot_value)>0) && (counter1 != pivot_point))
				{
				  //sorted_head[greaterthan_index] = ((int*)head)[counter1];
				  rtp_memcpy((unsigned char *)sorted_head + (greaterthan_index*size), 
							 ((unsigned char *)head + (counter1*size)),
							 size);
				  greaterthan_index++;
				}
			}
			}   /* [Feb/21/2007]ADD */
			/* Advance to the next section */
			last_p_point = next_p_point;
		  }
		
		/* copy back the semi-sorted array */
		rtp_memcpy(head, sorted_head, num*size);
	  }
	/* copy the fully sorted array */
	rtp_memcpy(head,sorted_head,num*size);
	
	rtp_free(sorted_head);
	rtp_free(mapp);
	rtp_free(val);
	rtp_free(pivot_value);
}

/*----------------------------------------------------------------------*
                             find_num_of_pivot_points
 *----------------------------------------------------------------------*/
int find_num_of_pivot_points(unsigned char* mapp, int num)
{
	int counter;
	int num_of_p = 0;

	for (counter = 0; counter < num; counter++)
	{
		if(mapp[counter] == 1)
		{
			num_of_p++;
		}
	}

	return num_of_p;
}


/*----------------------------------------------------------------------*
                             find_next_pivot_point
 *----------------------------------------------------------------------*/
int find_next_pivot_point(unsigned char *mapp, int last_p_point, int num)
{
	int counter;

	for (counter = last_p_point; counter < num; counter++)
	{
		if(mapp[counter] == 1)
		{
			return counter;
		}
	}
	return (num);
}

/*----------------------------------------------------------------------*
                             check_mapp_to_finish
 *----------------------------------------------------------------------*/
int check_mapp_to_finish(unsigned char *mapp, int num)
{
	int counter;
	for (counter = 0 ; counter < num; counter++)
	{
		if (mapp[counter] == 0)
		{
			return 0;
		}
	}
	
	return 1;
}

#endif
