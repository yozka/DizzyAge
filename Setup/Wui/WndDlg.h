//////////////////////////////////////////////////////////////////////////////////////////////////
// WndDlg.h
// TODO:
// 1. implement modal mechanism
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WNDDLG_H__
#define __WNDDLG_H__

#include "Wnd.h"
#include "WUIItem.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_WUIITEMS		128

#define WUIMSG_ACTIVATE		0
#define WUIMSG_LBUTTONDOWN	1
#define WUIMSG_LBUTTONUP	2

#define CAPTUREMODE_NONE		-1
#define CAPTUREMODE_PAGE		MAX_WUIITEMS

//////////////////////////////////////////////////////////////////////////////////////////////////
// cWndDlg
//////////////////////////////////////////////////////////////////////////////////////////////////
class cWndDlg : public cWnd
{
public:
					cWndDlg				();
virtual			   ~cWndDlg				();

virtual	BOOL		CreateDlg			( char*	name,
										dword	style,
										int		x,
										int		y,
										int		w,
										int		h,
										cWnd*	parent );

		void		CreateBKDC			();
		void		DestroyBKDC			();

// ACCESS
inline	HDC			GetBKDC				()									{ return m_bkdc; }
inline	BOOL		HasBKDC				()									{ return (m_bkdc!=NULL); }

// UPDATE & DRAW
virtual void		Update				( float dtime );					// not used :(
virtual	void		Draw				( HDC hdc );
//static	int		DoModal				();									// execute dialog modal
//static	void	EndModal			( int code );						// tell to end modal


// WINAPI MESSAGES OVERWRIDES
virtual	LRESULT		WndProc				( UINT msg, WPARAM wParam, LPARAM lParam );
virtual	LRESULT		OnHWndPaint			( HDC hdc );
virtual LRESULT		OnHWndSize			( int type, int w, int h );
virtual	LRESULT		OnHWndLButtonDown	( int flags, int x, int y );
virtual	LRESULT		OnHWndLButtonUp		( int flags, int x, int y );
virtual	LRESULT		OnHWndMouseMove		( int flags, int x, int y );


// OTHERS
		void		PageClear			();									// clear page and items
		BOOL		DlgSetRgn			( int res, dword colorkey );		// set a window mask using a bitmap resource; use NULL to remove mask
		void		SetCapture			( int mode );						// set or release capture; mode = none, page or item index
inline	void		ItemSetCmdText		( char* cmdtext )					{ if(m_itemcmdtext) sfree(m_itemcmdtext); m_itemcmdtext = sstrdup(cmdtext); }
static	void		SetCursorIdx		( int idx );						// set cursor from resources if available
		void		UpdateCapture		( int x, int y );					// update (refresh) capture; x,y = mouse coordonates in client

		HWND		TooltipCreate		(HWND hwndowner, HINSTANCE hinst);
		BOOL		TooltipAdd			(HWND hwndtip, HWND hwndowner, HWND hwndid, char* text);
		BOOL		TooltipAdd			(HWND hwndtip, HWND hwndowner, RECT rect, char* text);
		void		TooltipClear		(HWND hwndtip);
public:		
		HDC			m_bkdc;													// back hdc for smood draw
static	HDC			m_crthdc;												// current hdc set by dlg at draw time

		cWUIItem*	m_item[MAX_WUIITEMS];									// list of items
		int			m_itemcount;											// items count
		int			m_capture;												// captured mode (like pressed buttons or moving window); -1=none
		POINT		m_capturepos;											// cursor position when capture; relative to top-left window corner
	
		HWND		m_hwndtip;												// tooltip hwnd

protected:
		char*		m_itemcmdtext;											// itemcmdtext
//static	int		m_endmodal;
};


//////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////////////////////
cWndDlg*	DlgNew						();									// create a dlg instance
cWndDlg*	DlgFind						( HWND hwnd );						// search for a dlg with a specified HWND

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
