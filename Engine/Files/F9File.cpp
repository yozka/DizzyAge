///////////////////////////////////////////////////////////////////////////////////////////////////
// F9File.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "F9File.h"

f9File::f9File()
{
	guard(f9File::f9File);
	m_type		= F9_FILE_NONE;
	m_mode		= F9_READ;
	m_open		= FALSE;
	m_pos		= 0;
	m_size		= 0;
	unguard();
}

f9File::~f9File()
{
	guard(f9File::~f9File());
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
