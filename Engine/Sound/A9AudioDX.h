///////////////////////////////////////////////////////////////////////////////////////////////////
// A9AudioDX.h
// DirectSound audio imlementation
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9DRVDX_H__
#define __A9DRVDX_H__

#include "dsound.h"
#include "A9Audio.h"
#include "A9Codec.h"
#include "E9List.h"

#define	A9_STREAMBUFFERSIZE		2				// size in seconds of a the streaming buffer
#define A9_STREAMSLEEP			1				// stream sleep value (0=for no sleep but more cpu)
#define A9_STREAMTHREAD							// if a thread is used to update streaming files. if not, A9_Update must be called per frame
#define A9_STREAMPRECACHE						// precache small streamed files

struct a9BufferDX : a9Buffer
{
		LPDIRECTSOUNDBUFFER	m_dsbuffer;			// direct sound buffer
		byte*				m_lockdata;			// locked buffer data, non-NULL when locked
		a9BufferDX()		{ m_dsbuffer=NULL; m_lockdata=NULL; }
};

struct a9StreamDX : a9Stream
{
		a9BufferDX*			m_buffer;			// cache buffer
		A9CODEC				m_codec;			// codec
		int					m_loop;				// play mode
		
volatile int				m_status;			// playing status 0=stop, 1=play
volatile int				m_position;			// stream position (read/write)
volatile int				m_reposition;		// request reposition
volatile int				m_paccount;			// loaded pacs
volatile int				m_playseg;			// position where play started, in the stream

		byte*				m_filemem;			// cached file buffer; only small music formats are precached (not wav or ogg)
		int					m_filesize;			// cached file size

		a9StreamDX();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class a9AudioDX : public a9Audio
{
public:
					a9AudioDX();
virtual				~a9AudioDX();
virtual	int			Init( HWND hwnd );
virtual	void		Done();
virtual	int			Get( int prop );
virtual	void		Set( int prop, int val );
virtual	void		Update();

		A9BUFFER		BufferCreate( a9Info* info, int flags = A9_FLAG_VOLUME );
		byte*			BufferLock( A9BUFFER buffer );
		void			BufferUnlock( A9BUFFER buffer );
inline	BOOL			BufferIsLocked( A9BUFFER buffer )								{ guardfast(BufferIsLocked); sassert(buffer); return ((a9BufferDX*)buffer)->m_lockdata!=NULL; unguardfast(); }
virtual	A9BUFFER		BufferCreate( char* filename, int flags = A9_FLAG_VOLUME );
virtual	A9BUFFER		BufferCreateFromMemory( a9Info* info, void* audiodata, int flags = A9_FLAG_VOLUME );
virtual	void			BufferDestroy( A9BUFFER buffer );
virtual int				BufferPlay( A9BUFFER buffer, BOOL loop=FALSE );
virtual int				BufferStop( A9BUFFER buffer );
virtual	int				BufferGet( A9BUFFER buffer, int prop );
virtual	void			BufferSet( A9BUFFER buffer, int prop, int val );
virtual	int				BufferGetPosition( A9BUFFER buffer );
virtual	void			BufferSetPosition( A9BUFFER buffer, int pos );

		int				StreamPrecache( char* filename, A9STREAM stream );				// precache small files
virtual	A9STREAM		StreamCreate( char* filename, int flags = A9_FLAG_VOLUME ); 
virtual	void			StreamDestroy( A9STREAM stream );
virtual int				StreamPlay( A9STREAM stream, BOOL loop=FALSE );
virtual int				StreamStop( A9STREAM stream );
virtual	int				StreamGet( A9STREAM stream, int prop );
virtual	void			StreamSet( A9STREAM stream, int prop, int val );
virtual	int				StreamGetPosition( A9STREAM stream );
virtual	void			StreamSetPosition( A9STREAM stream, int pos );
		int				StreamUpdate( A9STREAM stream );								// update one stream to play stop or reposition, as requested (on thread)
		void			StreamFill( A9STREAM stream );									// fill a stream with more data from codec (on thread)
		int				StreamUpdateAll();												// update all playing streams (on thread)
		int				StreamStopAll();												// stop all playing streams now! (off thread)
inline	int				StreamFind( A9STREAM stream )									{ for(int i=0;i<m_playingstreams.Size();i++) { if(m_playingstreams[i]==stream) return i; } return -1; }

#ifdef A9_STREAMTHREAD
static DWORD WINAPI 	ThreadUpdate( LPVOID lpParameter );
#endif
		const char*	ErrorDesc( HRESULT hr );

		LPDIRECTSOUND8				m_ds;
		LPDIRECTSOUNDBUFFER			m_dsbuffer;
		LPDIRECTSOUND3DLISTENER		m_dslistener;
		DS3DLISTENER				m_dslistenerprops;

#ifdef A9_STREAMTHREAD
		 HANDLE		m_thread;			// thread handle
		 HSEMAPHORE	m_semaphore;		// thread semaphore
volatile BOOL		m_thread_running;	// thread is running
volatile BOOL		m_thread_kill;		// thread kill request
#endif

protected:
			cList<a9StreamDX*>		m_playingstreams;				// streams that are playing
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
