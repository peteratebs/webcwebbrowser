#ifndef __STRING_BUILDER_HPP__
#define __STRING_BUILDER_HPP__

/*****************************************************************************/
// Include files
/*****************************************************************************/

#include "util.h"


#ifdef __cplusplus

/*****************************************************************************/
// class StringBuilder
/*****************************************************************************/

class StringBuilder
{
public:
	virtual ~StringBuilder() {}
	void BuildString(WEBC_CHAR *pStringBuffer);
	WEBC_CHAR *MallocString(char *file, long line);
	void  FreeString(WEBC_CHAR *str, char *file, long line);
	
	virtual long GetStringLength(void) = 0;
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer) = 0;
};


/*****************************************************************************/
// class StringFilter
/*****************************************************************************/

class StringFilter : public StringBuilder
{
public:
	virtual void SetString(const WEBC_CHAR *pStr) = 0;
};


/*****************************************************************************/
// class FilteredStringBuilder
/*****************************************************************************/

class FilteredStringBuilder : public StringBuilder
{
public:
	FilteredStringBuilder(StringBuilder *pBuilder, StringFilter *pFilter);
	virtual ~FilteredStringBuilder();
	
	virtual void SetBuilder(StringBuilder *pBuilder);
	virtual void SetFilter(StringFilter *pFilter);
	virtual long GetStringLength(void);
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);
	
protected:
	StringBuilder *mpBuilder;
	StringFilter  *mpFilter;
	WEBC_CHAR     *mpTempBuffer;
};


/*****************************************************************************/
// class AggregateStringBuilder
/*****************************************************************************/

struct StringBuilderInfo
{
	StringBuilder *pBuilder;
	WEBC_UINT8 toDelete;
};

/*****************************************************************************/

class AggregateStringBuilder : public StringBuilder
{
public:
	AggregateStringBuilder(long iArraySizeHint = 10);
	virtual ~AggregateStringBuilder(void);
	
	int Add(StringBuilder *pPart, int toDelete = 0);
	void Reset(void);
	
	virtual long GetStringLength(void);
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);

private:
	StringBuilderInfo *mpParts;
	long miNumParts;
	long miPartsSize;
	long miStringLength;
};


/*****************************************************************************/
// class DecimalStringBuilder
/*****************************************************************************/

class DecimalStringBuilder : public StringBuilder
{
public:
	DecimalStringBuilder();
	DecimalStringBuilder(long i);
	virtual ~DecimalStringBuilder(void);

	virtual long GetStringLength(void);
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);

protected:
	WEBC_CHAR mpStr[13];
	long miStrLen;
};


/*****************************************************************************/
// class RomanNumeralBuilder
/*****************************************************************************/

class RomanNumeralBuilder : public AggregateStringBuilder
{
public:
	RomanNumeralBuilder(long i);
	virtual ~RomanNumeralBuilder(void);

	void SetNum(long i);
};


/*****************************************************************************/
// class HexStringBuilder
/*****************************************************************************/

class HexStringBuilder : public DecimalStringBuilder
{
public:
	HexStringBuilder(long i);
	virtual ~HexStringBuilder(void);
};


/*****************************************************************************/
// class StringCopyBuilder
/*****************************************************************************/

class StringCopyBuilder : public StringFilter
{
public:
	StringCopyBuilder(const WEBC_CHAR *str=0);
  #if (WEBC_SUPPORT_UNICODE)
	StringCopyBuilder(const char *str);
  #endif
	virtual ~StringCopyBuilder(void);

	virtual void SetString(const WEBC_CHAR *pStr);
  #if (WEBC_SUPPORT_UNICODE)
	virtual void SetString(const char *pStr);
  #endif
	virtual long GetStringLength(void);
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);

protected:
	const WEBC_CHAR *mpStr;
  #if (WEBC_SUPPORT_UNICODE)
	const char *mpStr8;
  #endif
	long miStrLen;
};


/*****************************************************************************/
// class ToUpperCaseBuilder
/*****************************************************************************/

class ToUpperCaseBuilder : public StringCopyBuilder
{
public:
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);
};


/*****************************************************************************/
// class ToLowerCaseBuilder
/*****************************************************************************/

class ToLowerCaseBuilder : public StringCopyBuilder
{
public:
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);
};


/*****************************************************************************/
// class IPStringBuilder
/*****************************************************************************/

class IPStringBuilder : public AggregateStringBuilder
{
public:
	IPStringBuilder(WEBC_UINT8 *ipaddr);
	virtual ~IPStringBuilder(void);

	void SetIpAddr(WEBC_UINT8 *ipaddr);
};


/*****************************************************************************/
// class DisplayStringFilter
/*****************************************************************************/

class DisplayStringFilter : public StringFilter
{
public:
	DisplayStringFilter(WEBC_CHAR *pStr = 0);
	virtual ~DisplayStringFilter(void);
	
	virtual long GetStringLength(void);
	virtual void BuildStringUT(WEBC_CHAR *pStringBuffer);
	virtual void SetString(const WEBC_CHAR *pStr);

protected:
	const WEBC_CHAR *mpStr;
	long miLength;
};

#endif // __cplusplus

#endif // __STRING_BUILDER_HPP__
