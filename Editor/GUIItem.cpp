//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIItem.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUIItem.h"
#include "GUI.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIItem
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUIItem::cGUIItem()
{
	guard(cGUIItem::cGUIItem)
	memset(&m_var,0,sizeof(m_var));
	// defaults
	m_var[IV_IMG].m_int = -1;
	m_var[IV_IMG+1].m_int = -1;
	SetInt(IV_COLOR,		GUICOLOR_GUI);
	SetInt(IV_TXTCOLOR,	GUICOLOR_TEXT);
	SetInt(IV_TXTOFFSET, 6 );
	SetInt(IV_IMGCOLOR,	0xffffffff);
	SetInt(IV_IMGALIGN,	GUIALIGN_CENTERXY);
	m_dlg = NULL;
	unguard()
}

cGUIItem::~cGUIItem()
{
	guard(cGUIItem::~cGUIItem)
	sassert(m_dlg==NULL);
	SetTxt(IV_TXT,NULL);		
	SetTxt(IV_CMDACTION,NULL);	
	SetTxt(IV_TOOLTIP,NULL);
	unguard()
}


void cGUIItem::Build()
{
	guard(cGUIItem::Build)
	unguard()
}

void cGUIItem::Update()
{
	guard(cGUIItem::Update)
	RECT rc;
	GetScrRect(rc);
	m_mousein = INRECT( g_gui->m_mousex, g_gui->m_mousey, rc);
	if(m_mousein)
	{
		SetInt(IV_CMDACTIONPARAM,0);
		if(I9_GetKeyDown(I9_MOUSE_B1))	SetInt(IV_CMDACTIONPARAM,1);
		if(I9_GetKeyDown(I9_MOUSE_B2))	SetInt(IV_CMDACTIONPARAM,2);
		if(GetInt(IV_CMDACTIONPARAM))		Action();
	}
	unguard()
}

void cGUIItem::Draw()
{
	guard(cGUIItem::Draw)
	RECT rc; 
	GetScrRect(rc);

	int style = GetInt(IV_STYLE);

	// background
	if( style & GUISTYLE_BACKGR )
		GUIDrawBar( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR) );
	else
	if( style & GUISTYLE_GRADIENT )
		GUIDrawGradient( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR), GetInt(IV_COLOR+1) );
	
	// image
	GUIDrawImg( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_IMG), GetInt(IV_IMGCOLOR), GetInt(IV_IMGALIGN));

	// text
	GUIDrawText( rc.left, rc.top, rc.right, rc.bottom, GetTxt(IV_TXT), GetInt(IV_TXTCOLOR), GetInt(IV_TXTALIGN), GetInt(IV_TXTOFFSET) );
	
	// border
	if( style & GUISTYLE_BORDER )
		GUIDrawRect( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR+2) );
	else
	if( style & GUISTYLE_BORDER3D )
		GUIDrawRect3D( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR+2), style & GUISTYLE_PRESSED );

	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////////////

void cGUIItem::SetInt( int idx, int val )
{
	guard(cGUIItem::SetInt)
	sassert(0<=idx && idx<IV_MAX);
	m_var[idx].m_int=val;
	unguard()
}

int cGUIItem::GetInt( int idx )
{
	guard(cGUIItem::GetInt)
	sassert(0<=idx && idx<IV_MAX);
	return m_var[idx].m_int;	
	unguard()
}

void cGUIItem::SetTxt( int idx, char* text )
{
	guard(cGUIItem::SetTxt)
	sassert(0<=idx && idx<IV_MAX);
	char* sz = m_var[idx].m_str;
	if(sz) sfree(sz);
	m_var[idx].m_str = sstrdup(text);
	unguard()
}

char* cGUIItem::GetTxt( int idx )
{
	guard(cGUIItem::GetTxt)
	sassert(0<=idx && idx<IV_MAX);
	return m_var[idx].m_str;
	unguard()
}

void cGUIItem::SetPoint( int idx, POINT point )
{
	guard(cGUIItem::SetPoint)
	sassert(0<=idx && idx<IV_MAX-1);
	m_var[idx+0].m_int = point.x;
	m_var[idx+1].m_int = point.y;
	unguard()
}

POINT cGUIItem::GetPoint( int idx )
{
	guard(cGUIItem::GetPoint)
	sassert(0<=idx && idx<IV_MAX-1);
	POINT point;
	point.x = m_var[idx+0].m_int;
	point.y = m_var[idx+1].m_int;
	return point;
	unguard()
}

