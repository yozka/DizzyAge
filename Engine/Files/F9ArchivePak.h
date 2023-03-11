///////////////////////////////////////////////////////////////////////////////////////////////////
// F9ArchivePak.h
// This file handles .pak archives
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9ARCHIVEPAK_H__
#define __F9ARCHIVEPAK_H__

#include "E9List.h"
#include "E9Hash.h"
#include "F9Archive.h"
#include "F9FileDisk.h"
//#include "zlib.h"

#define F9_PAK_VERSION			2
#define F9_PAK_COMPRESSED		(1<<0)
#define F9_PAK_PROTECTED		(1<<1)

struct f9PakHeader
{
	dword	m_id;				// pak id "PAKv"
	dword	m_fatfiles;			// files in the archive
	dword	m_fatoffset;		// fat offset (in archive)
	dword	m_fatsizec;			// fat size compressed (in archive)
	dword	m_fatsize;			// fat size uncompressed
	dword	m_fatcrc;			// fat crc
	dword	m_fatcrc2;			// fat crc2
};

struct f9PakFileInfo
{
	dword	m_flags;			// flags (compressed, protected...)
	dword	m_offset;			// start offset (in archive)
	dword	m_sizec;			// compressed size (in archive)
	dword	m_size;				// uncompressed size
	cStr	m_name;				// file name
};

class f9ArchivePak : public f9Archive
{
public:
						
						f9ArchivePak	();
virtual					~f9ArchivePak	();
						
virtual	int				Open			( char *name, int mode = F9_READ, char* password=NULL );
virtual	int				Close			();
						
virtual	f9File*	 		FileOpen		( char* name, int mode = F9_READ );
virtual	int				FileClose		( f9File* file );
virtual	int				FileCount		()									{ return m_fat.Size(); }
virtual	int				FileFind		( char* name );
virtual	char*			FileGetName		( int idx );
virtual	dword			FileGetSize		( int idx );
inline	f9PakFileInfo*	FileGetInfo		( int idx )							{ return m_fat.Get(idx); }

private:
		BOOL			ReadHeader		();
		BOOL			ReadFAT			();

	f9PakHeader				m_header;	// archive header
	cPList<f9PakFileInfo>	m_fat;		// file allocation table
	cHash					m_hash;		// hash for FAT (name,idx)
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
