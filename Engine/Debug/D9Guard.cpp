///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Guard.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Guard.h"

const char* __FUNC__ = "unknown"; // used if no guards
_se_translator_function d9Guard::m_oldtranslator = NULL;

void d9Guard::Init()
{
	#ifdef D9_ENABLE_GUARD
	m_oldtranslator = _set_se_translator(MyTranslator);
	#endif
	#ifdef D9_ENABLE_DH
	D9_DHInit();
	#endif
}

void d9Guard::Done()
{
	#ifdef D9_ENABLE_DH
	D9_DHDone();
	#endif
	#ifdef D9_ENABLE_GUARD
	_set_se_translator(m_oldtranslator);
	#endif
}

void d9Guard::MyTranslator(unsigned int nCode, EXCEPTION_POINTERS *pExp)
{
	dlog(LOGSYS,"TRANSLATOR:\n");
	#ifdef D9_ENABLE_DH
	D9_DHLogCallStack(pExp);
	// D9_DHWriteMiniDump("dump.dmp",pExp);
	#endif
	D9_LogStore(TRUE);
	LogException(pExp); dlog(LOGSYS,"\n");
	throw d9Exception();
}

void d9Guard::Assert( const char* desc, const char* func, int line, char* file )
{
	#ifdef D9_ENABLE_DH
	D9_DHLogCallStack();
	#endif
	D9_LogStore(TRUE);
	dlog(LOGSYS,"ASSERT: %s\n",desc);
	dlog(LOGSYS,"[%s; %i; %s]\n", func, line, file );
	dlog(LOGSYS, "Choose YES to exit, NO to ignore and CANCEL to break.\n");
	int ret = sys_msgbox( E9_GetHWND(), D9_LogGetBuffer(), "ASSERTION FAILED", MB_YESNOCANCEL|MB_ICONEXCLAMATION );
	D9_LogStore( FALSE );
	if( ret==IDYES )	ErrorExit("Assertion Failed");
	if( ret==IDCANCEL )	{ dlog(LOGSYS,"BREAK.\n"); DebugBreak(); }
	dlog(LOGSYS,"CONTINUE.\n"); 
}

void d9Guard::ErrorExit( char* desc )
{
	D9_LogStore(TRUE);
	dlog(LOGSYS,"ERROREXIT: %s\n",desc);
	#ifdef D9_ENABLE_GUARD
	throw d9Exception();
	#else
	exit(-1);
	#endif
}

void d9Guard::Report()
{ 
	D9_LogStore(FALSE);
	sys_msgbox( E9_GetHWND(), D9_LogGetBuffer(), "EXCEPTION", MB_OK|MB_ICONEXCLAMATION ); 
}

void d9Guard::LogException( LPEXCEPTION_POINTERS e )
{
	static struct {	int id;	char *desc; } exceptions[] = 
	{
		{EXCEPTION_ACCESS_VIOLATION,		"EXCEPTION_ACCESS_VIOLATION"},
		{EXCEPTION_ARRAY_BOUNDS_EXCEEDED,	"EXCEPTION_ARRAY_BOUNDS_EXCEEDED"},
		{EXCEPTION_BREAKPOINT,				"EXCEPTION_BREAKPOINT"},
		{EXCEPTION_DATATYPE_MISALIGNMENT,	"EXCEPTION_DATATYPE_MISALIGNMENT"},
		{EXCEPTION_FLT_DENORMAL_OPERAND,	"EXCEPTION_FLT_DENORMAL_OPERAND"},
		{EXCEPTION_FLT_DIVIDE_BY_ZERO,		"EXCEPTION_FLT_DIVIDE_BY_ZERO"},
		{EXCEPTION_FLT_INEXACT_RESULT,		"EXCEPTION_FLT_INEXACT_RESULT"},
		{EXCEPTION_FLT_INVALID_OPERATION,	"EXCEPTION_FLT_INVALID_OPERATION"},
		{EXCEPTION_FLT_OVERFLOW,			"EXCEPTION_FLT_OVERFLOW"},
		{EXCEPTION_FLT_STACK_CHECK,			"EXCEPTION_FLT_STACK_CHECK"},
		{EXCEPTION_FLT_UNDERFLOW,			"EXCEPTION_FLT_UNDERFLOW"},
		{EXCEPTION_ILLEGAL_INSTRUCTION,		"EXCEPTION_ILLEGAL_INSTRUCTION"},
		{EXCEPTION_IN_PAGE_ERROR,			"EXCEPTION_IN_PAGE_ERROR"},
		{EXCEPTION_INT_DIVIDE_BY_ZERO,		"EXCEPTION_INT_DIVIDE_BY_ZERO"},
		{EXCEPTION_INT_OVERFLOW,			"EXCEPTION_INT_OVERFLOW"},
		{EXCEPTION_PRIV_INSTRUCTION,		"EXCEPTION_PRIV_INSTRUCTION"},
		{EXCEPTION_SINGLE_STEP,				"EXCEPTION_SINGLE_STEP"},
		{EXCEPTION_STACK_OVERFLOW,			"EXCEPTION_STACK_OVERFLOW"}
	};

	int i;
	int count = 18;
	int code = e->ExceptionRecord->ExceptionCode;

	for(i=0; i<count; i++)
		if(code==exceptions[i].id) break;

	if(i<count)
	{
		if(code==EXCEPTION_ACCESS_VIOLATION)
		{
			dlog( LOGSYS, "%s at address 0x%x (%s 0x%x)", 
				exceptions[i].desc, 
				(sizet)e->ExceptionRecord->ExceptionAddress,
				e->ExceptionRecord->ExceptionInformation[0]?"write to":"read from",
				(sizet)e->ExceptionRecord->ExceptionInformation[1] );
		}
		else
		{
			dlog( LOGSYS, "%s at address 0x%x", 
				exceptions[i].desc, 
				(sizet)e->ExceptionRecord->ExceptionAddress );
		}
	}
	else
	{
		dlog( LOGSYS, "EXCEPTION_UNKNOWN (%x) at address 0x%x", 
			code, 
			(sizet)e->ExceptionRecord->ExceptionAddress );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
