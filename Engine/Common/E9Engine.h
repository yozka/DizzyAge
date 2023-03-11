///////////////////////////////////////////////////////////////////////////////////////////////////
// E9Engine.h
// Engine config and initialization
// Interface:
// LOGNUL, LOGSYS
// E9_Init, E9_Done, E9_GetHWND, E9_SetHWND, E9_GetHINSTANCE, E9_SetHINSTANCE, E9_GetFlag, E9_SetFlag
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9ENGINE_H__
#define __E9ENGINE_H__

#include "E9System.h"
#include "E9Config.h"

#ifdef _DEBUG
#pragma comment( lib, "..\\Engine\\engine_d.lib" )
#else
#pragma comment( lib, "..\\Engine\\engine.lib" )
#endif

class e9Engine
{
public:
static	HWND		m_hwnd;				// main hwnd
static	HINSTANCE	m_hinstance;		// main hinstance
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

// Engine log channels
#define LOGNUL				0			// default
#define LOGSYS				1			// system - open on release
#define LOGERR				2			// error - open on release
#define LOGENG				3			// engine
#define LOGDBG				4			// debug
#define LOGFIL				5			// files
#define LOGINP				6			// input
#define LOGRND				7			// render
#define LOGSND				8			// sound
#define LOGGS				9			// script
#define LOGAPP				10			// application
//		...

		void		E9_OpenChannels( BOOL open=TRUE );		// open debug channels; called by debug init
		BOOL		E9_Init();								// init engine general stuff
		void		E9_Done();								// done engine
inline	HWND		E9_GetHWND()							{ return e9Engine::m_hwnd; }
inline	void		E9_SetHWND( HWND hwnd )					{ e9Engine::m_hwnd = hwnd; }
inline	HINSTANCE	E9_GetHINSTANCE()						{ return e9Engine::m_hinstance; }
inline	void		E9_SetHINSTANCE( HINSTANCE hinstance )	{ e9Engine::m_hinstance = hinstance; }

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
