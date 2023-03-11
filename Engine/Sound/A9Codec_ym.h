///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec_ym.h
// Codec for YM
// Uses StSoundLibrary by Arnaud Carré (http://leonard.oxg.free.fr)
// Needs: StSoundLibrary.lib linked (check pragma path from .cpp)
// Needs: include additional directories up to StSoundLibrary.h (in project settings)
// A minimal requirement would be StSoundLibrary.h, YmTypes.h, StSoundLibrary.lib
// Adjusted to support file system (YmTypes.h, YmLoad.cpp) and 
// changed size report to be in msec not in sec
// Also fixed a static internal buffer leak, by using a ymStSoundDone function
// Also fixed a leak (pSongName) set with =mstrdup("Unknown") insteed of ="" and free it
// Also fixed bug in ReadCallback lzh handler from depacker.cpp, to consider input source size when read from it
// Also used malloc/free in lzh callbacks from depacker.cpp (just a preference instead of new delete[])
// Fixed bad bug in CYm2149Ex::reset (registers where not initialized, resulting in a DIV_BY_ZERO sometimes)
// File: loads file in own space at open
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9CODEC_YM_H__
#define __A9CODEC_YM_H__

#include "StSoundLibrary.h"
#include "A9Codec.h"

class a9Codec_ym : public a9Codec
{
public:
					a9Codec_ym();
virtual				~a9Codec_ym();
static	int			Init();
static	int			Done();

virtual	int			Open( char* name );
virtual	int			BeginRender( int pos, int loop );
virtual int			Render( byte* buffer, int size );
virtual	int			EndRender();
virtual	int			Close();

private:
		YMMUSIC*	m_ym;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
