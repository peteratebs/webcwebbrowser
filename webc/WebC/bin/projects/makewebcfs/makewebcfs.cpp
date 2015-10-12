// makewebcfs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int doMkWebCFileSystem(char *OutPath, char *OutBaseName, char *RootPath);
char *wtoc(char * Dest, const _TCHAR * Source)
{
char *r=Dest;
	int i = 0;

    while(Source[i] != '\0')
    {
        Dest[i] = (char)Source[i];
        ++i;
		Dest[i] = 0;
    }
	return r;
}
static _TCHAR *wProgramName(const _TCHAR * pSource);
int _tmain(int argc, _TCHAR* argv[])
{
char *OutPath, *OutBaseName, *RootPath;
char argbuffs[4][512];
	OutPath=OutBaseName=RootPath=0;
	for (int i=1; i < argc && i < 5; i++)
	{
		char *p =  (char *)wtoc(&argbuffs[i-1][0], argv[i]);
		if (*p == '-')
		{
			if (*(p+1)=='h')
				goto help;;
			if (*(p+1)=='o' || *(p+1)=='n')
			{
				char *_p=p+2;
				if (*_p!='=')
					goto help;
				if (*(p+1)=='o')
					OutPath=_p+1;
				else if (*(p+1)=='n')
					OutBaseName=_p+1;
			}
			else
				break;
		}
		else
		{
			RootPath=p;
			break;
		}
	}
	if (RootPath)
		return doMkWebCFileSystem(OutPath, OutBaseName, RootPath);
help:
	printf("Usage: %S [-h] [-o=pathname] [-n=filenamebase] <foldernametoconvert>\n",wProgramName(argv[0]));
	printf("      The default if pathname is not specified is to put the output files in the current directory\n");
	printf("      A better choice is the webc/source/protocol directory for the project you are building, otherwise you must copy the output files\n");
	printf("      The default filenamebase, if none is specified, is \"webc\" which creates the defaults expected by webcvfile.cpp, webcfiletable.h and webcfiledata.h.\n");
	printf("      Foldername to convert is required. This must be path to the directory to convert.\n");
	printf("      Example: Create files named webcfiletable.h and webcfiledata.h from content in \\mydevice\\contentfolder and place them in \\mydevice\\webc\\source\\protocol\n");
	printf("         %s -o=\\mydevice\\webc\\source\\protocol -n=webc \\mydevice\\contentfolder \n",wProgramName(argv[0]));
	return -1;

}
static _TCHAR *wProgramName(const _TCHAR * pSource)
{
_TCHAR * pProgram = (_TCHAR *) pSource;
	while (*pSource)
	{
		if ((char)*pSource == '\\')
		{
			pSource++; if (*pSource)  pProgram= (_TCHAR *) pSource;
		}
		else
			pSource++;
	}
	return pProgram;
}
