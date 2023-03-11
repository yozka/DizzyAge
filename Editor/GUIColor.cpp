//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIColor.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUIColor.h"
#include "GUI.h"
#include "E9App.h"
#include "R9ImgLoader.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIColorPick
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUIColorPick::cGUIColorPick()
{
	guard(cGUIColorPick::cGUIColorPick)
	unguard()
}

cGUIColorPick::~cGUIColorPick()
{
	guard(cGUIColorPick::~cGUIColorPick)
	SetTxt(IV_TXT,NULL);
	unguard()
}

void cGUIColorPick::Update()
{
	guard(cGUIColorPick::Update)
	RECT rc;
	GetScrRect(rc);
	m_mousein = INRECT( g_gui->m_mousex, g_gui->m_mousey, rc);
	int mx = g_gui->m_mousex - rc.left;
	int my = g_gui->m_mousey - rc.top;
	
	if(m_mousein && R9_ImgIsValid(&m_img))
	{
		float x = ((float)mx / (rc.right-rc.left))*m_img.m_width;
		float y = ((float)my / (rc.bottom-rc.top))*m_img.m_height;
		dword color	= R9_ImgGetColor(&m_img,(int)x,(int)y);
		SetInt(IV_COLOR,color);

		if(I9_GetKeyUp(I9_MOUSE_B1))
		{
			SetInt(IV_CMDACTIONPARAM,1);
			Action();
		}
		else
		if(I9_GetKeyUp(I9_MOUSE_B2))
		{
			SetInt(IV_CMDACTIONPARAM,2);
			Action();
		}
		else
		{
			char sz[32];
			sprintf(sz,"%06X",color & 0x00ffffff);
			g_gui->SetTooltip(sz);
		}
	}
	
	unguard()
}

void cGUIColorPick::Draw()
{
	guard(cGUIColorPick::Draw)
	return; // don't draw anything
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////////////

void cGUIColorPick::SetTxt( int idx, char* text )
{
	guard(cGUIColorPick::SetTxt)
	if(idx==IV_TXT)
	{
		char* txt = m_var[idx].m_str;
		if(txt!=NULL)	R9_ImgDestroy(&m_img);
		cGUIItem::SetTxt(idx,text);
		txt = m_var[idx].m_str;
		if(txt!=NULL)	R9_ImgLoadFile(txt,&m_img);
		return;
	}
	cGUIItem::SetTxt(idx,text);
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
