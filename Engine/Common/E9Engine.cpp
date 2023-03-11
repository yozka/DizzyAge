///////////////////////////////////////////////////////////////////////////////////////////////////
// E9Engine.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E9Engine.h"
#include "D9Debug.h"

HWND e9Engine::m_hwnd = NULL;
HINSTANCE e9Engine::m_hinstance = NULL;

void E9_OpenChannels( BOOL open )
{
	dword logflags = D9_LOGFLAG_DEFAULT;
	if(!open) logflags &= ~D9_LOGFLAG_OPEN;

	D9_LogSetChannel( LOGNUL, "NUL", logflags,				DWORD_GREY		);
	D9_LogSetChannel( LOGSYS, "SYS", D9_LOGFLAG_DEFAULT,	DWORD_RED		);
	D9_LogSetChannel( LOGERR, "ERR", D9_LOGFLAG_DEFAULT,	DWORD_RED		);
	D9_LogSetChannel( LOGENG, "ENG", logflags, 				DWORD_BLUE		);
	D9_LogSetChannel( LOGDBG, "DBG", logflags, 				DWORD_ORANGE	);
	D9_LogSetChannel( LOGFIL, "FIL", logflags, 				DWORD_DGREEN	);
	D9_LogSetChannel( LOGINP, "INP", logflags, 				DWORD_GREEN		);
	D9_LogSetChannel( LOGRND, "RND", logflags, 				DWORD_LRED		);
	D9_LogSetChannel( LOGSND, "SND", logflags, 				DWORD_LRED		);
	D9_LogSetChannel( LOGGS , "GS ", logflags, 				DWORD_LBLUE		);
	D9_LogSetChannel( LOGAPP, "APP", logflags, 				DWORD_GREEN		);
}

BOOL E9_Init()
{
	guard(E9_Init);
	if(FAILED(CoInitialize(NULL))) { dlog(LOGERR,"ENGINE: failed to initialize COM.\n"); return FALSE; }
	return TRUE;
	unguard();
}

void E9_Done()
{
	guard(E9_Done);
	CoUninitialize();
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
