//
// XML.C - XML parsing routines
//
// EBS - WebC
//
// Copyright EBS Inc. , 2003
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains routines to parse XML text.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "xml.h"
#include "xmlutil.h"

#define JANUARY_17_08_FIXES
#define PRINT_PARSE_PROGRESS 0
#ifdef JANUARY_17_08_FIXES
long problemcount = 0;
#endif

/*****************************************************************************/
// Macros
/*****************************************************************************/

/* as defined by the XML 1.0 spec:
tbd -
Extender      ::= 0x00B7 |
                  0x02D0 |
                  0x02D1 |
                  0x0387 |
                  0x0640 |
                  0x0E46 |
                  0x0EC6 |
                  0x3005 |
                  [0x3031-0x3035] |
                  [0x309D-0x309E] |
                  [0x30FC-0x30FE]
*/

#define XML_WHITESPACE(C)      ((C)==' ' || (C)=='\t' || (C)=='\n' || (C)=='\r')

#define XML_DIGIT(C)           XML_IsDigit(C)

#define XML_COMBINING_CHAR(C)  XML_IsCombiningChar(C)

#define XML_EXTENDER(C)        0

#define XML_BASE_CHAR(C)       XML_IsBaseChar(C)

#define XML_IDEOGRAPHIC(C)     (((C)>=0x4E00 && (C)<=0x9FA5) || \
                                ((C)>=0x3021 && (C)<=0x3029) || \
                                 (C)==0x3007)

#define XML_LETTER(C)          (XML_BASE_CHAR(C) || XML_IDEOGRAPHIC(C))

#define XML_NAME_CHAR(C)       (XML_LETTER(C) || XML_DIGIT(C) || (C)=='.' || \
                                (C)=='-' || (C)=='_' || (C)==':' ||          \
                                XML_COMBINING_CHAR(C) || XML_EXTENDER(C))

#define XML_NAME_START_CHAR(C) (XML_LETTER(C) || (C)=='_' || (C)==':')

#if(PRINT_PARSE_PROGRESS)

#define XML_TOKENIZE(S)        if ((S).str) \
                               { \
                                    printf("Tok delim=%x l = %d, S = [%S]\n", (S).delimiter, (S).len, (S).str);\
                                    (S).delimiter = (S).str[(S).len]; \
                                    (S).str[(S).len] = 0; \
                               }
#define XML_RESTORE(S)         if ((S).str) \
                               { \
                                    printf("Res delim=%x l = %d, S = [%S]\n", (S).delimiter, (S).len, (S).str);\
                                    (S).str[(S).len] = (S).delimiter; \
                               }
#else
#define XML_TOKENIZE(S)        if ((S).str) \
                               { \
                                    (S).delimiter = (S).str[(S).len]; \
                                    (S).str[(S).len] = 0; \
                               }

#define XML_RESTORE(S)         if ((S).str) \
                               { \
                                    (S).str[(S).len] = (S).delimiter; \
                               }
#endif


#define XML_MAX_BRACKET_NESTING  0x7FFF

#include "webcstr.h"

#ifdef XML_UNICODE

#define xml_strcmp     webc_strcmp
#define xml_strncmp    webc_strncmp
#define xml_strlen     webc_strlen
#define xml_strnicmp   webc_strnicmp
#define xml_stricmp    webc_stricmp
#define xml_c_strnicmp webc_c_strnicmp

#else

#define xml_strcmp     tc_strcmp
#define xml_strncmp    tc_strncmp
#define xml_strlen     tc_strlen
#define xml_strnicmp   tc_strnicmp
#define xml_stricmp    tc_stricmp
#define xml_c_strnicmp tc_strnicmp

#endif

#define XML_STR_END_COMMENT_LEN   3

#define XML_SCAN_COMPLETE     0
#define XML_SCAN_MISMATCH     1
#define XML_SCAN_PARTIAL      2

