//
// PalleteBuilder.cpp - WebC Screen Builder
//
// EBS - WebC
//
// Copyright EBS Inc.
//
// Module description:
//

#include "../include/NewApI.H"
#include "../include/AppBuilder.h"


char *webc_strmalloc(char *str)
{
	char *rstr = 0 ;
	if (str)
	{
		rstr = (char *)rtp_malloc(rtp_strlen(str)+1);
		rtp_strcpy(rstr, str);
	}
	return rstr;
}

