///////////////////////////////////////////////////////////////////////////////////////////////////
// A9AudioDX.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "A9AudioDX.h"
#include "E9Math.h"
#include "F9Files.h"

#define A9_LOGERROR( prefix )	dlog( LOGSND, "AUDIO: %s (%s)\n", prefix, ErrorDesc(hr) );

a9StreamDX::a9StreamDX()
{
	guard(a9StreamDX::a9StreamDX);
	m_buffer		= NULL; 
	m_codec			= NULL;
	m_loop			= 0;

	m_status		= 0;
	m_position		= 0;
	m_reposition	= 0;
	m_paccount		= 0;
	m_playseg		= 0;

	m_filemem		= NULL;
	m_filesize		= 0;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// DRIVER
///////////////////////////////////////////////////////////////////////////////////////////////////
a9AudioDX::a9AudioDX()
{
	guard(a9AudioDX::a9AudioDX);
	m_ds = NULL;
	m_dsbuffer = NULL;
	m_dslistener = NULL;
	memset(&m_dslistenerprops,0,sizeof(m_dslistenerprops));

	#ifdef A9_STREAMTHREAD
	m_thread			= NULL;
	m_semaphore			= NULL;
	m_thread_running	= FALSE;
	m_thread_kill		= FALSE;
	#endif

	unguard();
}

a9AudioDX::~a9AudioDX()
{
	guard(a9AudioDX::a9AudioDX);
	unguard();
}

int	a9AudioDX::Init( HWND hwnd )
{
	guard(a9AudioDX::Init);

	A9_CodecInitAll();
	a9Audio::Init(hwnd);
	
	HRESULT hr;
	DSBUFFERDESC dsbd;

	// create DirectSound
	// hr = DirectSoundCreate8( NULL, &m_ds, NULL ); // no initialize needed, or by COM
	hr = CoCreateInstance(CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound, (void**)&m_ds);
	if(FAILED(hr)) { A9_LOGERROR("could not create DirectSound"); goto error; }

	hr = m_ds->Initialize(NULL);
	if(FAILED(hr)) { A9_LOGERROR("could not initialize DirectSound"); goto error; }

	// set cooperative level
	hr = m_ds->SetCooperativeLevel(m_hwnd, DSSCL_PRIORITY);
	if(FAILED(hr)) { A9_LOGERROR("could not set the cooperative level"); goto error; }

	// create primary buffer
	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize	= sizeof(dsbd);
	dsbd.dwBufferBytes = 0;
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	hr = m_ds->CreateSoundBuffer(&dsbd, &m_dsbuffer, NULL);
	if(FAILED(hr)) { A9_LOGERROR("could not create primary sound buffer"); goto error; }

	// volume default
	m_dsbuffer->GetVolume(&m_volumedefault);
	
	// loop primary buffer to avoid sound artifacts
	m_dsbuffer->Play(0, 0, DSBPLAY_LOOPING);

	// query listener
	hr = m_dsbuffer->QueryInterface(IID_IDirectSound3DListener, (void**)&m_dslistener);
	if(FAILED(hr)) { A9_LOGERROR("could not create 3d listener"); goto error; }

	// get caps
	DSCAPS dscaps;
	memset(&dscaps, 0, sizeof(dscaps));
	dscaps.dwSize = sizeof(dscaps);
	m_ds->GetCaps(&dscaps);
	m_memory = dscaps.dwFreeHwMemBytes;
	m_voices = dscaps.dwFreeHw3DAllBuffers;
	m_hw = (m_voices>0);

	// playing streams list
	m_playingstreams.Init(16,8);

	#ifdef A9_STREAMTHREAD

	// create semaphore
	m_semaphore = sys_createsemaphore();
	if(m_semaphore==NULL) goto error;

	// create thread
	unsigned long id;
	m_thread = (HANDLE)CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadUpdate,(void*)this,0,&id);
	m_thread_running = TRUE; // set as quick as possible
	if(m_thread==NULL) goto error;
	// SetThreadPriority(m_thread, THREAD_PRIORITY_ABOVE_NORMAL);

	#endif

	return A9_OK;

	error:
	Done();
	return A9_FAIL;
	unguard();
}