/*****************************************************************************/
// Prototypes
/*****************************************************************************/

// returns: complete, mismatch, incomplete
int _XML_ScanString (
		const XMLchar* sourceData,
		long sourceLen,
		const XMLchar* str
	);

long _XML_ParseJunkAttrib (
		const XMLchar *buffer,
		long bufferLen
	);

unsigned _XML_NameStartChar (
		XMLParseContext *ctx,
		XMLchar c
	);

/*****************************************************************************/
// Data
/*****************************************************************************/

const XMLchar gpXMLStrBeginComment[]  = {'-','-',0};
const XMLchar gpXMLStrEndComment[]    = {'-','-','>',0};
const XMLchar gpXMLStrBeginCharData[] = {'[','C','D','A','T','A','[',0};
const XMLchar gpXMLStrEndCharData[]   = {']',']','>',0};

/*****************************************************************************/
// Functions
/*****************************************************************************/


void XML_InitParse (XMLParseContext *ctx)
{
	ctx->state = XML_STATE_NORMAL;
	ctx->allowAnyNameStartChar = 0;
	ctx->allowCloseTagAttribLists = 0;
	ctx->allowMissingTagGreaterThan = 1;
	ctx->bracketNesting = 0;
	ctx->caseSensitive = 1;
	ctx->ignoreBadAttribListChars = 0;
	ctx->ignoreMalformedAttribs = 0;
	ctx->parseCharData = 1;
}


/*-------------------------------------------------------------------------
  XML_ParseSpace - return number of whitespace chars
--------------------------------------------------------------------------*/

long XML_ParseSpace (const XMLchar *buffer, long bufferLen)
{
long processed = 0;

	while ((bufferLen > 0) && XML_WHITESPACE(*buffer))
	{
		buffer++;
		bufferLen--;
		processed++;
	}

	return (processed);
}

/*-------------------------------------------------------------------------
  XML_ParseAttribListSpace -
--------------------------------------------------------------------------*/

long XML_ParseAttribListSpace (
		XMLParseContext* ctx,
		const XMLchar *buffer,
		long bufferLen
	)
{
long processed = 0;

	while ((bufferLen > 0) &&
	       (XML_WHITESPACE(*buffer) || *buffer == '\'' || *buffer == '\"' ||
	        (ctx->ignoreBadAttribListChars && *buffer != '<' && *buffer != '>' &&
	         !_XML_NameStartChar(ctx, *buffer))))
	{
		buffer++;
		bufferLen--;
		processed++;
	}

	return (processed);
}

long _XML_ParseJunkAttrib (
		const XMLchar *buffer,
		long bufferLen
	)
{
long processed = 0;

	while ((bufferLen > 0) && !XML_WHITESPACE(*buffer) && *buffer != '<' && *buffer != '>')
	{
		buffer++;
		bufferLen--;
		processed++;
	}

	return (processed);
}


long XML_ParseName (XMLParseContext *ctx, XMLNameParse *name, const XMLchar *buffer, long bufferLen)
{
	name->str = 0;
	name->len = 0;

	if (_XML_NameStartChar(ctx, *buffer) && (bufferLen > 0))
	{
		name->str = (XMLchar*) buffer;
		name->len++;
		buffer++;
		bufferLen--;

		while (XML_NAME_CHAR(*buffer) && (bufferLen > 0))
		{
			name->len++;
			buffer++;
			bufferLen--;
		}
	}

	return (name->len);
}

