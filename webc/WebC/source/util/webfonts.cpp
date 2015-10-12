//
// WEBFONTS.CPP - Font management code and default font tables
//
// EBS -
//
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webfonts.hpp"
#include "webcfg.h"
#include "util.h"
#include "webc.h"
#include "webcstr.h"
#include "webcassert.h"
#include "webcmem.h"

/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// Types
/*****************************************************************************/

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/

/*****************************************************************************/
// Data
/*****************************************************************************/

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

int webc_font_family_index (
		const WEBC_CHAR* name,
		const WEBC_CHAR* genericName,
		const WEBC_CHAR** fontNames,
		const WEBC_CHAR** fontGenericNames,
		int numFonts
	)
{
	if (!name)
	{
		name = genericName;
		genericName = 0;
	}

	while (name)
	{
		const WEBC_CHAR* pos = name;
		const WEBC_CHAR* last;
		WEBC_CHAR quoteChar;

		while (*pos)
		{
			while (*pos && !IS_ALPHA(*pos) && *pos != '\'' && *pos != '\"')
			{
				pos++;
			}

			if (!*pos)
			{
				break;
			}

			if (*pos == '\'' || *pos == '\"')
			{
				quoteChar = *pos;
				pos++;
			}
			else
			{
				quoteChar = 0;
			}

			last = pos;
			while (*last && (quoteChar || (!IS_WHITESPACE(*last) && *last != ',')) && (!quoteChar || *last != quoteChar))
			{
				last++;
			}

			if (!*last)
			{
				int n;

				for (n=0; n<numFonts; n++)
				{
					if (fontNames[n])
					{
						if (!webc_stricmp(fontNames[n], pos) ||
							(fontGenericNames[n] && !webc_stricmp(fontGenericNames[n], pos)))
						{
							return (n);
						}
					}
				}
			}
			else
			{
				int len = last - pos;
				int n;

				for (n=0; n<numFonts; n++)
				{
					if (fontNames[n])
					{
						if (!webc_strnicmp(fontNames[n], pos, len) ||
							(fontGenericNames[n] && !webc_strnicmp(fontGenericNames[n], pos, len)))
						{
							return (n);
						}
					}
				}
			}

			if (quoteChar && *last == quoteChar)
			{
				last++;
			}

			pos = last;
		}

		name = genericName;
		genericName = 0;
	}

	return (-1);
}


/*****************************************************************************/
// WebGraphFont methods
/*****************************************************************************/

WebGraphFont::WebGraphFont (WebFont font)
{
	miUseCount = 0;
	mFont = font;
}

WebGraphFont::~WebGraphFont (void)
{
	// This was disabled and it ran fine, so dont be surprised if it traps.
	WEBC_ASSERT(miUseCount == 0);
}

void WebGraphFont::Claim (void)
{
	miUseCount++;
}

void WebGraphFont::Release (void)
{
	if (miUseCount > 0)
	{
		miUseCount--;
	}
}

int WebGraphFont::UseCount (void)
{
	return (miUseCount);
}


/*****************************************************************************/
// WebFontRef methods
/*****************************************************************************/

WebFontRef::WebFontRef (WebGraphFont *pInstance)
{
	mpFontInstance = 0;
	Set(pInstance);
}

WebFontRef::~WebFontRef (void)
{
	Set(0);
}

WebGraphFont *WebFontRef::Get (void)
{
	return (mpFontInstance);
}

void WebFontRef::Set (WebGraphFont *pInstance)
{
	if (pInstance)
	{
		pInstance->Claim();
	}

	if (mpFontInstance)
	{
		mpFontInstance->Release();
	}

	mpFontInstance = pInstance;
}

WebFont WebFontRef::GetFont (void)
{
	return ((mpFontInstance)? mpFontInstance->mFont : 0);
}
