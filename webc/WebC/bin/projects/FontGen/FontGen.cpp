// FontGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommandLineParser.h"
#include "..\\..\\..\\include\\util\\webc.h"
#include "..\\..\\..\\gui\\framebuffer\include\\FBAbstractionFonts.h"
#include <errno.h>

using namespace Gdiplus;

// font file output parameters
#define CHAR_MAP_BPP 1 // bits per pixel
#define GREYSCALE_THRESHOLD 128
#define BYTES_PER_LINE 16

struct FontGenCommandLine : public StandardCommandLineParser
{
public:
    // Create flags and params
    FlagArg                         verbose;
    ValueArg<int>                   startUnicode;
    ValueArg<int>                   endUnicode;
    ValueArg<int>                   fontSize;
    ValueArg<tstring>               familyName;
    ValueArg<tstring>               genericName;
    ValueArg<int>			        attributeIndex;
    ValueArg<tstring>               outputPrefix;

public:
    FontGenCommandLine()
        // Set names and descriptions for usage
    :   verbose(__T("verbose"), __T("Verbose output.")),
        startUnicode(__T("start"), __T("Starting Unicode value.")),
        endUnicode(__T("end"), __T("Ending Unicode value.")),
        fontSize(__T("size"), __T("Font size.")),
        familyName(__T("family"), __T("Font family name. (Times,Arial,Verdana,Courier New)")),
        genericName(__T("generic"), __T("Font generic name.(sans-serif,monospace,Serif,sans-serif)")),
        attributeIndex(__T("attr"), __T("Font attribute name.(0=Regular,1=Bold,2=Italic,3=BoldItalic)")),
        outputPrefix(__T("o"), __T("Output path prefix."))
    {
        // Add flags matched by name, e.g. /foo
        AddFlag(verbose);

        AddFlag(startUnicode);
        startUnicode.SetDefaultValue((int) L' ');
        AddFlag(endUnicode);
        endUnicode.SetDefaultValue((int) L'~');

        AddFlag(fontSize);
        fontSize.SetDefaultValue(10);

        AddFlag(familyName);
        familyName.SetDefaultValue(__T("Courier New"/*"Arial"*/));

        AddFlag(genericName);
        genericName.SetDefaultValue(__T("sans-serif"/*"monospace"*/));

        AddFlag(attributeIndex);
        attributeIndex.SetDefaultValue(0);

	    AddFlag(outputPrefix);
        outputPrefix.SetDefaultValue(__T("webCFont.out"));
	}
};


FB_Font* createFont (
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		WEBC_CHAR startChar,
		WEBC_CHAR endChar,
		WEBC_CHAR *pathStart
);


int _tmain(int argc, _TCHAR* argv[])
{
    // Parse the command line
    FontGenCommandLine cl;
    cl.Parse(argc, (const TCHAR **)argv);

    // Will show usage or version if there's an error, /?|h|help or /v|version
    if( !cl.Continue() ) return 0;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	WEBC_CHAR startChar = (WEBC_CHAR) cl.startUnicode.Value();
	WEBC_CHAR endChar = (WEBC_CHAR) cl.endUnicode.Value();

	FB_Font* f = createFont ((WEBC_CHAR *) cl.familyName.Value().c_str(), (WEBC_CHAR *) cl.genericName.Value().c_str(), cl.fontSize.Value(), cl.attributeIndex.Value(), startChar, endChar, (WEBC_CHAR *) cl.outputPrefix.Value().c_str());

	GdiplusShutdown(gdiplusToken);

	return 0;
}

WCHAR *getCharacters(WEBC_CHAR startChar, WEBC_CHAR endChar)
{
	WCHAR *retVal = (WCHAR *) malloc((size_t)(endChar - startChar + 2) * sizeof(WCHAR));
	if(retVal)
	{
		WCHAR *pOut = retVal;
		for(WEBC_CHAR c = startChar; c <= endChar; c++)
		{
			*pOut++ = (WCHAR)c;
		}
	
		// null terminator
		*pOut = (WCHAR)0;
	}
	return retVal;
}


