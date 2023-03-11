/////////////////////////////////////////////////////////////////////////////////////////////////
// WUIItem .cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WUIItemEdit.h"
#include "WndDlg.h"
#include "WUIApp.h"

cWUIItemEdit::cWUIItemEdit()
{
	guard(cWUIItemEdit::cWUIItemEdit)
	m_hwnd = NULL;
	unguard()
}

cWUIItemEdit::~cWUIItemEdit()
{
	guard(cWUIItemEdit::~cWUIItemEdit)
	if(m_hwnd) DestroyWindow(m_hwnd);
	unguard()
}

void cWUIItemEdit::SetInt( int idx, int val )
{
	guard(cWUIItemEdit::SetInt)
	cWUIItem::SetInt(idx,val);	

	if( idx==IITXTRES && m_hwnd )
	{
		cResFont* res = WUIResource()->GetFont( GetInt(IITXTRES) );
		if(!res) return;
		HFONT hfont = res->m_hfont;
		if(!hfont) return;
		SendMessage(m_hwnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
	}
	
	unguard()
}

int	cWUIItemEdit::GetInt( int idx )
{
	guard(cWUIItemEdit::GetInt)
	return cWUIItem::GetInt(idx);
	unguard()
}

void cWUIItemEdit::SetText( int idx, char* text )
{
	guard(cWUIItemEdit::SetText)
	if(!m_hwnd) return;
	cWUIItem::SetText(IITEXT,text);
	SetWindowText( m_hwnd, (char*)(m_var[IITEXT]) );
	unguard()
}

char* cWUIItemEdit::GetText( int idx )
{
	guard(cWUIItemEdit::GetText)
	if(!m_hwnd) return NULL;
	int size = GetWindowTextLength(m_hwnd);
	cWUIItem::SetText(IITEXT,NULL);
	char* sz = (char*)smalloc(size+1);
	GetWindowText(m_hwnd,sz,size+1);
	sz[size]=0;
	m_var[IITEXT] = (int)sz;
	return (char*)(m_var[IITEXT]);
	unguard()
}

BOOL cWUIItemEdit::Build()
{
	guard(cWUIItemEdit::Build)
	m_hwnd = CreateWindow(	"EDIT", NULL,
							WS_CHILD | ES_WANTRETURN | ES_AUTOHSCROLL,
							GetInt(IIX), GetInt(IIY),
							GetInt(IIX2)-GetInt(IIX), GetInt(IIY2)-GetInt(IIY), 
							m_dlg->GetHWND(),
							NULL,
							WUIApp()->GetHINSTANCE(),
							NULL
							);
	ShowWindow(m_hwnd,SW_SHOW);
	return (m_hwnd!=NULL);
	unguard()
}
