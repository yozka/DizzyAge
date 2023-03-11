///////////////////////////////////////////////////////////////////////////////////////////////////
// F9Files.h NONE
// Default files system (rename it and use it instead of F9Files.h)
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILES_H__
#define __F9FILES_H__

#include "E9System.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
#define F9_FAIL				-1
#define F9_OK				0

// open modes
#define F9_READ				0	// "rb"
#define F9_WRITE			1	// "wb"
#define F9_READWRITE		2	// "r+b"
#define F9_WRITEREAD		3	// "w+b"
#define F9_PROTECT			128	// protected flag

// seek modes ( better to match original values: 0,1,2 )
#define F9_SEEK_SET			SEEK_SET
#define F9_SEEK_CUR			SEEK_CUR
#define F9_SEEK_END			SEEK_END

// archive not supported
#define F9_ARCHIVE_NONE		(0<<8)
#define F9_ARCHIVE_ZIP		(1<<8)
#define F9_ARCHIVE_PAK		(2<<8)

typedef FILE* F9FILE;

inline	BOOL	F9_Init()																	{ return TRUE; }
inline	void	F9_Done()																	{}
inline	int		F9_ArchiveOpen		( char* name, int mode=F9_READ )						{ return F9_OK; }
inline	void	F9_ArchiveClose		( int idx )												{}
inline	int		F9_ArchiveGetFileCount	( int idx )											{ return 0; }
inline	char*	F9_ArchiveGetFileName	( int idx, int fileidx )							{ return NULL; }
inline	void	F9_ResourcesOpen	()														{}
inline	void	F9_ResourcesClose	()														{}
inline	F9FILE	F9_FileOpen			( char* name, int mode=F9_READ )						{ return fopen(name,(mode==F9_READ)?"rb":(mode==F9_WRITE)?"wb":"r+b"); }
inline	int		F9_FileClose		( F9FILE file )											{ return fclose(file)==0 ? F9_OK : F9_FAIL; }
inline	int		F9_FileRead			( void* buffer, int size, F9FILE file )					{ return (int)fread(buffer,1,size,file); }
inline	int		F9_FileWrite		( void* buffer, int size, F9FILE file )					{ return (int)fwrite(buffer,1,size,file); }
inline	int		F9_FileSeek			( F9FILE file, int offset, int origin=F9_SEEK_SET )		{ return fseek( file, offset, origin ); }
inline	int		F9_FileTell			( F9FILE file )											{ return (int)ftell(file); }
inline	int		F9_FileEof			( F9FILE file )											{ return feof(file); }
inline	int		F9_FileSize			( F9FILE file )											{ int pos=ftell(file); fseek(file,0,2); int size=ftell(file); fseek(file,pos,0); return size; }

inline	char*	F9_MakeFileName		( char* name, void* addr, int size )					{ return NULL; } // not supported

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
