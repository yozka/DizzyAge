///////////////////////////////////////////////////////////////////////////////////////////////////
// E9App.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Debug.h"
#include "E9App.h"

char*		e9App::m_cmdline					= NULL;
char	    e9App::m_path[MAX_PATH]				= "";
HINSTANCE	e9App::m_hinstance					= NULL;
HWND		e9App::m_hwnd						= NULL;
BOOL		e9App::m_active						= FALSE;
BOOL		e9App::m_minimized					= FALSE;
BOOL		e9App::m_windowed					= TRUE;
BOOL		e9App::m_cool						= TRUE;
int			e9App::m_cursor						= 1; //arrow default
HCURSOR		e9App::m_hcursor[E9_CURSOR_MAX]		= { NULL, NULL, NULL, NULL, NULL };

int			e9App::m_frame						= 0;
dword		e9App::m_tick						= 0;
dword		e9App::m_ticklast					= 0;
float		e9App::m_fps						= 0;

void*		e9App::m_param[4]					= {NULL,NULL,NULL,NULL};

e9AppCallback e9App::m_callback[E9_APP_CALLBACKS] = { NULL, NULL, NULL, NULL, NULL, NULL };

void e9App::SetCallback( int idx, e9AppCallback callback )
{
	guard(e9App::SetCallback);
	sassert(0<=idx && idx<E9_APP_CALLBACKS);
	m_callback[idx] = callback;
	unguard();
}

BOOL e9App::Init( HINSTANCE hinstance, char* cmdline )
{
	guard(e9App::Init);
	// init
	m_hinstance	= hinstance;
	m_cmdline = cmdline;
	GetModuleFileName( NULL, m_path, MAX_PATH );
	E9_SetHINSTANCE(hinstance);

	// cursors
	m_hcursor[E9_CURSOR_NONE]	= NULL;
	m_hcursor[E9_CURSOR_ARROW]	= LoadCursor(NULL, IDC_ARROW);
	m_hcursor[E9_CURSOR_WAIT]	= LoadCursor(NULL, IDC_WAIT);
	m_hcursor[E9_CURSOR_HAND]	= LoadCursor(NULL, IDC_HAND);
	m_hcursor[E9_CURSOR_CUSTOM]	= NULL;

	if(!InitWindow()) return FALSE;
	E9_SetHWND(m_hwnd);

	// user callback
	BOOL ok=TRUE;
	if(m_callback[E9_APP_ONINIT]) ok = m_callback[E9_APP_ONINIT]();
	if(!ok) return FALSE; //@TODO: needs DestroyWindow(m_hwnd) ???

	// show window
	SetForegroundWindow( m_hwnd );
	ShowWindow( m_hwnd, TRUE );
	UpdateWindow( m_hwnd );
	SetFocus( m_hwnd );

	return TRUE;
	unguard();
}

void e9App::Done()
{
	guard(e9App::Done);
	if(m_callback[E9_APP_ONDONE]) m_callback[E9_APP_ONDONE]();
	unguard();
}

void e9App::Run()
{
	guard(e9App::Run);
	dlog(LOGAPP, "Main loop start.\n\n" );

	MSG	msg;
	BOOL finished = FALSE;

	while( TRUE )
	{
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( GetMessage( &msg, NULL, 0, 0) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				finished = TRUE; break;
			}
		}

		if( finished ) break;

		if( m_active && !m_minimized )
		{
			UpdateClocks();
			BOOL ok = TRUE;
			if(m_callback[E9_APP_ONRUN]) ok = m_callback[E9_APP_ONRUN]();
			if(!ok) break;
		}
		else
		{
			Sleep(10); // do something good for the operation system
		}

		if(m_cool)
			Sleep(1); // STUPID HARDWARE (cpu cool)
	}

	dlog(LOGAPP, "\nMain loop finished.\n");
	unguard();
}

void e9App::SetStr( int prop, const char* value )
{
	guard(e9App::SetStr);
	switch(prop)
	{
		case E9_APP_NAME:
		{
			if(!m_hwnd) return;
			SetWindowText(m_hwnd,value);
		}
		case E9_APP_ICON:
		{
			if(!m_hwnd) return;
			HICON hIcon = LoadIcon(m_hinstance,value);
			if(hIcon==NULL) return;
			PostMessage(m_hwnd,WM_SETICON,ICON_BIG,(LPARAM)(HICON)hIcon);
			return;
		}
	}
	unguard();
}

const char* e9App::GetStr( int prop )
{
	guard(e9App::GetStr);
	switch(prop)
	{
		case E9_APP_NAME:
		{
			if(!m_hwnd) return NULL;
			static char name[64];
			GetWindowText(m_hwnd,name,64);
			return name;
		}
		case E9_APP_CMDLINE:	return m_cmdline;
		case E9_APP_PATH:		return m_path;
	}
	return NULL;
	unguard();
}

void e9App::SetInt( int prop, int value )
{
	guard(e9App::SetInt);
	switch(prop)
	{
		case E9_APP_WINDOWED:	m_windowed = value; break;
		case E9_APP_CURSOR:		m_cursor = value; break;
		case E9_APP_COOL:		m_cool = value; break;
	}
	unguard();
}

int e9App::GetInt( int prop )
{
	guard(e9App::GetInt);
	switch(prop)
	{
		case E9_APP_ACTIVE:		return m_active;
		case E9_APP_WINDOWED:	return m_windowed;
		case E9_APP_CURSOR:		return m_cursor;
		case E9_APP_COOL:		return m_cool;
		case E9_APP_FRAME:		return m_frame;
		case E9_APP_DELTATIME:	return m_tick-m_ticklast;
		case E9_APP_FPS:		return (int)m_fps;
	}
	return 0;
	unguard();
}

