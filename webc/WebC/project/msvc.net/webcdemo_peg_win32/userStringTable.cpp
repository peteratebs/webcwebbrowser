/*
|  userStringTable.cpp
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "peg.hpp"

/*---------------------------------------------------------------*/
ROMDATA PEGCHAR gsIDL_ENG_TITLE[] =
{   0x0048, 0x0065, 0x006c, 0x006c, 0x006f, 0x0020, 0x0057, 0x006f,
    0x0072, 0x006c, 0x0064, 0};
ROMDATA PEGCHAR gsIDL_ENG_MESSAGE[] =
{   0x004d, 0x0079, 0x0020, 0x0046, 0x0069, 0x0072, 0x0073, 0x0074,
    0x0020, 0x0045, 0x0046, 0x0049, 0x0020, 0x0041, 0x0070, 0x0070,
    0x006c, 0x0069, 0x0063, 0x0061, 0x0074, 0x0069, 0x006f, 0x006e,
    0x0021, 0};

/*---------------------------------------------------------------*/
ROMDATA PEGCHAR *IDL_ENGLISH_Table[] = {
    gsIDL_ENG_TITLE,
    gsIDL_ENG_MESSAGE
};


/*---------------------------------------------------------------*/
ROMDATA PEGCHAR **wbStringTable[1] = {
    IDL_ENGLISH_Table
};

/*---------------------------------------------------------------*/