long XML_ParseAttribValue (XMLParseContext *ctx, XMLAttribValueParse *value, const XMLchar *buffer, long bufferLen)
{
long processed = 0;
XMLchar quoteChar;

	value->str = 0;
	value->len = 0;

	if (bufferLen > 0)
	{
		switch (*buffer)
		{
			case '\"':
			case '\'':
				// quoted value
				quoteChar = *buffer;

				processed++;
				buffer++;
				bufferLen--;

				value->str = (XMLchar*) buffer;
				value->len = 0;

				while ((*buffer != quoteChar) && (bufferLen > 0))
				{
					value->len++;
					processed++;
					buffer++;
					bufferLen--;
				}

				if (*buffer == quoteChar)
				{
					processed++;
				}
				break;

			default:
				// non-quoted value; delimit by whitespace
				if (!XML_WHITESPACE(*buffer) && (bufferLen > 0) && *buffer != '>' && *buffer != '<')
				{
					value->str = (XMLchar*) buffer;

					do
					{
						value->len++;
						processed++;
						buffer++;
						bufferLen--;
					}
					while (!XML_WHITESPACE(*buffer) && (bufferLen > 0) && *buffer != '>' && *buffer != '<');
				}
				break;
		}
	}

	return (processed);
}

long XML_ParseAttrib (XMLParseContext *ctx, XMLAttribParse *attrib, const XMLchar *buffer, long bufferLen)
{
long count;
long processed = 0;

	count = XML_ParseName (ctx, &attrib->name, buffer, bufferLen);
	if (attrib->name.str)
	{
		processed = count;
		buffer += count;
		bufferLen -= count;

		// parse whitespace in between name and '='
		count = XML_ParseSpace(buffer, bufferLen);
		buffer += count;
		bufferLen -= count;

		if (*buffer == '=')
		{
			processed += count;

			// eat up the '='
			buffer++;
			bufferLen--;
			processed++;

			// parse whitespace in between '=' and value
			count = XML_ParseSpace(buffer, bufferLen);

			buffer += count;
			bufferLen -= count;
			processed += count;

			count = XML_ParseAttribValue(ctx, &attrib->value, buffer, bufferLen);

			processed += count;
		}
		else
		{
			attrib->value.str = 0;
		}

		/* if this attribute does not have an '=', then set its value equal to
		   its name (e.g. <td nowrap>, nowrap=nowrap) */
		if (!attrib->value.str)
		{
			attrib->value.str = attrib->name.str;
			attrib->value.len = attrib->name.len;
		}
	}

	return (processed);
}

long XML_ParseAttribList (XMLParseContext *ctx, XMLAttribListParse *list, const XMLchar *buffer, long bufferLen)
{
long processed = 0;
long count;

	list->count = 0;

	count = XML_ParseAttrib(ctx, &list->array[list->count], buffer, bufferLen);
	if (count > 0)
	{
		list->count++;
	}
	else
	{
		if (ctx->ignoreMalformedAttribs && *buffer != '<' && *buffer != '>' && bufferLen > 0)
		{
			count = _XML_ParseJunkAttrib(buffer, bufferLen);
		}
	}

	while ((count > 0) && (bufferLen > 0))
	{
		buffer += count;
		bufferLen -= count;
		processed += count;

		count = XML_ParseAttribListSpace(ctx, buffer, bufferLen);

		buffer += count;
		bufferLen -= count;
		processed += count;

		if (list->count >= XML_MAX_ATTRIBS)
		{
			XMLAttribParse discard;
			count = XML_ParseAttrib(ctx, &discard, buffer, bufferLen);
		}
		else
		{
			count = XML_ParseAttrib(ctx, &list->array[list->count], buffer, bufferLen);
			if (count > 0)
			{
				list->count++;
			}
		}

		if (count == 0)
		{
			if (ctx->ignoreMalformedAttribs && *buffer != '<' && *buffer != '>' && bufferLen > 0)
			{
				count = _XML_ParseJunkAttrib(buffer, bufferLen);
			}
		}
	}

	return (processed);
}