FB_Font* createFont (
		const WEBC_CHAR* familyName,
		const WEBC_CHAR* genericName,
		WEBC_UINT16 pointSize,
		WEBC_UINT16 attrib,
		WEBC_CHAR startChar,
		WEBC_CHAR endChar,
		WEBC_CHAR *pathStart
){
	//creation of font file name
	WEBC_CHAR tmppathBuffer[1024];
	WEBC_CHAR tempFamilyName[1024];
	WEBC_CHAR *style;

	WCHAR *p = (WCHAR *)familyName;
	int l = 0;
	while (*p)
	{
		if (*p == ' ' )
			break;
		tempFamilyName[l++] = *p++;
		tempFamilyName[l] = 0;
	}

	switch(attrib & 0x3){
		case 0: 
			style = (WEBC_CHAR*)L"Regular";
			break;
		case 1: 
			style = (WEBC_CHAR*)L"Bold";
			break;
		case 2: 
			style = (WEBC_CHAR*)L"Italic";
			break;
		case 3: 
			style = (WEBC_CHAR*)L"BoldItalic";
			break;
		default: 
			style = (WEBC_CHAR*)L"Regular";
			break;
	}

	wcscpy((wchar_t*)tmppathBuffer,(const wchar_t*)pathStart);
	WEBC_CHAR baseFileName[1024];

	_stprintf((wchar_t *)baseFileName,_T("%s%s%dB%04xE%04x"), tempFamilyName, style, (int) pointSize, (int) startChar, (int) endChar);

	wcscat((wchar_t*)tmppathBuffer,(const wchar_t*)baseFileName);
	WEBC_CHAR pathBuffer[1024];

	int count = 0;
	while(tmppathBuffer[count] != '\0'){
		pathBuffer[count] = tmppathBuffer[count];	
		count++;
	}
	pathBuffer[count] = tmppathBuffer[count];

	//Create the string containing needed characters this can be expanded to contain more characters as needed
	//'j' is out of place because of a conflict 
	//const WCHAR* astring = L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`jabcdefghiklmnopqrstuvwxyz{|}~\0";
	WCHAR* astring = getCharacters(startChar, endChar);
	int numChars = (int)(endChar - startChar);

	int charMapHeight = 2 * pointSize * numChars / 10;//10 rows of characters this must be changed if more characters are added.
	int charMapWidth =  32 * pointSize;//10 characters per row * 3 * pointSize, 3 is an arbitrary number that I chose
									   //because it worked when looking at the actual measurements of the widest chars

	int bitsPerPixel = CHAR_MAP_BPP;
	int dataSize = charMapWidth * bitsPerPixel * charMapHeight / 8;
	FB_Font* new_font = (FB_Font*)malloc(sizeof(FB_Font) + (sizeof(DisplayChar) * numChars));
	new_font->charMap = (WebGraphBitmapInfo*)malloc(sizeof(WebGraphBitmapInfo));
	new_font->charMap->pData = (WEBC_UINT8*)malloc(dataSize);
	
	new_font->startChar = startChar;
	new_font->endChar = endChar;

	FILE *font_file = 0;
	INT fontStyleMap[4] =
	{
		FontStyleRegular,
		FontStyleBold,
		FontStyleItalic,
		FontStyleBoldItalic
	};
	//this statement attrib & 0x3 make the attrib parameter modular, 
	//if it is greater than three it will wrap
	
			
	//Create Blank bitmap
	BITMAPINFOHEADER* bi;
	void* dataPtr;
	Bitmap* bitmap;
	bi = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	bi->biSize = sizeof(BITMAPINFOHEADER);
	bi->biWidth = charMapWidth;
	bi->biHeight = charMapHeight;
	bi->biPlanes = 1;
	bi->biBitCount = 24;
	bi->biCompression = BI_RGB;
	bi->biSizeImage = 0;
	bi->biXPelsPerMeter = 72 * 39; // 72dpi * 39 inches/meter
	bi->biYPelsPerMeter = 72 * 39; // 72dpi * 39 inches/meter
	bi->biClrUsed = 0;
	bi->biClrImportant = 0;
	
	WEBC_SIZE dstBytesPerRow = ((charMapWidth*3) + (3)) & ~(3);
	dataPtr = (WEBC_UINT8*)malloc(dstBytesPerRow * bi->biHeight);
	//Make the bitmap white for easier manipulation later
	memset((void*)dataPtr,0xFF,dstBytesPerRow * bi->biHeight);
	bitmap = new Bitmap((BITMAPINFO*)bi, dataPtr);
	//create graphics and bounds for the graphics
	Graphics* graphics;
	Color temp(0xFF,0,0,0);//full black
	SolidBrush brush(temp);
	PointF point(0,0);
	graphics = new Graphics(bitmap);

	FontFamily* fam = new FontFamily((const WCHAR *)familyName);
	Font* f = new Font(fam, pointSize, fontStyleMap[attrib & 0x3], UnitPoint);
				
	wcscpy((wchar_t*)new_font->font_family, (const wchar_t*)tempFamilyName);

	wcscpy((wchar_t*)new_font->generic_family, (const wchar_t*)genericName);
	new_font->pointSize = pointSize;
	new_font->style = (STYLIZED)(attrib & 0x3);
	//The following line takes the cell ascent(baseline) and mulitplies it by the size inorder to get design units per pixel, then the divided by the em height to get pixels
	new_font->baseline = (int) ((f->GetHeight(graphics) * (REAL)fam->GetCellAscent(f->GetStyle())) /
	                        ((REAL)fam->GetEmHeight(f->GetStyle()) - 1.0));
	new_font->height = (int) f->GetHeight(graphics); 

	WCHAR ch;
	RectF rect(0,0,0,0);
	ch = astring[0];
	int dataIndex=0;
	int kernValue = (int)(((float)f->GetSize()/9.0) * 4.0);
	while(ch != '\0'){
		
		graphics->MeasureString(&ch,1,f,point,&rect);

		// next "line" in the charater image
		if((point.X + rect.Width ) > bitmap->GetWidth()){
			point.X = 0;
			point.Y += (new_font->height + 3);//increase by three to avoid run over cause of desent
		}
	
		Status stat = graphics->DrawString(&ch,1,f,point,&brush);
	
		if(ch == ' '){
			new_font->charData[dataIndex].x = (point.X);
			new_font->charData[dataIndex].width = (rect.Width);
		}
//		else if(ch == 'f'){
//			new_font->charData[dataIndex].x = (point.X + (kernValue/2));
//			new_font->charData[dataIndex].width = (rect.Width-kernValue+2);
//		}
		else
		if(f->GetSize() < 10 ){
			new_font->charData[dataIndex].x = ((int)point.X + (kernValue/2));
			new_font->charData[dataIndex].width = ((int)rect.Width-kernValue+1);
		}
		else{
			new_font->charData[dataIndex].x = ((int)point.X + (kernValue/2));
			new_font->charData[dataIndex].width = ((int)rect.Width-kernValue);
		}
	
		new_font->charData[dataIndex].y = (int)point.Y;
		new_font->charData[dataIndex].height = (int)(rect.Height);
			
		point.X += (REAL) (new_font->charData[dataIndex].width + 5);
		ch = astring[dataIndex++];
	}
	//graphics no longer needed
	delete(graphics);
	graphics = NULL;
	
	// string no longer needed
	free(astring);

	new_font->charMap->bHasTransparentColor = false;
	new_font->charMap->iBytesPerRow = (bitmap->GetWidth() * bitsPerPixel / 8);
	new_font->charMap->iHeight = bitmap->GetHeight();
	new_font->charMap->iWidth = bitmap->GetWidth();
	new_font->charMap->iPaletteSize = 0;
	new_font->charMap->pixelFormat = WB_PIXEL_FORMAT_RGB_24;
	new_font->charMap->pPalette = NULL;
				
	WEBC_UINT8 *rowPtr,*colPtr;
	Color pixelColor;

	rowPtr = (WEBC_UINT8*)new_font->charMap->pData;
	for(int m = 0; m < new_font->charMap->iHeight; m++, rowPtr += new_font->charMap->iBytesPerRow){
		colPtr = rowPtr;
		for(int n = 0; n < new_font->charMap->iWidth; n++){
			bitmap->GetPixel(n,m,&pixelColor);
#if (8 == CHAR_MAP_BPP)
			WEBC_UINT32 temp = ((WEBC_UINT32) pixelColor.GetR() + (WEBC_UINT32) pixelColor.GetG() + (WEBC_UINT32) pixelColor.GetB() ) / 3;
			*colPtr++ = (WEBC_UINT8) temp;
#elif (4 == CHAR_MAP_BPP)
			// TODO
#elif (1 == CHAR_MAP_BPP)
			WEBC_UINT32 temp = ((WEBC_UINT32) pixelColor.GetR() + (WEBC_UINT32) pixelColor.GetG() + (WEBC_UINT32) pixelColor.GetB() ) / 3;
			int shiftVal = (7 - (n % 8));
			if(temp > GREYSCALE_THRESHOLD)
				*colPtr |= (WEBC_UINT8) (1 << shiftVal);
			else
				*colPtr &= (WEBC_UINT8) ~(1 << shiftVal);

			if(0 == shiftVal)
				colPtr++;
#endif
		}
	}
	
	//save font to a binary file
	font_file = _tfopen((TCHAR *)pathBuffer,_T("wb"));
	if(font_file){
		fwrite((void*)&new_font->baseline,sizeof(int), 1, font_file);
		fwrite((void*)&dataIndex,sizeof(int), 1, font_file);
		fwrite((void*)new_font->charData,sizeof(DisplayChar) * dataIndex, 1, font_file);
		fwrite((void*)new_font->font_family,sizeof(WEBC_CHAR[16]), 1, font_file);
		fwrite((void*)new_font->generic_family,sizeof(WEBC_CHAR[16]), 1, font_file);
		fwrite((void*)&new_font->height,sizeof(int), 1, font_file);
		fwrite((void*)&new_font->pointSize,sizeof(int), 1, font_file);
		fwrite((void*)&new_font->style,sizeof(enum STYLIZED), 1, font_file);
		fwrite((void*)&new_font->startChar,sizeof(WEBC_CHAR), 1, font_file);
		fwrite((void*)&new_font->endChar,sizeof(WEBC_CHAR), 1, font_file);
		fwrite((void*)&new_font->charMap->bHasTransparentColor,sizeof(WEBC_BOOL), 1, font_file);
		fwrite((void*)&new_font->charMap->iBytesPerRow,sizeof(WEBC_SIZE), 1, font_file);
		fwrite((void*)&new_font->charMap->iHeight,sizeof(DISPLAY_INT), 1, font_file);
		fwrite((void*)&new_font->charMap->iPaletteSize,sizeof(WEBC_UINT32), 1, font_file);
		fwrite((void*)&new_font->charMap->iTransparentIndex,sizeof(WEBC_UINT16), 1, font_file);
		fwrite((void*)&new_font->charMap->iWidth,sizeof(DISPLAY_INT), 1, font_file);
		fwrite((void*)&new_font->charMap->pixelFormat,sizeof(WebBitmapPixelFormat), 1, font_file);
		fwrite((void*)&new_font->charMap->uTransparentBlue,sizeof(WEBC_UINT8), 1, font_file);
		fwrite((void*)&new_font->charMap->uTransparentGreen,sizeof(WEBC_UINT8), 1, font_file);
		fwrite((void*)&new_font->charMap->uTransparentRed,sizeof(WEBC_UINT8), 1, font_file);
		fwrite((void*)new_font->charMap->pData,dataSize, 1, font_file);

#define DEBUG_IMAGE_OUTPUT 1
#ifdef DEBUG_IMAGE_OUTPUT
		{
			FILE *pDebugFile = fopen("debugBits.raw","wb");
			for(int dataIndex=0; dataIndex < dataSize; dataIndex++)
			{
				for(int bit=7; bit>=0; bit--)
				{
					unsigned char pix = (new_font->charMap->pData[dataIndex] & (1 << bit)) ? 0xFF : 0;
					fwrite((void *)(&pix),sizeof(unsigned char), 1, pDebugFile);
				}
			}
			fclose(pDebugFile);

		}
#endif
	}

	fclose(font_file);
	font_file = NULL;

	//save font to a "C" source code file

	// reopen for read access
	font_file = _tfopen((const TCHAR *)pathBuffer, _T("rb"));
	_tcscat((TCHAR *)pathBuffer, _T(".c"));
	FILE *source_file = _tfopen((TCHAR *)pathBuffer,_T("w"));

	_ftprintf(source_file,_T("/* %s.c */\n"), baseFileName);
	_ftprintf(source_file,_T("/* Automatically Created from %s */\n"), baseFileName);
	_ftprintf(source_file,_T("#include \"%s.h\"\n\n"), baseFileName);
	_ftprintf(source_file,_T("const unsigned char %s_file[] = { \n\n"), baseFileName);

	BYTE tempByte;
	int bytesThisLine = BYTES_PER_LINE;
	int bytesOutput=0;
	while(fread(&tempByte, sizeof(BYTE), 1, font_file))
	{
		_ftprintf(source_file,_T("0x%02x, "), (int) tempByte);
		bytesOutput++;
		if(--bytesThisLine == 0)
		{
			_ftprintf(source_file,_T("\n"));
			bytesThisLine = BYTES_PER_LINE;
		}
	}

	_ftprintf(source_file,_T("\n};\n"));

	fclose(font_file);
	fclose(source_file);

	// Now output the .h file

	pathBuffer[_tcslen((TCHAR *)pathBuffer) - 1] = _T('h');
	FILE *h_file = _tfopen((TCHAR *)pathBuffer,_T("w"));

	_ftprintf(h_file,_T("/* %s.h */\n"), baseFileName);
	_ftprintf(h_file,_T("/* Automatically Created from %s */\n\n"), baseFileName);
	_ftprintf(h_file,_T("#ifndef __%s_H__\n"), baseFileName);
	_ftprintf(h_file,_T("#define __%s_H__\n\n"), baseFileName);
	_ftprintf(h_file,_T("#define %s_SIZE %d\n\n"), baseFileName, bytesOutput);
	_ftprintf(h_file,_T("extern const unsigned char %s_file[];\n\n"), baseFileName);
	_ftprintf(h_file,_T("#endif\n"));
	fclose(h_file);


	// free everything
	free((void*)bi);
	bi = NULL;
	free((void*)dataPtr);
	dataPtr = NULL;
	delete(fam);
	fam = NULL;
	delete(bitmap);
	bitmap=NULL;
	delete(f);
	f = NULL;

	return new_font;

}