//////////////////////////////////////////////////////////////////////////////////////////////////
// Wnd.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WND_H__
#define __WND_H__

#include "E9System.h"
#include "D9Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////
#define	HWNDCLASS_CWND		"HWNDCLASS_CWND"	// classname for cWnd and derived classes
#define	WND_MAXHWND			1024				// lots of windows in management


//////////////////////////////////////////////////////////////////////////////////////////////////
// cWnd
//////////////////////////////////////////////////////////////////////////////////////////////////
class cWnd
{
public:
					cWnd				();
virtual			   ~cWnd				();
	

	virtual	BOOL		CreateHWND	  (	dword		styleex,
										LPCSTR		name,
										dword		style,
										int			x,
										int			y,
										int			w,
										int			h,
										cWnd*	parent,
										HMENU		menu,
										HINSTANCE	hInstance );


// ACCESS
inline	HWND		GetHWND				()									{ return m_hwnd; }
inline	int			GetHWNDID			()									{ return GetWindowLong( m_hwnd, GWL_ID ); }


// WINAPI
virtual LRESULT 	SendMessage			( UINT Msg, WPARAM wParam, LPARAM lParam )	{ return ::SendMessage( m_hwnd, Msg, wParam, lParam ); }
virtual BOOL		PostMessage			( UINT Msg, WPARAM wParam, LPARAM lParam )	{ return ::PostMessage( m_hwnd, Msg, wParam, lParam ); }
virtual	void		EnableWindow		( BOOL bEnable )					{ ::EnableWindow( m_hwnd, bEnable ); }
virtual BOOL		ShowWindow			( int nCmdShow )					{ return ::ShowWindow( m_hwnd, nCmdShow ); }
virtual	BOOL		DestroyWindow		()									{ return ::DestroyWindow(m_hwnd); }
virtual	BOOL		MoveWindow			( int X, int Y, int nWidth, int nHeight, BOOL bRepaint=FALSE ) { return ::MoveWindow( m_hwnd, X, Y, nWidth, nHeight, bRepaint ); }
virtual	void		InvalidateRect		( RECT* rect, BOOL bErase=FALSE )	{ ::InvalidateRect( m_hwnd, rect, bErase ); }
virtual	BOOL		GetWindowRect		( LPRECT lpRect )					{ return ::GetWindowRect( m_hwnd, lpRect ); }
virtual	BOOL		GetClientRect		( LPRECT lpRect )					{ return ::GetClientRect( m_hwnd, lpRect ); }



// WINAPI MESSAGES OVERWRIDES (m_hwnd must be correct attached)
virtual	LRESULT		WndProc				( UINT msg, WPARAM wParam, LPARAM lParam );
										
virtual	LRESULT		OnHWndCreate		( LPCREATESTRUCT lpCreateStruct );
virtual	LRESULT		OnHWndDestroy		();
virtual	LRESULT		OnHWndPaint			( HDC hdc );
virtual LRESULT		OnHWndMove			( int x, int y );
virtual LRESULT		OnHWndSize			( int type, int w, int h );
virtual	LRESULT		OnHWndTimer			( int id, TIMERPROC* tmprc );
virtual	LRESULT		OnHWndLButtonDown	( int flags, int x, int y );
virtual	LRESULT		OnHWndLButtonUp		( int flags, int x, int y );
virtual	LRESULT		OnHWndMouseMove		( int flags, int x, int y );
virtual	LRESULT		OnHWndNCHitTest		( int x, int y );
virtual	LRESULT		OnHWndCommand		( int code, int id, HWND hwnd );
virtual	LRESULT		OnHWndKeyDown		( int keycode, dword flags );
virtual	LRESULT		OnHWndKeyUp			( int keycode, dword flags );
virtual	LRESULT		OnHWndChar			( int keychar, dword flags );
										
// UTILS
inline	void		SetRect				( RECT rect )						{ MoveWindow( rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, FALSE ); }
inline	RECT		GetRect				()									{ RECT r; GetWindowRect(&r); return r; }
inline	int			GetX				()									{ RECT r; GetWindowRect(&r); return r.left; }
inline	int			GetY				()									{ RECT r; GetWindowRect(&r); return r.top; }
inline	int			GetWidth			()									{ RECT r; GetWindowRect(&r); return r.right-r.left; }
inline	int			GetHeight			()									{ RECT r; GetWindowRect(&r); return r.bottom-r.top; }
		RECT		Rect2Screen			( RECT rect );
		RECT		Rect2Client			( RECT rect );
		POINT		Point2Screen		( POINT point );
		POINT		Point2Client		( POINT point );
		BOOL		SetBitmapRgn		( HDC hdc, COLORREF colorkey );	 // use NULL to remove rgn; hdc must have a hbitmap selected
	
protected:
		HWND		m_hwnd;

//////////////////////////////////////////////////////////////////////////////////////////////////
// HWND SPECIAL
//////////////////////////////////////////////////////////////////////////////////////////////////
public:
											
static	int			GetWindowCount	()											{ return m_hwndcount; }
virtual	BOOL		Attach			( HWND hwnd );							
virtual	BOOL		Detach			();										
static	void		SetCWND			( HWND hwnd, cWnd* wnd )					{ SetWindowLong( hwnd, GWL_USERDATA, (LONG)wnd ); }
static	cWnd*		GetCWND			( HWND hwnd )								{ return (cWnd*)GetWindowLong( hwnd, GWL_USERDATA); }
static	cWnd*		GetWindow		( HWND hwnd );

static	LRESULT CALLBACK GlobalWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
static	int			AddHWND			( HWND hwnd );
static	void		DelHWND			( HWND hwnd );
static	int			FindHWND		( HWND hwnd );

static	int			m_hwndcount;
static	HWND		m_hwndlst[WND_MAXHWND];

};


//////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	WndRegisterClass		( LPCTSTR classname, UINT style, HINSTANCE hInstance, HICON hIcon, HCURSOR hCursor, HBRUSH hBrush );
cWnd*	WndNew					( char* classname );			// create a class instance using the replicators
void	WndDel					( cWnd* wnd );					// destroy a wnd (DO NOT CALL FROM INSIDE INSTANCE); wnd pointer is deleted and eventual attached HWND is destroyed
void	WndRun					();								// run a message loop for quick tests

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
