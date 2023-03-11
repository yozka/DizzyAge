///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "A9Codec.h"
#ifdef A9_ENABLE_WAV	
	#include "A9Codec_wav.h"
#endif
#ifdef A9_ENABLE_OGG	
	#include "A9Codec_ogg.h"
#endif
#ifdef A9_ENABLE_DUMB	
	#include "A9Codec_dumb.h"
#endif
#ifdef A9_ENABLE_YM	
	#include "A9Codec_ym.h"
#endif

a9Codec::a9Codec()
{
	guard(a9Codec::a9Codec);

	m_type				= A9_CODEC_UNKNOWN;
	m_loop				= 0;
	m_status			= A9_CODEC_CLOSED; 

	m_info.m_depth		= 0;
	m_info.m_signed		= 0;
	m_info.m_channels	= 0;
	m_info.m_frequency	= 0;
	m_info.m_size		= 0;

	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
int A9_CodecFind( char* filename )
{
	guard(A9_CodecFind);
	if(filename==NULL) return A9_CODEC_UNKNOWN;
	int len = (int)strlen(filename);
	if(len<3) return A9_CODEC_UNKNOWN;
	int i; // find extention dot
	for(i=len-1;i>=0;i--) 
		if(filename[i]=='.') break;
	if(i==-1) return A9_CODEC_UNKNOWN;
	char fileext[4];
	strcpy(fileext,filename+i+1);
	strlwr(fileext);

	struct tFileSuport { int codec; const char* extensions; };
	static tFileSuport filesupport[] = 
	{
		#ifdef A9_ENABLE_WAV
		{ A9_CODEC_WAV,		"wav" },
		#endif
		#ifdef A9_ENABLE_OGG
		{ A9_CODEC_OGG,		"ogg" },
		#endif
		#ifdef A9_ENABLE_DUMB
		{ A9_CODEC_DUMB,	"mod,it,xm,s3m" },
		#endif
		#ifdef A9_ENABLE_YM
		{ A9_CODEC_YM,		"ym" },
		#endif
		{ A9_CODEC_UNKNOWN, "" }
	};

	for(i=0;filesupport[i].codec!=A9_CODEC_UNKNOWN;i++)
		if(strstr(filesupport[i].extensions,fileext)!=NULL)
			return filesupport[i].codec;
	
	return A9_CODEC_UNKNOWN;
	unguard();
}

int A9_CodecInit( int type )
{
	guard(A9_CodecInit);
	switch(type)
	{
	#ifdef A9_ENABLE_WAV
		case A9_CODEC_WAV:	return a9Codec_wav::Init();
	#endif
	#ifdef A9_ENABLE_OGG
		case A9_CODEC_OGG:	return a9Codec_ogg::Init();
	#endif
	#ifdef A9_ENABLE_DUMB
		case A9_CODEC_DUMB:	return a9Codec_dumb::Init();
	#endif
	#ifdef A9_ENABLE_YM
		case A9_CODEC_YM:	return a9Codec_ym::Init();
	#endif
	}
	return A9_FAIL;
	unguard();
}

int A9_CodecDone( int type )
{
	guard(A9_CodecDone);
	switch(type)
	{
	#ifdef A9_ENABLE_WAV
		case A9_CODEC_WAV:	return a9Codec_wav::Done();
	#endif
	#ifdef A9_ENABLE_OGG
		case A9_CODEC_OGG:	return a9Codec_ogg::Done();
	#endif
	#ifdef A9_ENABLE_DUMB
		case A9_CODEC_DUMB:	return a9Codec_dumb::Done();
	#endif
	#ifdef A9_ENABLE_YM
		case A9_CODEC_YM:	return a9Codec_ym::Done();
	#endif
	}
	return A9_FAIL;
	unguard();
}

void A9_CodecInitAll()
{
	guard(A9_CodecInitAll);
	int codecs[4] = { A9_CODEC_WAV, A9_CODEC_OGG, A9_CODEC_DUMB, A9_CODEC_YM };
	for(int i=0;i<4;i++)
		A9_CodecInit(codecs[i]);
	unguard();
}

void A9_CodecDoneAll()
{
	guard(A9_CodecDoneAll);
	int codecs[4] = { A9_CODEC_WAV, A9_CODEC_OGG, A9_CODEC_DUMB, A9_CODEC_YM };
	for(int i=0;i<4;i++)
		A9_CodecDone(codecs[i]);
	unguard();
}

A9CODEC	A9_CodecCreate( int type )
{
	guard(A9_CodecCreate);
	switch(type)
	{
		#ifdef A9_ENABLE_WAV
		case A9_CODEC_WAV:	return snew a9Codec_wav();
		#endif
		#ifdef A9_ENABLE_OGG
		case A9_CODEC_OGG:	return snew a9Codec_ogg();
		#endif
		#ifdef A9_ENABLE_DUMB
		case A9_CODEC_DUMB:	return snew a9Codec_dumb();
		#endif
		#ifdef A9_ENABLE_YM
		case A9_CODEC_YM:	return snew a9Codec_ym();
		#endif
	}
	return NULL;
	unguard();
}

void A9_CodecDestroy( A9CODEC codec )
{
	guard(A9_CodecDestroy);
	sassert(codec);
	sdelete(codec);
	unguard();
}

int	A9_CodecDecodeToWave( A9CODEC codec, byte* buffer )
{
	guard(a9Codec::DecodeToWave);
	if(codec->m_status!=A9_CODEC_OPENED) return A9_FAIL;
	int ret;
	int datasize = codec->m_info.DataSize();
	a9WavHeader* header = (a9WavHeader*)buffer;
	
	header->m_riff[0]	= 'R';
	header->m_riff[1]	= 'I';
	header->m_riff[2]	= 'F';
	header->m_riff[3]	= 'F';
	header->m_filesize	= (8+16)+(8+datasize);

	header->m_wave[0]	= 'W';
	header->m_wave[1]	= 'A';
	header->m_wave[2]	= 'V';
	header->m_wave[3]	= 'E';

	header->m_fmt[0]	= 'f';
	header->m_fmt[1]	= 'm';
	header->m_fmt[2]	= 't';
	header->m_fmt[3]	= ' ';
	header->m_fmtsize	= 16;

	header->m_tag			= 1;
	header->m_channels		= codec->m_info.m_channels;
	header->m_frequency		= codec->m_info.m_frequency;
	header->m_depth			= codec->m_info.m_depth;
	header->m_blockalign	= header->m_channels * header->m_depth / 8;
	header->m_bytespersec	= header->m_frequency * header->m_blockalign;

	header->m_data[0]	= 'd';
	header->m_data[1]	= 'a';
	header->m_data[2]	= 't';
	header->m_data[3]	= 'a';
	header->m_datasize	= datasize;

	byte* data = buffer + sizeof(a9WavHeader);

	ret = codec->BeginRender(0,0); if(ret!=A9_OK) return ret;
	ret = codec->Render(data,codec->m_info.m_size);
	if( ret!=codec->m_info.m_size ) { codec->EndRender(); return A9_FAIL; }
	ret = codec->EndRender();

	return A9_OK;
	unguard();
}

int A9_CodecDecodeToWave( char* filename, byte* &buffer, int& size )
{
	guard(A9_CodecDecodeToWave);
	// load and decode file
	if(!filename) return A9_FAIL;
	int codectype = A9_CodecFind(filename);
	A9CODEC codec = A9_CodecCreate(codectype);
	if(!codec) return A9_FAIL;

	// open codec
	int ret = A9_CodecOpen(codec,filename);
	if(ret!=A9_OK) return ret;
	
	// alloc buffer
	size = sizeof(a9WavHeader) + codec->m_info.DataSize();
	buffer = (byte*)smalloc( size );
	if(!buffer) { A9_CodecClose(codec); A9_CodecDestroy(codec); return A9_FAIL; }
	
	// write header and decoded data
	ret = A9_CodecDecodeToWave(codec,buffer);
	if(ret!=A9_OK) { sfree(buffer); A9_CodecClose(codec); A9_CodecDestroy(codec); return A9_FAIL; }

	// close codec
	A9_CodecClose(codec);
	A9_CodecDestroy(codec);
	return A9_OK;		
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
