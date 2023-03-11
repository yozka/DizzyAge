#include "stdafx.h"
#include "gscfg.h"

/////////////////////////////////////////////////////////////////////////////////////////
// LOGS
/////////////////////////////////////////////////////////////////////////////////////////
void gslog(const char *fmt, ...)
{
	guard(gslog)
	va_list	args;
	va_start(args, fmt);
	// gslogv(fmt, args); // primitive log
	D9_LogPrintV(LOGGS,fmt,args);
	va_end(args);
	unguard()
}

void gslogv(const char *fmt, va_list args)
{
	guard(gslogv)
	static char	msg[512];
	_vsnprintf(msg, sizeof(msg), fmt, args);
	OutputDebugString(msg);
	printf(msg);
	unguard()
}

void _gserrorexit( char* text )
{ 
	guard(_gserrorexit)
	int ret = MessageBox( NULL,text,"GS9 ERROR: do you want to exit ?", MB_YESNO );
	if(ret==IDYES) errorexit("GS9 ERROR");  // exit(EXIT_FAILURE);
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
