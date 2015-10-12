//
// XML.H - Structs, prototypes, and defines for XML.C
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __XML_H__
#define __XML_H__

/*****************************************************************************
 * Defines
 *****************************************************************************/

#define XML_MAX_ATTRIBS 16

#if (WEBC_SUPPORT_UNICODE)
#define XML_UNICODE
#endif

/*****************************************************************************
 * Types
 *****************************************************************************/

#ifdef XML_UNICODE
 typedef unsigned short XMLchar;
#else
 typedef char XMLchar;
#endif

typedef int XML_BOOL;


typedef struct s_XMLNameParse        XMLNameParse;
typedef struct s_XMLAttribValueParse XMLAttribValueParse;
typedef struct s_XMLAttribParse      XMLAttribParse;
typedef struct s_XMLAttribListParse  XMLAttribListParse;
typedef struct s_XMLTagParse         XMLTagParse;
typedef struct s_XMLCommentParse     XMLCommentParse;
typedef struct s_XMLTextParse        XMLTextParse;
typedef struct s_XMLCharDataParse    XMLCharDataParse;
typedef struct s_XMLParseContext     XMLParseContext;
typedef struct s_XMLUnitParse        XMLUnitParse;

enum e_XMLTagType
{
	XML_TAG_START,
	XML_TAG_END,
	XML_TAG_EMPTY,
	XML_TAG_PARTIAL,
	XML_TAG_MALFORMED
};

enum e_XMLUnitType
{
	XML_UNIT_TEXT,
	XML_UNIT_TAG,
	XML_UNIT_COMMENT,
	XML_UNIT_UNKNOWN,
	XML_UNIT_CDATA
};

enum e_XMLParseState
{
	XML_STATE_NORMAL,
	XML_STATE_IN_CDATA,
	XML_STATE_IN_COMMENT,
	XML_STATE_IN_UNKNOWN
};

typedef enum e_XMLTagType            XMLTagType;
typedef enum e_XMLUnitType           XMLUnitType;
typedef enum e_XMLParseState         XMLParseState;

struct s_XMLNameParse
{
	XMLchar*                   str;
	long                       len;
	XMLchar                    delimiter;
};

struct s_XMLAttribValueParse
{
	XMLchar*                   str;
	long                       len;
	XMLchar                    delimiter;
};

struct s_XMLAttribParse
{
	XMLNameParse               name;
	XMLAttribValueParse        value;
	int                        id;
};

struct s_XMLAttribListParse
{
	XMLAttribParse             array[XML_MAX_ATTRIBS];
	long                       count;

};

struct s_XMLTagParse
{
	XMLTagType                 type;
	XMLNameParse               name;
	XMLAttribListParse         attribList;
};

struct s_XMLCommentParse
{
	XMLchar*                   str;
	long                       len;
	XMLchar                    delimiter;
};

struct s_XMLTextParse
{
	XMLchar*                   str;
	long                       len;
	XMLchar                    delimiter;
};

struct s_XMLCharDataParse
{
	XMLchar*                   str;
	long                       len;
	XMLchar                    delimiter;
};


// description of a string or tag which is a unit in the document;
// returned by html_parse_unit()
struct s_XMLUnitParse
{
	XMLUnitType type;
	union
	{
		XMLTextParse     text;
		XMLTagParse      tag;
		XMLCommentParse  comment;
		XMLCharDataParse cdata;
	} parse;
};


// *********************************************************************
// PARSER CONTEXT
// *********************************************************************

struct s_XMLParseContext
{
	XMLParseState              state;
	unsigned                   allowAnyNameStartChar;
	unsigned                   allowCloseTagAttribLists;
	unsigned                   allowMissingTagGreaterThan;
	unsigned                   bracketNesting;
	unsigned                   caseSensitive;
	unsigned                   ignoreBadAttribListChars;
	unsigned                   ignoreMalformedAttribs;
	unsigned                   parseCharData;
};

#ifdef __cplusplus
extern "C" {
#endif

// *********************************************************************
// Methods
// *********************************************************************

void XML_InitParse (
		XMLParseContext* ctx
	);

long XML_ParseSpace (
		const XMLchar* buffer,
		long bufferLen
	);

long XML_ParseAttribListSpace (
		XMLParseContext* ctx,
		const XMLchar *buffer, 
		long bufferLen
	);

long XML_ParseName (
		XMLParseContext* ctx,
		XMLNameParse* name,
		const XMLchar* buffer,
		long bufferLen
	);

long XML_ParseAttribValue (
		XMLParseContext* ctx,
		XMLAttribValueParse* value,
		const XMLchar* buffer,
		long bufferLen
	);

long XML_ParseAttrib (
		XMLParseContext* ctx,
		XMLAttribParse* attrib,
		const XMLchar* buffer,
		long bufferLen
	);

long XML_ParseAttribList (
		XMLParseContext* ctx,
		XMLAttribListParse* list,
		const XMLchar* buffer,
		long bufferLen
	);

long XML_ParseTag (
		XMLParseContext* ctx,
		XMLTagParse* parse,
		const XMLchar* buffer,
		long bufferLen
	);

long XML_ParseTagCDATA (
		XMLParseContext* ctx,
		XMLUnitParse* unit,
		const XMLchar* tagName,
		const XMLchar* buffer,
		long bufferLen,
		XML_BOOL caseSensitive,
		XML_BOOL parseComments
	);

long XML_ParseUnit (
		XMLParseContext* ctx,
		XMLUnitParse* unit,
		const XMLchar* buffer,
		long bufferLen
	);

void XML_TokenizeStrings (
		XMLUnitParse* unit
	);

void XML_RestoreStrings (
		XMLUnitParse* unit
	);

#ifdef XML_UNICODE
XMLAttribValueParse* XML_GetTagAttribute (
		XMLTagParse* parse,
		const XMLchar* attribName,
		XML_BOOL caseSensitive
	);
#else
#define XML_GetTagAttribute(P,A,C) XML_GetTagAttributeASCII(P,A,C)
#endif

XMLAttribValueParse* XML_GetTagAttributeASCII (
		XMLTagParse* parse,
		const char* attribName,
		XML_BOOL caseSensitive
	);
	
#ifdef __cplusplus
}
#endif

#endif		// __XML_H__

// END XML.H
