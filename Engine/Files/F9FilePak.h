///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FilePak.h
// Uncompressed file from Pak archive
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILEPAK_H__
#define __F9FILEPAK_H__

#include "f9File.h"
#include "f9FileDisk.h"

struct f9PakFileInfo;

class f9FilePak : public f9File
{
public:
				f9FilePak	();
virtual			~f9FilePak	();

virtual	int		Open		( char* name, int mode );
virtual	int		Close		();
virtual	int64	Read		( void* data, int64 size );
virtual	int		Seek		( int64 offset, int origin = F9_SEEK_SET );
virtual	int64	Tell		();
virtual	int64 	Size		();
virtual	int		Eof			();

protected:
		f9PakFileInfo*		m_fileinfo;				// info from pak
		char*				m_arcname;				// just a pointer to zip archive name
		f9FileDisk	 		m_arcfile;				// archive disk file, opened on Open


friend	class f9ArchivePak;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
