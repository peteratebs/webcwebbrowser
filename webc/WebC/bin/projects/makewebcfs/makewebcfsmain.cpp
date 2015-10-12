/*
|  MAKEWEBCFSMAIN.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/


// ===============================

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>
#include <io.h>
#include <direct.h>
#include <string.h>

FILE *IncludeFileHandle;
FILE *DataFileHandle;
int TableEntryNumber;
int content_array_count;
unsigned long TotalDataSize;
struct TreeView
{
	char *           pName;
	struct TreeView *pSibling;
	struct TreeView *pChild;
	int				isFile;
	int				TableEntryNumber;
	long            lSize;
	char           contentarrayname[64];
};
#define ARRAYNAMEBASE "__vfcontent"

#define TESTPATH "\\AppBuilder"
#define TESTNAME "AppBuilder"
#define TESTOUTPUTDATA "\\BintocOut\\Data"
#define TESTOUTPUTINCLUDE "\\BintocOut\\Table"
static int traverseDosFile(struct TreeView *pRoot, char *name, char *inPath, int level);
static char *malloc_string_copy(char *str);

int doMkWebCFileSystem(char *OutPath, char *OutBaseName, char *RootPath);
void doFileTest(void)
{

	doMkWebCFileSystem("\\BintocOut", "Rommed", TESTPATH);
}
static void  callErrorExit(char *message) {
	 if (message)
	 	printf("      %s\n", message);
	 printf("Error *********************************************************************************************** \n");
	 printf("      program terminated \n");
};

/*****************************************************************************/
// webcFsIntroduction
/**
 @memo The webc: url (also accessible as rom:) is a rom based read only file system that enables you to include content in your application like you do from a file system.
  @doc The rom file system lets you prototype content with webC on a PC workstation, organizing it using typical names like "images", "metadata" and "html". You can then burn this content it into your application using makewebcfs and acess it using the rom: url or the webc: url.
  @see makewebcfs wcAddFsTree ExDemonstrateWebcVirtualFiles
 */
/*****************************************************************************/
void webcFsIntroduction(void){};

//
/*****************************************************************************/
// makewebcfs
/**
 @memo Compile content from a workstation subdirectory into a form suitable for use with webc's read onnly rom based file system.
 @doc  webc has a url type called webc:, which can also be accessed as rom:. This url type acts just like the file: url type except the content is compiled into the webC library as constant data. <br>
 A convenient way to develop applications is to simulate your application on Windows with webC using the Windows file system, populaying a Windows subtree with images, html files , style sheets and javascript source files.<br>
 When you want to use the content on your embedded device you can create a webC rom file system image from the disk content with a single command.<br>

makewebcfs Converts an entire Windows subdirectory and its children into a set of 'C' declarations. These declarations are embedded in header files which you then include into your application code. You then make a call to wcAddFsTree() to add this subtree to the rom file system structure.
Once a table is added to the webc: file system it may be accessed with the webc: url. (see below for an example).<br>

 @param  -h help
 @param -o=pathname  (optional) Path to place the output files, (eg.-o="/MycompiledPages") if not specified output files are placed in the current directory.
 @param -n=filenamebase (optional) Base of the two output files to produce, "basenamefiletable.h" and "basenamefiledata.h". If not specified webcfiletable.h and webcfiledata.h are created.
 @param foldernametoconvert (required) path to the files to convert.

 <pre>

	To convert a file system subtree to a webc: subtree you must follow these steps:
	<ul>
	<li>Open a DOS box.</li>
	<li>For convenience change directory (CD) to the location of your project</li>
	<li>Run makewebcfs to create include files you will need.</li>
	<li>Edit one of your aplication files file to include the two header files created by makewebcfs.</li>
	<li>Edit the file and insert a call to \Ref{wcAddFsTree}() to insert the subdirectory tree into the webc: file system.</li>
   <ul>

   For example:
<br>Suppose you have a webc project located at \MyExampleProject and you have been experimenting using content in a Windows subdirectory named \MyContent. You have been accessing it with file: based urls
but now you want to build the content into your embedded applications rom image and have it be accessible from the webc: file system.<br>

	<ul>
	<li>Open a DOS box.</li>
	<li>type CD \MyExampleProject</li>
	<li>type makewebcfs -n=Examples \MyContent</li>
	<li><i> You could also have typed:</i> makewebcfs -n=Examples -o=\MyExampleProject \MyContent</li>
	<li>Edit your startup file and add these two lines:<br>
	#include "Examplesfiletable.h" <br>
	#include "Examplesfiledata.h" <br>
	</li>
	<li>Add this call to your code <b>wcAddFsTree(Examples);</b></li>
   </ul>

   To illustrate:
   &nbsp;If you have a subdirectory populated like this on the Windows workstation.
   &nbsp;&nbsp;\MyContent
   &nbsp;&nbsp;&nbsp;&nbsp;File.html
   &nbsp;&nbsp;&nbsp;&nbsp;File.css
   &nbsp;&nbsp;&nbsp;&nbsp;Images
   &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;Image1.gif<br>
    It will be acessible from webc: on the Windows workstation as.
	&nbsp;file:///MyContent/File.html
	&nbsp;file:///MyContent/File.css
    &nbsp;file:///MyContent/Images/Images.gif<br>
    But after successfully executing(makewebcfs -n=Examples \MyContent) the files will be accesible from these webc: urls.
    &nbsp;webc://File.html
    &nbsp;webc://File.css
    &nbsp;webc://Images/Images.gif<br>
    The files are also accesible from these rom: urls.
    &nbsp;rom://File.html
    &nbsp;rom://File.css
    &nbsp;rom://Images/Images.gif<br>
   Note: If you want them to be accessible as:
   &nbsp;webc:/MyContent/File.html
   &nbsp;webc:/MyContent/File.css
   &nbsp;webc:/MyContent/Images/Images.gif
   and.
   &nbsp;rom:/MyContent/File.html
   &nbsp;rom:/MyContent/File.css
   &nbsp;rom:/MyContent/Images/Images.gif
   You must have the the foldernametoconvert argument point to a parent folder that contains just the one folder named MyContent.

</pre>


 */
