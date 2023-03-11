/////////////////////////////////////////////////////////////////////////////////////////
// GSCFG.H
// Change settings in this file to personalize scripter
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GSCFG_
#define _GSCFG_

#include <stdlib.h>
#include <conio.h>
#include <assert.h>

#include "E9System.h"
#include "E9Engine.h"
#include "D9Debug.h"
#include "F9Files.h"

#define GS_VER	"v2.2"
#define GS_DATE	"080401"


/////////////////////////////////////////////////////////////////////////////////////////
// DATA TYPES
// Scripter need these data types ( int = 32bits )
/////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char		byte;		// 8  bit unsigned integer
typedef unsigned short		word;		// 16 bit unsigned integer
typedef unsigned int		dword;		// 32 bit
typedef	int					BOOL;		// 32 bit int
typedef	INT_PTR				intptr;		// int pointer


#ifndef TRUE
#define TRUE				1
#endif
#ifndef FALSE
#define FALSE				0
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// MEMORY FUNCTIONS
// Change if you want to use your memory manager
/////////////////////////////////////////////////////////////////////////////////////////
#define gsmalloc	smalloc
#define gsrealloc	srealloc
#define gsfree		sfree
#define	gsnew		snew
#define	gsdelete	sdelete

/*
// provided allocation functions
inline	void*		_gsmalloc	( int size )			{ return malloc(size); }
inline	void*		_gsrealloc	( void* ptr, int size )	{ return realloc(ptr,size); }
inline	void		_gsfree		( void* ptr )			{ if(ptr) free(ptr); ptr=NULL; }
*/


/////////////////////////////////////////////////////////////////////////////////////////
// FILE FUNCTIONS
// Change if you want to use your file manager
/////////////////////////////////////////////////////////////////////////////////////////
inline	void*	gsfile_open		( char* name, int mode=0 )							{ return F9_FileOpen(name, mode); }
inline	int		gsfile_close	( void* file )										{ return F9_FileClose((F9FILE)file); }
inline	long	gsfile_read		( void* buffer, long size, void* file )				{ return F9_FileRead(buffer, size, (F9FILE)file); }
inline	long	gsfile_write	( void* buffer, long size, void* file )				{ return F9_FileWrite(buffer, size, (F9FILE)file); }
inline	int		gsfile_seek		( void* file, long offset, int origin=SEEK_SET )	{ return F9_FileSeek((F9FILE)file, offset, origin); }
inline	long	gsfile_tell		( void* file )										{ return F9_FileTell((F9FILE)file); }

/*
// provided file functions
inline	void*	gsfile_open		( char* name, int mode=0 )							{ return fopen(name, (mode==0)?"rb":(mode==1)?"wb":"r+b"); }
inline	int		gsfile_close	( void* file )										{ return fclose((FILE*)file); }
inline	long	gsfile_read		( void* buffer, long size, void* file )				{ return fread(buffer, 1, size, (FILE*)file); }
inline	long	gsfile_write	( void* buffer, long size, void* file )				{ return fwrite(buffer, 1, size, (FILE*)file); }
inline	int		gsfile_seek		( void* file, long offset, int origin=SEEK_SET )	{ return fseek((FILE*)file, offset, origin); }
inline	long	gsfile_tell		( void* file )										{ return ftell((FILE*)file); }
*/


/////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION GUARDS
// Change if you want to use your function call tracking and try/catch system
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef guard
#define guard(fn)
#endif

#ifndef unguard
#define unguard()
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// LOGS
// Change if you want to adjust the way messages are logged
/////////////////////////////////////////////////////////////////////////////////////////
void	gslog		( const char *fmt, ...);
void	gslogv		( const char *fmt, va_list args);


/////////////////////////////////////////////////////////////////////////////////////////
// ASSERT
// Change if you want to use your own assert system
/////////////////////////////////////////////////////////////////////////////////////////
#define	gsassert	sassert
#define	gsassertvm	_gsassertvm

// provided assert
// #define	_gsassert( exp )		{ if(!(exp)) { printf("GSASSERT: %s AT: %i, %s\n", #exp, __LINE__, __FILE__ ); assert(exp); } }
#define _gsassertvm( vm, exp )	{ if(!(exp)) { printf("GSASSERT: %s AT: %i, %s\n", #exp, __LINE__, __FILE__ ); gs_error(vm, GSE_ASSERT, #exp); } }


/////////////////////////////////////////////////////////////////////////////////////////
// ERROREXIT
// Change if you want to use your own error exit
/////////////////////////////////////////////////////////////////////////////////////////
#define	gserrorexit	_gserrorexit

// provided errorexit
void	_gserrorexit( char* text );


#endif
/////////////////////////////////////////////////////////////////////////////////////////
