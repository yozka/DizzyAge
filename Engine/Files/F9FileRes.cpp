///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileRes.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "F9FileRes.h"

f9FileRes::f9FileRes()
{
	guard(f9FileRes::f9FileRes);
	m_type	= F9_FILE_RES;
	m_addr	= NULL;
	unguard();
}

f9FileRes::~f9FileRes()
{
	guard(f9FileRes::~f9FileRes);
	unguard();
}

int f9FileRes::Open( char* name, int mode )
{
	guard(f9FileRes::Open);
	if(IsOpen()) Close();
	if(name==NULL) return F9_FAIL;
	if(!F9_ISREADONLY(mode)) return F9_FAIL; // readonly

	// open
	m_mode = mode;
	m_addr = NULL;
	m_size = 0;
	m_pos  = 0;

	// resource
	HRSRC hrsrc;
	hrsrc = FindResourceEx(NULL, F9_FILERES_GROUP, name,  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) ); 
	if(hrsrc==NULL) return F9_FAIL;
	m_size = SizeofResource(NULL,hrsrc); if(m_size<=0) return F9_FAIL;
	HGLOBAL hglobal = LoadResource(NULL,hrsrc); if(hglobal==NULL) return F9_FAIL;
	m_addr = (byte*)LockResource(hglobal); if(m_addr==NULL) return F9_FAIL;

	m_open = TRUE;
	return F9_OK;
	unguard();
}

int f9FileRes::Close()
{
	guard(f9FileRes::Close);
	if(!IsOpen()) return F9_FAIL;
	m_addr = NULL;
	m_size = 0;
	m_pos  = 0;
	m_open = FALSE;
	return F9_OK;
	unguard();
}

int f9FileRes::Seek( int64 offset, int origin )
{
	guard(f9FileRes::Seek);
	if(!IsOpen()) return F9_FAIL;
	if(origin==F9_SEEK_SET && offset>=0 && offset<=m_size) { m_pos = offset; return F9_OK; }
	else
	if(origin==F9_SEEK_CUR && m_pos+offset>=0 && m_pos+offset<=m_size) { m_pos += offset; return F9_OK; }
	else
	if(origin==F9_SEEK_END && m_size-offset>=0 && m_size-offset<=m_size) { m_pos = m_size-offset; return F9_OK; }
	else
	return F9_FAIL;
	unguard();
}

int64 f9FileRes::Tell()
{
	guard(f9FileRes::Tell);
	if(!IsOpen()) return F9_FAIL;
	return m_pos;
	unguard();
}

int64 f9FileRes::Size()
{
	guard(f9FileRes::Size);
	if(!IsOpen()) return F9_FAIL;
	return m_size;
	unguard();
}

int f9FileRes::Eof()
{
	guard(f9FileRes::Eof);
	if(!IsOpen()) return F9_FAIL;
	return (m_pos==m_size) ? 1 : 0;
	unguard();
}

int64 f9FileRes::Read( void* data, int64 size )
{
	guard(f9FileRes::Read);
	if(!IsOpen() || data==NULL) return 0;
	int64 s = size;
	if(m_pos+s>m_size) s=m_size-m_pos;
	memcpy(data,m_addr+m_pos,(sizet)s);
	m_pos+=s;
	return s;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
