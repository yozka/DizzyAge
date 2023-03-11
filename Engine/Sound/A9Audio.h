///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Audio.h
// Generic audio class. Platform audio is derived from this.
// Obs: sound sizes and positions are in samples, volume is in DB [-10000,0], pan is [-10000,10000]
// Interface:
// A9BUFFERPROTO, A9BUFFER, A9STREAM
// A9_AudioInit, A9_AudioDone, A9_AudioIsReady, A9_AudioGet, A9_AudioSet, A9_Update
// A9_BufferPrecache, A9_BufferDeprecache, A9_BufferCreate, A9_BufferCreateFromProto, A9_BufferCreateFromMemory, A9_BufferDestroy, A9_BufferPlay, A9_BufferStop, A9_BufferGet, A9_BufferSet, A9_BufferGetPosition, A9_BufferSetPosition, A9_BufferIsPlaying
// A9_StreamCreate, A9_StreamDestroy, A9_StreamDispose, A9_StreamPlay, A9_StreamStop, A9_StreamGet, A9_StreamSet, A9_StreamGetPosition, A9_StreamSetPosition, A9_StreamIsPlaying
// A9_VolumeDecibelToPercent, A9_VolumePercentToDecibel
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9AUDIO_H__
#define __A9AUDIO_H__

#include "A9Def.h"

#define A9_API_DEFAULT		0		// default api

// driver props
#define A9_HW				0		// if driver have hardware acceleration	drv; r
#define A9_MEMORY			1		// reported driver mempry				drv; r
#define A9_VOICES			2		// max hardware voices					drv; r
#define A9_MASTERVOLUME		3		// global volume						drv; rw
									
#define A9_FLAGS			10		// buffer flags							buf,str; r
#define A9_DEPTH			11		// buffer depth							buf,str; r
#define A9_CHANNELS			12		// buffer channels						buf,str; r
#define A9_FREQUENCY		13		// buffer frequency						buf,str; r
#define A9_SIZE				14		// buffer size (in samples)				buf,str; r
#define A9_STATUS			15		// buffer playing status 0/1			buf,str; r
#define A9_VOLUME			16		// buffer volume control				buf,str; rw
#define A9_PAN				17		// buffer pan control					buf,str; rw
#define A9_3DATTR			18		// buffer 3d attributes control			buf,str; rw
#define A9_3DDIST			19		// buffer 3d distances control			buf,str; rw
#define A9_USER				20		// sounds user data						buf,str; rw

// flags set in a9Info for buffers and streams
#define A9_FLAG_HW			(1<<0)	// force hardware buffer
#define A9_FLAG_SW			(1<<1)	// force software buffer
#define A9_FLAG_3D			(1<<2)	// 3d buffer
#define A9_FLAG_VOLUME		(1<<3)	// control volume
#define A9_FLAG_PAN			(1<<4)	// control pan
#define A9_FLAG_FREQUENCY	(1<<5)	// control frequency

// others
#define A9_VOLUMEMIN		(-10000)
#define A9_VOLUMEMAX		(0)
#define A9_PANLEFT			(-10000)
#define A9_PANRIGHT			(10000)

struct a9BufferProto
{
		a9Info				m_info;				// audio info
		byte*				m_data;				// audio data (decompressed)
		a9BufferProto()		{ m_data=NULL; }
};
typedef a9BufferProto* A9BUFFERPROTO;

struct a9Buffer
{
	a9Info		m_info;			// buffer info
	int			m_flags;		// buffer flags
	int			m_user;			// user data
	a9Buffer()	{ m_flags=0; m_user=0; };
};
typedef a9Buffer* A9BUFFER;

struct a9Stream
{
	int			m_user;
	a9Stream()	{ m_user=0; };
};
typedef a9Stream* A9STREAM;

