///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Log.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Log.h"

char			d9Log::m_logfile[D9_LOG_NAMESIZE] = "";
d9LogChannel	d9Log::m_logc[D9_LOG_CHANNELMAX];
BOOL			d9Log::m_store = FALSE;
char			d9Log::m_buffer[D9_LOG_BUFFERSIZE] = "";
d9LogCallback	d9Log::m_callback = NULL;

d9LogChannel::d9LogChannel()				
{	
	m_name[0]=0; 
	m_flags=0; 
	m_color=0xffffffff; 
}

void d9Log::Init( const char* logfile, d9LogCallback callback )
{ 
	if(logfile)
	{
		strncpy(m_logfile,logfile,D9_LOG_NAMESIZE-1); 
		m_logfile[D9_LOG_NAMESIZE-1]=0; 
		Clear();
	}
	m_callback = callback;
}

void d9Log::Clear()
{
	file_delete(m_logfile);
}

void d9Log::SetChannel( int ch, const char* name, dword flags, dword color )
{ 
	if(ch<0 || ch>=D9_LOG_CHANNELMAX) return;
	strncpy(m_logc[ch].m_name,name,D9_LOG_NAMESIZE-1); m_logc[ch].m_name[D9_LOG_NAMESIZE-1]=0;
	m_logc[ch].m_flags = flags;
	m_logc[ch].m_color = color;
}

void d9Log::Store( BOOL enable )
{
	if( enable && !m_store ) m_buffer[0]=0; // reset
	m_store = enable;
}

void d9Log::PrintV( int ch, const char* fmt, va_list args )
{
	if(ch<0 || ch>=D9_LOG_CHANNELMAX) return;
	int flags = m_logc[ch].m_flags;
	if(!(flags & D9_LOGFLAG_OPEN)) return;

	static char msg[D9_LOG_BUFFERSIZE];
	if( flags & (D9_LOGFLAG_DEBUG|D9_LOGFLAG_CALLBACK) )
	{
		_vsnprintf(msg, D9_LOG_BUFFERSIZE, fmt, args);
		msg[D9_LOG_BUFFERSIZE-1]=0;
	}
	
	// send to debug
	if( flags & D9_LOGFLAG_DEBUG )
		sys_outputdebugstring(msg);

	// send to file
	if( (flags & D9_LOGFLAG_FILE) && m_logfile[0] )
	{
		FILE* f;
		f = fopen(m_logfile, "at");
		if( f!=NULL )
		{
			vfprintf( f, fmt, args );
			fclose(f);
		}
	}

	// send to callback
	if( (flags & D9_LOGFLAG_CALLBACK) && m_callback )
		m_callback(ch, msg);

	// store
	if(m_store)
	{
		int size = D9_LOG_BUFFERSIZE - (int)strlen(m_buffer) - 1;
		strncat(m_buffer, msg, size);
		m_buffer[D9_LOG_BUFFERSIZE-1]=0;
	}
}

void d9Log::PrintF( int ch, const char* fmt, ... )
{
	va_list	args;
	va_start(args, fmt);
	PrintV(ch, fmt, args);
	va_end(args);
}

void d9Log::PrintF( const char* fmt, ... )
{
	va_list	args;
	va_start(args, fmt);
	PrintV(0, fmt, args);
	va_end(args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