long XML_ParseTag (XMLParseContext *ctx, XMLTagParse *tag, const XMLchar *buffer, long bufferLen)
{
long processed = 0;
long count;
XMLTagType type = XML_TAG_START;

	tag->type = XML_TAG_PARTIAL;
	tag->name.str = 0;
	tag->attribList.count = 0;

	if (*buffer == '<')
	{
		buffer++;
		bufferLen--;
		processed++;

		if (*buffer == '/')
		{
			buffer++;
			bufferLen--;
			processed++;

			type = XML_TAG_END;
		}

		count = XML_ParseName(ctx, &tag->name, buffer, bufferLen);
		if (tag->name.str)
		{
			buffer += count;
			bufferLen -= count;
			processed += count;

			count = XML_ParseAttribListSpace(ctx, buffer, bufferLen);
			if (count > 0)
			{
				buffer += count;
				bufferLen -= count;
				processed += count;
			}

			if (type != XML_TAG_END || ctx->allowCloseTagAttribLists)
			{
				count = XML_ParseAttribList(ctx, &tag->attribList, buffer, bufferLen);
				if (count > 0)
				{
					buffer += count;
					bufferLen -= count;
					processed += count;

					count = XML_ParseAttribListSpace(ctx, buffer, bufferLen);
					if (count > 0)
					{
						buffer += count;
						bufferLen -= count;
						processed += count;
					}
				}

				if (type != XML_TAG_END && *buffer == '/')
				{
					type = XML_TAG_EMPTY;

					buffer++;
					bufferLen--;
					processed++;
				}
			}

			if (*buffer == '>')
			{
				buffer++;
				bufferLen--;
				processed++;

				tag->type = type;
			}
			else
			{
				if (ctx->allowMissingTagGreaterThan && (*buffer == '<'))
				{
					tag->type = type;
				}
				else
				{
					tag->type = XML_TAG_MALFORMED;
				}
			}
		}
		else
		{
			tag->type = XML_TAG_MALFORMED;
		}
	}
	else
	{
		tag->type = XML_TAG_MALFORMED;
	}

	return (processed);
}