void a9AudioDX::Done()
{
	guard(a9AudioDX::Done);

	// stop all playing streams
	StreamStopAll();

	#ifdef A9_STREAMTHREAD

	// stop thread
	while(m_thread_running) // wait for thread (CPU consuming)
		m_thread_kill = TRUE; // request kill
	
	// destroy thread
	if(m_thread!=INVALID_HANDLE_VALUE) { CloseHandle(m_thread); m_thread = NULL; }
	if(m_semaphore)		{ sys_destroysemaphore(m_semaphore); m_semaphore=NULL; }

	#endif

	// playing streams list
	m_playingstreams.Done();

	if(m_dslistener)	{ srelease(m_dslistener); m_dslistener = NULL; }
	if(m_dsbuffer)		{ m_dsbuffer->SetVolume(m_volumedefault); srelease(m_dsbuffer); m_dsbuffer = NULL; }
	if(m_ds)			{ srelease(m_ds); m_ds = NULL; }
	A9_CodecDoneAll();
	unguard();
}

int	a9AudioDX::Get( int prop )
{
	guard(a9AudioDX::Get);
	long volume=0;
	switch(prop)
	{
		case A9_MASTERVOLUME:	m_dsbuffer->GetVolume(&volume);	return volume;
		default:				return a9Audio::Get(prop);
	}
	return 0;
	unguard();
}

void a9AudioDX::Set( int prop, int val )
{
	guard(a9AudioDX::Set);
	switch(prop)
	{
		case A9_MASTERVOLUME:	m_dsbuffer->SetVolume(val); return;
		default:				a9Audio::Get(prop); return;
	}
	unguard();
}