///////////////////////////////////////////////////////////////////////////////////////////////////
class a9Audio
{
public:
						a9Audio();
virtual					~a9Audio();
virtual	int				Init( HWND hwnd );											// initialize driver
virtual	void			Done();														// done driver
virtual	int				Get( int prop );											// get driver prop
virtual	void			Set( int prop, int val );									// set driver prop
virtual	void			Update();													// update driver

virtual A9BUFFERPROTO	BufferPrecache( char* filename );							// create a buffer proto, for faster creation of the buffer at runtime (file decompressed in memory)
virtual void			BufferDeprecache( A9BUFFERPROTO proto );					// destroy a buffer proto
virtual	A9BUFFER		BufferCreate( char* filename, int flags = A9_FLAG_VOLUME );	// create buffer
virtual	A9BUFFER		BufferCreateFromProto( A9BUFFERPROTO proto, int flags = A9_FLAG_VOLUME );			// create buffer from proto
virtual	A9BUFFER		BufferCreateFromMemory( a9Info* info, void* audiodata, int flags = A9_FLAG_VOLUME );	// create buffer from memory; audiodata must have the right size (info->DataSize())
virtual	void			BufferDestroy( A9BUFFER buffer );							// destroy buffer
virtual int				BufferPlay( A9BUFFER buffer, BOOL loop=FALSE );				// play buffer
virtual int				BufferStop( A9BUFFER buffer );								// stop buffer
virtual	int				BufferGet( A9BUFFER buffer, int prop );						// get buffer prop
virtual	void			BufferSet( A9BUFFER buffer, int prop, int val );			// set buffer prop
virtual	int				BufferGetPosition( A9BUFFER buffer );						// get Buffer position
virtual	void			BufferSetPosition( A9BUFFER buffer, int pos );				// set Buffer position
inline	BOOL			BufferIsPlaying( A9BUFFER buffer )							{ return BufferGet(buffer,A9_STATUS); }

virtual	A9STREAM		StreamCreate( char* filename, int flags = A9_FLAG_VOLUME ); // create stream
virtual	void			StreamDestroy( A9STREAM stream );							// destroy stream
virtual int				StreamPlay( A9STREAM stream, BOOL loop=FALSE );				// play stream
virtual int				StreamStop( A9STREAM stream );								// stop stream
virtual	int				StreamGet( A9STREAM stream, int prop );						// get stream prop
virtual	void			StreamSet( A9STREAM stream, int prop, int val );			// set stream prop
virtual	int				StreamGetPosition( A9STREAM stream );						// get stream position
virtual	void			StreamSetPosition( A9STREAM stream, int pos );				// set stream position
inline	BOOL			StreamIsPlaying( A9STREAM stream )							{ return StreamGet(stream,A9_STATUS); }

		HWND			m_hwnd;				// application window; used to initialize the driver
		int				m_memory;			// detected memory
		int				m_voices;			// detected hardware voices; user can overwrite this after init
		int				m_hw;				// detected hardware; user can overwrite this after init

protected:
		long			m_volumedefault;	// default volume when 
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
extern	a9Audio* a9_audio;

