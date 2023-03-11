//////////////////////////////////////////////////////////////////////////////////////////////////
// F9Archive.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9ARCHIVE_H__
#define __F9ARCHIVE_H__

#include "F9File.h"

#define F9_ARCHIVE_NONE		(0<<8)
#define F9_ARCHIVE_ZIP		(1<<8)
#define F9_ARCHIVE_PAK		(2<<8)

class f9Archive
{
public:
						f9Archive		();
virtual					~f9Archive		();
						
virtual	int				Open			( char *name, int mode = F9_READ, char* password=NULL );
virtual	int				Close			();
inline	BOOL			IsOpen			()									{ return m_open; }
						
// file serve			
virtual	f9File*			FileOpen		( char* name, int mode = F9_READ )	{ return NULL; }
virtual	int				FileClose		( f9File* file )					{ return F9_FAIL; }
virtual	int				FileCount		()									{ return 0; }
virtual	int				FileFind		( char* name )						{ return -1; }		// get arc file's index in the archive
virtual	char*			FileGetName		( int idx )							{ return NULL; }	// get arc file's name
virtual	dword			FileGetSize		( int idx )							{ return 0; }		// get arc file's size

public:
		int				m_type;			// archive type
		int				m_mode;			// open mode
		int				m_open;			// if opened
		char*			m_name;			// archive name
		char*			m_password;		// archive password
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