void a9AudioDX::Update()
{
	guard(a9AudioDX::Update);
	#ifndef A9_STREAMTHREAD
	// if thread is not used, user should call update as often as possible
	StreamUpdateAll();
	#endif
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#define DSERR_DETAIL
#ifdef DSERR_DETAIL
#define CASE_DSERR(hr,detail) case hr: return #hr" "detail
#else
#define CASE_DSERR(hr,detail) case hr: return #hr
#endif

const char* a9AudioDX::ErrorDesc( HRESULT hr )
{
	guard(a9AudioDX::ErrorDesc);
	switch(hr)
	{
		CASE_DSERR(	DS_OK,						"The method succeeded");
		CASE_DSERR(	DS_NO_VIRTUALIZATION,		"The buffer was created, but another 3-D algorithm was substituted");
//@		CASE_DSERR(	DS_INCOMPLETE,				"The method succeeded, but not all the optional effects were obtained");
		CASE_DSERR(	DSERR_ACCESSDENIED,			"The request failed because access was denied");
		CASE_DSERR(	DSERR_ALLOCATED,			"The request failed because resources, such as a priority level, were already in use by another caller");
		CASE_DSERR(	DSERR_ALREADYINITIALIZED,	"The object is already initialized");
		CASE_DSERR(	DSERR_BADFORMAT,			"The specified wave format is not supported");
		CASE_DSERR(	DSERR_BUFFERLOST,			"The buffer memory has been lost and must be restored");
		CASE_DSERR(	DSERR_BUFFERTOOSMALL,		"The buffer size is not great enough to enable effects processing");
		CASE_DSERR(	DSERR_CONTROLUNAVAIL,		"The buffer control (volume, pan, and so on) requested by the caller is not available");
		CASE_DSERR(	DSERR_DS8_REQUIRED,			"A DirectSound object of class CLSID_DirectSound8 or later is required for the requested functionality");
		CASE_DSERR(	DSERR_GENERIC,				"An undetermined error occurred inside the DirectSound subsystem");
		CASE_DSERR(	DSERR_INVALIDCALL,			"This function is not valid for the current state of this object");
		CASE_DSERR(	DSERR_INVALIDPARAM,			"An invalid parameter was passed to the returning function");
		CASE_DSERR(	DSERR_NOAGGREGATION,		"The object does not support aggregation");
		CASE_DSERR(	DSERR_NODRIVER,				"No sound driver is available for use, or the given GUID is not a valid DirectSound device ID");
		CASE_DSERR(	DSERR_NOINTERFACE,			"The requested COM interface is not available");
		CASE_DSERR(	DSERR_OBJECTNOTFOUND,		"The requested object was not found");
		CASE_DSERR(	DSERR_OTHERAPPHASPRIO,		"Another application has a higher priority level, preventing this call from succeeding");
		CASE_DSERR(	DSERR_OUTOFMEMORY,			"The DirectSound subsystem could not allocate sufficient memory to complete the caller's request");
		CASE_DSERR(	DSERR_PRIOLEVELNEEDED,		"The caller does not have the priority level required for the function to succeed");
		CASE_DSERR(	DSERR_UNINITIALIZED,		"The IDirectSound8::Initialize method has not been called or has not been called successfully before other methods were called");
		CASE_DSERR(	DSERR_UNSUPPORTED,			"The function called is not supported at this time");
		CASE_DSERR(	CO_E_NOTINITIALIZED,		"COM not initialized");
		default: return "Unknown error";
	}
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// BUFFERS
///////////////////////////////////////////////////////////////////////////////////////////////////
A9BUFFER a9AudioDX::BufferCreate( a9Info* info, int flags )
{
	guard(a9AudioDX::BufferCreate);
	sassert(info!=NULL);
	sassert(m_ds!=NULL);
	HRESULT hr;
	a9BufferDX* buffer = snew a9BufferDX();
	buffer->m_info = *info;
	buffer->m_flags = flags;

	// wave format structure
	WAVEFORMATEX wfe; 
	memset(&wfe, 0, sizeof(WAVEFORMATEX)); 
	wfe.wFormatTag			= WAVE_FORMAT_PCM; 
	wfe.nChannels			= info->m_channels; 
	wfe.nSamplesPerSec		= info->m_frequency; 
	wfe.nBlockAlign			= info->m_channels * info->m_depth / 8; 
	wfe.nAvgBytesPerSec		= wfe.nSamplesPerSec * wfe.nBlockAlign; 
	wfe.wBitsPerSample		= info->m_depth; 

	// ds buffer description
	DSBUFFERDESC dsbd;
	memset(&dsbd, 0, sizeof(dsbd));
	dsbd.dwSize	= sizeof(DSBUFFERDESC);
	if( flags & A9_FLAG_HW )		dsbd.dwFlags |= DSBCAPS_LOCHARDWARE;
	if( flags & A9_FLAG_SW )		dsbd.dwFlags |= DSBCAPS_LOCSOFTWARE;
	if( flags & A9_FLAG_3D )		dsbd.dwFlags |= DSBCAPS_CTRL3D;
	if( flags & A9_FLAG_VOLUME )	dsbd.dwFlags |= DSBCAPS_CTRLVOLUME;
	if( flags & A9_FLAG_PAN )		dsbd.dwFlags |= DSBCAPS_CTRLPAN;
	if( flags & A9_FLAG_FREQUENCY )	dsbd.dwFlags |= DSBCAPS_CTRLFREQUENCY;

	if( !(flags & A9_FLAG_HW) && (flags & A9_FLAG_3D ) ) // software 3d
		dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	
	dsbd.dwBufferBytes		= info->DataSize();
	dsbd.lpwfxFormat		= &wfe;

	// create direct sound buffer
	buffer->m_dsbuffer = NULL;
	hr = m_ds->CreateSoundBuffer(&dsbd, &(buffer->m_dsbuffer), NULL);
	if(FAILED(hr)) 
	{
		if(buffer->m_dsbuffer) buffer->m_dsbuffer->Release(); // ds safety !?
		sdelete(buffer);
		return NULL;
	}

    return buffer; 
	unguard();
}

byte* a9AudioDX::BufferLock( A9BUFFER _buffer )
{
	guard(a9AudioDX::BufferLock);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	sassert(buffer->m_lockdata==NULL); // already locked

	void *data1, *data2;
	unsigned long size1, size2;
	HRESULT hr = buffer->m_dsbuffer->Lock( 0, 0, &data1, &size1, &data2, &size2, DSBLOCK_ENTIREBUFFER );
	if(FAILED(hr)) return NULL; // may be lost, but what can I do...

	sassert(data1!=NULL);
	sassert(buffer->m_info.DataSize()==(int)size1); // must match
	buffer->m_lockdata = (byte*)data1;
	return buffer->m_lockdata;
	unguard();
}

void a9AudioDX::BufferUnlock( A9BUFFER _buffer )
{
	guard(a9AudioDX::BufferUnlock);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	if(!BufferIsLocked(buffer)) return; // not locked
	HRESULT hr = buffer->m_dsbuffer->Unlock( buffer->m_lockdata, buffer->m_info.DataSize(), NULL, 0 );
	if(FAILED(hr)) return;
	buffer->m_lockdata = NULL;
	unguard();
}

A9BUFFER a9AudioDX::BufferCreate( char* filename, int flags )
{
	guard(a9AudioDX::BufferCreate);
	sassert(filename!=NULL);	
	int ret;
	a9BufferDX* buffer = NULL;
	a9Info* info;
	byte* data;

	// create temporary codec
	int codectype = A9_CodecFind(filename);
	A9CODEC codec = A9_CodecCreate(codectype);
	if(codec==NULL) goto error;
	ret = A9_CodecOpen(codec,filename); 
	if(ret!=A9_OK) goto error;
	ret = A9_CodecBeginRender(codec,0,0);
	if(ret!=A9_OK) goto error;

	// create buffer
	info = A9_CodecGetInfo(codec);
	buffer = (a9BufferDX*)BufferCreate(info, flags); 
	if(buffer==NULL) goto error;

	// fill buffer
	data = BufferLock(buffer); 
	if(data==NULL) goto error;
	ret = A9_CodecRender(codec,data,info->m_size); 
	BufferUnlock(buffer);
	if(ret!=info->m_size) goto error;
	
	// close codec
	A9_CodecEndRender(codec);
	A9_CodecClose(codec);
	A9_CodecDestroy(codec);
	
	return buffer;

	error:
	if(buffer) BufferDestroy(buffer);
	if(codec) { A9_CodecEndRender(codec); A9_CodecClose(codec); A9_CodecDestroy(codec); }
	return NULL;
	unguard();
}

A9BUFFER a9AudioDX::BufferCreateFromMemory( a9Info* info, void* audiodata, int flags )
{
	guard(a9AudioDX::BufferCreateFromMemory);
	sassert(info && audiodata);
	a9BufferDX* buffer = (a9BufferDX*)BufferCreate(info, flags); 
	if(buffer==NULL) return NULL;
	byte* data = BufferLock(buffer); 
	if(data==NULL) { BufferDestroy(buffer); return NULL; }
	memcpy(data,audiodata,info->DataSize());
	BufferUnlock(buffer);
	return buffer;
	unguard();
}

void a9AudioDX::BufferDestroy( A9BUFFER _buffer )
{
	guard(a9AudioDX::BufferDestroy);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	if(BufferIsLocked(buffer))
		BufferUnlock(buffer);
	if(BufferIsPlaying(buffer))
		BufferStop(buffer);
	if(buffer->m_dsbuffer)
		buffer->m_dsbuffer->Release();
	sdelete(buffer);
	unguard();
}

int a9AudioDX::BufferPlay( A9BUFFER _buffer, BOOL loop )
{
	guard(a9AudioDX::BufferPlay);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	HRESULT hr = buffer->m_dsbuffer->Play( 0, 0, loop?DSBPLAY_LOOPING:0 );
	if(FAILED(hr)) return A9_FAIL;
	return A9_OK;
	unguard();
}

int a9AudioDX::BufferStop( A9BUFFER _buffer )
{
	guard(a9AudioDX::BufferStop);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	HRESULT hr = buffer->m_dsbuffer->Stop();
	if(FAILED(hr)) return A9_FAIL;
	return A9_OK;
	unguard();
}

int a9AudioDX::BufferGet( A9BUFFER _buffer, int prop )
{
	guard(a9AudioDX::BufferGet);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	DWORD status = 0;
	long temp=0;
	switch(prop)
	{
		case A9_FLAGS:			return buffer->m_flags;
		case A9_DEPTH:			return buffer->m_info.m_depth;
		case A9_CHANNELS:		return buffer->m_info.m_channels;
		case A9_FREQUENCY:		return buffer->m_info.m_frequency;
		case A9_SIZE:			return buffer->m_info.m_size;
		case A9_STATUS:			buffer->m_dsbuffer->GetStatus(&status); return ((status & DSBSTATUS_PLAYING)?1:0) | ((status & DSBSTATUS_LOOPING)?1:0);
		case A9_VOLUME:			buffer->m_dsbuffer->GetVolume(&temp); return temp;
		case A9_PAN:			buffer->m_dsbuffer->GetPan(&temp); return temp;
		case A9_USER:			return buffer->m_user;
	}
	return 0;
	unguard();
}

void a9AudioDX::BufferSet( A9BUFFER _buffer, int prop, int val )
{
	guard(a9AudioDX::BufferSet);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	switch(prop)
	{
		case A9_VOLUME:			buffer->m_dsbuffer->SetVolume(val); return;
		case A9_PAN:			buffer->m_dsbuffer->SetPan(val); return;
		case A9_USER:			buffer->m_user = val; return;
	}
	unguard();
}

int	a9AudioDX::BufferGetPosition( A9BUFFER _buffer )
{
	guard(a9AudioDX::BufferGetPosition);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	unsigned long playpos = 0;
	unsigned long writepos = 0;
	buffer->m_dsbuffer->GetCurrentPosition( &playpos, &writepos ); 
	return playpos / buffer->m_info.SampleSize();	
	unguard();
}

void a9AudioDX::BufferSetPosition( A9BUFFER _buffer, int pos )
{
	guard(a9AudioDX::BufferSetPosition);
	a9BufferDX* buffer = (a9BufferDX*)_buffer;
	sassert(buffer);
	sassert(buffer->m_dsbuffer!=NULL);
	buffer->m_dsbuffer->SetCurrentPosition( (DWORD)pos*buffer->m_info.SampleSize() );
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// STREAMS
///////////////////////////////////////////////////////////////////////////////////////////////////
int a9AudioDX::StreamPrecache( char* filename, A9STREAM _stream )
{
	guard(a9AudioDX::StreamPrecache);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);
	
	// check format
	int ret;
	int codectype = A9_CodecFind(filename);
	if(codectype==-1) return A9_UNSUPORTED;

	// precache file if file format is small
	if(codectype!=A9_CODEC_WAV && codectype!=A9_CODEC_OGG)
	{
		F9FILE f = F9_FileOpen(filename);
		if(!f) return A9_FAIL;
		F9_FileSeek(f,0,2);
		stream->m_filesize = F9_FileTell(f);
		// @TODO: test size if necessary ...
		F9_FileSeek(f,0,0);
		stream->m_filemem = (byte*)smalloc(stream->m_filesize );
		if(stream->m_filemem==NULL) { F9_FileClose(f); return A9_FAIL; }
		ret = F9_FileRead(stream->m_filemem,stream->m_filesize ,f);
		F9_FileClose(f);
		if(ret!=stream->m_filesize) { sfree(stream->m_filemem); stream->m_filemem=NULL; return A9_FAIL; }
	}

	return A9_OK;
	unguard();
}

A9STREAM a9AudioDX::StreamCreate( char* filename, int flags )
{
	guard(a9AudioDX::StreamCreate);
	sassert(filename!=NULL);
	int ret;
	a9Info info;

	// create stream
	a9StreamDX* stream = snew a9StreamDX();

	// precache (small music files)
	#ifdef A9_STREAMPRECACHE
	ret = StreamPrecache( filename, stream );
	if(ret==A9_OK && stream->m_filemem)
	{
		// change the filename to point to the memory buffer
		char* name = file_path2file(filename); sassert(name);
		filename = F9_MakeFileName(name, stream->m_filemem, stream->m_filesize );
		if(filename==NULL) goto error; // fail
	}
	#endif

	// create codec
	int codectype = A9_CodecFind(filename);
	stream->m_codec = A9_CodecCreate(codectype);
	if(stream->m_codec==NULL) goto error;
	ret = A9_CodecOpen(stream->m_codec,filename); 
	if(ret!=A9_OK) goto error;

	// create buffer
	info = *A9_CodecGetInfo(stream->m_codec);
	info.m_size = info.m_frequency * A9_STREAMBUFFERSIZE; // stream cache buffer size
	stream->m_buffer = (a9BufferDX*)BufferCreate(&info,flags);
	if(stream->m_buffer==NULL) goto error;

	return stream;

	error:
	if(stream->m_buffer) BufferDestroy(stream->m_buffer);
	if(stream->m_codec)	{ A9_CodecClose(stream->m_codec); A9_CodecDestroy(stream->m_codec); }
	if(stream->m_filemem) sfree(stream->m_filemem);
	sdelete(stream);
	return NULL;
	unguard();
}

void a9AudioDX::StreamDestroy( A9STREAM _stream )
{
	guard(a9AudioDX::StreamDestroy);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);

	#ifdef A9_STREAMTHREAD
	sys_waitsemaphore(m_semaphore,2000); // protect
	#endif
	
	// remove from playingstrams list, if found there
	int i = StreamFind(stream);
	if(i!=-1) m_playingstreams.Del(i);
	
	// stop and destroy buffer
	if(stream->m_buffer)
		BufferDestroy(stream->m_buffer);

	// destroy codec
	if(stream->m_codec)
	{
		A9_CodecEndRender(stream->m_codec);
		A9_CodecClose(stream->m_codec);
		A9_CodecDestroy(stream->m_codec);
	}

	// destroy cache
	if(stream->m_filemem)
		sfree(stream->m_filemem);

	sdelete(stream);

	#ifdef A9_STREAMTHREAD
	sys_releasesemaphore(m_semaphore);
	#endif

	unguard();
}

int a9AudioDX::StreamPlay( A9STREAM _stream, BOOL loop )
{
	guard(a9AudioDX::StreamPlay);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);
	
	#ifdef A9_STREAMTHREAD
	sys_waitsemaphore(m_semaphore,2000);
	#endif
	
	// set status to playing and let the thread sincronize itself and do the playing
	stream->m_loop = loop;
	stream->m_status = 1;
	// we add it to the playingstreams list, for the thread to consider it (if not already there)
	if( StreamFind(stream)==-1 )
		m_playingstreams.Add(stream);
	
	#ifdef A9_STREAMTHREAD
	sys_releasesemaphore(m_semaphore);
	#endif
	
	return A9_OK;
	unguard();
}