		BOOL			A9_Init( HWND hwnd, int api=A9_API_DEFAULT );					// init a9 driver
		void			A9_Done();														// done a9 driver
inline	BOOL			A9_IsReady()													{ return a9_audio!=NULL; }
inline	int				A9_Get( int prop )												{ guardfast(A9_AudioGet);			sassert(a9_audio); return a9_audio->Get(prop); unguardfast(); }
inline	void			A9_Set( int prop, int val )										{ guardfast(A9_AudioSet);			sassert(a9_audio); a9_audio->Set(prop,val); unguardfast(); }
inline	void			A9_Update()														{ guardfast(A9_AudioSet);			sassert(a9_audio); a9_audio->Update(); unguardfast(); }

inline	A9BUFFERPROTO	A9_BufferPrecache( char* filename )								{ guardfast(A9_BufferCreate);		sassert(a9_audio); return a9_audio->BufferPrecache(filename); unguardfast(); }
inline	void			A9_BufferDeprecache( A9BUFFERPROTO proto )						{ guardfast(A9_BufferCreate);		sassert(a9_audio); a9_audio->BufferDeprecache(proto); unguardfast(); }
inline	A9BUFFER		A9_BufferCreate( char* filename, int flags=A9_FLAG_VOLUME )		{ guardfast(A9_BufferCreate);		sassert(a9_audio); return a9_audio->BufferCreate(filename,flags); unguardfast(); }
inline	A9BUFFER		A9_BufferCreateFromProto( A9BUFFERPROTO proto, int flags = A9_FLAG_VOLUME )					{ guardfast(A9_BufferCreate);		sassert(a9_audio); return a9_audio->BufferCreateFromProto(proto,flags); unguardfast(); }
inline	A9BUFFER		A9_BufferCreateFromMemory( a9Info* info, void* audiodata, int flags = A9_FLAG_VOLUME )	{ guardfast(A9_BufferCreate);		sassert(a9_audio); return a9_audio->BufferCreateFromMemory(info,audiodata,flags); unguardfast(); }
inline	void			A9_BufferDestroy( A9BUFFER buffer )								{ guardfast(A9_BufferDestroy);		sassert(a9_audio); a9_audio->BufferDestroy(buffer); unguardfast(); }
inline	int				A9_BufferPlay( A9BUFFER buffer, BOOL loop=FALSE )				{ guardfast(A9_BufferPlay);			sassert(a9_audio); return a9_audio->BufferPlay(buffer,loop); unguardfast(); }
inline	int				A9_BufferStop( A9BUFFER buffer )								{ guardfast(A9_BufferStop);			sassert(a9_audio); return a9_audio->BufferStop(buffer); unguardfast(); }
inline	int				A9_BufferGet( A9BUFFER buffer, int prop )						{ guardfast(A9_BufferGet);			sassert(a9_audio); return a9_audio->BufferGet(buffer,prop); unguardfast(); }
inline	void			A9_BufferSet( A9BUFFER buffer, int prop, int val )				{ guardfast(A9_BufferSet);			sassert(a9_audio); a9_audio->BufferSet(buffer,prop,val); unguardfast(); }
inline	int				A9_BufferGetPosition( A9BUFFER buffer )							{ guardfast(A9_BufferGetPosition);	sassert(a9_audio); return a9_audio->BufferGetPosition(buffer); unguardfast(); }
inline	void			A9_BufferSetPosition( A9BUFFER buffer, int pos )				{ guardfast(A9_BufferSetPosition);	sassert(a9_audio); a9_audio->BufferSetPosition(buffer,pos); unguardfast(); }
inline	BOOL			A9_BufferIsPlaying( A9BUFFER buffer )							{ guardfast(A9_BufferIsPlaying);	sassert(a9_audio); return a9_audio->BufferIsPlaying(buffer); unguardfast(); }

inline	A9STREAM		A9_StreamCreate( char* filename, int flags=A9_FLAG_VOLUME )		{ guardfast(A9_StreamCreate);		sassert(a9_audio); return a9_audio->StreamCreate(filename,flags); unguardfast(); }
inline	void			A9_StreamDestroy( A9STREAM stream )								{ guardfast(A9_StreamDestroy);		sassert(a9_audio); a9_audio->StreamDestroy(stream); unguardfast(); }
inline	int				A9_StreamPlay( A9STREAM stream, BOOL loop )						{ guardfast(A9_StreamPlay);			sassert(a9_audio); return a9_audio->StreamPlay(stream,loop); unguardfast(); }
inline	int				A9_StreamStop( A9STREAM stream )								{ guardfast(A9_StreamStop);			sassert(a9_audio); return a9_audio->StreamStop(stream); unguardfast(); }
inline	int				A9_StreamGet( A9STREAM stream, int prop )						{ guardfast(A9_StreamGet);			sassert(a9_audio); return a9_audio->StreamGet(stream,prop); unguardfast(); }
inline	void			A9_StreamSet( A9STREAM stream, int prop, int val )				{ guardfast(A9_StreamSet);			sassert(a9_audio); a9_audio->StreamSet(stream,prop,val); unguardfast(); }
inline	int				A9_StreamGetPosition( A9STREAM stream )							{ guardfast(A9_StreamGetPosition);	sassert(a9_audio); return a9_audio->StreamGetPosition(stream); unguardfast(); }
inline	void			A9_StreamSetPosition( A9STREAM stream, int pos )				{ guardfast(A9_StreamSetPosition);	sassert(a9_audio); a9_audio->StreamSetPosition(stream,pos); unguardfast(); }
inline	BOOL			A9_StreamIsPlaying( A9STREAM stream )							{ guardfast(A9_StreamIsPlaying);	sassert(a9_audio); return a9_audio->StreamIsPlaying(stream); unguardfast(); }

		int				A9_VolumeDecibelToPercent( int vol );							// conversion from volume in decibels [-10000,0] to percent [0,100]
		int				A9_VolumePercentToDecibel( int vol );							// conversion from volume in percent [0,100] to decibels [-10000,0]

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
