//
// STRBLD.CPP - 
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Include files
/*****************************************************************************/
#include "strbld.hpp"
//#include "webload.hpp"
#include "wcconst.hpp"
#include "html.h"
#include "webcstr.h"
#include "webcmem.h"

/*****************************************************************************/
// class StringBuilder
/*****************************************************************************/

void StringBuilder::BuildString(WEBC_CHAR *pStringBuffer)
{
	BuildStringUT(pStringBuffer);
	pStringBuffer[GetStringLength()] = (WEBC_CHAR) 0;
}

/*****************************************************************************/

WEBC_CHAR *StringBuilder::MallocString(char *file, long line)
{
	WEBC_CHAR *str = (WEBC_CHAR *) WEBC_DEBUG_MALLOC(sizeof(WEBC_CHAR) * (GetStringLength() + 1), file, line,"MallocString", 0);
	if (str)
	{
		BuildString(str);
	}
	return str;
}

/*****************************************************************************/

void StringBuilder::FreeString(WEBC_CHAR *str, char *file, long line)
{
	WEBC_DEBUG_FREE(str, file, line);
}


/*****************************************************************************/
// class FilteredStringBuilder
/*****************************************************************************/

FilteredStringBuilder::FilteredStringBuilder(StringBuilder *pBuilder, StringFilter *pFilter)
{
	mpBuilder = 0;
	mpFilter = 0;
	mpTempBuffer = 0;
	
	SetBuilder(pBuilder);
	SetFilter(pFilter);
}	

/*****************************************************************************/

FilteredStringBuilder::~FilteredStringBuilder()
{
	SetBuilder(0);
}

/*****************************************************************************/

void FilteredStringBuilder::SetBuilder(StringBuilder *pBuilder)
{
	if (mpBuilder && mpTempBuffer)
	{
		mpBuilder->FreeString(mpTempBuffer, __FILE__, __LINE__);
		mpTempBuffer = 0;
	}
	mpBuilder = pBuilder;
	if (mpBuilder)
	{
		mpTempBuffer = mpBuilder->MallocString(__FILE__, __LINE__);
		if (mpFilter && mpTempBuffer)
		{
			mpFilter->SetString(mpTempBuffer);
		}
	}
	else
	{
		if (mpFilter)
		{
			mpFilter->SetString(0);
		}
	}
}

/*****************************************************************************/

void FilteredStringBuilder::SetFilter(StringFilter *pFilter)
{
	if (mpFilter)
	{
		mpFilter->SetString(0);
	}
	mpFilter = pFilter;
	if (mpFilter)
	{
		mpFilter->SetString(mpTempBuffer);
	}
}

/*****************************************************************************/

long FilteredStringBuilder::GetStringLength(void)
{
	if (mpFilter)
	{
		return (mpFilter->GetStringLength());
	}
	return (0);
}

/*****************************************************************************/

void FilteredStringBuilder::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
	if (mpFilter)
	{
		mpFilter->BuildStringUT(pStringBuffer);
	}
}
	

/*****************************************************************************/
// class AggregateStringBuilder
/*****************************************************************************/

AggregateStringBuilder::AggregateStringBuilder(long iArraySizeHint)
{
	miStringLength = 0;
	miNumParts = 0;
	miPartsSize = iArraySizeHint;
	mpParts = (StringBuilderInfo *) WEBC_MALLOC(sizeof(StringBuilderInfo) * (miPartsSize));
	if (!mpParts)
	{
		miPartsSize = 0;
	}
}

/*****************************************************************************/

AggregateStringBuilder::~AggregateStringBuilder(void)
{
	Reset();
	
	if (mpParts)
	{
		WEBC_FREE(mpParts);
	}
}

/*****************************************************************************/
	
int AggregateStringBuilder::Add(StringBuilder *pPart, int toDelete)
{
	if (!pPart)
	{
		return (-1);
	}
	
	if (!mpParts)
	{
		if (toDelete)
		{
			WEBC_DELETE(pPart);
		}
		return (-1);
	}

	if (miNumParts + 1 > miPartsSize)
	{
		// Our parts array is too small; re-alloc it
		StringBuilderInfo *pNewParts = (StringBuilderInfo *) WEBC_MALLOC(sizeof(StringBuilderInfo) * (miPartsSize + 5));
		if (pNewParts)
		{
			tc_movebytes((WEBC_PFBYTE) pNewParts, (WEBC_PFBYTE) mpParts, sizeof(StringBuilderInfo) * miNumParts);
			WEBC_FREE(mpParts);
			mpParts = pNewParts;
			miPartsSize += 5;
		}
		else
		{
			return (-1);
		}
	}
	
	mpParts[miNumParts].pBuilder = pPart;
	mpParts[miNumParts].toDelete = toDelete;
	miNumParts++;
	miStringLength += pPart->GetStringLength();
	
	return (0);
}

/*****************************************************************************/

