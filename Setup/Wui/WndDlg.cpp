//////////////////////////////////////////////////////////////////////////////////////////////////
// WndDlg.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WndDlg.h"
#include "WUIApp.h"
#include "WUIScript.h"
#include "E9Math.h"
#include "commctrl.h"

HDC cWndDlg::m_crthdc = NULL;
//int	cWndDlg::m_endmodal = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
// CREATION
//////////////////////////////////////////////////////////////////////////////////////////////////
cWndDlg::cWndDlg()
{
	guard(cWndDlg::cWndDlg)
	m_bkdc = NULL;
	m_itemcmdtext = NULL;
	m_itemcount = 0;
	memset(&m_item,0,MAX_WUIITEMS*sizeof(cWUIItem*));
	m_capture = CAPTUREMODE_NONE;
	m_capturepos.x = 0;
	m_capturepos.y = 0;
	m_hwndtip = NULL;
	unguard();
}

cWndDlg::~cWndDlg()
{
	guard(cWndDlg::~cWndDlg)
	PageClear();
	if(HasBKDC()) DestroyBKDC();
	unguard()
}


BOOL cWndDlg::CreateDlg(	char* name,
							dword	style,
							int		x,
							int		y,
							int		w,
							int		h,
							cWnd*	parent )
{
	guard(cWndDlg::CreateDlg)
	if(!cWnd::CreateHWND( 0, name, style, x, y, w, h, parent, NULL, WUIApp()->GetHINSTANCE() )) return FALSE;
	CreateBKDC();
	m_hwndtip = TooltipCreate(GetHWND(),WUIApp()->GetHINSTANCE());
	return TRUE;
	unguard()
}

void cWndDlg::CreateBKDC()
{
	guard(cWndDlg::CreateBKDC)
	if(!m_hwnd) return;
	if(m_bkdc) return;

	m_bkdc = CreateCompatibleDC( NULL );
	if(!m_bkdc) return;

	HBITMAP hbitmap = CreateCompatibleBitmap( ::GetDC(NULL), GetWidth(), GetHeight() );
	if(!hbitmap) { DeleteDC(m_bkdc); m_bkdc=NULL; dlog(LOGERR,"WUI: error creating hbitmap for BKDC \n");return; }
	SelectObject(m_bkdc,hbitmap);
	unguard()
}

