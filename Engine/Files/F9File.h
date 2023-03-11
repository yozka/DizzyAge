///////////////////////////////////////////////////////////////////////////////////////////////////
// F9File.h
// This is the base class for the files used in the file system
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILE_H__
#define __F9FILE_H__

#include "E9System.h"
#include "E9String.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////////////////////
#define F9_FAIL				-1
#define F9_OK				0

// open modes
#define F9_READ				0	// "rb"
#define F9_WRITE			1	// "wb"
#define F9_READWRITE		2	// "r+b"
#define F9_WRITEREAD		3	// "w+b"

#define F9_ISREADONLY(mode)	((mode & 3)==0)

// seek modes ( better to match original values: 0,1,2 )
#define F9_SEEK_SET			SEEK_SET
#define F9_SEEK_CUR			SEEK_CUR
#define F9_SEEK_END			SEEK_END

//@HM: move in classes
// file types
#define F9_FILE_NONE		0
#define F9_FILE_DISK		1
#define F9_FILE_MEM			2
#define F9_FILE_RES			3
#define F9_FILE_PAK			4
#define F9_FILE_PAKZ		5
#define F9_FILE_ZIP			6

///////////////////////////////////////////////////////////////////////////////////////////////////
// f9File class
///////////////////////////////////////////////////////////////////////////////////////////////////
class f9File
{
public:
				f9File		();
virtual			~f9File		();
								
virtual	int		Open		( char* name, int mode )					{ return F9_FAIL; }
virtual	int		Close		()											{ return F9_FAIL; }
inline	BOOL	IsOpen		()											{ return m_open; }
virtual	int64	Read		( void* data, int64 size )					{ return 0; }
virtual	int64	Write		( void* data, int64 size )					{ return 0; }
virtual	int		Seek		( int64 offset, int origin = F9_SEEK_SET )	{ return F9_FAIL; }
virtual	int64	Tell		()											{ return F9_FAIL; }
virtual	int64 	Size		()											{ return F9_FAIL; }
virtual	int		Eof			()											{ return (m_pos<m_size) ? F9_OK : F9_FAIL; }

public:
		int		m_type;		// file type
		int		m_mode;		// open mode
		BOOL	m_open;		// if opened

protected:
		int64	m_pos;		// current pos
		int64	m_size;		// file size
};

typedef f9File* F9FILE;

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