/*****************************************************************************/
void makewebcfs(void){};

static void EmitContentFileTable(char *TableName, int TableSize, struct TreeView *Top);
static void EmitFileContentDeclarations(struct TreeView *Top);
static int CalculateTableEntryNumbers(struct TreeView *Top);
static struct TreeView *InitializeSubdirectoryEntry(char *name,int isFile);
int doMkWebCFileSystem(char *OutPath, char *OutBaseName, char *RootPath)
{
char Buffer[256];
	// Check arguments
	if (!OutPath)
		OutPath = ".";
	if (!OutBaseName)
		OutBaseName= "webc";
	if (!RootPath)
	{
		printf("You must specify a directory to convert to a webC rom file system.\n");
		callErrorExit(0);
		return -1;
	}
	printf("Converting %s to a webC rom file system, all files and folders.\n", RootPath);
	printf("Placing results in %s\\%sfiletable.h\n", OutPath,OutBaseName);
	printf("Placing results in %s\\%sfiledata.h\n", OutPath,OutBaseName );
	printf("These files must be included into webvfile.cpp\n");

	// The first record is the phoney root, we don't want that in our table.
	TableEntryNumber = -1;
	TotalDataSize = 0;

	struct TreeView *pTest = InitializeSubdirectoryEntry("Test",0);
	sprintf(Buffer, "%s\\%sfiletable.h",OutPath, OutBaseName);
	IncludeFileHandle = fopen(Buffer,"w+");
	sprintf(Buffer, "%s\\%sfiledata.h",OutPath, OutBaseName);
	DataFileHandle = fopen(Buffer,"w+");
	if (!IncludeFileHandle || !DataFileHandle)
	{
		printf("Open of path,base %s:%s failed\n",  OutPath, OutBaseName);
		callErrorExit(0);
		return -1;
	}
	fprintf(IncludeFileHandle, "\n\n/*webC virtual file table */\n\n/*\n========= Generated Automatically, \n========= From Source Code At:\n========= %s \n********* Do Not modify !! \n*/\n",RootPath);
	fprintf(DataFileHandle,    "\n\n/*webC virtual file table */\n\n/*\n========= Generated Automatically, \n========= From Source Code At:\n========= %s \n********* Do Not modify !! \n*/\n",RootPath);

	fprintf(IncludeFileHandle, "#include \"webc.h\"  /* required for file system entry data structure definitions) */\n" );
	fprintf(DataFileHandle, "#include \"webc.h\"  /* required for file system entry data structure definitions) */\n");

	content_array_count = 0; // use this to name the arrays that store the data
	pTest->lSize = traverseDosFile(pTest,"root", RootPath, 0);


	int TableSize= CalculateTableEntryNumbers(pTest)-1;

	printf("File system contains %d files with a total of %d bytes\n", TableEntryNumber,TotalDataSize);
	EmitFileContentDeclarations(pTest);

	fprintf(IncludeFileHandle,"\n /* webC File system contains %d files with a total of %d bytes */\n", TableEntryNumber,TotalDataSize);

	EmitContentFileTable(OutBaseName,TableSize, pTest);

	fprintf(IncludeFileHandle, "\n\n/*========= Include Arrays containing file content ======*/\n\n\n");
	fprintf(IncludeFileHandle, "#include \"%sfiledata.h\"\n", OutBaseName);
	fclose(IncludeFileHandle);
	fclose(DataFileHandle);
	return 0;
}
static void FormatFileTable(struct TreeView *Top, int depth);
// "name", iMimeTypeName, lsize, contentarrayname, flags, WebcCharsetname)
static void EmitContentFileTable(char *TableName, int TableSize, struct TreeView *Top)
{
	fprintf(IncludeFileHandle, "\n\nconst struct WebcVFileEntry %s[%d] = {\n",TableName,TableSize+2);
	FormatFileTable(Top,0);
	fprintf(IncludeFileHandle,"/*END*/{0,WEBC_FILE_TYPE_UNKNOWN,0,0,0,0,WEBC_CHARSET_UNKNOWN},\n");	// Null terminate the file list to make it easier to work with.
	fprintf(IncludeFileHandle,"\n};\n");
}
static void EmitFileContentDeclarations(struct TreeView *Top)
{
struct TreeView *pTv;
	pTv = Top;

	while (pTv)
	{
		if (pTv->isFile)
		{
			if (!pTv->lSize)
				fprintf(IncludeFileHandle, "extern const unsigned char %s[%d];\n",pTv->contentarrayname,1);
			else
				fprintf(IncludeFileHandle, "extern const unsigned char %s[%d];\n",pTv->contentarrayname,pTv->lSize);
		}
		pTv = pTv->pSibling;
	}

	pTv = Top;
	while (pTv)
	{
		if (pTv->pChild)
		{
			EmitFileContentDeclarations(pTv->pChild);
		}
		pTv = pTv->pSibling;
	}
}