#ifdef JANUARY_17_08_FIXES
long XML_ParseTagCDATA (
		XMLParseContext *ctx,
		XMLUnitParse *unit,
		const XMLchar* tagName,
		const XMLchar *buffer,
		long bufferLen,
		XML_BOOL caseSensitive,
		XML_BOOL parseComments
	)
{
	long tagNameLen = xml_strlen(tagName);
	long pos;
	long count;
	int result;
#if(PRINT_PARSE_PROGRESS)
{
XMLchar c;
XMLchar *pbuffer;
    pbuffer = (XMLchar *)buffer;
	c = *(pbuffer+bufferLen);
	*(pbuffer+bufferLen) = 0;
    printf("Parsing CDATA tag == <%S> len == %d contents == \nX>%S<X\n", tagName, bufferLen, pbuffer);
	*(pbuffer+bufferLen) = c;
}
#endif
	// there are 5 cases to handle:
	//   1. "</tagname>..."
	//   2. "...</tagName>..."
	//   3. "</tagn" (truncated close tag, but could be the correct tag name)
	//   4. "...</tagn" (truncated close tag, but could be the correct tag name)
	//   5. none of the above



	if (caseSensitive || 1)
	{
		pos = 0;
		while (pos < bufferLen)
		{
		    if (!buffer[pos])
		    {
#if(PRINT_PARSE_PROGRESS)
                printf("buffer[pos]==0\n");
#endif
                break;
            }
		    if (!buffer[pos+1])
		    {
#if(PRINT_PARSE_PROGRESS)
                printf("buffer[pos+1]==0\n");
#endif
                break;
            }
			if (parseComments && ctx->state == XML_STATE_IN_COMMENT)
			{
				result = _XML_ScanString(buffer + pos, bufferLen - pos, gpXMLStrEndComment); /* --> */

				if (result == XML_SCAN_PARTIAL)
				{
					break;
				}
				else if (result == XML_SCAN_COMPLETE)
				{
					pos += 3;   /* Consume --> */
					ctx->state = XML_STATE_NORMAL;
				}
                else
					pos += 1;  /* Still in comment consume a single character */
			}
			else
			{
				if (buffer[pos] == '<')
				{
					if (pos == bufferLen - 1)
					{
						// if the buffer ends with a '<', then don't include it
						break;
					}
					else
					{   /* </tag> ? */
						if (buffer[pos + 1] == '/')
						{
							if (bufferLen - pos >= tagNameLen + 3)
							{
								if (!xml_strnicmp(&buffer[pos + 2], tagName, tagNameLen) && buffer[pos + 2 + tagNameLen] == '>')
								{
									if (pos == 0)
									{
										count = XML_ParseTag(ctx, &unit->parse.tag, buffer, bufferLen);
										unit->type = XML_UNIT_TAG;
										return (count); /* Consume tag and contents */
									}
									else
									{
										break;
									}
								}
                                else
                                    pos += 1; /* </Notourtag.. consume one char */
							}
							else
							{
								if (bufferLen - pos > 2)
								{
									if (!xml_strnicmp(&buffer[pos + 2], tagName, bufferLen - pos - 2))
									{
										// buffer ends with "</" + some prefix of the tagName
										break;
									}
									else
                                        pos += 1; /* </Notour tag consume one char */
								}
								else
								{
									// buffer ends with "</"
									break;
								}
							}
						}
						else if (buffer[pos + 1] == '!')
                        {
                            if (parseComments)         /* Check for <!-- */
                            {
							    result = _XML_ScanString (buffer + pos + 2, bufferLen - pos - 2, gpXMLStrBeginComment);
							    if (result == XML_SCAN_PARTIAL)
							    {
								    break;
							    }
							    else if (result == XML_SCAN_COMPLETE)
							    {
								    pos += 4; /* Consume <!-- */
								    ctx->state = XML_STATE_IN_COMMENT;
                                }
                                else
								    pos += 1; /* not comment Consume < */
                            }
                            else
                                pos += 1; /* not parsing comments Consume < */
                        }
                        else
                        {
							pos += 1; /* not </ or <! just consume < */
                        }
					}
				}
                else
                {
				    pos += 1; /* < just consume the character */
                }
			}
		}
#if(PRINT_PARSE_PROGRESS)
{
XMLchar c;
XMLchar *pbuffer;
    pbuffer = (XMLchar *)buffer;
	c = *(pbuffer+pos);
	*(pbuffer+bufferLen) = 0;
    printf("Returning UNIT TEXT pos= %d \nY>%S<Y\n", pos, pbuffer);
	*(pbuffer+pos) = c;
}
#endif
		unit->type = XML_UNIT_TEXT;
		unit->parse.text.str = (XMLchar*) buffer;
		unit->parse.text.len = pos;
		return (pos);
	}

	return (0);
}

