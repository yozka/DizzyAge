///////////////////////////////////////////////////////////////////////////////////////////////////
// F9ArchiveZip.h
// This file handles .zip archives
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9ARCHIVEZIP_H__
#define __F9ARCHIVEZIP_H__

#include "E9List.h"
#include "E9Hash.h"
#include "F9Archive.h"
#include "F9FileDisk.h"
#include "..\\Libs\\zlib\\zlib.h"

struct f9ZipFileInfo
{
	dword	m_offset;
	dword	m_size;
	cStr	m_name;
};

class f9ArchiveZip : public f9Archive
{
public:

						f9ArchiveZip	();
virtual					~f9ArchiveZip	();
						
virtual	int				Open			( char *name, int mode = F9_READ, char* password=NULL );
virtual	int				Close			();
						
virtual	f9File*	 		FileOpen		( char* name, int mode = F9_READ );
virtual	int				FileClose		( f9File* file );
virtual	int				FileCount		()									{ return m_fat.Size(); }
virtual	int				FileFind		( char* name );
virtual	char*			FileGetName		( int idx );
virtual	dword			FileGetSize		( int idx );
inline	f9ZipFileInfo*	FileGetInfo		( int idx )							{ return m_fat.Get(idx); }

private:
		BOOL			ReadFAT			();

	cPList<f9ZipFileInfo>	m_fat;		// file allocation table
	cHash					m_hash;		// hash for FAT (name,idx)
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
