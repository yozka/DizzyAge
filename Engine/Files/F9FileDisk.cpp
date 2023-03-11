///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileDisk.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "F9FileDisk.h"

f9FileDisk::f9FileDisk()
{
	guard(f9FileDisk::f9FileDisk);
	m_type	= F9_FILE_DISK;
	m_file	= NULL;
	unguard();
}

f9FileDisk::~f9FileDisk()
{
	guard(f9FileDisk::~f9FileDisk);
	unguard();
}

int f9FileDisk::Open( char* name, int mode )
{
	guard(f9FileDisk::Open);
	if(IsOpen()) Close();
	if(name==NULL) return F9_FAIL;

	// open
	m_mode = mode;
	char szmode[4][4] = {"rb","wb","r+b","w+b"};
	m_file = fopen(name, szmode[mode & 3]);
	if(!m_file) return F9_FAIL;
	
	m_open = TRUE;
	return F9_OK;
	unguard();
}

int f9FileDisk::Close()
{
	guard(f9FileDisk::Close);
	if(!IsOpen()) return F9_FAIL;
	if(m_file) if(fclose(m_file)!=0) return F9_FAIL;
	m_open = FALSE;
	return F9_OK;
	unguard();
}

int f9FileDisk::Seek( int64 offset, int origin )
{
	guard(f9FileDisk::Seek);
	if(!IsOpen()) return F9_FAIL;
	return (fseek( m_file, (long)offset, origin )==0) ? F9_OK : F9_FAIL;
	unguard();
}

int64 f9FileDisk::Tell()
{
	guard(f9FileDisk::Tell);
	if(!IsOpen()) return F9_FAIL;
	return ftell( m_file );
	unguard();
}

int64 f9FileDisk::Size()
{
	guard(f9FileDisk::Size);
	if(!IsOpen()) return F9_FAIL;
	int64 p = Tell(); 
	if(p!=F9_OK) return F9_FAIL;
	if(Seek(0,SEEK_END)!=F9_OK) return F9_FAIL;
	int64 s = Tell();
	if(Seek(p)!=F9_OK) return F9_FAIL;
	return s;
	unguard();
}

int f9FileDisk::Eof()
{
	guard(f9FileDisk::Eof);
	if(!IsOpen()) return F9_FAIL;
	return feof( m_file );
	unguard();
}

int64 f9FileDisk::Read( void* data, int64 size )
{
	guard(f9FileDisk::Read);
	if(!IsOpen() || data==NULL) return 0;
	dword ret = (dword)fread(data, 1, (sizet)size, m_file);
	return ret;
	unguard();
}

int64 f9FileDisk::Write(void* data, int64 size )
{
	guard(f9FileDisk::Write);
	if(!IsOpen() || data==NULL) return 0;
	dword ret = (dword)fwrite(data, 1, (sizet)size, m_file);
	return ret;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