void cWndDlg::DestroyBKDC()
{
	guard(cWndDlg::DestroyBKDC)
	if(!m_bkdc) return;
	HBITMAP hbitmap = (HBITMAP)GetCurrentObject(m_bkdc,OBJ_BITMAP);
	DeleteDC(m_bkdc); m_bkdc = NULL;
	DeleteObject(hbitmap);
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE & DRAW
//////////////////////////////////////////////////////////////////////////////////////////////////
void cWndDlg::Update( float dtime )
{
	guard(cWndDlg::Update)
	unguard()
}

void cWndDlg::Draw( HDC hdc )
{
	guard(cWndDlg::Draw)
	m_crthdc = hdc;

	for(int i=0;i<m_itemcount;i++)
		m_item[i]->Draw();

	m_crthdc = NULL;
	unguard()
}

/*
int	cWndDlg::DoModal()
{
	guard(cWndDlg::DoModal)
	
	m_endmodal = 0;

	while(!m_endmodal)
	{
		MSG msg;
		if(App()->IsActive())
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
			{
				if (msg.message == WM_QUIT)	break;
//				if (IsDialogMessage(m_hwnd, &msg)) continue;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(GetMessage( &msg, NULL, 0, 0 ))
			{ 
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
				if(App()->IsActive()) continue;
			}
			if(msg.message == WM_QUIT) break;
		}

	};

	return m_endmodal;

	unguard()
}

void cWndDlg::EndModal( int code )
{
	guard(cWndDlg::EndModal)
	m_endmodal = code;
	::SendMessage(NULL,0,0,0); // notify
	unguard()
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
// WINAPI MESSAGES OVERWRIDES
//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT	cWndDlg::WndProc( UINT msg, WPARAM wParam, LPARAM lParam )
{
	guard(cWndDlg::WndProc)
	BOOL active=TRUE;

	if(msg==WM_ACTIVATEAPP)
	{
		active = (wParam!=0);
	}
	else
	if(msg==WM_ACTIVATE)
	{
		active = !(LOWORD(wParam)==WA_INACTIVE);
	}
	else
	if(msg==WM_CTLCOLOREDIT)
	{
		if ( HIWORD(lParam) == CTLCOLOR_EDIT ) 
		{
			::SetBkColor((HDC)wParam, RGB(128,128,128));
			::SetTextColor((HDC)wParam, RGB(255, 255, 255) );
            return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		else
			return (LRESULT)GetStockObject(WHITE_BRUSH);
	}

	if(!active)
	{
		if(m_capture!=CAPTUREMODE_NONE)
		{
			if(0<=m_capture && m_capture<MAX_WUIITEMS)
			{
				m_item[m_capture]->Notify( WUIMSG_ACTIVATE, 0, 0 );
			}
			SetCapture(CAPTUREMODE_NONE);
			SetCursorIdx(CURSOR_ARROW);
		}
	}

	return cWnd::WndProc( msg, wParam, lParam );
	unguard()
}

LRESULT	cWndDlg::OnHWndPaint( HDC hdc )
{
	guard(cWndDlg::OnHWndPaint)
	HDC thdc = hdc;
	PAINTSTRUCT wps;
	::BeginPaint(m_hwnd,&wps);

	if(HasBKDC()) 
	{
		Draw( m_bkdc );

		BitBlt( wps.hdc, 
				wps.rcPaint.left, wps.rcPaint.top, wps.rcPaint.right - wps.rcPaint.left, wps.rcPaint.bottom - wps.rcPaint.top,
				GetBKDC(), wps.rcPaint.left, wps.rcPaint.top, 
				SRCCOPY );
	}
	else
	{
		Draw( wps.hdc );
	}
	
	::EndPaint(m_hwnd,&wps);
	return 0;
	unguard()
}

LRESULT cWndDlg::OnHWndSize( int type, int w, int h )
{
	guard(cWndDlg::OnHWndSize)
	// OBS: when minimiwed, type=1, w=h=0  Shuld I take care of this resizing ?
	if(HasBKDC())
	{
		DestroyBKDC();
		CreateBKDC();
	}
	return 0; // DefWindowProc( m_hwnd, WM_SIZE, type, MAKELPARAM(w,h) );
	unguard()
}

LRESULT	cWndDlg::OnHWndLButtonDown( int flags, int x, int y )
{
	guard(cWndDlg::OnHWndLButtonDown)
	if(0<=m_capture && m_capture<MAX_WUIITEMS)
	{
		m_item[m_capture]->Notify(WUIMSG_LBUTTONDOWN, x, y );
		if(m_itemcmdtext) 
		{
			WUIScriptDo(m_itemcmdtext);
			ItemSetCmdText(NULL);
		}
		return 0;
	}
	else
	if(m_capture==CAPTUREMODE_NONE) // click on page
	{
		SetCapture(CAPTUREMODE_PAGE);
		SetCursorIdx(CURSOR_HAND);
	}

	return cWnd::OnHWndLButtonDown( flags, x, y );
	unguard()
}

LRESULT	cWndDlg::OnHWndLButtonUp( int flags, int x, int y )
{
	guard(cWndDlg::OnHWndLButtonUp)
	if(m_capture==CAPTUREMODE_PAGE)
	{
		SetCapture(CAPTUREMODE_NONE);
		SetCursorIdx(CURSOR_ARROW);
	}
	else
	if(0<=m_capture && m_capture<MAX_WUIITEMS && m_item[m_capture]->GetInt(IIBUTPRESSED))
	{
		m_item[m_capture]->Notify(WUIMSG_LBUTTONUP, x, y );
		if(m_itemcmdtext) 
		{
			WUIScriptDo(m_itemcmdtext);
			ItemSetCmdText(NULL);
		}
		SetCapture(CAPTUREMODE_NONE);
		SetCursorIdx(CURSOR_ARROW);
		UpdateCapture(x,y);
	}
	return 0;
	unguard()
}

LRESULT cWndDlg::OnHWndMouseMove( int flags, int x, int y )
{
	guard(cWndDlg::OnHWndMouseMove)
	if(m_capture==CAPTUREMODE_PAGE)
	{
		POINT p;
		GetCursorPos(&p);
		p.x -= m_capturepos.x;
		p.y -= m_capturepos.y;
		MoveWindow( p.x, p.y, GetWidth(), GetHeight(), TRUE );
		return 0;
	}
	else
	{
		cWUIItem* item=NULL;
		// if captured item is pressed then do not change capture
		if(0<=m_capture && m_capture<MAX_WUIITEMS)
		{
			item = m_item[m_capture];
			iRect rect = item->GetRect(IIRECT);
			if(rect.IsInside(iV2(x,y)))
				SetCursorIdx(CURSOR_FINGER);
			else
				SetCursorIdx(CURSOR_ARROW);
	
			if( (item->GetInt(IIBUTCHECK) || !item->GetInt(IIBUTPRESSED) ) && !INRECT(x,y,rect))
				SetCapture(CAPTUREMODE_NONE);

			return 0;
		}
		else
		{
			UpdateCapture(x,y);
			return 0;
		}
	}

	return cWnd::OnHWndMouseMove( flags, x, y );
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// OTHERS
//////////////////////////////////////////////////////////////////////////////////////////////////
void cWndDlg::UpdateCapture( int x, int y )
{
	guard(cWndDlg::UpdateCapture)
	cWUIItem* item=NULL;
	if(m_capture==CAPTUREMODE_NONE)
	{
		// check current item
		int i;
		for(i=0;i<m_itemcount;i++)
		{
			item = m_item[i];
			if(item->GetInt(IIINTERACTIVE)==0) continue;
			RECT rect = item->GetRect(IIRECT);
			if(!INRECT(x,y,rect)) continue;
			break;
		}
		
		if(i<m_itemcount) // over an iteractive item
		{
			SetCapture(i);
			SetCursorIdx(CURSOR_FINGER);
		}
	}
	

	unguard()
}

void cWndDlg::PageClear()
{
	guard(cWndDlg::PageClear)
	int i;
	for(i=0;i<m_itemcount;i++) sdelete(m_item[i]);
	m_itemcount = 0;
	SetCapture(CAPTUREMODE_NONE);
	ItemSetCmdText(NULL);
	TooltipClear(m_hwndtip);
	unguard()
}

void cWndDlg::SetCapture(int mode)
{
	guard(cWndDlg::SetCapture)
	if(mode==-1)
		::ReleaseCapture();
	else
	{
		::SetCapture(m_hwnd);
		GetCursorPos(&m_capturepos);
		m_capturepos.x -= GetX();
		m_capturepos.y -= GetY();
	}
	m_capture = mode;
	unguard()
}

void cWndDlg::SetCursorIdx( int idx )
{
	guard(cWndDlg::SetCursorIdx)
	if(idx<0 || idx>=MAX_CURSORS || WUIResource()->GetCursor(idx)==NULL) return;
	::SetCursor( WUIResource()->GetCursor(idx) );
	unguard()
}

BOOL cWndDlg::DlgSetRgn( int res, dword colorkey )
{
	guard(cWndDlg::DlgSetRgn)
	if(res==-1) return SetBitmapRgn(NULL,colorkey); // loose mask

	// find bitmap
	cResBitmap* resbmp = WUIResource()->GetBitmap( res );
	if(!resbmp || !resbmp || !resbmp->m_handle) return FALSE;
	return SetBitmapRgn(resbmp->m_hdc, colorkey);
	unguard()
}

HWND cWndDlg::TooltipCreate(HWND hwndowner, HINSTANCE hinst)
{
	guard(cWndDlg::TooltipCreate);
	HWND hwndtip = CreateWindowEx(WS_EX_TOOLWINDOW, TOOLTIPS_CLASS, NULL, 
								  TTS_ALWAYSTIP|WS_CLIPSIBLINGS|WS_OVERLAPPED|WS_POPUP,	
								  0,0,0,0,GetHWND(),NULL,hinst,NULL);
	::SendMessage(hwndtip,TTM_SETMAXTIPWIDTH,0,325);
	::SendMessage(hwndtip,TTM_ACTIVATE,(WPARAM)(BOOL)FALSE,0);
	return hwndtip;
	unguard();
}

BOOL cWndDlg::TooltipAdd(HWND hwndtip, HWND hwndowner, HWND hwndid, char* text)
{
	guard(cWndDlg::TooltipAdd);
	TOOLINFO tInfo;
	ZeroMemory(&tInfo,sizeof(TOOLINFO));
	tInfo.cbSize = sizeof(TOOLINFO);
	tInfo.hwnd = hwndowner;
	tInfo.uFlags = TTF_IDISHWND|TTF_SUBCLASS;
	tInfo.uId = (UINT_PTR)hwndid;
	tInfo.lpszText = text;
	if(::SendMessage(hwndtip,TTM_ADDTOOL,(WPARAM)0,(LPARAM)&tInfo)==FALSE)
		return FALSE;
	::SendMessage(hwndtip,TTM_ACTIVATE,(WPARAM)(BOOL)TRUE,0);
	return TRUE;
	unguard();
}

BOOL cWndDlg::TooltipAdd(HWND hwndtip, HWND hwndowner, RECT rect, char* text)
{
	guard(cWndDlg::TooltipAdd);
	TOOLINFO tInfo;
	ZeroMemory(&tInfo,sizeof(TOOLINFO));
	tInfo.cbSize = sizeof(TOOLINFO);
	tInfo.hwnd = hwndowner;
	tInfo.uFlags = TTF_SUBCLASS;
	tInfo.rect = rect;
	tInfo.lpszText = text;
	if(::SendMessage(hwndtip,TTM_ADDTOOL,(WPARAM)0,(LPARAM)&tInfo)==FALSE)
		return FALSE;
	::SendMessage(hwndtip,TTM_ACTIVATE,(WPARAM)(BOOL)TRUE,0);
	return TRUE;
	unguard();
}

void cWndDlg::TooltipClear(HWND hwndtip)
{
	guard(cWndDlg::TooltipClear);
	while(TRUE)
	{
		TOOLINFO tInfo;
		ZeroMemory(&tInfo,sizeof(TOOLINFO));
		tInfo.cbSize = sizeof(TOOLINFO);
		if(::SendMessage(hwndtip,TTM_ENUMTOOLS,(WPARAM)0,(LPARAM)&tInfo))
			::SendMessage(hwndtip,TTM_DELTOOL,0,(LPARAM)&tInfo);
		else
			break;
	}
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////////////////////////
cWndDlg* DlgNew()
{
	guard(DlgNew)
	cWnd* wnd = (cWnd*)snew cWndDlg();
	if(!wnd) return NULL;
	return (cWndDlg*)wnd;	
	unguard()
}

cWndDlg* DlgFind( HWND hwnd )
{
	guard(DlgFind)
	cWnd* wnd = cWnd::GetWindow(hwnd);
	if(!wnd) return NULL;
	return (cWndDlg*)wnd;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
