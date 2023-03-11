//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIDlg.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUIDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIDlg
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUIDlg::cGUIDlg()
{
	guard(cGUIDlg::cGUIDlg)
	memset(&m_var,0,sizeof(m_var));
	SetInt(DV_TESTKEY, 1);
	m_mustclose = FALSE;
	m_mousein = FALSE;
	unguard()
}

cGUIDlg::~cGUIDlg()
{
	guard(cGUIDlg::~cGUIDlg)
	SetTxt(DV_CLOSECMD,NULL);
	for(int i=0;i<m_item.Size();i++)
		m_item.Get(i)->m_dlg = NULL;
	m_item.Done();
	m_keys.Done();
	unguard()
}

void cGUIDlg::Update()
{
	guard(cGUIDlg::Update)
	// mouse in	
	RECT rc;
	rc = GetRect(DV_RECT);
	m_mousein = INRECT( g_gui->m_mousex, g_gui->m_mousey, rc);

	// update children
	for(int i=0;i<m_item.Size();i++)
	{
		cGUIItem* item = m_item.Get(i);
		if(!item->GetInt(IV_DISABLE))
		{
			item->Update();
			if( item->m_mousein && 
				item->GetInt(IV_HIDDEN)==0 &&
				item->GetInt(IV_DISABLE)==0 &&
				item->GetTxt(IV_TOOLTIP) )
				g_gui->SetTooltip(item->GetTxt(IV_TOOLTIP));
		}
	}

	// test key
	if( (GetInt(DV_TESTKEY) == 1) || 
		(GetInt(DV_TESTKEY) == 2 && m_mousein) )
		TestKey();

	// test click
	if( GetInt(DV_CLOSEOUT) && I9_GetKeyDown(I9_MOUSE_B1) && !m_mousein )
		Close(-1);

	unguard()
}

void cGUIDlg::Draw()
{
	guard(cGUIDlg::Draw)
	for(int i=0;i<m_item.Size();i++)
	{
		if(!m_item.Get(i)->GetInt(IV_HIDDEN))
			m_item.Get(i)->Draw();
	}
	unguard()
}

void cGUIDlg::Close(int ret)
{
	guard(cGUIDlg::Close)
	// select this dialog when we call Close
	g_gui->m_lastdlg = g_gui->DlgFind(this);
	g_gui->m_lastitem = -1;

	SetInt(DV_CLOSERET,ret);
	g_gui->ScriptDo(GetTxt(DV_CLOSECMD)); 
	
	g_gui->m_lastdlg = -1;
	g_gui->m_lastitem = -1;
	m_mustclose = TRUE;
	// from here the dialog should not be used anymore (it will be destroyed asp)
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////////////

void cGUIDlg::SetInt( int idx, int val )
{
	guard(cGUIDlg::SetInt)
	sassert(0<=idx && idx<DV_MAX);
	m_var[idx].m_int=val;
	unguard()
}

int cGUIDlg::GetInt( int idx )
{
	guard(cGUIDlg::GetInt)
	sassert(0<=idx && idx<DV_MAX);
	return m_var[idx].m_int;	
	unguard()
}

void cGUIDlg::SetTxt( int idx, char* text )
{
	guard(cGUIDlg::SetTxt)
	sassert(0<=idx && idx<DV_MAX);
	char* sz = m_var[idx].m_str;
	if(sz) sfree(sz);
	m_var[idx].m_str = sstrdup(text);
	unguard()
}

char* cGUIDlg::GetTxt( int idx )
{
	guard(cGUIDlg::GetTxt)
	sassert(0<=idx && idx<DV_MAX);
	return m_var[idx].m_str;
	unguard()
}

void cGUIDlg::SetPoint( int idx, POINT point )
{
	guard(cGUIDlg::SetPoint)
	sassert(0<=idx && idx<DV_MAX-1);
	m_var[idx+0].m_int=point.x;
	m_var[idx+1].m_int=point.y;
	unguard()
}

POINT cGUIDlg::GetPoint( int idx )
{
	guard(cGUIDlg::GetPoint)
	sassert(0<=idx && idx<DV_MAX-1);
	POINT point;
	point.x = m_var[idx+0].m_int;
	point.y = m_var[idx+1].m_int;
	return point;
	unguard()
}

void cGUIDlg::SetRect( int idx, RECT rect )
{
	guard(cGUIDlg::SetRect)
	sassert(0<=idx && idx<DV_MAX-3);
	m_var[idx+0].m_int = rect.left;
	m_var[idx+1].m_int = rect.top;
	m_var[idx+2].m_int = rect.right;
	m_var[idx+3].m_int = rect.bottom;
	unguard()
}

RECT cGUIDlg::GetRect( int idx )
{
	guard(cGUIDlg::GetRect)
	sassert(0<=idx && idx<DV_MAX-3);
	RECT rect;
	rect.left = m_var[idx+0].m_int;
	rect.top = m_var[idx+1].m_int;
	rect.right = m_var[idx+2].m_int;
	rect.bottom = m_var[idx+3].m_int;
	return rect;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Inheritance
//////////////////////////////////////////////////////////////////////////////////////////////////

int cGUIDlg::ItemFind( int id )
{
	guard(cGUIDlg::ItemFind)
	for(int i=0;i<m_item.Size();i++)
		if(m_item.Get(i)->GetInt(IV_ID) == id)
			return i;
	return -1;
	unguard()
}

int cGUIDlg::ItemFind( cGUIItem* item )
{
	guard(cGUIDlg::ItemFind)
	for(int i=0;i<m_item.Size();i++)
		if(m_item.Get(i) == item)
			return i;
	return -1;
	unguard()
}

void cGUIDlg::AddKey( int key, byte flags, char* cmd )
{
	guard(cGUIDlg::AddKey)
	sassert(cmd!=NULL);
	tDlgKey *t	= snew tDlgKey;
	t->m_key	= key;
	t->m_flags	= flags;
	t->m_cmd	= sstrdup(cmd);
	m_keys.Add(t);
	unguard()
}

void cGUIDlg::TestKey()
{
	guard(cGUIDlg::TestKey)

	for(int i=0;i<m_keys.Size();i++)
	{
		if(I9_GetKeyDown(m_keys.Get(i)->m_key))
		{
			int keyflags = m_keys.Get(i)->m_flags;
			int flags = 0;
			if( I9_GetKeyValue(I9K_LSHIFT) || I9_GetKeyValue(I9K_RSHIFT) )		flags |= GUIKEYFLAG_SHIFT;
			if( I9_GetKeyValue(I9K_LCONTROL) || I9_GetKeyValue(I9K_RCONTROL) )	flags |= GUIKEYFLAG_CTRL;
			if( I9_GetKeyValue(I9K_LALT) || I9_GetKeyValue(I9K_RALT) )			flags |= GUIKEYFLAG_ALT;

			if( keyflags == flags ) // flags match exactly
			{
				// select this dialog when we call OnKey command
				g_gui->m_lastdlg = g_gui->DlgFind(this);
				g_gui->m_lastitem = -1;
				g_gui->ScriptDo(m_keys.Get(i)->m_cmd);
				return;
			}
		}
	}
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
