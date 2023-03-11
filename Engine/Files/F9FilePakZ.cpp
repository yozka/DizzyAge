///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FilePakZ.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E9Math.h"
#include "F9FilePakZ.h"
#include "F9ArchivePak.h"

f9FilePakZ::f9FilePakZ()
{
	guard(f9FilePakZ::f9FilePakZ);
	m_type		= F9_FILE_PAKZ;
	m_fileinfo	= NULL;
	m_arcname	= NULL;
	m_data		= NULL;
	unguard();
}

f9FilePakZ::~f9FilePakZ()
{
	guard(f9FilePakZ::~f9FilePakZ);
	unguard();
}

int f9FilePakZ::Open( char* name, int mode )
{
	guard(f9FilePakZ::Open);
	if(IsOpen()) Close();
	if(name==NULL) return F9_FAIL;
	if(!F9_ISREADONLY(mode)) return F9_FAIL; // readonly

	// archive should set those
	if( !m_arcname || m_fileinfo==NULL ) return F9_FAIL;

	m_mode	= mode;
	m_data	= NULL;
	m_size	= 0;
	m_pos	= 0;

	// read whole file data and uncompress it
	if(m_fileinfo->m_sizec>0 && m_fileinfo->m_size>0)
	{
		f9FileDisk file;
		if(file.Open( m_arcname, m_mode )!=F9_OK) return F9_FAIL;
		if(file.Seek( m_fileinfo->m_offset, F9_SEEK_SET )!=F9_OK) { file.Close(); return F9_FAIL; }
		byte* datac = (byte*)smalloc(m_fileinfo->m_sizec);
		int sizec = (int)file.Read(datac, m_fileinfo->m_sizec);
		file.Close();
		if(sizec != m_fileinfo->m_sizec) { sfree(datac); return F9_FAIL; }
		
		// size may be less than sizec !
		dword size = MAX( m_fileinfo->m_size, MAX(64,m_fileinfo->m_sizec) );

		m_data = (byte*)smalloc(size);
		m_size = m_fileinfo->m_size;
		if( !decompress_data( datac, m_fileinfo->m_sizec, m_data, size ) ||
			size != m_fileinfo->m_size )
		{
			sfree(datac);
			sfree(m_data); 
			m_data=NULL;
			return F9_FAIL;
		}
		sfree(datac);
	}

	m_open = TRUE;
	return F9_OK;
	unguard();
}

int f9FilePakZ::Close()
{
	guard(f9FilePakZ::Close);
	if(!IsOpen()) return F9_FAIL;
	if(m_data) sfree(m_data);

	m_arcname	= NULL;
	m_fileinfo	= NULL;
	m_data		= NULL;
	m_size		= 0;
	m_pos		= 0;

	m_open = FALSE;
	return F9_OK;
	unguard();
}

int f9FilePakZ::Seek( int64 offset, int origin )
{
	guard(f9FilePakZ::Seek);
	if(!IsOpen()) return F9_FAIL;
	
	// convert to F9_SEEK_SET
	int64 pos = offset;
	if(origin==F9_SEEK_END)	pos = m_size - offset;
	if(origin==F9_SEEK_CUR)	pos = m_pos + offset;
		
	// bounds
	if(pos<0) pos = 0;
	if(pos>m_size) pos = m_size;

	m_pos = pos;
	return F9_OK;
	unguard();
}

int64 f9FilePakZ::Tell()
{
	guard(f9FilePakZ::Tell);
	if(!IsOpen()) return F9_FAIL;
	return m_pos;
	unguard();
}

int64 f9FilePakZ::Size()
{
	guard(f9FilePakZ::Size);
	if(!IsOpen()) return F9_FAIL;
	return m_size;
	unguard();
}

int f9FilePakZ::Eof()
{
	guard(f9FilePakZ::Eof);
	if(!IsOpen()) return F9_FAIL;
	return (m_pos==m_size) ? 1 : 0;
	unguard();
}

int64 f9FilePakZ::Read( void* data, int64 size )
{
	guard(f9FilePakZ::Read);
	if(!IsOpen() || data==NULL) return 0;
	if( m_pos+size > m_size ) size = m_size - m_pos; // bound
	if(size>0) memcpy( data, m_data+m_pos, (sizet)size );
	m_pos += size;
	return size;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