void cGUIItem::SetRect( int idx, RECT rect )
{
	guard(cGUIItem::SetRect)
	sassert(0<=idx && idx<IV_MAX-3);
	m_var[idx+0].m_int = rect.left;
	m_var[idx+1].m_int = rect.top;
	m_var[idx+2].m_int = rect.right;
	m_var[idx+3].m_int = rect.bottom;
	unguard()
}

RECT cGUIItem::GetRect( int idx )
{
	guard(cGUIItem::GetRect)
	sassert(0<=idx && idx<IV_MAX-3);
	RECT rect;
	rect.left = m_var[idx+0].m_int;
	rect.top = m_var[idx+1].m_int;
	rect.right = m_var[idx+2].m_int;
	rect.bottom = m_var[idx+3].m_int;
	return rect;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////////////////////////////////////////////////////

void cGUIItem::GetScrRect( RECT &rc )
{
	guard(cGUIItem::GetScrRect)
	rc = GetRect(IV_RECT);
	cGUIDlg *p = m_dlg;
	if(p!=NULL)
	{
		rc.left	  += p->GetInt(DV_X);
		rc.top	  += p->GetInt(DV_Y);
		rc.right  += p->GetInt(DV_X);
		rc.bottom += p->GetInt(DV_Y);
	}
	unguard()
}


int cGUIItem::SetParent( cGUIDlg* dlg)
{
	guard(cGUIItem::SetParent)
	sassert(m_dlg==NULL); 
	if(dlg!=NULL) return dlg->ItemAdd(this);
	return -1;
	unguard()
}

void cGUIItem::Capture( BOOL on )
{
	guard(cGUIItem::Capture)
	if(on)
	{
		if(g_gui->m_capture==NULL) 
			g_gui->m_capture = this;	
	}
	else
	{
		if(g_gui->m_capture==this) 
			g_gui->m_capture = NULL;
	}
	unguard()
}

BOOL cGUIItem::IsCaptured()
{
	guard(cGUIItem::IsCaptured)
	return (g_gui->m_capture==this);
	unguard()
}


void cGUIItem::Select()	
{
	guard(cGUIItem::Select)
	g_gui->m_lastdlg = g_gui->DlgFind(m_dlg);
	g_gui->m_lastitem = m_dlg->ItemFind(this);
	unguard()
}


void cGUIItem::Action()
{
	guard(cGUIItem::Action)
	char* cmd = GetTxt(IV_CMDACTION);
	if(cmd==NULL || cmd[0]==0) return;
	Select();	
	g_gui->ScriptDo(cmd);
	unguard()
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUITitle
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUITitle::cGUITitle()
{
	guard(cGUITitle::cGUITitle)
	SetInt(IV_VALUE,1);
	m_movex = m_movey = 0;
	unguard()
}

cGUITitle::~cGUITitle()
{
	guard(cGUITitle::~cGUITitle)
	unguard()
}

void cGUITitle::Update()
{
	guard(cGUITitle::Update)
	RECT rc;
	GetScrRect(rc);
	m_mousein = INRECT( g_gui->m_mousex, g_gui->m_mousey, rc);

	if(I9_GetKeyDown(I9_MOUSE_B1))
	{
		if(m_mousein)
		{
			m_movex = g_gui->m_mousex-rc.left;
			m_movey = g_gui->m_mousey-rc.top;
			Capture(TRUE);
		}
	}
	else
	if(!I9_GetKeyValue(I9_MOUSE_B1))
	{
		if(IsCaptured()) 
		{ 
			Capture(FALSE);
		}
	}

	if(IsCaptured() && m_dlg) // move parent dialog
	{
		int x = g_gui->m_mousex;
		int y = g_gui->m_mousey;
		if(x<0) x=0;
		if(x>R9_GetWidth()-1) x=R9_GetWidth()-1;
		if(y<0) y=0;
		if(y>R9_GetHeight()-1) y=R9_GetHeight()-1;
		x -= m_movex;
		y -= m_movey;
		x -= GetInt(IV_X);
		y -= GetInt(IV_Y);
		int w = m_dlg->GetInt(DV_X2)-m_dlg->GetInt(DV_X);
		int h = m_dlg->GetInt(DV_Y2)-m_dlg->GetInt(DV_Y);
		m_dlg->SetInt(DV_X, x);
		m_dlg->SetInt(DV_Y, y);
		m_dlg->SetInt(DV_X2, x+w);
		m_dlg->SetInt(DV_Y2, y+h);
	}
	
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
