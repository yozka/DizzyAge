//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_H__
#define __UTIL_H__

#define MAX_TAGNUMBER	999999

//////////////////////////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char		byte;		// 8  bit unsigned integer
typedef unsigned short		word;		// 16 bit unsigned integer
typedef unsigned int		dword;		// 32 bit unsigned integer
typedef int					BOOL;		// boolean data type

typedef	INT_PTR				intptr;		// pointer int
typedef	UINT_PTR			uintptr;	// pointer unsigned int
typedef	UINT_PTR			dwordptr;	// pointer dword
typedef	LONG_PTR			longptr;	// pointer long
typedef ULONG_PTR			ulongptr;	// pointer unsigned long

#define sassert(e)
#define smalloc	malloc
#define sfree	free
#define snew	new
#define sdelete	delete
#define guard(e)
#define unguard()

//////////////////////////////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG( text )				Log(g_hwndlog, text)
#define LOGLN( text )			{ Log(g_hwndlog, text); Log(g_hwndlog, "\r\n"); }
void	Log( HWND hwnd, char* text );

///////////////////////////////////////////////////////////////////////////////////////////////////
// Files
///////////////////////////////////////////////////////////////////////////////////////////////////
#define	FILE_FINDDIR		(1<<0)								// request directories list
#define	FILE_FINDREC		(1<<1)								// request to go recursive on found directories
typedef void (*file_ffcallback) ( char* filepath, BOOL dir );	// filepath=found file including requested path, dir=1 if filepath is a directory

char*	file_getfullpath	( char* file );
char*	file_path2file		( char* path );
int		file_path2dir		( char* path );
char*	file_path2ext		( char* path );
char	file_path2drive		( char* path );
void	file_pathsplit		( char* path, char* drive, char* dir, char* fname, char* ext );	// bufers must have enought space _MAX_DRIVE, _MAX_DIR, _MAX_FNAME, _MAX_EXT
void	file_findfiles		( char* path, file_ffcallback ffcallback, dword flags );		// path must include terminal '\'; use FINDFILE flags
void	file_delete			( char* path );

///////////////////////////////////////////////////////////////////////////////////////////////////
// Parser
///////////////////////////////////////////////////////////////////////////////////////////////////
char*	parser_skipchar( char* buffer, char* charlist, int& parsedsize );		// skip any of those chars
char*	parser_skiptochar( char* buffer, char* charlist, int& parsedsize );		// skip until one of those chars
char*	parser_skipline( char* buffer, int& parsedsize );
char*	parser_skipspace( char* buffer, int& parsedsize );
char*	parser_skiptotoken( char* buffer, char* token, int& parsedsize );		// skip all until token find
BOOL	parser_readtoken( char* buffer, char* token, int& parsedsize );
BOOL	parser_readword( char* buffer, char* value, int valuesize, int& parsedsize );
BOOL	parser_readline( char* buffer, char* value, int valuesize, int& parsedsize );
BOOL	parser_readvarstr( char* buffer, char* name, char* value, int valuesize, int& parsedsize );
BOOL	parser_readvarint( char* buffer, char* name, int* value, int& parsedsize );
BOOL	parser_readvarfloat( char* buffer, char* name, float* value, int& parsedsize );
void	parser_trimbackspace( char* buffer, int& pos );							// cuts end spaces, and updates pos (pos=strlen(buffer))

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
