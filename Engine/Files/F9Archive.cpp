///////////////////////////////////////////////////////////////////////////////////////////////////
// F9Archive.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "F9Archive.h"

f9Archive::f9Archive()
{
	guard(f9Archive::f9Archive);
	m_type		= F9_ARCHIVE_NONE;
	m_mode		= F9_READ;
	m_open		= FALSE;
	m_name		= NULL;
	m_password	= NULL;
	unguard();
}

f9Archive::~f9Archive()
{
	guard(f9Archive::~f9Archive);
	unguard();
}

int f9Archive::Open( char* name, int mode, char* password )
{
	guard(f9Archive::Open);
	if( IsOpen() ) Close();
	if( name == NULL ) return F9_FAIL;

	m_name = sstrdup( name );
	m_password = sstrdup( password );
	m_mode = mode;
	m_open = TRUE;
	return F9_OK;
	unguard();
}

int f9Archive::Close() 
{
	guard(f9Archive::Close);
	if( !IsOpen() ) return F9_FAIL;
	if(m_name) { sfree(m_name); m_name=NULL; }
	if(m_password) { sfree(m_password); m_password=NULL; }
	m_open = FALSE;
	return F9_OK;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