int a9AudioDX::StreamStop( A9STREAM _stream )
{
	guard(a9AudioDX::StreamStop);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);

	#ifdef A9_STREAMTHREAD
	sys_waitsemaphore(m_semaphore,2000);
	#endif

	// set status to stopped and let the thread sincronize itself and do the stopping;
	// the thread will remove it from the playingstreams list, after it stops it
	stream->m_status = 0; 

	#ifdef A9_STREAMTHREAD
	sys_releasesemaphore(m_semaphore);
	#endif

	return A9_OK;
	unguard();
}

int a9AudioDX::StreamGet( A9STREAM _stream, int prop )
{
	guard(a9AudioDX::StreamGet);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);
	switch(prop)
	{
		case A9_SIZE:			return stream->m_codec->m_info.m_size;
		case A9_USER:			return stream->m_user;
		case A9_STATUS:			return stream->m_status; // the requested play status, not the real one
		default:				return BufferGet(stream->m_buffer,prop);
	}
	unguard();
}

void a9AudioDX::StreamSet( A9STREAM _stream, int prop, int val )
{
	guard(a9AudioDX::StreamSet);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);
	switch(prop)
	{
		case A9_USER:			stream->m_user = val; return;
		default:				BufferSet(stream->m_buffer,prop,val);
	}
	unguard();
}

