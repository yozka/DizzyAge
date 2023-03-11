//////////////////////////////////////////////////////////////////////////////////////////////////
// Wnd.cpp
//
// TODO:
//
//	1. check the PostQuitMessage(0) form OnDestroy !
//	2. Why not Create the window and then attach to it? Why use NC_CREATE message? Maybe to get OnNCCreate from the cWnd instance.
//	   Then, maybe I shoud Attach on NCCreate since I detach on NCDestroy...
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Wnd.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
int		cWnd::m_hwndcount = 0;
HWND	cWnd::m_hwndlst[WND_MAXHWND];

//////////////////////////////////////////////////////////////////////////////////////////////////
// CREATION
//////////////////////////////////////////////////////////////////////////////////////////////////
cWnd::cWnd()
{
	guard(cWnd::cWnd)
	m_hwnd = NULL;
	unguard()
}

cWnd::~cWnd()
{
	guard(cWnd::~cWnd)
	if(m_hwnd==NULL) // still attached
	{
		Detach();
		DestroyWindow(); // cWnd is not attached so it's ok
	}
	unguard()
}


BOOL cWnd::CreateHWND(		dword		styleex,
							LPCSTR		name,
							dword		style,
							int			x,
							int			y,
							int			w,
							int			h,
							cWnd*	parent,
							HMENU		menu,
							HINSTANCE	hInstance )
{
	guard(cWnd::CreateHWND)
	sassert(m_hwnd==NULL);
	HWND hwndparent = NULL;
	if( parent ) hwndparent = parent->GetHWND();
	
	m_hwnd = ::CreateWindowEx( styleex, HWNDCLASS_CWND, name, style, x, y, w, h, hwndparent, menu, hInstance, this );
	if(!m_hwnd) 
	{
		dlog(LOGERR, "WUI: create window failed (%i).\n", GetLastError() );
		return FALSE;
	}

	hwndparent = ::GetParent(m_hwnd);

	return TRUE;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// WINAPI
//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT	cWnd::WndProc( UINT msg, WPARAM wParam, LPARAM lParam )
{
	guard(cWnd::WndProc)

	switch(msg)
	{
		case WM_CREATE:		return OnHWndCreate( (LPCREATESTRUCT)lParam );
		case WM_DESTROY:	return OnHWndDestroy();
		case WM_PAINT:		return OnHWndPaint( (HDC)wParam );
		case WM_MOVE:		return OnHWndMove( (int)(short) LOWORD(lParam), (int)(short) HIWORD(lParam) );
		case WM_SIZE:		return OnHWndSize( wParam, LOWORD(lParam), HIWORD(lParam) );
		case WM_TIMER:		return OnHWndTimer( wParam, (TIMERPROC*)lParam );
		case WM_LBUTTONDOWN:return OnHWndLButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
		case WM_LBUTTONUP:	return OnHWndLButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
		case WM_MOUSEMOVE:	return OnHWndMouseMove( wParam, LOWORD(lParam), HIWORD(lParam) );
		case WM_NCHITTEST:	return OnHWndNCHitTest( LOWORD(lParam), HIWORD(lParam) );
		case WM_COMMAND:	return OnHWndCommand( HIWORD(wParam), LOWORD(wParam), (HWND)lParam );
		case WM_KEYDOWN:	return OnHWndKeyDown( (char)wParam, (dword)wParam );
		case WM_KEYUP:		return OnHWndKeyUp( (char)wParam, (dword)wParam );
		case WM_CHAR:		return OnHWndChar( (char)wParam, (dword)wParam );

		default:			return DefWindowProc( m_hwnd, msg, wParam, lParam ); // childs use cWndHWNDParent::WndProc( msg, wParam, lParam );

	}
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// WINAPI MESSAGES OVERWRIDES
//////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT	cWnd::OnHWndCreate( LPCREATESTRUCT lpCreateStruct )
{
	guard(cWnd::OnHWndCreate)
	return DefWindowProc( m_hwnd, WM_CREATE, 0, (LPARAM)lpCreateStruct );
	unguard()
}

LRESULT	cWnd::OnHWndDestroy()
{
	guard(cWnd::OnHWndDestroy)
	// @OBS: better DO THIS if main app window, not only top window ?!
	HWND hwndparent = ::GetParent(m_hwnd);
	if( hwndparent==NULL ) 
	{
		PostQuitMessage(0); 
	}
	return DefWindowProc( m_hwnd, WM_DESTROY, 0, 0 );
	unguard()
}

LRESULT	cWnd::OnHWndPaint( HDC hdc )
{
	guard(cWnd::OnHWndPaint)
	PAINTSTRUCT wps;
	HBRUSH hbrush = (HBRUSH)(COLOR_WINDOW+1);
	::BeginPaint(m_hwnd,&wps);
	FillRect( wps.hdc, &wps.rcPaint, hbrush ); // clear since we do not provide default brush to window class
	::EndPaint(m_hwnd,&wps);
	return 0; // DefWindowProc( m_hwnd, WM_PAINT, (WPARAM)hdc, 0 );
	unguard()
}

LRESULT cWnd::OnHWndMove( int x, int y )
{
	guard(cWnd::OnHWndMove)
	return DefWindowProc( m_hwnd, WM_MOVE, 0, MAKELPARAM(x,y) );
	unguard()
}

LRESULT cWnd::OnHWndSize( int type, int w, int h )
{
	guard(cWnd::OnHWndSize)
	// OBS: when minimiwed, type=1, w=h=0  Shuld I take care of this resizing ?
	return DefWindowProc( m_hwnd, WM_SIZE, type, MAKELPARAM(w,h) );
	unguard()
}


LRESULT	cWnd::OnHWndTimer( int id, TIMERPROC* tmprc )
{
	guard(cWnd::OnHWndTimer)
	return DefWindowProc( m_hwnd, WM_TIMER, (WPARAM)id, (LPARAM)tmprc );
	unguard()
}

LRESULT	cWnd::OnHWndLButtonDown( int flags, int x, int y )
{
	guard(cWnd::OnLButtonDown)
	return DefWindowProc( m_hwnd, WM_LBUTTONDOWN, (WPARAM)flags, MAKELPARAM(x,y) );
	unguard()
}

LRESULT	cWnd::OnHWndLButtonUp( int flags, int x, int y )
{
	guard(cWnd::OnHWndLButtonUp)
	return DefWindowProc( m_hwnd, WM_LBUTTONUP, (WPARAM)flags, MAKELPARAM(x,y) );
	unguard()
}

LRESULT	cWnd::OnHWndMouseMove( int flags, int x, int y )
{
	guard(cWnd::OnHWndMouseMove)
	return DefWindowProc( m_hwnd, WM_MOUSEMOVE, (WPARAM)flags, MAKELPARAM(x,y) );
	unguard()
}

LRESULT	cWnd::OnHWndNCHitTest( int x, int y )
{
	guard(cWnd::OnHWndNCHitTest)
	return DefWindowProc( m_hwnd, WM_NCHITTEST, 0, MAKELPARAM(x,y) );
	unguard()
}

LRESULT	cWnd::OnHWndCommand( int code, int id, HWND hwnd )
{
	guard(cWnd::OnHWndCommand)
	return DefWindowProc( m_hwnd, WM_COMMAND, MAKEWPARAM(id,code), (LPARAM)hwnd );
	unguard()
}

LRESULT	cWnd::OnHWndKeyDown( int keycode, dword flags )
{
	guard(cWnd::OnHWndKeyDown)
	return DefWindowProc( m_hwnd, WM_KEYDOWN, keycode, flags );
	unguard()
}

LRESULT	cWnd::OnHWndKeyUp( int keycode, dword flags )
{
	guard(cWnd::OnHWndKeyUp)
	return DefWindowProc( m_hwnd, WM_KEYUP, keycode, flags );
	unguard()
}

LRESULT	cWnd::OnHWndChar( int keychar, dword flags )
{
	guard(cWnd::OnHWndChar)
	return DefWindowProc( m_hwnd, WM_CHAR, keychar, flags );
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// HWND SPECIAL
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cWnd::Attach( HWND hwnd )
{
	guard(cWnd::Attach)
	if( m_hwnd!=NULL ) return FALSE;			// must not be attached
	sassert(FindHWND(hwnd)==-1);				// hwnd must not be attached (check map!)

	SetCWND( hwnd, this );
	AddHWND( hwnd );

	return TRUE;
	unguard()
}

BOOL cWnd::Detach()
{
	guard(cWnd::Detach)
	if( m_hwnd==NULL ) return FALSE;			// must be attached
	sassert(GetCWND(m_hwnd)==this);				// to this cWnd (!IMPORTANT!)
	sassert(FindHWND(m_hwnd)!=-1);				// must be managed (safety)

	SetCWND( m_hwnd, NULL );
	DelHWND( m_hwnd );
	m_hwnd = NULL;

	return TRUE;
	unguard()
}

cWnd* cWnd::GetWindow(HWND hwnd)
{
	guard(cWnd::GetWindow)
	if( FindHWND(hwnd)==-1 ) return NULL;
	return GetCWND(hwnd);
	unguard()
}

LRESULT CALLBACK cWnd::GlobalWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	guard(cWnd::GlobalWndProc);

	cWnd* wnd;
	if( msg == WM_NCCREATE )
	{
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		wnd = (cWnd*)lpcs->lpCreateParams;
		if( wnd != NULL )
		{
			SetCWND(hwnd,wnd);
			wnd->m_hwnd = hwnd;
			AddHWND(hwnd);
			return DefWindowProc( hwnd, msg, wParam, lParam );
		}
	}
	else
	if( msg == WM_NCDESTROY ) // DO NOT CALL THIS FROM INSIDE THE INSTANCE (use PostMessage) !!!
	{
		wnd = GetWindow(hwnd);
		wnd->Detach();
		sdelete(wnd);
		return DefWindowProc( hwnd, msg, wParam, lParam );
	}

	wnd = GetWindow(hwnd);
	if( wnd != NULL )
		return wnd->WndProc( msg, wParam, lParam );

	return DefWindowProc( hwnd, msg, wParam, lParam );

	unguard();
}

int	cWnd::AddHWND( HWND hwnd )
{
	guard(cWnd::AddHWND)
	sassert(m_hwndcount<=WND_MAXHWND); // too many windows!
	m_hwndlst[m_hwndcount]=hwnd;
	return m_hwndcount++;
	unguard()
}

void cWnd::DelHWND( HWND hwnd )
{
	guard(cWnd::DelHWND)
	int i=FindHWND( hwnd );
	if(i==-1) return;
	if(i<m_hwndcount-1)
	memcpy( &m_hwndlst[i], &m_hwndlst[i+1], sizeof(HWND)*(m_hwndcount-1-i) );
	m_hwndcount--;
	unguard()
}

int	cWnd::FindHWND( HWND hwnd )
{
	guard(cWnd::FindHWND)
	for(int i=0;i<m_hwndcount;i++)
		if(m_hwndlst[i]==hwnd) return i;
	return -1;
	unguard()
}


RECT cWnd::Rect2Screen( RECT rect )
{
	guard(cWnd::Rect2Screen)
	POINT p1;
	POINT p2;
	p1.x = rect.left;
	p1.y = rect.top;
	p2.x = rect.right;
	p2.y = rect.bottom;
	::ClientToScreen( m_hwnd, &p1 );
	::ClientToScreen( m_hwnd, &p2 );
	RECT r;
	r.left	= p1.x;
	r.top	= p1.y;
	r.right = p2.x;
	r.bottom= p2.y;	
	return r;
	unguard()
}

RECT cWnd::Rect2Client( RECT rect )
{
	guard(cWnd::Rect2Client)
	POINT p1;
	POINT p2;
	p1.x = rect.left;
	p1.y = rect.top;
	p2.x = rect.right;
	p2.y = rect.bottom;
	::ScreenToClient( m_hwnd, &p1 );
	::ScreenToClient( m_hwnd, &p2 );
	RECT r;
	r.left	= p1.x;
	r.top	= p1.y;
	r.right = p2.x;
	r.bottom= p2.y;	
	return r;
	unguard()
}

POINT cWnd::Point2Screen( POINT point )
{
	guard(cWnd::Point2Screen)
	POINT p = point;
	::ClientToScreen(m_hwnd, &p);
	return p;
	unguard()
}

POINT cWnd::Point2Client( POINT point )
{
	guard(cWnd::Point2Client)
	POINT p = point;
	::ScreenToClient(m_hwnd, &p);
	return p;
	unguard()
}

BOOL cWnd::SetBitmapRgn( HDC hdc, COLORREF colorkey )
{
	guard(cWnd::SetBitmapRgn)
	if(hdc==NULL) { return SetWindowRgn(m_hwnd, NULL, TRUE); }
	colorkey &= 0x00ffffff;

	HRGN hRgn = NULL;
	BITMAP bmpInfo;

	HBITMAP hbitmap = (HBITMAP)GetCurrentObject(hdc,OBJ_BITMAP); // @HM needs delete ???
	if(!hbitmap) return FALSE;
	GetObject(hbitmap, sizeof(bmpInfo), &bmpInfo);	// Get info about the bitmap 

	hRgn = CreateRectRgn(0,0,0,0);

	int ret = 0;
	for(int y=0; y<bmpInfo.bmHeight; y++)
	{
		int x = 0;
		while(x<bmpInfo.bmWidth)
		{
			//skip over transparent pixels at start of lines.
			while (x < bmpInfo.bmWidth && (GetPixel(hdc, x, y) & 0x00ffffff) == colorkey) x++;
			int xstart = x;
			if(xstart==bmpInfo.bmWidth) break;
			//now find first non-transparent pixel
			while (x < bmpInfo.bmWidth && (GetPixel(hdc, x, y) & 0x00ffffff) != colorkey) x++;
			//create a temp region on this info
			HRGN hRgnTemp = CreateRectRgn(xstart, y, x, y+1);
			//combine into main region
			ret = CombineRgn(hRgn, hRgn, hRgnTemp, RGN_OR);
			DeleteObject(hRgnTemp);
			if(ret==ERROR) return FALSE;
		}
	}

	ret = SetWindowRgn(m_hwnd, hRgn, TRUE);
	return (ret!=ERROR);
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WndRegisterClass( LPCTSTR classname, UINT style, HINSTANCE hInstance, HICON hIcon, HCURSOR hCursor, HBRUSH hBrush )
{
	guard(WndRegisterClass)
	sassert( classname!=NULL );

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= style;
	wcex.lpfnWndProc	= (WNDPROC)cWnd::GlobalWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hIcon;
	wcex.hCursor		= hCursor;
	wcex.hbrBackground	= hBrush;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classname;
	//wcex.hIconSm		= hIcon;

	return RegisterClassEx(&wcex);
	unguard()
}

cWnd* WndNew( char* classname )
{
	guard(WndNew)
	cWnd* wnd = NULL;
	if(strcmp(classname,"cWnd")==0)
		wnd = (cWnd*)snew cWnd();
	if(!wnd) return NULL;
	return wnd;	
	unguard()
}

void WndDel( cWnd* wnd )
{
	guard(WndDel)
	if(wnd) sdelete(wnd);
	// at this point wnd is no longer valid pointer
	// so do not call this from inside the wnd !
	unguard()
}


void WndRun()
{
	guard(WndRun)
	MSG		msg;
	BOOL	finished = FALSE;
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
	}
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
