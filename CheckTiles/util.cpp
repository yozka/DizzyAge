//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Files
//////////////////////////////////////////////////////////////////////////////////////////////////
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
			if( flags & FINDFILE_DIR )	ffcallback( fpath, 1 );
			strcat(fpath,"\\");
			if( flags & FINDFILE_REC )	file_findfiles( fpath, ffcallback, flags );
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

char* file_path2file(char* path)
{
	char* p = NULL;
	if(!path) return NULL;
	if(!(p = strrchr(path, '\\')))	return path;
	if(strlen((p + 1)) > 0)			return (p + 1);
	return NULL;
}

char* file_path2ext(char* path)
{
	char *p = NULL;
	if(!path) return NULL;
	if(!(p = strrchr(path, '.')))	return NULL;
	if(strlen((p + 1)) > 0)			return  (p + 1);
	return NULL;
}

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

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