int a9AudioDX::StreamGetPosition( A9STREAM _stream )
{
	guard(a9AudioDX::StreamGetPosition);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);
	return stream->m_position;
	unguard();
}

void a9AudioDX::StreamSetPosition( A9STREAM _stream, int pos )
{
	guard(a9AudioDX::StreamSetPosition);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	sassert(stream);

	#ifdef A9_STREAMTHREAD
	sys_waitsemaphore(m_semaphore,2000);
	#endif

	stream->m_reposition = 1; // request reposition
	stream->m_position = pos; // set position and let the thread process the reposition command

	#ifdef A9_STREAMTHREAD
	sys_releasesemaphore(m_semaphore);
	#endif
	unguard();
}

int a9AudioDX::StreamUpdate( A9STREAM _stream )
{
	guard(a9AudioDX::StreamUpdate);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	if( a9_audio==NULL || stream==NULL || stream->m_codec==NULL || stream->m_buffer==NULL ) return A9_FAIL; // safety

	int ret;
	a9BufferDX* buffer = stream->m_buffer;
	a9Info& info = buffer->m_info;

	// check buffer playing status
	BOOL bufferplaying = a9_audio->BufferIsPlaying(buffer);  // *** REAL PLAYING ***

	// buffer is playing, but i asked to stop
	if(bufferplaying && stream->m_status==0) // syncronize to stopped status
	{
		// *** REAL STOP ***
		stream->m_codec->EndRender();
		ret = a9_audio->BufferStop(buffer);
		return A9_OK;
	}

	// buffer is playing, i want it to remain playing, but i also want to reposition
	if(bufferplaying && stream->m_reposition) // process reposition request
	{
		// *** REAL STOP ***
		stream->m_codec->EndRender();
		a9_audio->BufferStop(buffer);
		stream->m_reposition = 0; // clear command
		bufferplaying = FALSE;
	}

	// buffer is playing as i want and no reposition was requested
	if(bufferplaying)
	{
		// get buffer position
		int pacsize		= info.m_size/2;					// pac size (half of buffer size)
		int bufferpos	= a9_audio->BufferGetPosition(buffer);// current buffer playing pos [0..buffersize)
		int paccrt		= MAX(stream->m_paccount-2,0);		// current playing [0..)

		// fill buffer
		BOOL needfill = ((bufferpos / pacsize) != (paccrt % 2)) || stream->m_paccount<2;
		if( needfill )
		{
			StreamFill(stream);
			paccrt = MAX(stream->m_paccount-2,0); // update current playing pac
		}

		// check end of stream
		int playofs = paccrt * pacsize + bufferpos % pacsize;
		int streampos = stream->m_playseg + playofs; // real position in stream
		int streamsize = stream->m_codec->m_info.m_size;
		if( !stream->m_loop && (streampos >= streamsize) )
		{
			// *** REAL STOP ***
			stream->m_codec->EndRender();
			ret = a9_audio->BufferStop(buffer);
			stream->m_position = 0; // rewind position
			stream->m_status = 0; // set status to stopped
			return A9_OK;
		}

		// update stream position
		stream->m_position = streampos % streamsize;
	}
	else // buffer is stopped, because it was not playing, or i stopped it to reposition
	{
		if(stream->m_status) // syncronize to playing status
		{
			// *** REAL PLAY ***

			// prepare codec
			A9CODEC codec = stream->m_codec;
			if(codec->m_status!=A9_CODEC_OPENED) return A9_FAIL; // unexpected error: codec must be opened (and not rendering) when play is performed
			int streamsize = stream->m_codec->m_info.m_size;
			stream->m_playseg = stream->m_position % streamsize; // remember requested position when play started
			stream->m_paccount = 0;
			ret = codec->BeginRender( stream->m_playseg, stream->m_loop );
			if(ret!=A9_OK) return A9_FAIL;

			// fill first pac
			StreamFill(stream);

			// play buffer
			a9_audio->BufferSetPosition(buffer,0); // make sure the stream buffer starts from 0
			ret = a9_audio->BufferPlay(buffer,TRUE); // always loop the stream buffer
			if(ret!=A9_OK) { codec->EndRender(); return A9_FAIL; } // unexpected error
			return A9_OK;
		}
	}

	return A9_OK;
	unguard();
}

