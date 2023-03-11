///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileRes.h
// This handles files from win32 resources (READONLY)
// User resources must be in "FILE" group on neutral(!?) language. 
// File name is the resource name
// Reads the resource in memory when opened and then work on the memory buffer
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILERES_H__
#define __F9FILERES_H__

#include "F9File.h"

#define F9_FILERES_GROUP	"FILES"

class f9FileRes : public f9File
{
public:
				f9FileRes	();
virtual			~f9FileRes	();

virtual	int		Open		( char* name, int mode );
virtual	int		Close		();
virtual	int64	Read		( void* data, int64 size );
virtual	int		Seek		( int64 offset, int origin = F9_SEEK_SET );
virtual	int64	Tell		();
virtual	int64 	Size		();
virtual	int		Eof			();

protected:
		byte*	m_addr;		// data buffer address
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