void AggregateStringBuilder::Reset(void)
{
long i;

	for (i=0; i<miNumParts; i++)
	{
		if (mpParts[i].toDelete)
		{
			WEBC_DELETE(mpParts[i].pBuilder);
		}
	}

	miNumParts = 0;
	miStringLength = 0;
}

/*****************************************************************************/
	
long AggregateStringBuilder::GetStringLength(void)
{
	return (miStringLength);
}

/*****************************************************************************/

void AggregateStringBuilder::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
long i, iLen = 0;

	for (i=0; i<miNumParts; i++)
	{
		mpParts[i].pBuilder->BuildStringUT(&pStringBuffer[iLen]);
		iLen += mpParts[i].pBuilder->GetStringLength();
	}
}


/*****************************************************************************/
// class DecimalStringBuilder
/*****************************************************************************/

DecimalStringBuilder::DecimalStringBuilder()
{
	miStrLen = 0;
}

DecimalStringBuilder::DecimalStringBuilder(long i)
{
	webc_print_dec(mpStr, i, 1, 12);
	miStrLen = webc_strlen(mpStr);
}

/*****************************************************************************/

DecimalStringBuilder::~DecimalStringBuilder(void)
{
}

/*****************************************************************************/

long DecimalStringBuilder::GetStringLength(void)
{
	return miStrLen;
}

/*****************************************************************************/

void DecimalStringBuilder::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
	tc_movebytes(pStringBuffer, mpStr, GetStringLength()*sizeof(WEBC_CHAR));
}



/*****************************************************************************/
// class HexStringBuilder
/*****************************************************************************/

HexStringBuilder::HexStringBuilder(long i)
{
	webc_print_hex(mpStr, i, 1, 12, 1);
	miStrLen = webc_strlen(mpStr);
}

/*****************************************************************************/

HexStringBuilder::~HexStringBuilder(void)
{
}


/*****************************************************************************/
// class StringCopyBuilder
/*****************************************************************************/

StringCopyBuilder::StringCopyBuilder(const WEBC_CHAR *str)
{
  #if (WEBC_SUPPORT_UNICODE)
	mpStr8 = 0;
  #endif

	SetString(str);
}

#if (WEBC_SUPPORT_UNICODE)
StringCopyBuilder::StringCopyBuilder(const char *str)
{
	SetString(str);
}
#endif

/*****************************************************************************/

StringCopyBuilder::~StringCopyBuilder(void)
{
}

/*****************************************************************************/

void StringCopyBuilder::SetString(const WEBC_CHAR *pStr)
{
	mpStr = pStr;
	miStrLen = pStr? webc_strlen(pStr) : 0;
}

#if (WEBC_SUPPORT_UNICODE)
void StringCopyBuilder::SetString(const char *pStr)
{
	mpStr8 = pStr;
	miStrLen = pStr? tc_strlen(pStr) : 0;
}
#endif

/*****************************************************************************/

long StringCopyBuilder::GetStringLength(void)
{
	return (miStrLen);
}

/*****************************************************************************/

void StringCopyBuilder::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
  #if (WEBC_SUPPORT_UNICODE)
	if (mpStr8)
	{
		const char *copyFrom = mpStr8;
		WEBC_CHAR tempStr[32];
		int charsLeft = GetStringLength(), toCopy;
		while (charsLeft > 0)
		{
			toCopy = EBSMIN(31, charsLeft);
			webc_c_strncpy(tempStr, copyFrom, toCopy);
			tc_movebytes(pStringBuffer, tempStr, toCopy*sizeof(WEBC_CHAR));
			pStringBuffer += toCopy;
			copyFrom += toCopy;
			charsLeft -= toCopy;
		}
	}
	else
	{
		tc_movebytes(pStringBuffer, mpStr, GetStringLength()*sizeof(WEBC_CHAR));
	}
  #else
	tc_movebytes(pStringBuffer, mpStr, GetStringLength()*sizeof(WEBC_CHAR));
  #endif	
}


/*****************************************************************************/
// class IPStringBuilder
/*****************************************************************************/

IPStringBuilder::IPStringBuilder(WEBC_UINT8 *ipaddr)
{
	SetIpAddr(ipaddr);
}

/*****************************************************************************/

IPStringBuilder::~IPStringBuilder(void)
{
}

/*****************************************************************************/

void IPStringBuilder::SetIpAddr(WEBC_UINT8 *ipaddr)
{
	StringBuilder *pDummy;
	
	Reset();
	
	Add(WEBC_NEW(pDummy, DecimalStringBuilder(ipaddr[0])), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_DOT)), 1);
	Add(WEBC_NEW(pDummy, DecimalStringBuilder(ipaddr[1])), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_DOT)), 1);
	Add(WEBC_NEW(pDummy, DecimalStringBuilder(ipaddr[2])), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(WEBC_STR_DOT)), 1);
	Add(WEBC_NEW(pDummy, DecimalStringBuilder(ipaddr[3])), 1);
}

/*****************************************************************************/
// class RomanNumeralBuilder
/*****************************************************************************/

