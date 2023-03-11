///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec_wav.h
// Codec for WAV
// No extern library dependences, all in here!
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9CODEC_WAV_H__
#define __A9CODEC_WAV_H__

#include "A9Codec.h"

class a9Codec_wav : public a9Codec
{
public:
					a9Codec_wav();
virtual				~a9Codec_wav();
static	int			Init();
static	int			Done();	

virtual	int			Open( char* name );
virtual	int			BeginRender( int pos, int loop );
virtual int			Render( byte* buffer, int size );
virtual	int			EndRender();
virtual	int			Close();

protected:
		int			ReadWaveInfo( F9FILE file, WAVEFORMATEX* wfe, int* datapos, int* datasize ); // if success, set wfe, datapos and datasize 

		F9FILE		m_file;				// file handle
		int			m_datapos;			// data starting position in file
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
