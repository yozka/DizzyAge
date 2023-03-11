///////////////////////////////////////////////////////////////////////////////////////////////////
// F9Files.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "F9Files.h"
#include "F9FileMem.h"
#include "F9FileRes.h"
#include "F9ArchiveZip.h"
#include "F9ArchivePak.h"

f9Files::f9Files()
{
	guard(f9Files::f9Files);
	m_resources = 0;
	unguard();
}

f9Files::~f9Files()
{
	guard(f9Files::~f9Files);
	unguard();
}

void f9Files::Init()
{
	guard(f9Files::Init);
	m_archives.Init(16);
	unguard();
}

void f9Files::Done()
{
	guard(f9Files::Done);
	for(int i=0;i<m_archives.Size();i++) ArchiveClose(i);
	m_archives.Done();
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int f9Files::ArchiveOpen( char* name, int mode, char* password )
{
	guard(f9Files::ArchiveOpen);

	// check if already opened (in list)
	int idx = ArchiveFind( name );
	if( idx!=-1 ) return idx;

	// create archive
	f9Archive* arc = NULL;
	#ifndef EXCLUDE_FILEZIP
	if( mode & F9_ARCHIVE_ZIP )	arc = snew f9ArchiveZip(); else
	#endif
	#ifndef	EXCLUDE_FILEPAK
	if( mode & F9_ARCHIVE_PAK )	arc = snew f9ArchivePak();
	#endif
	if( arc==NULL ) return -1;
	mode &= 255; // remove archive type from the mode

	if( arc->Open( name, mode, password )!=F9_OK )
	{
		sdelete(arc);
		return -1;
	}

	idx = m_archives.Add(arc); sassert(idx!=-1);
	return idx;
	unguard();
}

void f9Files::ArchiveClose( int idx )
{
	guard(f9Files::ArchiveClose);
	if(!m_archives.Get(idx)) return;
	m_archives.Get(idx)->Close();
	m_archives.Del(idx);
	unguard();
}

f9Archive* f9Files::ArchiveGet( int idx )
{
	guard(f9Files::ArchiveGet);
	return m_archives.Get(idx);
	unguard();
}

int f9Files::ArchiveFind( char* name )
{
	guard(f9Files::ArchiveFind);
	for(int i=0; i<m_archives.Size(); i++)
		if(m_archives.Get(i) && 0==stricmp(m_archives.Get(i)->m_name, name) ) 
			return i;
	return -1;
	unguard();
}

int f9Files::ArchiveFindContaining( char* filename )
{
	guard(f9Files::GetArchiveContaining);
	if(m_archives.Size()==0) return -1;
	for(int i=0; i<m_archives.Size(); i++)
		if( m_archives.Get(i) && m_archives.Get(i)->FileFind( filename ) != -1 )	
			return i;
	return -1;
	unguard();
}

int f9Files::ArchiveFindContainingEx( char* path )
{
	guard(f9Files::ArchiveFindContainingEx);
	if(m_archives.Size()==0) return -1;

	int i,j,k;
	k = (int)strlen(path);
	for(i=0; i<m_archives.Size(); i++)
	{
		if(m_archives.Get(i)==NULL) continue;
		char* arcname = m_archives.Get(i)->m_name;	sassert(arcname);
		char* sz = file_path2file( arcname );		sassert(sz);
		int ap =(int)(sz - arcname);
		if( ap >= k ) continue; // file path is smaller than archive dir !
		
		for( j=0; j<ap; j++)
			if( tolower(path[j]) != tolower(arcname[j]) ) break;

		if( j<ap ) continue; // archive dir not at the begining of file dir !

		if( m_archives.Get(i)->FileFind( path+ap ) != -1 )	return i;
	}

	return -1;
	unguard();
}

int	f9Files::ArchiveGetFileCount( int idx )
{
	guard(f9Files::ArchiveGetFileCount);
	f9Archive* archive = m_archives.Get(idx);		
	if(!archive) return 0;
	return archive->FileCount();
	unguard();
}

char* f9Files::ArchiveGetFileName( int idx, int fileidx )
{
	guard(f9Files::ArchiveGetFileName);
	f9Archive* archive = m_archives.Get(idx);		
	if(!archive) return NULL;
	if(idx<0 || idx>=archive->FileCount()) return NULL;
	return archive->FileGetName(fileidx);
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
f9File* f9Files::FileOpen( char* name, int mode )
{
	guard(f9Files::FileOpen);
	if( name==NULL ) return NULL;
	BOOL readonly = F9_ISREADONLY(mode);

	// ARCHIVE FILE
	if(readonly)
	{
		int idx = ArchiveFindContainingEx( name );
		if(idx!=-1)
		{
			f9Archive* arc;
			arc = ArchiveGet(idx); sassert(arc);
			char* sz = file_path2file(arc->m_name); sassert(sz);
			int ap = (int)( sz - arc->m_name );
			return arc->FileOpen( name+ap, mode ); // we open the path from inside the archive
		}
	}

	f9File* file;

	// MEMORY FILE
	if( name[0]=='#' )
	{
		file = snew f9FileMem();
		if(file->Open(name,mode)==F9_OK) return file;
		sdelete(file);
	}
	
	// RESOURCES FILES
	if( readonly && m_resources )
	{
		file = snew f9FileRes();
		if(file->Open(name,mode)==F9_OK) return file;
		sdelete(file);
	}
	
	//FILE DISK
	file = snew f9FileDisk();
	if(file->Open(name,mode)==F9_OK) return file;
	sdelete(file);

	return NULL;
	unguard();
}

int f9Files::FileClose( f9File* file )
{
	guard(f9Files::FileClose);
	if(file==NULL) return F9_FAIL;
	if(file->Close()!=F9_OK) return F9_FAIL;
	sdelete(file);
	return F9_OK;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
f9Files* f9_files = NULL;

BOOL F9_Init()
{
	guard(F9_Init);
	if(f9_files) return TRUE;
	dlog(LOGFIL, "Files init.\n");
	f9_files = snew f9Files();
	f9_files->Init();
	return TRUE;
	unguard();
}

void F9_Done()
{
	guard(F9_Done);
	if(!f9_files) return;
	f9_files->Done();
	sdelete( f9_files );
	f9_files = NULL;
	dlog(LOGFIL, "Files done.\n");
	unguard();
}

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
// User funcions (can be used as callbacks)
///////////////////////////////////////////////////////////////////////////////////////////////////
void*	file_open		( char* name, int mode )					{ return f9Files::FileOpen(name, mode); }
int		file_close		( void* file )								{ return f9Files::FileClose((cFile*)file); }
int		file_read		( void* buffer, int size, void* file )		{ return ((cFile*)file)->Read( buffer, size ); }
int		file_write		( void* buffer, int size, void* file )		{ return ((cFile*)file)->Write( buffer, size ); }
int		file_seek		( void* file, int offset, int origin )		{ return ((cFile*)file)->Seek( offset, origin ); }
int		file_tell		( void* file )								{ return ((cFile*)file)->Tell(); }
int		file_eof		( void* file )								{ return ((cFile*)file)->Eof(); }
int		file_size		( void* file )								{ return ((cFile*)file)->Size(); }
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
