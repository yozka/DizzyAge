///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec_dumb.h
// Codec for MOD,IT,XM,S3M
// Uses the Dumb library by Ben Davis (entheh@users.sf.net)
// Needs: dumb.lib or dumbd.lib linked (check pragma path from .cpp)
// Needs: include additional directories up to dumb.h (in project settings)
// Needs: Multithreaded DLL (in project settings) (hm?)
// A minimal requirement would be dumb.h and dumb.lib
// No changes have been made to the original dumb library
// File: loads file in own space at open
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9CODEC_DUMB_H__
#define __A9CODEC_DUMB_H__

#include "dumb.h"
#include "A9Codec.h"

class a9Codec_dumb : public a9Codec
{
public:
					a9Codec_dumb();
virtual				~a9Codec_dumb();
static	int			Init();
static	int			Done();	

virtual	int			Open( char* name );
virtual	int			BeginRender( int pos, int loop );
virtual int			Render( byte* buffer, int size );
virtual	int			EndRender();
virtual	int			Close();

private:
		DUH*				m_duh;
		DUH_SIGRENDERER*	m_duhsr;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
