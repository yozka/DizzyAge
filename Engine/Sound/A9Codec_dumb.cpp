///////////////////////////////////////////////////////////////////////////////////////////////////
// A9Codec_dumb.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "A9Codec_dumb.h"

// make sure the paths to the libs are ok
#ifdef _DEBUG
#pragma comment( lib, "..\\Libs\\Dumb\\vc6\\dumb\\Debug\\dumbd.lib" )
#else
#pragma comment( lib, "..\\Libs\\Dumb\\vc6\\dumb\\Release\\dumb.lib" )
#endif

#define DUMB_SEC 65536.0f

// use our file system
void*	dumb_open	(const char *filename)			{ return F9_FileOpen((char*)filename); }
int		dumb_skip	(void *f, long n)				{ return F9_FileSeek((F9FILE)f,n,1); }
int		dumb_getc	(void *f)						{ int c=0; if(1!=F9_FileRead(&c,1,(F9FILE)f)) return -1; else return c; }
long	dumb_getnc	(char *ptr, long n, void *f)	{ return F9_FileRead(ptr,n,(F9FILE)f); }
void	dumb_close	(void *f)						{ F9_FileClose((F9FILE)f); }

DUMBFILE_SYSTEM dumb_fs;

///////////////////////////////////////////////////////////////////////////////////////////////////
a9Codec_dumb::a9Codec_dumb()
{
	guard(a9Codec_dumb::a9Codec_dumb);
	m_type		= A9_CODEC_DUMB;
	m_duh		= NULL;
	m_duhsr		= NULL;
	unguard();
}

a9Codec_dumb::~a9Codec_dumb()
{
	guard(a9Codec_dumb::~a9Codec_dumb);
	unguard();
}

int a9Codec_dumb::Init()
{
	guard(a9Codec_dumb::Init);
	atexit(&dumb_exit);
	//dumb_register_stdfiles();
	dumb_fs.open	= dumb_open;	
	dumb_fs.skip	= dumb_skip;
	dumb_fs.getc	= dumb_getc;
	dumb_fs.getnc	= dumb_getnc;
	dumb_fs.close	= dumb_close;
	register_dumbfile_system(&dumb_fs);
	dumb_it_max_to_mix = 256;
	return A9_OK;
	unguard();
}

int a9Codec_dumb::Done()
{
	guard(a9Codec_dumb::Done);
	return A9_OK;
	unguard();
}

int	a9Codec_dumb::Open( char* name )
{
	guard(a9Codec_dumb::Open);
	if(m_status!=A9_CODEC_CLOSED) return A9_FAIL;
	// set user callbacks
	// load
	m_duh = load_duh(name);
	if(!m_duh) 
	{
		m_duh = dumb_load_it(name);
		if(!m_duh) 
		{
			m_duh = dumb_load_xm(name);
			if(!m_duh) 
			{
				m_duh = dumb_load_s3m(name);
				if(!m_duh) 
				{
					m_duh = dumb_load_mod(name);
					if(!m_duh) 
					{
						return A9_FAIL;
					}
				}
			}
		}
	}
	
	int duhsize = duh_get_length(m_duh);

	m_info.m_depth		= 16;
	m_info.m_signed		= 1;
	m_info.m_channels	= 2;
	m_info.m_frequency	= 44100;
	m_info.m_size		= (int) (((float)duhsize / DUMB_SEC) * m_info.m_frequency);

	m_status = A9_CODEC_OPENED;
	return A9_OK;
	unguard();
}

int	a9Codec_dumb::BeginRender( int pos, int loop )
{
	guard(a9Codec_dumb::BeginRender);
	if(m_status!=A9_CODEC_OPENED) return A9_FAIL;
	m_loop = loop;

	m_duhsr = duh_start_sigrenderer(m_duh, 0, 2, pos); // 0sig, 2channels, pos
	if(!m_duhsr) return A9_FAIL;

	// no loop
	if(!loop)
	{
		DUMB_IT_SIGRENDERER * itsr = duh_get_it_sigrenderer(m_duhsr);
		dumb_it_set_loop_callback(itsr,dumb_it_callback_terminate,NULL);
	}

	m_status = A9_CODEC_RENDERING;
	return A9_OK;
	unguard();
}

int	a9Codec_dumb::Render( byte* buffer, int size )
{
	guard(a9Codec_dumb::Render);
	if(m_status!=A9_CODEC_RENDERING) return A9_FAIL;
	if(size<=0) return A9_FAIL;
	float volume = 1.0f;
	float delta = DUMB_SEC / m_info.m_frequency;
	int ret = duh_render(m_duhsr, m_info.m_depth, !m_info.m_signed, volume, delta, size, buffer);
	return ret;
	unguard();
}

int	a9Codec_dumb::EndRender()
{
	guard(a9Codec_dumb::EndRender);
	if(m_status!=A9_CODEC_RENDERING) return A9_FAIL;
	duh_end_sigrenderer(m_duhsr);
	m_status = A9_CODEC_OPENED;
	return A9_OK;
	unguard();
}

int	a9Codec_dumb::Close()
{
	guard(a9Codec_dumb::Close);
	if(m_status!=A9_CODEC_OPENED) return A9_FAIL;
	unload_duh(m_duh);
	m_status = A9_CODEC_CLOSED;
	return A9_OK;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