void a9AudioDX::StreamFill( A9STREAM _stream )
{
	guard(a9AudioDX::StreamFill);
	a9StreamDX* stream = (a9StreamDX*)_stream;
	a9BufferDX* buffer = stream->m_buffer;
	a9Info& info = buffer->m_info;
	LPDIRECTSOUNDBUFFER dsbuffer = buffer->m_dsbuffer;

	int pacsize = info.DataSize()/2;
	int pacpos = (stream->m_paccount%2)*pacsize;

	// lock
	LPVOID data1,data2;
	unsigned long size1,size2;
	while(FAILED(dsbuffer->Lock(pacpos,pacsize,&data1,&size1,&data2,&size2,0)))
	{
		// failed to lock, buffer might be lost
		dsbuffer->Restore(); 
		dsbuffer->Play(0, 0, DSBPLAY_LOOPING);
	}

	// fill
	int samplesize = info.SampleSize();
	byte* data = (byte*)data1;
	int size = (int)size1 / samplesize;
	if( data!=NULL && size>0 )
	{
		int ret = stream->m_codec->Render(data,size);
		if(ret<=0) // error or eof
			memset(data, 0, size*samplesize ); // @TODO clear buffer with silence
		else
		if(ret<size) // last
			memset(data+ret*samplesize, 0, (size-ret)*samplesize ); // @TODO clear buffer with silence
	}

	// unlock
	dsbuffer->Unlock(data1,size1,data2,size2);

	// dlog(LOGSND,"filled pac %i\n",stream->m_paccount);
	stream->m_paccount++;
	unguard();
}

