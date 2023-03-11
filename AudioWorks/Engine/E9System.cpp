///////////////////////////////////////////////////////////////////////////////////////////////////
// E9System.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E9System.h"

#ifdef E9_ENABLE_ZIP
// make sure the paths to the libs are ok
#include "zlib.h"
#ifdef _DEBUG
#pragma comment( lib, "..\\Libs\\ZLib\\zlib_d.lib" )
#else
#pragma comment( lib, "..\\Libs\\ZLib\\zlib.lib" )
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// System
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL sys_senddata( HWND fromhwnd, HWND tohwnd, int cmd, int slot, int size, char* data )
{
	guard(sys_senddata);
	if(!tohwnd) return FALSE;
	COPYDATASTRUCT copystruct;
	copystruct.dwData = MAKEWORD(cmd,slot); // low,high
	copystruct.cbData = size;
	copystruct.lpData = data;
	LRESULT hr = SendMessage( tohwnd, WM_COPYDATA, (WPARAM)fromhwnd, (LPARAM)&copystruct );
	return (!FAILED(hr));
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Ini Tools
///////////////////////////////////////////////////////////////////////////////////////////////////
#define	_GETPRIVATEPROFILEINT( type, tc )												\
{																						\
	char sz[64]; sz[0]=0;																\
	type temp;																			\
	GetPrivateProfileString( group, key, "", sz, 64, file );							\
	if( 1==sscanf( sz, tc, &temp ) ) { *(type*)value = temp; return TRUE; }				\
	return FALSE;																		\
}

#define	_SETPRIVATEPROFILEINT( type, tc )												\
{																						\
	char sz[64];																		\
	sprintf(sz,tc,(type)value);															\
	WritePrivateProfileString( group, key, sz, file );									\
}

BOOL ini_getchr( char* file, char* group, char* key, char* value )
{
	guard(ini_getchr)
	_GETPRIVATEPROFILEINT( char, "%c" );
	unguard()
}

BOOL ini_getint( char* file, char* group, char* key, int* value )
{
	guard(ini_getint)
	_GETPRIVATEPROFILEINT( int, "%i" );
	unguard()
}

BOOL ini_getflt( char* file, char* group, char* key, float* value )
{
	guard(ini_getflt)
	_GETPRIVATEPROFILEINT( float, "%f" );
	unguard()
}

BOOL ini_getstr( char* file, char* group, char* key, char* value, int size )
{
	guard(ini_getchr)
	return(0!=GetPrivateProfileString( group, key, "", value, size, file ));
	unguard()
}

BOOL ini_getbin( char* file, char* group, char* key, void* value, int size )
{
	guard(ini_getchr)
	return GetPrivateProfileStruct( group, key, value, size, file );
	unguard()
}

void ini_setchr( char* file, char* group, char* key, char value )
{
	guard(ini_setchr)
	_SETPRIVATEPROFILEINT( char, "%c" );
	unguard()
}

void ini_setint( char* file, char* group, char* key, int value )
{
	guard(ini_setint)
	_SETPRIVATEPROFILEINT( int, "%i" );
	unguard()
}

void ini_setflt( char* file, char* group, char* key, float value )
{
	guard(ini_setflt)
	_SETPRIVATEPROFILEINT( float, "%f" );
	unguard()
}

void ini_setstr( char* file, char* group, char* key, char* value )
{
	guard(ini_setstr)
	WritePrivateProfileString( group, key, value, file );
	unguard()
}

void ini_setbin( char* file, char* group, char* key, void* value, int size )
{
	guard(ini_setbin)
	WritePrivateProfileStruct( group, key, value, size, file );
	unguard()
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


///////////////////////////////////////////////////////////////////////////////////////////////////
// Encription
///////////////////////////////////////////////////////////////////////////////////////////////////
void encrypt_data( void* data, dword size, int key )
{
	guard(encrypt_data)
	if(data==NULL || size==0) return;
	for(dword i=0;i<size;i++)
		*((byte*)data+i) ^= key;
	unguard()
}

void decrypt_data( void* data, dword size, int key )
{
	guard(decrypt_data)
	if(data==NULL || size==0) return;
	encrypt_data( data, size, key ); // we use a simple xor method
	unguard()
}

void encrypt_data( void* data, dword size, char* key )
{
	guard(encrypt_data)
	if(data==NULL || size==0 || key==NULL) return;
	int len = (int)strlen(key);
	for(dword i=0;i<size;i++)
		*((byte*)data+i) ^= (byte)i + (byte)key[i%len];
	unguard()
}

void decrypt_data( void* data, dword size, char* key )
{
	guard(decrypt_data)
	if(data==NULL || size==0 || key==NULL) return;
	encrypt_data( data, size, key ); // we use a simple xor method
	unguard()
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Compression
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef E9_ENABLE_ZIP

BOOL compress_data( void* srcdata, dword srcsize, void* dstdata, dword &dstsize )
{
	guard(compress_data)
	sassert(srcdata!=NULL && srcsize>0);
	sassert(dstdata!=NULL && dstsize>=srcsize+srcsize/1000+32);
	unsigned long ss = srcsize;
	unsigned long ds = dstsize;
	if(Z_OK!=compress( (unsigned char *)dstdata, &ds, (unsigned char *)srcdata, ss ))  return FALSE;
	dstsize = ds;
	return TRUE;
	unguard()
}

BOOL decompress_data( void* srcdata, dword srcsize, void* dstdata, dword &dstsize )
{
	guard(decompress_data)
	sassert(srcdata!=NULL && srcsize>0);
	sassert(dstdata!=NULL);
	unsigned long ss = srcsize;
	unsigned long ds = dstsize;
	if(Z_OK!=uncompress( (unsigned char *)dstdata, &ds, (unsigned char *)srcdata, ss )) return FALSE;
	dstsize = ds;
	return TRUE;
	unguard()
}

#else

BOOL compress_data( void* srcdata, dword srcsize, void* dstdata, dword &dstsize ) { return FALSE; }
BOOL decompress_data( void* srcdata, dword srcsize, void* dstdata, dword &dstsize ) { return FALSE; }

#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
