///////////////////////////////////////////////////////////////////////////////////////////////////
// E9App.h
// Game application
// Interface:
// E9_APP_ONINIT, E9_APP_ONDONE, E9_APP_ONRUN, E9_APP_ONACTIVATE, E9_APP_ONPAINT
// E9_APP_NAME, E9_APP_ICON, E9_APP_CMDLINE, E9_APP_PATH, E9_APP_ACTIVE, E9_APP_WINDOWED, E9_APP_FRAME, E9_APP_DELTATIME, E9_APP_FPS
// E9_AppSetCallbacks, E9_AppInit, E9_AppDone, E9_AppRun, E9_AppSetStr, E9_AppGetStr, E9_AppSetInt, E9_AppGetInt
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9APP_H__
#define __E9APP_H__

#include "E9System.h"
#include "E9Engine.h"

// callbacks
#define	E9_APP_ONINIT			0	// called in Init, after window was created but is still hidden; user return 0 if failure or 1 if success
#define	E9_APP_ONDONE			1	// called in Done
#define	E9_APP_ONRUN			2	// called in main loop when the application is active; return 0 to exit or 1 to continue
#define	E9_APP_ONACTIVATE		3	// called when application gets activated or deactivated; check the active state
#define	E9_APP_ONCLOSE			4	// called when application receive close message return 0 to refuse closing
#define	E9_APP_ONPAINT			5	// called when application need painting and is windowed, deactivated and not minimized
#define	E9_APP_ONMSG			7	// called on every window message, uses params 0..3 (available only in windows)
#define	E9_APP_CALLBACKS		8	// dummy

// properties
#define E9_APP_NAME				0	// window name (str-rw)
#define E9_APP_ICON				1	// icon resource name (can be MAKEINTRESOURCE(IDI_ICON)) (str-w)
#define E9_APP_CMDLINE			2	// command line as given in init (str-r)
#define E9_APP_PATH				3	// path of the executable (str-r)
#define E9_APP_ACTIVE			4	// if application is active (int-r)
#define E9_APP_WINDOWED			5	// if application is windowed or fullscreen (int-rw)
#define	E9_APP_CURSOR			6	// current cursor index
#define E9_APP_COOL				7	// cool cpu option, sleep a little each frame (int-rw)
#define E9_APP_FRAME			8	// internal frame count (int-r)
#define E9_APP_DELTATIME		9	// internal delta time in ms (int-r)
#define E9_APP_FPS				10	// internal fps (int-r)
#define E9_APP_PARAM			11	// generic callback param (void-r) 4 values (0=msg,1=wparam,2=lparam,3=return)
#define E9_APP_DUMMIE			15	// generic callback param (void-r)

// cursors
#define E9_CURSOR_NONE		0	// none
#define E9_CURSOR_ARROW		1	// arrow
#define E9_CURSOR_WAIT		2	// wait
#define E9_CURSOR_HAND		3	// hand
#define E9_CURSOR_CUSTOM	4	// users could set a cursor here //@TODO: not implemented
#define E9_CURSOR_MAX		5	// dummy

#define E9_APP_CLASSNAME		"E9_APPCLASS"

typedef int	(*e9AppCallback)();

class e9App
{
public:

static	void		SetCallback( int idx, e9AppCallback callback );	// set a callback
static	BOOL		Init( HINSTANCE	hinstance, char* cmdline );		// init the application
static	void		Done();											// done the application
static	void		Run();											// main loop
static	void		SetStr( int prop, const char* value );			// set string property
static	const char*	GetStr( int prop );								// get string property
static	void		SetInt( int prop, int value );					// set integer property
static	int			GetInt( int prop );			  					// get integer property
static	void		SetVoid( int prop, void* value );				// set void* property
static	void*		GetVoid( int prop );			 				// get void* property
static	void		SetCursor( int cursor );						// set mouse cursor

protected:

static	int			InitWindow();
static	void		UpdateClocks();
static	LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

// members
static	char*		m_cmdline;					// pointer to command line string
static	char		m_path[MAX_PATH];			// path to exe
static	HWND		m_hwnd;						// main window handle
static	HINSTANCE	m_hinstance;				// instance handle
static	BOOL		m_windowed;					// if windowed or fullscreen
static	BOOL		m_active;					// active state
static	BOOL		m_minimized;				// minimized state
static	BOOL		m_cool;						// cool cpu
static	int			m_cursor;					// current cursor index
static	HCURSOR		m_hcursor[E9_CURSOR_MAX];	// mouse cursors

static	int			m_frame;					// current frame
static	dword		m_tick;						// real tick
static	dword		m_ticklast;					// old real tick
static	float		m_fps;						// fps value

static	void*		m_param[4];					// generic void params

// callbacks
static	e9AppCallback	m_callback[E9_APP_CALLBACKS];
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
inline	void		E9_AppSetCallback( int idx, e9AppCallback callback )	{ e9App::SetCallback( idx, callback ); }
inline	BOOL		E9_AppInit( HINSTANCE hinstance, char* cmdline )		{ return e9App::Init( hinstance, cmdline ); }
inline	void		E9_AppDone()											{ e9App::Done(); }
inline	void		E9_AppRun()												{ e9App::Run(); }
inline	void		E9_AppSetStr( int prop, const char* value )				{ e9App::SetStr( prop, value ); }
inline	const char*	E9_AppGetStr( int prop )								{ return e9App::GetStr( prop ); }
inline	void		E9_AppSetInt( int prop, int value )						{ e9App::SetInt( prop, value ); }
inline	int			E9_AppGetInt( int prop )								{ return e9App::GetInt( prop ); }
inline	void		E9_AppSetVoid( int prop, void* value )					{ e9App::SetVoid( prop, value ); }
inline	void*		E9_AppGetVoid( int prop )								{ return e9App::GetVoid( prop ); }
inline	void		E9_AppSetCursor( int cursor )							{ e9App::SetCursor( cursor ); }

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