// Calculate the index in the table that each record will end up in and return the number of records
static int CalculateTableEntryNumbers(struct TreeView *Top)
{
struct TreeView *pTv;
	pTv = Top;

	while (pTv)
	{
		pTv->TableEntryNumber=TableEntryNumber++;
		pTv = pTv->pSibling;
	}
	pTv = Top;
	while (pTv)
	{
		if (pTv->pChild)
		{
			CalculateTableEntryNumbers(pTv->pChild);
		}
		else
			TotalDataSize += pTv->lSize;
		pTv = pTv->pSibling;
	}
	return 	TableEntryNumber;
}


//"HTML_MIME_TYPE_UNKNOWN"
//"HTML_MIME_TYPE_HTML"
//"HTML_MIME_TYPE_GIF"
//"HTML_MIME_TYPE_JPEG"
//"HTML_MIME_TYPE_PLAIN_TEXT"
//"HTML_MIME_TYPE_CSS"
//"HTML_MIME_TYPE_JSCRIPT"
//"HTML_MIME_TYPE_PNG"
//"HTML_MIME_TYPE_MNG"
//"HTML_MIME_TYPE_BMP"

char *NameToExt(char *Name)
{
char *p,*pext;

	p = pext = strstr(Name, ".");
	if (p)
	{
		p++;
		pext=p;
	}
	while (p)
	{
		p = strstr(pext, ".");
		if (p)
		{
			p++;
			pext = p;
		}
	}
	return pext;
}
char *NameToMimeTypeName(char *Name)
{
char *p,*pext;
	pext = NameToExt(Name);

	p="HTML_MIME_TYPE_PLAIN_TEXT";
	if (pext)
	{
		if (stricmp(pext, "html")==0)
			p="HTML_MIME_TYPE_HTML";
		else if (stricmp(pext, "htm")==0)
			p="HTML_MIME_TYPE_HTML";
		else if (stricmp(pext, "gif")==0)
			p="HTML_MIME_TYPE_GIF";
		else if (stricmp(pext, "jpg")==0)
			p="HTML_MIME_TYPE_JPEG";
		else if (stricmp(pext, "jpeg")==0)
			p="HTML_MIME_TYPE_JPEG";
		else if (stricmp(pext, "css")==0)
			p="HTML_MIME_TYPE_CSS";
		else if (stricmp(pext, "js")==0)
			p="HTML_MIME_TYPE_JSCRIPT";
		else if (stricmp(pext, "png")==0)
			p="HTML_MIME_TYPE_PNG";
		else if (stricmp(pext, "mng")==0)
			p="HTML_MIME_TYPE_MNG";
		else if (stricmp(pext, "bmp")==0)
			p="HTML_MIME_TYPE_BMP";
		else
			p="HTML_MIME_TYPE_PLAIN_TEXT";
	}
	return p;
}
char *NameToMimeCharSetName(char *Name)
{
char *p,*pext;
	pext = NameToExt(Name);
	p="WEBC_CHARSET_UNKNOWN";
	if (pext)
	{
		if (stricmp(pext, "html")==0)
			p="WEBC_CHARSET_ASCII";
		else if (stricmp(pext, "htm")==0)
			p="WEBC_CHARSET_ASCII";
		else if (stricmp(pext, "js")==0)
			p="WEBC_CHARSET_ASCII";
		else if (stricmp(pext, "gif")==0)
			p="WEBC_CHARSET_ASCII";
		else if (stricmp(pext, "css")==0)
			p="WEBC_CHARSET_ASCII";
		else
			p="WEBC_CHARSET_UNKNOWN";
	}
	return p;
}
char *TreeViewToFlags(struct TreeView * pTv,int depth)
{
	if (depth!=0)
	{
		if (!pTv->isFile)
			return "WEBC_VFILE_FOLDER";
		else
			return "0";
	}
	else
	{		
		if (!pTv->isFile)
			return "WEBC_VFILE_FOLDER|WEBC_VFILE_INROOT";
		else
			return "WEBC_VFILE_INROOT";
	}

}