int	a9AudioDX::StreamUpdateAll()
{
	guard(a9AudioDX::StreamUpdateAll);
	int ret = A9_OK;
	for(int i=0;i<m_playingstreams.Size();i++)
	{
		a9StreamDX* stream = m_playingstreams[i];
		if(StreamUpdate(stream)!=A9_OK) ret = A9_FAIL;
		// if not playing, remove from list
		if(stream->m_status==0) 
		{
			m_playingstreams.Del(i);
			i--;
		}
	}
	return ret;
	unguard();
}

int	a9AudioDX::StreamStopAll()
{
	guard(a9AudioDX::StreamStopAll);
	#ifdef A9_STREAMTHREAD
	sys_waitsemaphore(m_semaphore,2000); // protect
	#endif

	for(int i=0;i<m_playingstreams.Size();i++)
	{
		a9StreamDX* stream = m_playingstreams[i];
		// stop buffer if playing, no matter what status stream has
		A9BUFFER buffer = stream->m_buffer;
		if(buffer && a9_audio->BufferIsPlaying(buffer))
		{
			// *** REAL STOP ***
			stream->m_codec->EndRender();
			a9_audio->BufferStop(buffer);
		}
		stream->m_status = 0;
		stream->m_reposition = 0;
	}

	#ifdef A9_STREAMTHREAD
	sys_releasesemaphore(m_semaphore);
	#endif
	return A9_OK;
	unguard();
}

#ifdef A9_STREAMTHREAD
DWORD WINAPI a9AudioDX::ThreadUpdate( LPVOID lpParameter )
{
	guard(a9AudioDX::StreamThreadUpdate);
	a9AudioDX* audio = (a9AudioDX*)a9_audio;
	if( audio==NULL ) { ExitThread(0); return 0; } // safety

	while(!audio->m_thread_kill) // while not requested to exit
	{
		sys_waitsemaphore(audio->m_semaphore,2000); // protect: don't allow play, stop or setposition here
		int ret = audio->StreamUpdateAll();
		// if(ret!=A9_OK) break;
		sys_releasesemaphore(audio->m_semaphore);
		if( A9_STREAMSLEEP>0 )
			Sleep(A9_STREAMSLEEP); // precision vs cpu
	}

	// stop all playing streams (semaphore used inside)
	audio->StreamStopAll();

	// exit thread
	audio->m_thread_running = FALSE;
	ExitThread(0);
	return 0;
	unguard();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
