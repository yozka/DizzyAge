///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileDisk.h
// This is the standard FILE* wrapper
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILEDISK_H__
#define __F9FILEDISK_H__

#include "F9File.h"

class f9FileDisk : public f9File
{
public:
				f9FileDisk	();
virtual			~f9FileDisk	();

virtual	int		Open		( char* name, int mode );
virtual	int		Close		();
virtual	int64	Read		( void* data, int64 size );
virtual	int64	Write		( void* data, int64 size );
virtual	int		Seek		( int64 offset, int origin = F9_SEEK_SET );
virtual	int64	Tell		();
virtual	int64 	Size		();
virtual	int		Eof			();

inline	FILE*	GetFILE		() { return m_file; }

protected:
		FILE*	m_file;		// file handler
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