#else
long XML_ParseTagCDATA (
		XMLParseContext *ctx,
		XMLUnitParse *unit,
		const XMLchar* tagName,
		const XMLchar *buffer,
		long bufferLen,
		XML_BOOL caseSensitive,
		XML_BOOL parseComments
	)
{
	long tagNameLen = xml_strlen(tagName);
	long pos;
	long count;
	int result;

	// there are 5 cases to handle:
	//   1. "</tagname>..."
	//   2. "...</tagName>..."
	//   3. "</tagn" (truncated close tag, but could be the correct tag name)
	//   4. "...</tagn" (truncated close tag, but could be the correct tag name)
	//   5. none of the above



	if (caseSensitive || 1)
	{
		for (pos = 0; (pos < bufferLen) && buffer[pos] && buffer[pos + 1]; pos++)
		{
			if (parseComments && ctx->state == XML_STATE_IN_COMMENT)
			{
				result = _XML_ScanString(buffer + pos, bufferLen - pos, gpXMLStrEndComment); /* --> */

				if (result == XML_SCAN_PARTIAL)
				{
					break;
				}
				else if (result == XML_SCAN_COMPLETE)
				{
					pos += 2;
					ctx->state = XML_STATE_NORMAL;
				}
			}
			else
			{
				if (buffer[pos] == '<')
				{
					if (pos == bufferLen - 1)
					{
						// if the buffer ends with a '<', then don't include it
						break;
					}
					else
					{   /* </tag> ? */
						if (buffer[pos + 1] == '/')
						{
							if (bufferLen - pos >= tagNameLen + 3)
							{
								if (!xml_strnicmp(&buffer[pos + 2], tagName, tagNameLen) && buffer[pos + 2 + tagNameLen] == '>')
								{
									if (pos == 0)
									{
										count = XML_ParseTag(ctx, &unit->parse.tag, buffer, bufferLen);
										unit->type = XML_UNIT_TAG;
										return (count);
									}
									else
									{
										break;
									}
								}
							}
							else
							{
								if (bufferLen - pos > 2)
								{
									if (!xml_strnicmp(&buffer[pos + 2], tagName, bufferLen - pos - 2))
									{
										// buffer ends with "</" + some prefix of the tagName
										break;
									}
								}
								else
								{
									// buffer ends with "</"
									break;
								}
							}
						}
						else if (buffer[pos + 1] == '!' && parseComments)     /* <!-- */
						{
							result = _XML_ScanString (buffer + pos + 2, bufferLen - pos - 2, gpXMLStrBeginComment);

							if (result == XML_SCAN_PARTIAL)
							{
								break;
							}
							else if (result == XML_SCAN_COMPLETE)
							{
								pos += 3;
								ctx->state = XML_STATE_IN_COMMENT;
							}
						}
					}
				}
			}
		}
		unit->type = XML_UNIT_TEXT;
		unit->parse.text.str = (XMLchar*) buffer;
		unit->parse.text.len = pos;
		return (pos);
	}

	return (0);
}
#endif

