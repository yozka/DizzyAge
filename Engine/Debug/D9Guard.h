///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Guard.h
// This module handles exceptions
// Config:
// D9_ENABLE_GUARD, D9_ENABLE_FUNC, D9_ENABLE_ASSERT, D9_ENABLE_DH
// Interface:
// guard, unguard, unguardmain, fastguard, fastunguard
// sassert, sverify, errorexit
// D9_GuardInit, D9_GuardDone, D9_GuardReport, D9_LogCallStack
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9GUARD_H__
#define __D9GUARD_H__

#include <assert.h>
#include "E9System.h"
#include "E9Engine.h"
#include "D9Log.h"
#ifdef D9_ENABLE_DH
#include "D9DH.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// d9Guard class
///////////////////////////////////////////////////////////////////////////////////////////////////
extern const char* __FUNC__;

struct d9Exception {};

class d9Guard
{
public:

static	void	Init();
static	void	Done();
static	void	MyTranslator(unsigned int nCode, EXCEPTION_POINTERS *pExp);
static	void	Assert( const char* desc, const char* func, int line, char* file );
static	void	ErrorExit( char* desc=NULL );
static	void	Report();
static	void	LogException( LPEXCEPTION_POINTERS e );

static	_se_translator_function m_oldtranslator;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef D9_ENABLE_FUNC
#define __FUNC__TRACKER(func)	static char* __FUNC__ = #func;
#else
#define __FUNC__TRACKER(func)
#endif


#ifdef D9_ENABLE_GUARD

#define guard(func)									\
{													\
	__FUNC__TRACKER(func)							\
	try												\
	{												

#define unguard()									\
	}												\
	catch(d9Exception e)							\
	{												\
		D9_LogStore(TRUE);							\
		dlog(LOGSYS, "%s < ", __FUNC__);			\
		throw e;									\
	}												\
	catch(...)										\
	{												\
		D9_LogStore(TRUE);							\
		dlog(LOGSYS,"Unknown exception caught in function %s", __FUNC__); \
		throw d9Exception();						\
	}												\
}													
	
#define unguardmain(code)							\
	}												\
	catch(...)										\
	{												\
		D9_LogStore(TRUE);							\
		dlog(LOGSYS, "%s\n", __FUNC__);				\
		code;										\
	}												\
}

#define guardfast(func)								\
{													\
	__FUNC__TRACKER(func)							

#define unguardfast()								\
}

#else

#define guard(func)				__FUNC__TRACKER(func)
#define unguard()
#define unguardmain(code)		
#define guardfast(func)			__FUNC__TRACKER(func)
#define unguardfast()

#endif

#ifdef D9_ENABLE_ASSERT
#define sassert(exp)	((void)( (exp) || (d9Guard::Assert(#exp, __FUNC__, __LINE__, __FILE__), 0)))
#else
#define sassert(exp)	; // assert(exp)
#endif
#define sverify(exp)	((void)( (exp) || (d9Guard::Assert(#exp, __FUNC__, __LINE__, __FILE__), 0)))

#define errorexit(desc)	d9Guard::ErrorExit(desc)

inline	void	D9_GuardInit()							{ d9Guard::Init(); }
inline	void	D9_GuardDone()							{ d9Guard::Done(); }
inline	void	D9_GuardReport()						{ d9Guard::Report(); }

#ifdef D9_ENABLE_DH
inline	void	D9_LogCallStack()						{ D9_DHLogCallStack(); }
#else
inline	void	D9_LogCallStack()						{ }; // unsupported
#endif

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