void e9App::SetVoid( int prop, void* value )
{
	guard(e9App::SetVoid);
	switch(prop)
	{
		case E9_APP_PARAM:		m_param[0] = value; break;
		case E9_APP_PARAM+1:	m_param[1] = value; break;
		case E9_APP_PARAM+2:	m_param[2] = value; break;
		case E9_APP_PARAM+3:	m_param[3] = value; break;
	}
	unguard();
}

void* e9App::GetVoid( int prop )
{
	guard(e9App::GetVoid);
	switch(prop)
	{
		case E9_APP_PARAM:		return m_param[0];
		case E9_APP_PARAM+1:	return m_param[1];
		case E9_APP_PARAM+2:	return m_param[2];
		case E9_APP_PARAM+3:	return m_param[3];
	}
	return NULL;
	unguard();
}

void e9App::SetCursor( int cursor )
{
	guard(e9App::SetCursor);
	if(cursor<0 || cursor>=E9_CURSOR_MAX) return;
	::SetCursor(m_hcursor[cursor]);
	m_cursor = cursor;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int e9App::InitWindow()
{
	guard(e9App::InitWindow);
	BOOL ok;
	
	// register window
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hinstance;
	wcex.hIcon			= NULL;	// let the user set the icon later
	wcex.hCursor		= NULL; // LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH); //(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= E9_APP_CLASSNAME;
	wcex.hIconSm		= NULL;	// use small icon from default icon
	ok = RegisterClassEx(&wcex);
	if(!ok) { dlog(LOGERR, "APP: failed to register main window class.\n"); return FALSE; }

	// create window
	int style = (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN ) & ~(WS_MAXIMIZEBOX|WS_SIZEBOX); // WS_POPUP
	int width = 640;
	int height= 480;
	int cx = (sys_desktopwidth()-width) / 2;
	int cy = (sys_desktopheight()-height) / 2;
	RECT rec = {cx,cy,cx+width,cy+height};
	AdjustWindowRectEx( &rec, style, FALSE, 0 );
	m_hwnd = CreateWindowEx(	0, 
								E9_APP_CLASSNAME, 
								"E9APP", 
								style,
								rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top, 
								NULL, NULL, m_hinstance, 
								NULL );
	if(m_hwnd==NULL) { dlog(LOGERR, "APP: failed to create main window.\n"); return FALSE; }

	return TRUE;
	unguard();
}

void e9App::UpdateClocks()
{
	guard(e9App::UpdateClocks);
	m_frame++;

	// real time
	m_ticklast = m_tick;
	m_tick = sys_gettickcount();
	if( m_ticklast==0 ) m_ticklast=m_tick;
	if( m_tick<m_ticklast ) m_ticklast=0; // overflow

	// fps
	static int fpstick = 0;
	static int fpscount = 0;
	if( fpstick==0 ) fpstick=m_tick;
	if( m_tick - fpstick >= 1000 )
	{
		m_fps = (float)fpscount*1000.0f / (float)(m_tick-fpstick);
		fpscount = 0;
		fpstick = m_tick;
	}
	fpscount++;
			
	unguard();
}

LRESULT	CALLBACK e9App::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	guard(e9App::WndProc)
	if(m_callback[E9_APP_ONMSG]) 
	{
		m_param[0] = (void*)(intptr)msg;
		m_param[1] = (void*)(intptr)wParam;
		m_param[2] = (void*)(intptr)lParam;
		m_param[3] = NULL;
		if(m_callback[E9_APP_ONMSG]()) return (LRESULT)m_param[3]; // return result fi user processed it
	}

	int cmd;
	switch(msg)
	{
		case WM_ACTIVATEAPP:
		{
			BOOL active = (wParam!=0);
			BOOL changed = (m_active!=active);
			m_active = active;
			m_minimized = FALSE;
			if(changed) dlog(LOGAPP,"APP: activate %s\n",m_active?"on":"off");
			if(changed && m_callback[E9_APP_ONACTIVATE]) m_callback[E9_APP_ONACTIVATE]();
			break;
		}

		case WM_ACTIVATE:
		{
			BOOL active = !(LOWORD(wParam)==WA_INACTIVE);
			BOOL changed = (m_active!=active);
			m_active = active;
			m_minimized = (HIWORD(wParam)!=0);
			if(changed) dlog(LOGAPP,"APP: activate %s\n",m_active?"on":"off");
			if(changed && m_callback[E9_APP_ONACTIVATE]) m_callback[E9_APP_ONACTIVATE]();
			break;
		}

		case WM_CLOSE:
			if(m_callback[E9_APP_ONCLOSE]) 
			{
				int ret = m_callback[E9_APP_ONCLOSE]();
				if(!ret) return 0;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0); 
			break;

		case WM_PAINT:				
			if(m_windowed && !m_minimized)
				if(m_callback[E9_APP_ONPAINT]) m_callback[E9_APP_ONPAINT]();
			break;

		case WM_SETCURSOR:			
			if(!m_windowed)	{ SetCursor(NULL); return 0; } // @TODO d3ddevice ShowCursor(FALSE);
			::SetCursor(m_hcursor[m_cursor]);
			break;

		case WM_SYSKEYUP:		// ignore ALT key press; was loosing app focus 
			return 0;				

		case WM_SYSCOMMAND:		// ignore sysmenu and others
			cmd = (int)wParam & 0xffff0;
			if(cmd!=SC_RESTORE && cmd!=SC_MINIMIZE && cmd!=SC_CLOSE && cmd!=SC_MOVE )
				return 0;
			break;

		case WM_CONTEXTMENU:	// ignore sysmen when rclick
			return 0;
	}
	return DefWindowProc( hwnd, msg, wParam, lParam );
	unguard()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
