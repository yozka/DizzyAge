//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "util.h"
#include "shlobj.h"
#include "resource.h"

extern HWND g_hwnd;
extern HINSTANCE g_hinstance;

//////////////////////////////////////////////////////////////////////////////////////////////////
// LOG
//////////////////////////////////////////////////////////////////////////////////////////////////
void Log( HWND hwnd, char* text )
{
	if(text==NULL)
	{
		SetWindowText(hwnd,"");
		return;
	}

	int size = strlen(text);
	if(size==0) return;

	// buffer
	char* buffer;
	int buffersize;
	buffersize = GetWindowTextLength(hwnd);
	buffersize += size+1;
	buffer = (char*)malloc(buffersize); buffer[0]=0;
	GetWindowText(hwnd,buffer,buffersize);
	strcat(buffer,text);
	SetWindowText(hwnd,buffer);
	free(buffer);

	// scroll
	int lines = SendMessage( hwnd, EM_GETLINECOUNT, 0, 0 );
	SendMessage( hwnd, EM_LINESCROLL, 0, lines );
	// update
	UpdateWindow(hwnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Files
///////////////////////////////////////////////////////////////////////////////////////////////////
char* file_getfullpath( char* file )
{
	guard(file_getfullpath)
	static char path[256];
	if(GetFullPathName( file, 255, path, NULL )!=0) return path;
	return NULL;
	unguard()
}

char* file_path2file(char* path)
{
	guard(file_path2file)
	char* p = NULL;
	if(!path) return NULL;
	if(!(p = strrchr(path, '\\')))	return path;
	if(strlen((p + 1)) > 0)			return (p + 1);
	return NULL;
	unguard()
}

int	file_path2dir(char* path)
{
	guard(file_path2dir)
	char *p = NULL;
	if(!path || !(p = file_path2file(path))) return 0;
	return (int)(p - path);
	unguard()
}

char* file_path2ext(char* path)
{
	guard(file_path2ext)
	char *p = NULL;
	if(!path) return NULL;
	if(!(p = strrchr(path, '.')))	return NULL;
	if(strlen((p + 1)) > 0)			return  (p + 1);
	return NULL;
	unguard()
}

char file_path2drive(char* path)
{
	guard(file_path2drive);
	if( !path || path[0]==0 || path[1] != ':') return 0;
	return path[0];
	unguard();
}

void file_pathsplit( char* path, char* drive, char* dir, char* fname, char* ext )
{
	guard(file_pathsplit)
	_splitpath( path, drive, dir, fname, ext );
	unguard()
}

void file_findfiles( char* path, file_ffcallback ffcallback, dword flags )
{
	char fpath[256];
	char spath[256];
	if(path) strcpy(spath,path); else spath[0]=0;
	strcat(spath,"*.*");

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile( spath, &data );
	if(h==INVALID_HANDLE_VALUE) return;
	do
	{	// check
		if(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) continue;

		// directory
		if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
		{
			if( 0==strcmp(data.cFileName,"." ) ) continue;
			if( 0==strcmp(data.cFileName,".." ) ) continue;

			if(path) strcpy(fpath,path); else fpath[0]=0;
			strcat(fpath,data.cFileName);
			_strlwr(fpath);
			if( flags & FILE_FINDDIR )	ffcallback( fpath, 1 );
			strcat(fpath,"\\");
			if( flags & FILE_FINDREC )	file_findfiles( fpath, ffcallback, flags );
		}
		else // file
		{
			if(path) strcpy(fpath,path); else fpath[0]=0;
			strcat(fpath,data.cFileName);
			_strlwr(fpath);
			ffcallback( fpath, 0 );
		}

	}while(FindNextFile(h,&data));
	
	FindClose(h);
}

void file_delete( char* path )
{
	DeleteFile(path);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// PARSER
//////////////////////////////////////////////////////////////////////////////////////////////////
char* parser_skipchar( char* buffer, char* charlist, int& parsedsize )
{
	guard(parser_skipchar);
	parsedsize = 0;
	while(*buffer!=0)
	{
		int i=0;
		while(charlist[i]!=0)
		{
			if(*buffer==charlist[i])
				break;
			i++;
		}
		if(charlist[i]==0) // none matched
			return buffer;
		buffer++;
		parsedsize++;
	}
	return buffer;
	unguard();
}

char* parser_skiptochar( char* buffer, char* charlist, int& parsedsize )
{
	guard(parser_skiptochar);
	parsedsize = 0;
	while(*buffer!=0)
	{
		int i=0;
		while(charlist[i]!=0)
		{
			if(*buffer==charlist[i])
				break;
			i++;
		}
		if(charlist[i]!=0) // one matched
			return buffer;
		buffer++;
		parsedsize++;
	}
	return buffer;
	unguard();
}

char* parser_skipline( char* buffer, int& parsedsize )
{
	guard(parser_skipline);
	char* bufferstart = buffer;
	buffer = parser_skiptochar(buffer,"\n\r", parsedsize);
	buffer = parser_skipchar(buffer,"\n\r", parsedsize);
	parsedsize = (int)(intptr)(buffer-bufferstart);
	return buffer;
	unguard();
}


char* parser_skipspace( char* buffer, int& parsedsize )
{
	guard(parser_skipspace);
	return parser_skipchar(buffer," \t\r\n",parsedsize);
	unguard();
}

char* parser_skiptotoken( char* buffer, char* token, int& parsedsize )
{
	guard(parser_skiptotoken);
	parsedsize=0;
	while(*buffer!=0)
	{
		int i=0;
		while(buffer[i]==token[i] && buffer[i]!=0 && token[i]!=0)
			i++;
		if(token[i]==0) return buffer; // got it !
		buffer++;
		parsedsize++;
	}
	return buffer;
	unguard();
}

BOOL parser_readtoken( char* buffer, char* token, int& parsedsize )
{
	guard(parser_readtoken);
	parsedsize=0;
	while(*buffer==*token)
	{
		buffer++;
		token++;
		parsedsize++;
	}
	return *token==0;
	unguard();
}

BOOL parser_readword( char* buffer, char* value, int valuesize, int& parsedsize )
{
	guard(parser_readword);
	parsedsize=0;
	while(*buffer!=0 && *buffer!=' ' && *buffer!='\t' && *buffer!='\n' && *buffer!='\r' && parsedsize<valuesize-1)
	{
		*value = *buffer;
		buffer++;
		value++;
		parsedsize++;
	}
	*value=0;
	return parsedsize>0;
	unguard();
}

BOOL parser_readline( char* buffer, char* value, int valuesize, int& parsedsize )
{
	guard(parser_readline);
	int read=0;
	parsedsize=0;
	while(*buffer!=0 && *buffer!='\n' && *buffer!='\r' && read<valuesize-1)
	{
		*value=*buffer;
		buffer++;
		value++;
		read++;
		parsedsize++;
	}
	if(*buffer=='\r')
	{
		buffer++;
		parsedsize++;
	}
	if(*buffer=='\n')
	{
		buffer++;
		parsedsize++;
	}
	*value = 0;
	return parsedsize>0;
	unguard();
}

BOOL parser_readvarstr( char* buffer, char* name, char* value, int valuesize, int& parsedsize )
{
	guard(parser_readvarstr);
	char* bufferstart = buffer;
	
	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,name,parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,"=",parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	parser_readline(buffer,value,valuesize,parsedsize);
	buffer += parsedsize;

	parsedsize = (int)(intptr)(buffer-bufferstart);
	return TRUE;
	unguard();
}

BOOL parser_readvarint( char* buffer, char* name, int* value, int& parsedsize )
{
	guard(parser_readvarint);
		char* bufferstart = buffer;
	
	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,name,parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,"=",parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	char sztmp[64];	sztmp[0]=0;
	parser_readword(buffer,sztmp,64,parsedsize);
	buffer += parsedsize;
	buffer=parser_skipline(buffer,parsedsize);

	int ret = sscanf(sztmp,"%i",value);
	if(ret!=1) return FALSE;

	parsedsize = (int)(intptr)(buffer-bufferstart);
	return TRUE;
	unguard();
}

BOOL parser_readvarfloat( char* buffer, char* name, float* value, int& parsedsize )
{
	guard(parser_readvarfloat);
		char* bufferstart = buffer;
	
	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,name,parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,"=",parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	char sztmp[64];	sztmp[0]=0;
	parser_readword(buffer,sztmp,64,parsedsize);
	buffer += parsedsize;
	buffer=parser_skipline(buffer,parsedsize);

	int ret = sscanf(sztmp,"%f",value);
	if(ret!=1) return FALSE;

	parsedsize = (int)(intptr)(buffer-bufferstart);
	return TRUE;
	unguard();
}

void parser_trimbackspace( char* buffer, int& pos )
{
	guard(parser_trimbackspace);
	while(pos>0 && (buffer[pos-1]==' ' || buffer[pos-1]=='\t' || buffer[pos-1]=='\r' || buffer[pos-1]=='\n') )
		pos--;
	buffer[pos]=0;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
