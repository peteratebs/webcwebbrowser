/*
|  UTIL.C -
| 
|  EBS - WebC
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "util.h"
#include "webcstr.h"
#include "rtpprint.h"
#include "rtpdate.h"

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

int GetSum(DISPLAY_INT *array, int start, int end)
{
	int total=0,index;
	
	for (index=start; index<end; index++)
		total += array[index];
		
	return (total);
}

int GetMax(DISPLAY_INT *array, int start, int end)
{
	int value,index;
	
	value = array[start];
	for (index=start+1; index<end; index++)
		value = EBSMAX(array[index],value);
		
	return (value);
}

int GetMin(DISPLAY_INT *array, int start, int end)
{
	int value,index;
	
	value = array[start];
	for (index=start+1; index<end; index++)
		value = EBSMIN(array[index],value);
		
	return (value);
}

int GetMinIndex(DISPLAY_INT *array, int start, int end)
{
	int value,index,min;
	
	value = array[start];
	min = start;
	for (index=start+1; index<end; index++)
	{
		if (value > array[index])
		{
			value = array[index];
			min = index;
		}
	}
		
	return (min);
}

void ClipCeiling(DISPLAY_INT *array, int start, int end, int ceiling)
{
	int index;
	
	for (index=start; index<end; index++)
		array[index] = EBSMIN(array[index],ceiling);
}

void ClipFloor(DISPLAY_INT *array, int start, int end, int floor)
{
	int index;
	
	for (index=start; index<end; index++)
		array[index] = EBSMAX(array[index],floor);
}

void DistributeEven(DISPLAY_INT *array, int start, int end, int pool)
{
	int index, portion;
	
	for (index=start; index<end; index++)
	{
		portion = (pool / (end-index));
		array[index] += portion;
		pool -= portion;
	}
}

void DistributeWeighted(DISPLAY_INT *array, int start, int end, int pool)
{
	int index, portion, sum;
	
	sum = GetSum(array, start, end);
	if (sum == 0)
	{
		DistributeEven(array,start,end,pool);
		return;	
	}
	
	for (index=start; index<end; index++)
	{
		if (sum == 0) break;
		portion = (array[index] * pool) / sum;
		sum -= array[index];
		array[index] += portion;
		pool -= portion;
	}
}

void DistributeWeightedStrict(DISPLAY_INT *array, int start, int end, int pool)
{
	int index, portion, sum;
	
	sum = GetSum(array, start, end);

	for (index=start; index<end; index++)
	{
		if (sum == 0) break;
		portion = (array[index] * pool) / sum;
		sum -= array[index];
		array[index] += portion;
		pool -= portion;
	}
}

void VectorSum(DISPLAY_INT *sum, int start, int end, DISPLAY_INT *v1, int v1index, DISPLAY_INT *v2, int v2index)
{
	while (start < end)
	{
		sum[start++] = v1[v1index++] + v2[v2index++];
	}
}

void VectorDiff(DISPLAY_INT *diff, int start, int end, DISPLAY_INT *v1, int v1index, DISPLAY_INT *v2, int v2index)
{
	while (start < end)
	{
		diff[start++] = v1[v1index++] - v2[v2index++];
	}
}

void VectorMax(DISPLAY_INT *max, int start, int end, DISPLAY_INT *v1, int v1index, DISPLAY_INT *v2, int v2index)
{
	while (start < end)
	{
		max[start++] = EBSMAX(v1[v1index], v2[v2index]);
		v1index++;
		v2index++;
	}
}

void VectorMin(DISPLAY_INT *min, int start, int end, DISPLAY_INT *v1, int v1index, DISPLAY_INT *v2, int v2index)
{
	while (start < end)
	{
		min[start++] = EBSMIN(v1[v1index], v2[v2index]);
		v1index++;
		v2index++;
	}
}


long tc_strlistcmp(char *str, const char **list, long size)
{
// do a binary search on the alphabetical list 
int low=0, high=size-1, midpoint, i; // bug fix

	while (low < high)
	{
		midpoint = (high + low) >> 1;
		
		i = 0;
		while (((str[i]) == (list[midpoint][i])) && str[i])
		{
			i++;
		}
	
		if (str[i] == list[midpoint][i])
		{
			return (midpoint);
		}
		
		if ((str[i]) > (list[midpoint][i]))
		{
			if (low == midpoint)
			{
				low = high;
				break;
			}
			low = midpoint;
		}
		else
		{
			if (high == midpoint)
			{
				break;
			}
			high = midpoint;
		}
	}

	i = 0;
	while (((str[i]) == (list[low][i])) && str[i])
	{
		i++;
	}

	if (str[i] == list[low][i])
	{
		return (low);
	}

	return (-1);
}


int ebs_BinarySearch(void *item, void *list, int size, int (*cmp)(void *, void *, int))
{
	int midPoint, compareResult;
	int lowBound = 0, highBound = size-1;
	
	while (lowBound <= highBound)
	{
		midPoint = (lowBound + highBound) >> 1;
		compareResult = cmp(item, list, midPoint);
		
		if (compareResult == 0)
		{
			return (midPoint);
		}
		else if (compareResult > 0)
		{
			lowBound = midPoint + 1;
			continue;
		}
		else
		{
			highBound = midPoint - 1;
			continue;
		}
	}
	
	return (-1);
}

int ebs_LinearSearch(void *item, void *list, int size, int (*cmp)(void *, void *, int))
{
	int n;

	for (n=0; n<size; n++)
	{
		if (cmp(item, list, n) == 0)
		{
			return (n);
		}
	}
	
	return (-1);
}
