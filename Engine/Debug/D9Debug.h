///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Debug.h
// The debug system (include this file)
// Interface:
// D9_INIT, D9_DONE
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9DEBUG_H__
#define __D9DEBUG_H__

#include "D9Log.h"
#include "D9Guard.h"
#include "D9Shutdown.h"
#include "D9Memory.h"
#include "D9Breakpoint.h"

#ifdef _DEBUG
#define D9_BUILDMODE	"_DEBUG  "
#else
#define D9_BUILDMODE	"_RELEASE"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

// must be placed at the begining of main function
#define D9_INIT( logfile, callback, openlog )										\
	D9_LogInit(logfile,callback);													\
	E9_OpenChannels( openlog );														\
	D9_GuardInit();																	\
	D9_MemoryInit();																\
	dlog( LOGDBG, "------------------------------------------\n" );					\
	dlog( LOGDBG, "| RENE DEBUG INIT ( %s )           |\n", D9_BUILDMODE );			\
	dlog( LOGDBG, "------------------------------------------\n" );					\
	guard(main);

// must be placed at the end of main function
#define D9_DONE()																	\
	D9_Shutdown();																	\
	D9_GuardDone();																	\
	D9_MemoryReport(TRUE);															\
	D9_MemoryDone();																\
	D9_MemoryDebugReport();															\
	dlog( LOGDBG, "------------------------------------------\n" );					\
	dlog( LOGDBG, "| RENE DEBUG DONE                        |\n" );					\
	dlog( LOGDBG, "------------------------------------------\n" );					\
	unguardmain(																	\
		D9_GuardReport();															\
		D9_Shutdown();																\
		D9_GuardDone();																\
		D9_MemoryDone();															\
		dlog( LOGDBG, "------------------------------------------\n" );				\
		dlog( LOGDBG, "| RENE DEBUG EXCEPTION                   |\n" );				\
		dlog( LOGDBG, "------------------------------------------\n" );				\
		);

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
