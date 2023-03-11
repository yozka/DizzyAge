///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Audio.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "A9Audio.h"
#include "A9AudioDX.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// DRIVER
///////////////////////////////////////////////////////////////////////////////////////////////////
a9Audio::a9Audio()
{
	guard(a9Audio::a9Audio);
	m_hwnd = NULL;
	m_volumedefault	= 100;
	m_memory = 0;
	m_voices = 0;
	m_hw = 0;
	unguard();
}

a9Audio::~a9Audio()
{
	guard(a9Audio::a9Audio);
	unguard();
}

int	a9Audio::Init( HWND hwnd )
{
	guard(a9Audio::Init);
	m_hwnd = hwnd;
	return A9_OK;
	unguard();
}

void a9Audio::Done()
{
	guard(a9Audio::Done);
	unguard();
}

int	a9Audio::Get( int prop )
{
	guard(a9Audio::Get);
	long volume=0;
	switch(prop)
	{
		case A9_HW:				return m_hw;
		case A9_MEMORY:			return m_memory;
		case A9_VOICES:			return m_voices;
	}
	return 0;
	unguard();
}

void a9Audio::Set( int prop, int val )
{
	guard(a9Audio::Set);
	unguard();
}

void a9Audio::Update()
{
	guard(a9Audio::Update);
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// BUFFERS
///////////////////////////////////////////////////////////////////////////////////////////////////
A9BUFFERPROTO a9Audio::BufferPrecache( char* filename ) 
{ 
	guard(a9Audio::BufferPrecache);
	int ret;
	a9BufferProto* proto = NULL;
	
	// codec
	int codectype = A9_CodecFind(filename);
	A9CODEC codec = A9_CodecCreate(codectype);
	if(codec==NULL) goto error;
	ret = A9_CodecOpen(codec,filename); 
	if(ret!=A9_OK) goto error;
	ret = A9_CodecBeginRender(codec,0,0);
	if(ret!=A9_OK) goto error;
	if(A9_CodecGetInfo(codec)->DataSize()==0) goto error;

	// create proto
	proto = snew a9BufferProto();
	proto->m_info = *A9_CodecGetInfo(codec);
	proto->m_data = (byte*)smalloc( proto->m_info.DataSize() );

	// fill data
	if(proto->m_data==NULL) goto error;
	ret = A9_CodecRender(codec,proto->m_data,proto->m_info.m_size); 
	if(ret!=proto->m_info.m_size) goto error;
	
	// close codec
	A9_CodecEndRender(codec);
	A9_CodecClose(codec);
	A9_CodecDestroy(codec);
	
	return proto;	

	error:
	if(proto) { if(proto->m_data) sfree(proto->m_data); sdelete(proto); }
	if(codec) { A9_CodecEndRender(codec); A9_CodecClose(codec); A9_CodecDestroy(codec); }
	return NULL;

	unguard();
}

void a9Audio::BufferDeprecache( A9BUFFERPROTO proto ) 
{
	guard(a9Audio::BufferDeprecache);
	if(!proto) return;
	if(proto->m_data) sfree(proto->m_data);
	sdelete(proto);
	unguard();
}

A9BUFFER a9Audio::BufferCreateFromProto( A9BUFFERPROTO proto, int flags )
{
	guard(a9Audio::BufferCreateFromProto);
	sassert(proto);
	return BufferCreateFromMemory(&proto->m_info, proto->m_data, flags);
	unguard();
}

A9BUFFER a9Audio::BufferCreate( char* filename, int flags ) { return NULL; }
A9BUFFER a9Audio::BufferCreateFromMemory( a9Info* info, void* audiodata, int flags ) { return NULL; }
void a9Audio::BufferDestroy( A9BUFFER buffer ) {}
int a9Audio::BufferPlay( A9BUFFER buffer, BOOL loop ) { return A9_FAIL; }
int a9Audio::BufferStop( A9BUFFER buffer ) { return A9_FAIL; }
int a9Audio::BufferGet( A9BUFFER buffer, int prop ) { return 0; }
void a9Audio::BufferSet( A9BUFFER buffer, int prop, int val ) {}
int	a9Audio::BufferGetPosition( A9BUFFER buffer ) { return 0; }
void a9Audio::BufferSetPosition( A9BUFFER buffer, int pos ) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
// STREAMS
///////////////////////////////////////////////////////////////////////////////////////////////////
A9STREAM a9Audio::StreamCreate( char* filename, int flags ) { return NULL; }
void a9Audio::StreamDestroy( A9STREAM stream ) {}
int a9Audio::StreamPlay( A9STREAM stream, BOOL loop ) { return A9_FAIL; }
int a9Audio::StreamStop( A9STREAM stream ) { return A9_FAIL; }
int a9Audio::StreamGet( A9STREAM stream, int prop ) { return 0; }
void a9Audio::StreamSet( A9STREAM stream, int prop, int val ) {}
int a9Audio::StreamGetPosition( A9STREAM stream ) { return 0; }
void a9Audio::StreamSetPosition( A9STREAM stream, int pos ) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
a9Audio* a9_audio = NULL;

BOOL A9_Init( HWND hwnd, int api )
{
	guard(A9_Init);
	if(a9_audio) return TRUE;
	dlog(LOGSND, "Audio init (api=%i).\n",api);
	a9_audio = snew a9AudioDX();
	if(a9_audio->Init(hwnd)==A9_FAIL)
	{
		sdelete(a9_audio);
		a9_audio = NULL;
		return FALSE;
	}
	return TRUE;
	unguard();
}

void A9_Done()
{
	guard(A9_Done);
	if(!a9_audio) return;
	a9_audio->Done();
	sdelete(a9_audio);
	a9_audio = NULL;
	dlog(LOGSND, "Audio done.\n");
	unguard();
}

int A9_VolumeDecibelToPercent( int vol )
{
	guard(A9_VolumeDecibelToPercent);
	vol/=100;
	if(vol>0) return 100;
	if(vol<=-100) return 0;
	return (int)(100.f / powf(10.f,(-vol/20.f)));
	unguard();
}

int A9_VolumePercentToDecibel( int vol )
{
	guard(A9_VolumePercentToDecibel);
	if(vol<=0) return -10000;
	if(vol>=100) return 0;
	return (int)(-100.f*(20.f * log10(100.f/vol)));
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