const WEBC_CHAR gpRomanNumLowercaseOnes[10][5] = 
{
	{0}, 
	{'i',0}, 
	{'i','i',0}, 
	{'i','i','i',0}, 
	{'i','v',0}, 
	{'v',0}, 
	{'v','i',0},
	{'v','i','i',0},
	{'v','i','i','i',0},
	{'i','x',0}
};

const WEBC_CHAR gpRomanNumLowercaseTens[10][5] =
{
	{0}, 
	{'x',0},
	{'x','x',0},
	{'x','x','x',0},
	{'x','l',0},
	{'l',0},
	{'l','x',0},
	{'l','x','x',0},
	{'l','x','x','x',0},
	{'x','c',0}
};

const WEBC_CHAR gpRomanNumLowercaseHundreds[10][5] =
{
	{0},
	{'c',0},
	{'c','c',0},
	{'c','c','c',0},
	{'c','d',0},
	{'d',0},
	{'d','c',0},
	{'d','c','c',0},
	{'d','c','c','c',0},
	{'c','m',0}
};

const WEBC_CHAR gpRomanNumLowercaseThousands[5][5] = 
{
	{0},
	{'m',0},
	{'m','m',0},
	{'m','m','m',0},
	{'m','m','m','m',0}
};

/*****************************************************************************/

RomanNumeralBuilder::RomanNumeralBuilder(long i)
{
	SetNum(i);
}

/*****************************************************************************/

RomanNumeralBuilder::~RomanNumeralBuilder(void)
{
}

/*****************************************************************************/

void RomanNumeralBuilder::SetNum(long i)
{
StringBuilder *pDummy;

	Reset();
	
	Add(WEBC_NEW(pDummy, StringCopyBuilder(gpRomanNumLowercaseThousands[EBSMIN((i/1000),4)])), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(gpRomanNumLowercaseHundreds[(i/100)%10])), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(gpRomanNumLowercaseTens[(i/10)%10])), 1);
	Add(WEBC_NEW(pDummy, StringCopyBuilder(gpRomanNumLowercaseOnes[i%10])), 1);
}


/*****************************************************************************/
// class ToUpperCaseBuilder
/*****************************************************************************/

void ToUpperCaseBuilder::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
int n;
	
	for (n=0; n<GetStringLength(); n++)
	{
		pStringBuffer[n] = UPPERCASE(mpStr[n]);
	}
}


/*****************************************************************************/
// class ToLowerCaseBuilder
/*****************************************************************************/

void ToLowerCaseBuilder::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
int n;
	
	for (n=0; n<GetStringLength(); n++)
	{
		pStringBuffer[n] = LOWERCASE(mpStr[n]);
	}
}


/* tbd - implement & use class LazyBuffer

// all lazy buffers are kept track of globally; this allows one to free the memory
//  used by one lazy buffer, but the buffer itself is kept around until the lazy buffer
//  is either re-allocated or deleted (this is why they are called lazy), or another 
//  lazy buffer needs the memory for what it is doing.  Good idea or what?

class LazyBuffer
{
public:
	LazyBuffer(long size, char *file, long line);
	~LazyBuffer();
	
	void Alloc(long size, char *file, long line);   // Alloc will alloc a new buffer, discarding the contents of the old one
	void ReAlloc(long size, char *file, long line); // ReAlloc modifies the size of the buffer, preserving at most size bytes
	                                                //  of the old buffer
	void Free(char *file, long line);
	unsigned char *Bytes();
	void SetBytes(WEBC_UINT8 ch);
	long Size();
};

*/

/*****************************************************************************/
// class DisplayStringFilter
/*****************************************************************************/
DisplayStringFilter::DisplayStringFilter(WEBC_CHAR *pStr)
{
	SetString(pStr);
	miLength = -1;
}

DisplayStringFilter::~DisplayStringFilter(void)
{
}

long DisplayStringFilter::GetStringLength(void)
{
	if (miLength < 0)
	{
		const WEBC_CHAR *pStr;
		
		miLength = 0;
		
		for (pStr = mpStr; pStr; pStr++)
		{
			switch (*pStr)
			{
			case '\r':
			case '\n':
			case '\t':
				break;
				
			default:
				miLength++;
				break;
			}
		}
	}

	return (miLength+1);
}

void DisplayStringFilter::BuildStringUT(WEBC_CHAR *pStringBuffer)
{
	const WEBC_CHAR *pStr = mpStr;
	
	while (*pStr)
	{
		switch (*pStr)
		{
		case '\r':
		case '\n':
		case '\t':
			break;

		case (WEBC_CHAR) HTML_NBSP_CHAR:
			*(pStringBuffer++) = ' ';
			break;
						
		default:
			*(pStringBuffer++) = *pStr;
			break;
		}
		
		pStr++;
	}
}

void DisplayStringFilter::SetString(const WEBC_CHAR *pStr)
{
	mpStr = pStr;
}

	
