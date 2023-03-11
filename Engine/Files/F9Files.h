///////////////////////////////////////////////////////////////////////////////////////////////////
// F9Files.h
// order for opening files:
// 1. check archives if readonly
// 2. check resources if resources opened and readonly and IFile initialized
// 3. check if memory file ( name[0]=='#' as for "#hexaddr#hexsize#name.ext" )
// 4. open from disk
// Interface:
// F9FILE
// F9_Init, F9_Done, F9_IsReady
// F9_ArchiveOpen, F9_ArchiveClose, F9_ArchiveGetFileCount, F9_ArchiveGetFileName
// F9_ResourcesOpen, F9_ResourcesClose	
// F9_FileOpen, F9_FileClose, F9_FileRead, F9_FileWrite, F9_FileSeek, F9_FileTell, F9_FileEof, F9_FileSize
// F9_MakeFileName
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILES_H__
#define __F9FILES_H__

#include "E9Array.h"
#include "F9Archive.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// f9Files class
///////////////////////////////////////////////////////////////////////////////////////////////////
class f9Files
{
public:
					f9Files			();
virtual				~f9Files		();	
		void		Init			();
		void		Done			();

// archives serve
		int			ArchiveOpen				( char* name, int mode = F9_READ, char* password=NULL );	// returning archive index
		void		ArchiveClose			( int idx );												// close an archive
		f9Archive*	ArchiveGet				( int idx );												// get archive pointer
		int			ArchiveFind				( char* name );												// return archive index or -1 if not found
		int			ArchiveFindContaining	( char* filename );											// strictly look for the file name
		int			ArchiveFindContainingEx	( char* path );												// try to fit path with the archive path and look for the rest
		int			ArchiveGetFileCount		( int idx );												// return number of files in an archive
		char*		ArchiveGetFileName		( int idx, int fileidx );									// return a file's name in an archive

// resource serve
inline	void		ResourcesOpen			()										{ m_resources=1; } // open resources (files will be searched in resources)
inline	void		ResourcesClose			()										{ m_resources=0; } // close resources (files will not be searched in resources)

// files serve
		f9File*		FileOpen		( char* name, int mode = F9_READ );				// open a file; with search in archives and other tricks
		int			FileClose		( f9File* file );								// closes a file
	
public:
		cPArray<f9Archive>	m_archives;
		int					m_resources;	// 1 to search resources
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
extern	f9Files*	f9_files;			// global instance

		BOOL	F9_Init();
		void	F9_Done();
inline	BOOL	F9_IsReady()																	{ return f9_files!=NULL; }
inline	int		F9_ArchiveOpen			( char* name, int mode=F9_READ, char* password=NULL )	{ guardfast(F9_ArchiveOpen);	sassert(f9_files); return f9_files->ArchiveOpen(name,mode,password); unguardfast(); }
inline	void	F9_ArchiveClose			( int idx )												{ guardfast(F9_ArchiveClose);	sassert(f9_files); f9_files->ArchiveClose(idx); unguardfast(); }
inline	int		F9_ArchiveGetFileCount	( int idx )												{ guardfast(F9_ArchiveClose);	sassert(f9_files); return f9_files->ArchiveGetFileCount(idx); unguardfast(); }
inline	char*	F9_ArchiveGetFileName	( int idx, int fileidx )								{ guardfast(F9_ArchiveClose);	sassert(f9_files); return f9_files->ArchiveGetFileName(idx,fileidx); unguardfast(); }
inline	void	F9_ResourcesOpen		()														{ guardfast(F9_ResourcesOpen);	sassert(f9_files); f9_files->ResourcesOpen(); unguardfast(); }
inline	void	F9_ResourcesClose		()														{ guardfast(F9_ResourcesClose);	sassert(f9_files); f9_files->ResourcesClose(); unguardfast(); }
inline	F9FILE	F9_FileOpen				( char* name, int mode=F9_READ )						{ guardfast(F9_FileOpen);		sassert(f9_files); return f9_files->FileOpen(name, mode); unguardfast(); }
inline	int		F9_FileClose			( F9FILE file )											{ guardfast(F9_FileClose);		sassert(f9_files); return f9_files->FileClose(file); unguardfast(); }
inline	int		F9_FileRead				( void* buffer, int size, F9FILE file )					{ guardfast(F9_FileRead);		sassert(file); return (int)file->Read( buffer, size ); unguardfast(); }
inline	int		F9_FileWrite			( void* buffer, int size, F9FILE file )					{ guardfast(F9_FileWrite);		sassert(file); return (int)file->Write( buffer, size ); unguardfast(); }
inline	int		F9_FileSeek				( F9FILE file, int offset, int origin=F9_SEEK_SET )		{ guardfast(F9_FileSeek);		sassert(file); return file->Seek( offset, origin ); unguardfast(); }
inline	int		F9_FileTell				( F9FILE file )											{ guardfast(F9_FileTell);		sassert(file); return (int)file->Tell(); unguardfast(); }
inline	int		F9_FileEof				( F9FILE file )											{ guardfast(F9_FileEof);		sassert(file); return file->Eof(); unguardfast(); }
inline	int		F9_FileSize				( F9FILE file )											{ guardfast(F9_FileSize);		sassert(file); return (int)file->Size(); unguardfast(); }

// helper for memory files ext must not exceed 5 characters but may be NULL - uses sprint!
inline	char*	F9_MakeFileName		( char* name, void* addr, int size )						{ return sprint("#%x#%x#%s",(dwordptr)addr,size,name?name:""); }

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
// User funcions (can be used as callbacks)
///////////////////////////////////////////////////////////////////////////////////////////////////
void*	file_open		( char* name, int mode=FS_READ );
int		file_close		( void* file );
int		file_read		( void* buffer, int size, void* file );
int		file_write		( void* buffer, int size, void* file );
int		file_seek		( void* file, int offset, int origin=SEEK_SET );
int		file_tell		( void* file );
int		file_eof		( void* file );
int		file_size		( void* file );
*/

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
