///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec_ogg.h
// Codec for OGG
// Uses the Ogg Vorbis library by Xiph.org Foundation (http://www.xiph.org)
// Needs: ogg_static.lib and vorbisfile_static.lib linked (check pragma path from .cpp)
// Needs: include additional directories up to vorbis/codec.h and vorbis/vorbisfile.h (in project settings)
// Might need Multithreaded DLL
// A minimal requirement would be 
//		include/ogg/ogg.h
//		include/ogg/os_types.h
//		include/vorbis/codec.h
//		include/vorbis/vorbisenc.h
//		include/vorbis/vorbisfile.h
//		and 
//		lib/ogg_static.lib
//		lib/vorbisfile_static.lib
// No changes have been made to the original library
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9CODEC_OGG_H__
#define __A9CODEC_OGG_H__

#include "vorbis\\codec.h"
#include "vorbis\\vorbisfile.h"
#include "A9Codec.h"

class a9Codec_ogg : public a9Codec
{
public:
					a9Codec_ogg();
virtual				~a9Codec_ogg();
static	int			Init();
static	int			Done();	

virtual	int			Open( char* name );
virtual	int			BeginRender( int pos, int loop );
virtual int			Render( byte* buffer, int size );
virtual	int			EndRender();
virtual	int			Close();

protected:
		F9FILE			m_file;			// file handle
		OggVorbis_File	m_oggfile;		// ogg file handle
		byte*			m_pcmbuf;		// data buffer
		int				m_pcmpos;		// position in data buffer
		int				m_pcmcnt;		// size of data in buffer
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
