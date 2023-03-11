///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Debug.cpp NONE
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Debug.h"

static char d9_logfile[D9_LOG_BUFFERSIZE] = "debug.log";

void D9_LogInit( const char* logfile, d9LogCallback callback )
{ 
	if(logfile)
	{
		strncpy(d9_logfile,logfile,D9_LOG_BUFFERSIZE);
		d9_logfile[D9_LOG_BUFFERSIZE-1]=0;
	}
	file_delete((char*)logfile); 
}

void D9_LogPrintV( int ch, const char* fmt, va_list args )
{
	static char msg[D9_LOG_BUFFERSIZE];
	_vsnprintf(msg, D9_LOG_BUFFERSIZE, fmt, args);
	msg[D9_LOG_BUFFERSIZE-1]=0;

	// just send to debug
	sys_outputdebugstring(msg);

	// send to file
	FILE* f;
	f = fopen(d9_logfile, "at");
	if( f!=NULL )
	{
		vfprintf( f, fmt, args );
		fclose(f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
