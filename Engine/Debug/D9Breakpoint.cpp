///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Breakpoint.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Breakpoint.h"
#include "D9Log.h"

#define _SETBITS( dw, lowBit, bits, newValue)	dw = (dw & ~( ((1<<(bits))-1)<<(lowBit) )) | ((newValue)<<(lowBit));

BOOL D9_HardwareBreakpoint( int idx, DWORD address, int mode, int size )
{
	if(idx<0 || idx>=4) return FALSE;
	CONTEXT context;
	HANDLE thread = GetCurrentThread();
	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if(!GetThreadContext(thread, &context)) return FALSE;

	if(address!=NULL)
	{
		if((context.Dr7 & (1 << (idx*2))) != 0) return FALSE; // already set
		*(&context.Dr0 + idx) = address;
		_SETBITS(context.Dr7, 16 + (idx*4), 2, mode);
		_SETBITS(context.Dr7, 18 + (idx*4), 2, size);
		_SETBITS(context.Dr7, idx*2, 1, 1);
	}
	else
	{
		_SETBITS(context.Dr7, idx*2, 1, 0); // off
	}

	if(!SetThreadContext(thread, &context)) return FALSE;
	dlog(LOGDBG,"HardwareBreakpoint %i = %i\n",idx,address);
	return TRUE;
}

BOOL D9_HardwareSingleStep()
{
	CONTEXT context;
	HANDLE thread = GetCurrentThread();
	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if(!GetThreadContext(thread, &context)) return FALSE;
	context.EFlags |= 0x100; // set the "trap" flag for single step
	if(!SetThreadContext(thread, &context)) return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