// Recursively emit all entries in the file table.
static void FormatFileTable(struct TreeView *Top, int depth)
{
struct TreeView *pTv;
int TableEntry;
	pTv = Top;

	while (pTv)
	{
		/* The first record has Entrynumber -1, we don't want it */
		 if (pTv->TableEntryNumber == -1)
		 {
			 pTv = pTv->pSibling;
			 depth-=1; // We are staill at the root so hack it so our depth stay at 0
			 continue;
		 }
		if (!pTv->isFile)
		{	// Folders: size is the number of entries, pData contain the index record of the first record in the subfolder
			if (pTv->pChild)
				TableEntry = pTv->pChild->TableEntryNumber;
			else
				TableEntry = -1;
			fprintf(IncludeFileHandle,"/*%2d*/{\"%s\",%s,%d,(WEBC_PFBYTE)%d,%s,0,%s},\n",pTv->TableEntryNumber, pTv->pName, NameToMimeTypeName(pTv->pName), pTv->lSize, TableEntry, TreeViewToFlags(pTv,depth),	NameToMimeCharSetName(pTv->pName));

		}
		else
		{ // Files: size is the size of the data, pData contain the lable of the array holding the data.
			fprintf(IncludeFileHandle,"/*%2d*/{\"%s\",%s,%d,(WEBC_PFBYTE)%s,%s,0,%s},\n",pTv->TableEntryNumber, pTv->pName, NameToMimeTypeName(pTv->pName), pTv->lSize, pTv->contentarrayname, TreeViewToFlags(pTv,depth),	NameToMimeCharSetName(pTv->pName));
		}
		pTv = pTv->pSibling;
	}
	pTv = Top;
	while (pTv)
	{
		if (pTv->pChild)
		{
			FormatFileTable(pTv->pChild, depth+1);
		}
		pTv = pTv->pSibling;
	}
}



static struct TreeView *InitializeSubdirectoryEntry(char *name,int isFile)
{
	struct TreeView *pResult;
	pResult= (struct TreeView *)malloc(sizeof(struct TreeView));
	if (!pResult)
		callErrorExit("allocation error");
	memset(pResult, 0, sizeof(*pResult));
	pResult->pName = malloc_string_copy(name);
	char Buffer[256];
	sprintf(Buffer, "%s_%d", ARRAYNAMEBASE, content_array_count++);
	strcpy(pResult->contentarrayname,Buffer);
	pResult->isFile=isFile;
	return pResult;
}

