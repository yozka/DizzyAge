///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileMem.h
// This handles files from memory
// They use a special naming format "#hexaddr#hexsize#name.ext"
// Obs: handle write calls with great care, because if they exceeds real buffer space...
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILEMEM_H__
#define __F9FILEMEM_H__

#include "F9File.h"

class f9FileMem : public f9File
{
public:
				f9FileMem	();
virtual			~f9FileMem	();

virtual	int		Open		( char* name, int mode );
virtual	int		Close		();
virtual	int64	Read		( void* data, int64 size );
virtual	int64	Write		( void* data, int64 size );
virtual	int		Seek		( int64 offset, int origin = F9_SEEK_SET );
virtual	int64	Tell		();
virtual	int64 	Size		();
virtual	int		Eof			();

protected:
		byte*	m_addr;		// data buffer address
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