long XML_ParseUnit (XMLParseContext *ctx, XMLUnitParse *unit, const XMLchar *buffer, long bufferLen)
{
long count;
long processed = 0;
int done;
int result;

	do
	{
		done = 1;

		switch (ctx->state)
		{
			case XML_STATE_NORMAL:
				if (bufferLen > 0 && buffer[0] == '<')
				{
					if (bufferLen > 1 && buffer[1] == '!')
					{
						// "<!" could be a comment, declaration, or CDATA
						if (bufferLen == 2)
						{
							// get this unit in the next buffer segment
							processed = 0;
							break;
						}

						// search for "<!--"
						switch (_XML_ScanString(buffer + 2, bufferLen - 2, gpXMLStrBeginComment))
						{
							case XML_SCAN_COMPLETE:
								ctx->state = XML_STATE_IN_COMMENT;

								// step past the "<!--"
								buffer += 4;
								processed += 4;
								bufferLen -= 4;

								done = 0;
								break;

							case XML_SCAN_PARTIAL:
								processed = 0;
								break;

							case XML_SCAN_MISMATCH:
								// search for "<![CDATA["
								switch (_XML_ScanString(buffer + 2, bufferLen - 2, gpXMLStrBeginCharData))
								{
									case XML_SCAN_COMPLETE:
										if (ctx->parseCharData)
										{
											ctx->state = XML_STATE_IN_CDATA;

											// step past the "<![CDATA["
											buffer += 9;
											processed += 9;
											bufferLen -= 9;
										}
										else
										{
											ctx->state = XML_STATE_IN_UNKNOWN;

											// step past "<!"
											buffer += 2;
											processed += 2;
											bufferLen -= 2;
										}

										done = 0;
										break;

									case XML_SCAN_PARTIAL:
										processed = 0;
										break;

									case XML_SCAN_MISMATCH:
										ctx->state = XML_STATE_IN_UNKNOWN;

										// step past "<!"
										buffer += 2;
										processed += 2;
										bufferLen -= 2;

										done = 0;
										break;
								}
								break;
						}
					}
					else if (bufferLen > 1 && buffer[1] == '?')
					{
						// "<?" could be a processing instruction or <?xml ... ?>
						ctx->state = XML_STATE_IN_UNKNOWN;

						buffer += 2;
						processed += 2;
						bufferLen -= 2;

						done = 0;
					}
					else
					{
						// it is likely a tag
						count = XML_ParseTag(ctx, &unit->parse.tag, buffer, bufferLen);
						if (count == 1 && unit->parse.tag.type == XML_TAG_MALFORMED)
						{
							// if the tag's second character is no good, then just parse it as text?
							unit->type = XML_UNIT_TEXT;
							unit->parse.text.str = (XMLchar*) buffer;
							unit->parse.text.len = 1;
#ifdef JANUARY_17_08_FIXES
// printf("Fix malformed problem %d tag == [%S]\n", problemcount++, buffer);
                            // HEREHERE missing processed++; looks like '<' not accounted for ??
							processed++;
#endif
							buffer++;
							bufferLen--;

							while ((*buffer != '<') && (bufferLen > 0))
							{
								unit->parse.text.len++;
								processed++;
								buffer++;
								bufferLen--;
							}
						}
						else
						{
							unit->type = XML_UNIT_TAG;
							processed += count;
						}
					}
				}
				else
				{
					// read string data
					unit->type = XML_UNIT_TEXT;
					unit->parse.text.str = (XMLchar*) buffer;
					unit->parse.text.len = 0;
					while ((*buffer != '<') && (bufferLen > 0))
					{
						unit->parse.text.len++;
						processed++;
						buffer++;
						bufferLen--;
					}
				}
				break;

			case XML_STATE_IN_COMMENT:
			{
				unit->type = XML_UNIT_COMMENT;
				unit->parse.comment.str = (XMLchar*) buffer;
				unit->parse.comment.len = 0;

				while (bufferLen > 0)
				{
					result = _XML_ScanString(buffer, bufferLen, gpXMLStrEndComment);

					if (result == XML_SCAN_COMPLETE)
					{
						// we matched the end cdata string
						ctx->state = XML_STATE_NORMAL;
						processed += 3;
						break;
					}
					else if (result == XML_SCAN_PARTIAL)
					{
						break;
					}
					else
					{
#ifdef JANUARY_17_08_FIXES
						unit->parse.cdata.len++; // printf bug ??
#else
						unit->parse.comment.len++; // printf bug ??
#endif
					}

					buffer++;
					bufferLen--;
					processed++;
				}

				break;
			}

			case XML_STATE_IN_CDATA:
			{
				unit->type = XML_UNIT_CDATA;
				unit->parse.cdata.str = (XMLchar*) buffer;
				unit->parse.cdata.len = 0;

				while (bufferLen > 0)
				{
					result = _XML_ScanString(buffer, bufferLen, gpXMLStrEndCharData);

					if (result == XML_SCAN_COMPLETE)
					{
						// we matched the end cdata string
						ctx->state = XML_STATE_NORMAL;
						processed += 3;
						break;
					}
					else if (result == XML_SCAN_PARTIAL)
					{
						break;
					}
					else
					{
						unit->parse.cdata.len++;
					}

					buffer++;
					bufferLen--;
					processed++;
				}

				break;
			}

			case XML_STATE_IN_UNKNOWN:
				unit->type = XML_UNIT_UNKNOWN;

				while (bufferLen > 0)
				{
					if (*buffer == '[')
					{
						if (ctx->bracketNesting < XML_MAX_BRACKET_NESTING)
						{
							ctx->bracketNesting++;
						}
					}
					else if (*buffer == ']')
					{
						if (ctx->bracketNesting > 0)
						{
							ctx->bracketNesting--;
						}
					}
					else if ((*buffer == '>') && (ctx->bracketNesting == 0))
					{
						// return to normal parse state - the unknown thing is over
						// missing buffer++; et al but we are breaking out anyway
						ctx->state = XML_STATE_NORMAL;
						processed++;
						break;
					}

					buffer++;
					bufferLen--;
					processed++;
				}
				break;

			default:
				break;
		}
	}
	while (!done);

	return (processed);
}


