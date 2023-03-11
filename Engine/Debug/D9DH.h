///////////////////////////////////////////////////////////////////////////////////////////////////
// D9DH.h
// Functions from DebugHelp.dll
// Interface:
// D9_DHInit, D9_DHDone, D9_DHWriteMiniDump, D9_DHLogCallStack
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9DH_H__
#define __D9DH_H__
#ifdef D9_ENABLE_DH

#include "E9System.h"
#include "dbghelp.h"

class d9DH
{
public:
static	BOOL		Init();
static	void		Done();
static	BOOL		WriteMiniDump( char* file, EXCEPTION_POINTERS *pExp, BOOL full=FALSE );
static	void		LogCallStack( DWORD eip, DWORD esp, DWORD ebp );
static	void		LogModule( DWORD addr );
static	void		LogSource( DWORD addr );
static	void		LogSymbol( DWORD addr );

typedef DWORD		(WINAPI *tSymSetOptions)			( IN DWORD SymOptions );
typedef BOOL		(WINAPI *tSymInitialize)			( IN HANDLE hProcess, IN LPSTR UserSearchPath, IN BOOL fInvadeProcess );
typedef BOOL		(WINAPI *tSymCleanup)				( IN HANDLE hProcess );
typedef LPVOID		(WINAPI *tSymFunctionTableAccess)	( IN HANDLE hProcess, IN DWORD AddrBase );
typedef DWORD		(WINAPI *tSymGetModuleBase)			( IN HANDLE hProcess, IN DWORD dwAddr );
typedef BOOL		(WINAPI *tSymGetModuleInfo)			( IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo );
typedef BOOL		(WINAPI *tSymGetLineFromAddr)		( HANDLE hProcess, DWORD dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE Line );
typedef BOOL		(WINAPI *tSymGetSymFromAddr)		( IN HANDLE hProcess, IN DWORD Address, OUT LPDWORD Displacement, IN OUT PIMAGEHLP_SYMBOL Symbol );
typedef BOOL		(WINAPI *tStackWalk)				( IN DWORD MachineType, IN HANDLE hProcess, IN HANDLE hThread, IN OUT LPSTACKFRAME StackFrame, IN OUT LPVOID ContextRecord, IN PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine, IN PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine, IN PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, IN PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );
typedef BOOL		(WINAPI *tMiniDumpWriteDump)		( HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam );

static	HMODULE						m_dh;

static	tSymSetOptions				m_SymSetOptions;
static	tSymInitialize				m_SymInitialize;
static	tSymCleanup					m_SymCleanup;
static	tSymFunctionTableAccess		m_SymFunctionTableAccess;
static	tSymGetModuleBase			m_SymGetModuleBase;
static	tSymGetModuleInfo			m_SymGetModuleInfo;
static	tSymGetLineFromAddr			m_SymGetLineFromAddr;
static	tSymGetSymFromAddr			m_SymGetSymFromAddr;
static	tStackWalk					m_StackWalk;
static	tMiniDumpWriteDump			m_MiniDumpWriteDump;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
inline	BOOL	D9_DHInit()																	{ return d9DH::Init(); }
inline	void	D9_DHDone()																	{ return d9DH::Done(); }
inline	BOOL	D9_DHWriteMiniDump( char* file, EXCEPTION_POINTERS *pExp, BOOL full=FALSE )	{ return d9DH::WriteMiniDump( file, pExp, full ); }
inline	void	D9_DHLogCallStack( EXCEPTION_POINTERS *pExp )								{ return d9DH::LogCallStack( pExp->ContextRecord->Eip, pExp->ContextRecord->Esp, pExp->ContextRecord->Ebp ); }
inline	void	D9_DHLogCallStack()
{
	DWORD _eip, _esp, _ebp;
	__asm
	{
		call mylabel
		mylabel:
		pop eax;
		mov _eip, eax
		mov _esp, esp
		mov _ebp, ebp
	}
	d9DH::LogCallStack( _eip, _esp, _ebp );
}

#endif
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////