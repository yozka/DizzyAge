///////////////////////////////////////////////////////////////////////////////////////////////////
// D9DH.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef D9_ENABLE_DH
#include "tchar.h"
#include "D9DH.h"
#include "D9Log.h"

HMODULE							d9DH::m_dh = NULL;
d9DH::tSymSetOptions			d9DH::m_SymSetOptions = NULL;
d9DH::tSymInitialize			d9DH::m_SymInitialize = NULL;
d9DH::tSymCleanup				d9DH::m_SymCleanup = NULL;
d9DH::tSymFunctionTableAccess	d9DH::m_SymFunctionTableAccess=NULL;
d9DH::tSymGetModuleBase			d9DH::m_SymGetModuleBase=NULL;
d9DH::tSymGetModuleInfo			d9DH::m_SymGetModuleInfo = NULL;
d9DH::tSymGetLineFromAddr		d9DH::m_SymGetLineFromAddr = NULL;
d9DH::tSymGetSymFromAddr		d9DH::m_SymGetSymFromAddr = NULL;
d9DH::tStackWalk				d9DH::m_StackWalk = NULL;
d9DH::tMiniDumpWriteDump		d9DH::m_MiniDumpWriteDump = NULL;

BOOL d9DH::Init()
{
	if(m_dh) return TRUE; // already loaded

	char dllpath[MAX_PATH];
	if(GetModuleFileName( NULL, dllpath, MAX_PATH ))
	{
		char* szslash = _tcsrchr( dllpath, '\\' );
		if(szslash)
		{
			_tcscpy( szslash+1, "dbghelp.dll" );
			m_dh = ::LoadLibrary( dllpath );
		}
	}

	if( !m_dh )
		m_dh = ::LoadLibrary( "dbghelp.dll" ); // load any version we can
	if( !m_dh )	return FALSE;

	m_SymSetOptions = (tSymSetOptions)::GetProcAddress( m_dh, "SymSetOptions" );
	m_SymInitialize =(tSymInitialize)::GetProcAddress( m_dh, "SymInitialize" );
	m_SymCleanup = (tSymCleanup)::GetProcAddress( m_dh, "SymCleanup" );
	m_SymFunctionTableAccess = (tSymFunctionTableAccess)::GetProcAddress( m_dh, "SymFunctionTableAccess" );
	m_SymGetModuleBase = (tSymGetModuleBase)::GetProcAddress( m_dh, "SymGetModuleBase" );
	m_SymGetModuleInfo = (tSymGetModuleInfo)::GetProcAddress( m_dh, "SymGetModuleInfo" );
	m_SymGetLineFromAddr = (tSymGetLineFromAddr)::GetProcAddress( m_dh, "SymGetLineFromAddr" );
	m_SymGetSymFromAddr = (tSymGetSymFromAddr)::GetProcAddress( m_dh, "SymGetSymFromAddr" );
	m_StackWalk = (tStackWalk)::GetProcAddress( m_dh, "StackWalk" );
	m_MiniDumpWriteDump	= (tMiniDumpWriteDump)::GetProcAddress( m_dh, "MiniDumpWriteDump" );

	if(	!m_SymSetOptions ||
		!m_SymInitialize ||
		!m_SymCleanup ||
		!m_SymFunctionTableAccess ||
		!m_SymGetModuleBase ||
		!m_SymGetModuleInfo ||
		!m_SymGetLineFromAddr ||
		!m_SymGetSymFromAddr ||
		!m_StackWalk ||
		!m_MiniDumpWriteDump )
	{
		FreeLibrary(m_dh);
		return FALSE;
	}

	// load symbols if available
	dword dw = m_SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_LINES );
	BOOL ok = m_SymInitialize( GetCurrentProcess(), NULL, TRUE );

	return TRUE;
}

void d9DH::Done()
{
	if(m_dh) 
	{
		FreeLibrary(m_dh); 
		m_dh=NULL;
	}
}