int  EmitFileContent(char *arrayName, long size, char *path, char *filename)
{
	int charsInBuffer=0;
	unsigned char Buffer[1024];
	int l=0;
	long WhatsLeft = size;
	FILE *f;
	sprintf((char *) Buffer, "%s\\%s", path, filename);
	f = fopen((char *)Buffer,"rb");
	if (!f)
	{
		printf("Failed opening %s\n", Buffer);
		callErrorExit(0);
		return -1;
	}
	fprintf(DataFileHandle, "/* %s */\n", filename);

	if (!size)
	{
		fprintf(DataFileHandle, "const unsigned char %s[%d] =  {\n",arrayName,1);
		fprintf(DataFileHandle, "0x%2.2x",0);
	}
	else
		fprintf(DataFileHandle, "const unsigned char %s[%d] =  {\n",arrayName,size);
	// open with path , path, filename);
	while(WhatsLeft)
	{
		int whatIread = fread(Buffer, 1, 1024,f);
		if (whatIread < 0)
		{
			callErrorExit("Unexpected read error when converting file\n");
			return -1;
		}
		charsInBuffer = whatIread;
		for (int charsRead = 0; charsRead < charsInBuffer;charsRead++)
		{
			WhatsLeft--;
			if (WhatsLeft)
				l += fprintf(DataFileHandle, "0x%2.2x,",Buffer[charsRead]);
			else
				l += fprintf(DataFileHandle, "0x%2.2x",Buffer[charsRead]);

			if (l > 132)
			{
				fprintf(DataFileHandle, "\n");
				l = 0;
			}
		}
		if (charsInBuffer < 1024 && WhatsLeft)
		{
			printf("Size missmatch when converting file by %d\n", WhatsLeft);
			callErrorExit(0);
			break;
		}
	}
	fprintf(DataFileHandle, "\n};\n");
	fclose(f);
	return size;
}


static int traverseDosFile(struct TreeView *pRoot, char *name, char *inPath, int level)
{
struct _finddata_t statobj;
char path[256];
intptr_t dd;
int nEntries=0;
struct TreeView *pCurrentEntry = 0;

	pRoot->pName = malloc_string_copy(name);
    strcpy((char *)path, (char *)inPath);
    strcat((char *)path, "\\*.*");

    if ((dd = _findfirst((char *)path, &statobj)) < 0)
    {
        return(0);
    }
    else
    {
       do
        {
			struct TreeView *pResult=0;
            if ( (strcmp(".", statobj.name)==0) || (strcmp("..", statobj.name)==0)  )
                ;
            else if( statobj.attrib & _A_SUBDIR )
 				;
           else
           {
				nEntries += 1;
				struct TreeView *pResult = InitializeSubdirectoryEntry(statobj.name,1);
				if (pCurrentEntry)
					pCurrentEntry->pSibling=pResult;
				else
					pRoot->pChild=pResult;
				pCurrentEntry=pResult;
				pCurrentEntry->lSize = statobj.size;
				if (EmitFileContent(pCurrentEntry->contentarrayname,statobj.size, inPath,statobj.name)<0)
				{
					return -1;
				}
			}
        } while (_findnext(dd, &statobj) == 0);
        _findclose(dd);
    }
    if ((dd = _findfirst((char *)path, &statobj)) < 0)
    {
        return(0);
    }
    else
    {
        do
        {
			struct TreeView *pResult=0;
            if ( (strcmp(".", statobj.name)==0) ||  (strcmp("..", statobj.name)==0)  )
                ;
            else if( statobj.attrib & _A_SUBDIR )
            {
				nEntries += 1;
				int l = strlen(inPath) + strlen(statobj.name) + 2;
				char *path = (char *) malloc(l);
				strcpy((char *)path, (char *)inPath);
				strcat((char *)path, "\\");
				strcat((char *)path, statobj.name);
				struct TreeView *pResult = InitializeSubdirectoryEntry(statobj.name,0);

				if (pCurrentEntry)
					pCurrentEntry->pSibling=pResult;
				else
					pRoot->pChild=pResult;
				pCurrentEntry=pResult;
				pCurrentEntry->lSize = traverseDosFile(pResult, statobj.name, path, level+1);
				if (pCurrentEntry->lSize < 0)
				{
					callErrorExit("Unexpected error traversing file stystem\n");
					return -1;
				}
				free(path);
			}
        } while (_findnext(dd, &statobj) == 0);
        _findclose(dd);
    }
	return nEntries;
}


static char *malloc_string_copy(char *str)
{
	char *p = (char *)malloc(strlen(str)+1);
	if (p)
		strcpy(p, str);
	else
		callErrorExit("Memory allocation failed\n");

	return p;
}
