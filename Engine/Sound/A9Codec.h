///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec.h
// obs: sound positons and sizes are in samples unless otherwise noted
// Control:
// A9_ENABLE_WAV, A9_ENABLE_OGG, A9_ENABLE_DUMB, A9_ENABLE_YM
// Interface:
// A9CODEC
// A9_CodecFind, A9_CodecInit, A9_CodecDone, A9_CodecInitAll, A9_CodecDoneAll, 
// A9_CodecCreate, A9_CodecDestroy, A9_CodecOpen, A9_CodecBeginRender, A9_CodecRender, A9_CodecEndRender, A9_CodecClose, A9_CodecGetStatus, A9_CodecGetInfo
// A9_CodecDecodeToWave
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9CODEC_H__
#define __A9CODEC_H__

#include "A9Def.h"

#define A9_CODEC_CLOSED		(0)		// codec status closed, before Open and after Close
#define A9_CODEC_OPENED		(1)		// codec status opened, after Open and after EndRender
#define A9_CODEC_RENDERING	(2)		// codec status render, after BeginRender

#define A9_CODEC_UNKNOWN	-1		// invalid codec
#define A9_CODEC_WAV		0		// wav codec: *.wav
#define A9_CODEC_OGG		1		// ogg codec: *.ogg
#define A9_CODEC_DUMB		2		// dumb codec: *.mod, *it, *.xm, *.s3m
#define A9_CODEC_YM			3		// ym codec: *.ym

class a9Codec
{
public:
					a9Codec();
virtual				~a9Codec()								{};
virtual	int			Open( char* name )						{ return A9_FAIL; }
virtual	int			BeginRender( int pos, int loop )		{ return A9_FAIL; }
virtual int			Render( byte* buffer, int size )		{ return 0; }
virtual	int			EndRender()								{ return A9_FAIL; }
virtual	int			Close()									{ return A9_FAIL; }

		int			m_type;									// codec type
		int			m_loop;									// if looping was requested when render begun
		int			m_status;								// codec status
		a9Info		m_info;									// info
};

typedef a9Codec*	A9CODEC;

struct a9WavHeader
{
	char	m_riff[4];		// "RIFF"
	dword	m_filesize;		// size ( (8+fmtsize)+(8+datasize) )
	char	m_wave[4];		// "WAVE"
	
	char	m_fmt[4];		// "fmt "
	dword	m_fmtsize;		// header length (16)
	
	word	m_tag;			// 1 = WAVE_FORMAT_PCM
	word	m_channels;		// 2
	dword	m_frequency;	// 44100
	dword	m_bytespersec;	// freq * blockalign
	word	m_blockalign;	// channels * depth / 8
	word	m_depth;		// bits per sample

	char	m_data[4];		// "data"
	dword	m_datasize;		// data size  (writesize);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
		int			A9_CodecFind( char* filename );								// find a codec that can play this file
		int			A9_CodecInit( int type );									// initialize a codec library
		int			A9_CodecDone( int type );									// deinitialize a codec library
		void		A9_CodecInitAll();											// initialize all codec libraries
		void		A9_CodecDoneAll();											// deinitialize all codec libraries
		A9CODEC		A9_CodecCreate( int type );									// creates a codec by type
		void		A9_CodecDestroy( A9CODEC codec );							// destroys a codec
inline	int			A9_CodecOpen( A9CODEC codec, char* name )					{ guardfast(A9_CodecOpen);			sassert(codec); return codec->Open(name); unguardfast(); }
inline	int			A9_CodecBeginRender( A9CODEC codec, int pos, int loop )		{ guardfast(A9_CodecBeginRender);	sassert(codec); return codec->BeginRender(pos,loop); unguardfast(); }
inline	int			A9_CodecRender( A9CODEC codec, byte* buffer, int size )		{ guardfast(A9_CodecRender); 		sassert(codec); return codec->Render(buffer,size); unguardfast(); }
inline	int			A9_CodecEndRender( A9CODEC codec )							{ guardfast(A9_CodecEndRender); 	sassert(codec); return codec->EndRender(); unguardfast(); }
inline	int			A9_CodecClose( A9CODEC codec )								{ guardfast(A9_CodecClose); 		sassert(codec); return codec->Close(); unguardfast(); }
inline	int			A9_CodecGetStatus( A9CODEC codec )							{ guardfast(A9_CodecGetStatus);		sassert(codec); return codec->m_status; unguardfast(); }
inline	a9Info*		A9_CodecGetInfo( A9CODEC codec )							{ guardfast(A9_CodecGetInfo); 		sassert(codec); return &(codec->m_info); unguardfast(); }

		int			A9_CodecDecodeToWave( A9CODEC codec, byte* buffer );				// decode into a wave file buffer; buffer must be big enought = sizeof(a9WavHeader)+m_info.DataSize(); codec must be opened
		int			A9_CodecDecodeToWave( char* filename, byte* &buffer, int& size );	// decode into a wave file buffer; user must free the buffer when done; needed codec library must be initialized; returns a9 error

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
