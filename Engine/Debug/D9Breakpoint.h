///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Breakpoint.h
// Set hardware breakpoint on memory access
// Inspired from source of Mike Morearty
// idx: breakpoint register 0, 1, 2 or 3
// address: breakpoint address or NULL to clear (must be aligned properly)
// mode: 0=execute, 1=write, 3=write+read
// size = 0, 1, 2 or 4 (1 recommended)
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9BREAKPOINT_H__
#define __D9BREAKPOINT_H__

#include "E9System.h"

#define D9_BREAKPOINT_EXECUTE		0
#define D9_BREAKPOINT_WRITE			1
#define D9_BREAKPOINT_READWRITE		3

BOOL D9_HardwareBreakpoint( int idx, DWORD address, int mode=D9_BREAKPOINT_WRITE, int size=1 );	// set a breakpoint
BOOL D9_HardwareSingleStep(); // hm?

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