void XML_TokenizeStrings (XMLUnitParse *unit)
{
	switch (unit->type)
	{
		case XML_UNIT_TEXT:
			XML_TOKENIZE(unit->parse.text);
			break;

		case XML_UNIT_TAG:
		{
			int n;

			// tag name
			XML_TOKENIZE(unit->parse.tag.name);

			// attribute list
			for (n = 0; n < unit->parse.tag.attribList.count; n++)
			{
				XML_TOKENIZE(unit->parse.tag.attribList.array[n].name);
				if (unit->parse.tag.attribList.array[n].value.str != unit->parse.tag.attribList.array[n].name.str)
				{
					XML_TOKENIZE(unit->parse.tag.attribList.array[n].value);
				}
			}
			break;
		}

		case XML_UNIT_COMMENT:
			XML_TOKENIZE(unit->parse.comment);
			break;

		case XML_UNIT_CDATA:
			XML_TOKENIZE(unit->parse.cdata);
			break;
	}
}

void XML_RestoreStrings (XMLUnitParse *unit)
{
	switch (unit->type)
	{
		case XML_UNIT_TEXT:
			XML_RESTORE(unit->parse.text);
			break;

		case XML_UNIT_TAG:
		{
			int n;

			// tag name
			XML_RESTORE(unit->parse.tag.name);

			// attribute list
			for (n = 0; n < unit->parse.tag.attribList.count; n++)
			{
				XML_RESTORE(unit->parse.tag.attribList.array[n].name);
				if (unit->parse.tag.attribList.array[n].value.str != unit->parse.tag.attribList.array[n].name.str)
				{
					XML_RESTORE(unit->parse.tag.attribList.array[n].value);
				}
			}
			break;
		}

		case XML_UNIT_COMMENT:
			XML_RESTORE(unit->parse.comment);
			break;

		case XML_UNIT_CDATA:
			XML_RESTORE(unit->parse.cdata);
			break;
	}
}

#ifdef XML_UNICODE
XMLAttribValueParse* XML_GetTagAttribute (XMLTagParse* parse, const XMLchar* attribName,
                              XML_BOOL caseSensitive)
{
	long n;

	for (n = 0; n < parse->attribList.count; n++)
	{
		if (!xml_strnicmp(parse->attribList.array[n].name.str, attribName, parse->attribList.array[n].name.len))
		{
			return (&parse->attribList.array[n].value);
		}
	}

	return (0);
}
#endif

XMLAttribValueParse* XML_GetTagAttributeASCII (
		XMLTagParse* parse,
		const char* attribName,
		XML_BOOL caseSensitive
	)
{
	long n;

	for (n = 0; n < parse->attribList.count; n++)
	{
		if (!xml_c_strnicmp(parse->attribList.array[n].name.str, attribName, parse->attribList.array[n].name.len))
		{
			return (&parse->attribList.array[n].value);
		}
	}

	return (0);
}


int _XML_ScanString (
		const XMLchar* sourceData,
		long sourceLen,
		const XMLchar* str
	)
{
	int n;

	for (n=0; n<sourceLen; n++)
	{
		if (!str[n])
		{
			return (XML_SCAN_COMPLETE);
		}

		if (sourceData[n] != str[n])
		{
			return (XML_SCAN_MISMATCH);
		}
	}

	return (XML_SCAN_PARTIAL);
}

unsigned _XML_NameStartChar (
		XMLParseContext *ctx,
		XMLchar c
	)
{
	return (XML_NAME_START_CHAR(c) || (ctx->allowAnyNameStartChar && XML_NAME_CHAR(c)));
}
