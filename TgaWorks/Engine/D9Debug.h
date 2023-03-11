///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Debug.h NONE
// Empty (almost) debug system (rename it and use it instead of D9Debug.h)
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9DEBUG_H__
#define __D9DEBUG_H__

#include "E9System.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

// LOG
#define D9_LOG_BUFFERSIZE				1024

#define D9_LOGFLAG_OPEN					(1<<0)				// channel is opened
#define D9_LOGFLAG_FILE					(1<<1)				// send to file
#define D9_LOGFLAG_DEBUG				(1<<2)				// send to debug window
#define D9_LOGFLAG_CALLBACK				(1<<3)				// send to callback
#define D9_LOGFLAG_DEFAULT				(D9_LOGFLAG_OPEN|D9_LOGFLAG_FILE|D9_LOGFLAG_DEBUG|D9_LOGFLAG_CALLBACK)

typedef void (*d9LogCallback)( int ch, char* msg );

#define	dlog	D9_LogPrintF

		void	D9_LogInit( const char* logfile, d9LogCallback callback=NULL  );
		void	D9_LogPrintV( int ch, const char* fmt, va_list args );
inline	void	D9_LogPrintF( int ch, const char* fmt, ... )															{ va_list args;	va_start(args, fmt); D9_LogPrintV(ch, fmt, args); va_end(args); }
inline	void	D9_LogPrintF( const char* fmt, ... )																	{ va_list args; va_start(args, fmt); D9_LogPrintV(0, fmt, args); va_end(args); }
inline	void	D9_LogSetCallback( d9LogCallback callback )																{ }
inline	void	D9_LogSetChannel( int ch, char* name, dword flags=D9_LOGFLAG_DEFAULT, dword color=0xffffffff )			{ }
inline	void	D9_LogSetChannelFlag( int ch, dword flag, BOOL on )														{ }
inline	void	D9_LogOpenChannel( int ch, BOOL open )																	{ }
inline	void	D9_LogStore( BOOL enable )																				{ }
inline	char*	D9_LogGetBuffer()																						{ static char sz[]=""; return sz; }

// GUARD
#define guard(name)
#define unguard()
#define unguardmain(code)
#define guardfast(name)
#define unguardfast()
#define errorexit(desc)					{ dlog("ERROREXIT: %s\n",desc); exit(-1); }
#ifdef ASSERT
#define sassert(exp)					ASSERT(exp)
#else
#define sassert(exp)					{ if(!(exp)) { dlog("ASSERT: %s\n",#exp); errorexit("asert"); } }
#endif
#define sverify(exp)					{ if(!(exp)) { dlog("ASSERT: %s\n",#exp); errorexit("asert"); } }

inline	void	D9_GuardInit()			{}
inline	void	D9_GuardDone()			{}
inline	void	D9_GuardReport()		{}
inline	void	D9_LogCallStack()		{}

// SHUTDOWN
typedef void (*d9ShutdownFn)();
inline	void	D9_ShutdownAdd( d9ShutdownFn fn )		{}
inline	void	D9_Shutdown()							{}

// MEMORY
#define	smalloc							malloc
#define srealloc						realloc
#define sfree(addr)						{ if(addr!=NULL) { free(addr); addr=NULL; } }
#define snew							new
#define sdelete(addr)					{ if(addr!=NULL) { delete addr; addr=NULL; } }
#define	srelease(a)						{ if(a!=NULL){ a->Release(); a=NULL; } }

inline	void	D9_MemoryInit()				{}
inline	void	D9_MemoryDone()				{}
inline	void	D9_MemoryReport(BOOL full)	{}
inline	void	D9_MemoryDebugReport()		{ _CrtCheckMemory(); _CrtDumpMemoryLeaks(); }
// tips: _crtBreakAlloc, _CrtSetBreakAlloc

// BREAKPOINT
#define D9_BREAKPOINT_EXECUTE		0
#define D9_BREAKPOINT_WRITE			1
#define D9_BREAKPOINT_READWRITE		3
inline	BOOL D9_HardwareBreakpoint( int idx, DWORD address, int mode=D9_BREAKPOINT_WRITE, int size=1 ) { return FALSE; }
inline	BOOL D9_HardwareSingleStep() { return FALSE; }

// DEBUG
#ifdef _DEBUG
#define D9_BUILDMODE	"_DEBUG  "
#else
#define D9_BUILDMODE	"_RELEASE"
#endif

#define D9_INIT( logfile, callback )												\
	D9_LogInit(logfile,callback);													\
	dlog( "------------------------------------------\n" );							\
	dlog( "| RENE NO-DEBUG INIT ( %s )      |\n", D9_BUILDMODE );					\
	dlog( "------------------------------------------\n" );							\
	guard(main);

#define D9_DONE()																	\
	dlog( "------------------------------------------\n" );							\
	dlog( "| RENE NO-DEBUG DONE                     |\n" );							\
	dlog( "------------------------------------------\n" );							\
	unguardmain(																	\
		dlog( "------------------------------------------\n" );						\
		dlog( "| RENE DEBUG EXCEPTION                   |\n" );						\
		dlog( "------------------------------------------\n" );						\
		);

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
