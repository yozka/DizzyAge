///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Log.h
// Log debug messages
// Interface:
// dlog
// D9_LogInit, D9_LogPrintF, D9_LogPrintV, D9_LogSetCallback, 
// D9_LogSetChannel, D9_LogSetChannelFlag, D9_LogOpenChannel
// D9_LogStore, D9_LogGetBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9LOG_H__
#define __D9LOG_H__

#include "E9System.h"
#include "E9Engine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////////////////////
#define D9_LOG_CHANNELMAX				64					// max number of log channels
#define D9_LOG_NAMESIZE					32					// size of a channel name
#define D9_LOG_BUFFERSIZE				1024				// size of a single log message and of the storing buffer

// flags
#define D9_LOGFLAG_OPEN					(1<<0)				// channel is opened
#define D9_LOGFLAG_FILE					(1<<1)				// send to file
#define D9_LOGFLAG_DEBUG				(1<<2)				// send to debug window
#define D9_LOGFLAG_CALLBACK				(1<<3)				// send to callback
#define D9_LOGFLAG_DEFAULT				(D9_LOGFLAG_OPEN|D9_LOGFLAG_FILE|D9_LOGFLAG_DEBUG|D9_LOGFLAG_CALLBACK)

///////////////////////////////////////////////////////////////////////////////////////////////////
// d9Log class
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (*d9LogCallback)( int ch, char* msg );

struct d9LogChannel
{
			d9LogChannel();

	char	m_name[D9_LOG_NAMESIZE];	// channel name
	dword	m_flags;					// status flags (0=closed)
	dword	m_color;					// color option
};

class d9Log
{
public:

static	void			Init( const char* logfile, d9LogCallback callback=NULL );
static	void			Clear();
static	void			SetChannel( int ch, const char* name, dword flags, dword color );
static	void			Store( BOOL enable );

static	void			PrintV( int ch, const char* fmt, va_list args );
static	void			PrintF( int ch, const char* fmt, ... );
static	void			PrintF( const char* fmt, ... );

static	char			m_logfile[D9_LOG_NAMESIZE];		// log file
static	d9LogChannel	m_logc[D9_LOG_CHANNELMAX];		// channels
static	BOOL			m_store;						// if log is stored or not
static	char			m_buffer[D9_LOG_BUFFERSIZE];	// stored log
static	d9LogCallback	m_callback;						// log user callback

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

#define	dlog	D9_LogPrintF

inline	void	D9_LogInit( const char* logfile, d9LogCallback callback=NULL  )											{ d9Log::Init( logfile, callback ); }
inline	void	D9_LogPrintV( int ch, const char* fmt, va_list args )													{ d9Log::PrintV( ch, fmt, args ); }
inline	void	D9_LogPrintF( int ch, const char* fmt, ... )															{ va_list args;	va_start(args, fmt); D9_LogPrintV(ch, fmt, args); va_end(args); }
inline	void	D9_LogPrintF( const char* fmt, ... )																	{ va_list args; va_start(args, fmt); D9_LogPrintV(0, fmt, args); va_end(args); }
inline	void	D9_LogSetCallback( d9LogCallback callback )																{ d9Log::m_callback = callback; }
inline	void	D9_LogSetChannel( int ch, char* name, dword flags=D9_LOGFLAG_DEFAULT, dword color=0xffffffff )			{ d9Log::SetChannel( ch, name, flags, color ); }
inline	void	D9_LogSetChannelFlag( int ch, dword flag, BOOL on )														{ if(on) d9Log::m_logc[ch].m_flags |= flag; else d9Log::m_logc[ch].m_flags &= ~flag; }
inline	dword	D9_LogGetChannelColor( int ch )																			{ return d9Log::m_logc[ch].m_color; }
inline	void	D9_LogOpenChannel( int ch, BOOL open )																	{ D9_LogSetChannelFlag( ch, D9_LOGFLAG_OPEN, open ); }
inline	void	D9_LogStore( BOOL enable )																				{ d9Log::Store( enable ); }
inline	char*	D9_LogGetBuffer()																						{ return d9Log::m_buffer; }

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