BOOL d9DH::WriteMiniDump( char* file, EXCEPTION_POINTERS *pExp, BOOL full )
{
	if(!m_dh) return FALSE;

	LPCTSTR szResult = NULL;
	HANDLE hFile = ::CreateFile( file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile==INVALID_HANDLE_VALUE) return FALSE;

	MINIDUMP_EXCEPTION_INFORMATION exinfo;
	exinfo.ThreadId = ::GetCurrentThreadId();
	exinfo.ExceptionPointers = pExp;
	exinfo.ClientPointers = NULL;

	// write the dump
	BOOL ok = m_MiniDumpWriteDump(	GetCurrentProcess(), 
									GetCurrentProcessId(), 
									hFile, 
									full ? MiniDumpWithFullMemory : MiniDumpNormal, 
									&exinfo, 
									NULL, NULL );

	dlog(LOGSYS, "MINIDUMP %s%s (%s)\n", full?"FULL ":"", ok?"saved":"failed", file);
	
	::CloseHandle(hFile);
	return ok;
}


void d9DH::LogCallStack( DWORD eip, DWORD esp, DWORD ebp )
{
	dlog(LOGSYS,"callstack:\n");
	HANDLE hprocess = GetCurrentProcess();
	HANDLE hthread = GetCurrentThread();
	STACKFRAME callstack;
	memset(&callstack,0,sizeof(callstack));
	callstack.AddrPC.Offset = eip;
	callstack.AddrStack.Offset = esp;
	callstack.AddrFrame.Offset = ebp;
	callstack.AddrPC.Mode = AddrModeFlat;
	callstack.AddrStack.Mode = AddrModeFlat;
	callstack.AddrFrame.Mode = AddrModeFlat;
	int i;
	for(i=0;i<100;i++)
	{
		BOOL ok = m_StackWalk(	IMAGE_FILE_MACHINE_I386,
								hprocess, hthread,
								&callstack,
								NULL, NULL,
								m_SymFunctionTableAccess,
								m_SymGetModuleBase,
								NULL );
		if(!ok || callstack.AddrFrame.Offset==0) break; // done
		// dlog(LOGSYS," %x\n",callstack.AddrPC.Offset);
		DWORD addr = callstack.AddrPC.Offset;
		dlog(LOGSYS,"0x%08x : ",addr);
		LogModule( addr );
		dlog(LOGSYS," : ");
		LogSource( addr );
		dlog(LOGSYS," : ");
		LogSymbol( addr );
		dlog(LOGSYS,"\n");
	}
}

void d9DH::LogModule( DWORD addr )
{
	IMAGEHLP_MODULE module;
	memset(&module,0,sizeof(module));
	module.SizeOfStruct = sizeof(module);
	if( m_SymGetModuleInfo( GetCurrentProcess(), addr, &module ) )
		dlog( LOGSYS, module.ModuleName);
	else
		dlog( LOGSYS, "?");
}

void d9DH::LogSource( DWORD addr )
{
	dword displacement;
	IMAGEHLP_LINE line;
	memset(&line,0,sizeof(line));
	line.SizeOfStruct = sizeof(line);
	if( m_SymGetLineFromAddr( GetCurrentProcess(), addr, (PDWORD)&displacement, &line ) )
		dlog(LOGSYS, "%s(%d)", line.FileName, line.LineNumber );
	else
		dlog(LOGSYS, "?");
}

void d9DH::LogSymbol( DWORD addr )
{
	static char symbol[33];
	PIMAGEHLP_SYMBOL psymbol = (PIMAGEHLP_SYMBOL)symbol;
	memset(symbol,0,sizeof(symbol));
	psymbol->SizeOfStruct = 32;
	psymbol->MaxNameLength = 32 - sizeof(IMAGEHLP_SYMBOL);
	if( m_SymGetSymFromAddr( GetCurrentProcess(), addr, 0, psymbol ) )
		dlog(LOGSYS, psymbol->Name);
	else
		dlog(LOGSYS, "?");
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
