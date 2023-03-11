//////////////////////////////////////////////////////////////////////////////////////////////////
// GUI.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUI.h"
#include "E9App.h"
#include "I9Input.h"

#include "GUIItem.h"
#include "GUIButton.h"
#include "GUIEdit.h"
#include "GUITile.h"
#include "GUIColor.h"
#include "GUIDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUI
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUI* g_gui = NULL;

cGUI::cGUI()
{
	guard(cGUI::cGUI)
	m_mousex		= 0;
	m_mousey		= 0;
	m_font			= NULL;
	m_vm			= NULL;
	m_lastdlg		= -1;
	m_lastitem		= NULL;
	m_capture		= NULL;	
	m_isbusy		= FALSE;
	m_tooltip[0]	=0;
	memset(&m_var,0,sizeof(m_var));
	unguard()
}

cGUI::~cGUI()
{
	guard(cGUI::~cGUI)
	unguard()
}

BOOL cGUI::Init()
{
	guard(cGUI::Init)
	
	GUIInitResources();

	// texture pool
	m_texturepool.Init();

	// fonts
	BOOL ok;
	m_font = snew r9Font(); 
	ok = m_font->Create("Editor\\Font\\font.fnt");
	R9TEXTURE tex = R9_TextureLoad("Editor\\Font\\font.tga");
	m_font->SetTexture(tex);
	m_font->SetSpace(4); // !
	
	// script
	ok = ScriptInit();

	// mouse
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(E9_GetHWND(), &pt); 
	m_mousex = pt.x;//Input()->GetMouseX();
	m_mousey = pt.y;//Input()->GetMouseY();

	return ok;
	unguard()
}
	
void cGUI::Done()
{
	guard(cGUI::Done)
	m_dlg.Done();
	m_capture = NULL;
	ScriptDone();
	if(m_font) 
	{
		R9_TextureDestroy(m_font->GetTexture());
		sdelete(m_font);
	}
	m_texturepool.Done();
	GUIDoneResources();
	unguard()
}

void cGUI::Update()
{
	guard(cGUI::Update)
	int i;
	m_isbusy = FALSE;
	
	if(m_capture!=NULL)
	{
		m_capture->Update();
		m_isbusy = TRUE;
	}
	else
	{
		// serach top most modal
		for(i=m_dlg.Size()-1; i>=0 ;i--)
		{
			if(m_dlg.Get(i)->GetInt(DV_MODAL))
			{
				m_dlg.Get(i)->Update();
				m_isbusy = TRUE;
				break;
			}
		}
		// no modal -> update all
		if(i<0)
		{
			for(i=0;i<m_dlg.Size();i++)
			{
				m_dlg.Get(i)->Update();
			}
		}
	}
	// delete mustclose dialogs
	for(i=0;i<m_dlg.Size();i++)
		if(m_dlg.Get(i)->m_mustclose)
		{
			m_capture = NULL; // clear captrure (colud be int the dying dialog)
			m_dlg.Del(i);
			if(m_lastdlg==i) m_lastdlg=-1; else
			if(m_lastdlg>i) m_lastdlg--; // fix index
			i--;
		}
	unguard()
}

void cGUI::Draw()
{
	guard(cGUI::Draw)
	R9_SetState(R9_STATE_BLEND,R9_BLEND_ALPHA);

	for(int i=0;i<m_dlg.Size();i++)
	{
		// if(m_dlg.Get(i)->GetInt(DV_MODAL))
		//	R9_DrawBar(fRect(0,0,R9_GetWidth(),R9_GetHeight()),0x60000000); 
		m_dlg.Get(i)->Draw();
	}
	
	// tooltip
	if(m_tooltip[0])
	{
		float w,h;
		m_font->GetTextBox(m_tooltip,w,h);
		w+=8; h+=4;
		GUIDrawBar(  m_mousex+16, m_mousey+16, m_mousex+16+(int)w, m_mousey+16+(int)h, 0xffffa000 );
		GUIDrawRect( m_mousex+16, m_mousey+16, m_mousex+16+(int)w, m_mousey+16+(int)h, 0xff000000 );
		GUIDrawText( m_mousex+16+4, m_mousey+16+2, m_mousex+16+(int)w-4, m_mousey+16+(int)h-2, m_tooltip, 0xff000000, GUIALIGN_LEFT|GUIALIGN_TOP );
	}

	// mouse cursor (for full screen tests)
	if(!E9_AppGetInt(E9_APP_WINDOWED))
		R9_DrawLine( fV2(m_mousex, m_mousey), fV2(m_mousex+10, m_mousey+10), 0xffffffff );

	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////////////
void cGUI::SetInt( int idx, int val )
{
	guard(cGUI::SetInt)
	sassert(0<=idx && idx<GV_MAX);
	m_var[idx].m_int=val;
	unguard()
}

int cGUI::GetInt( int idx )
{
	guard(cGUI::GetInt)
	sassert(0<=idx && idx<GV_MAX);
	return m_var[idx].m_int;	
	unguard()
}

void cGUI::SetTxt( int idx, char* text )
{
	guard(cGUI::SetTxt)
	sassert(0<=idx && idx<GV_MAX);
	char* sz = m_var[idx].m_str;
	if(sz) sfree(sz);
	m_var[idx].m_str = sstrdup(text);
	unguard()
}

char* cGUI::GetTxt( int idx )
{
	guard(cGUI::GetTxt)
	sassert(0<=idx && idx<GV_MAX);
	return m_var[idx].m_str;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Input
//////////////////////////////////////////////////////////////////////////////////////////////////
void cGUI::ReadInput()
{
	guard(cGUI::ReadInput)

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(E9_GetHWND(), &pt); 

	m_mousex = pt.x;//Input()->GetMouseX();
	m_mousey = pt.y;//Input()->GetMouseY();

	m_key[GUIKEY_MB1]	= I9_GetKeyValue(I9_MOUSE_B1); 
	m_key[GUIKEY_MB2]	= I9_GetKeyValue(I9_MOUSE_B2);
	m_key[GUIKEY_MB3]	= I9_GetKeyValue(I9_MOUSE_B3);
	m_key[GUIKEY_CTRL]	= I9_GetKeyValue(I9K_LCONTROL) || I9_GetKeyValue(I9K_RCONTROL);
	m_key[GUIKEY_SHIFT]	= I9_GetKeyValue(I9K_LSHIFT) || I9_GetKeyValue(I9K_RSHIFT);
	m_key[GUIKEY_ALT]	= I9_GetKeyValue(I9K_LALT) || I9_GetKeyValue(I9K_RALT);

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Images
//////////////////////////////////////////////////////////////////////////////////////////////////
int cGUI::ImgLoad( char* image )
{
	guard(cGUI::ImgLoad)
	if(image==NULL) return -1;
	return m_texturepool.Load(image);
	unguard()
}

int	cGUI::ImgFind( char* image )
{
	guard(cGUI::ImgFind)
	if(image==NULL) return -1;
	return m_texturepool.Find(image);
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Inheritance
//////////////////////////////////////////////////////////////////////////////////////////////////

int cGUI::DlgFind( int id )
{
	guard(cGUI::DlgFind)
	for(int i=0;i<m_dlg.Size();i++)
		if(m_dlg.Get(i)->GetInt(DV_ID) == id)
			return i;
	return -1;
	unguard()
}

int cGUI::DlgFind( cGUIDlg *dlg )
{
	guard(cGUI::DlgFind)
	for(int i=0;i<m_dlg.Size();i++)
		if(m_dlg.Get(i) == dlg)
			return i;
	return -1;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Script
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cGUI::ScriptInit()
{
	guard(cGUI::ScriptInit)
	m_vm = gs_init();
	gslib_file(m_vm);
	gslib_win(m_vm);
	ScriptRegister();
	return TRUE;
	unguard()
}

void cGUI::ScriptDone()
{
	guard(cGUI::ScriptDone)
	gs_done(m_vm);
	unguard()
}

void cGUI::ScriptDo( char* szcmd )
{
	guard(cGUI::ScriptDo)
	if(szcmd) gs_dostring(m_vm,szcmd);
	unguard()
}

int	cGUI::ScriptCallback( int fid )
{
	guard(cGUI::ScriptCallback)
	gs_runfn(m_vm,fid,0);
	int ret=0;
	if( gs_top(m_vm)>0 )
	{
		ret = gs_toint(m_vm,gs_top(m_vm)-1);
		gs_pop(m_vm);
	}
	return ret;	
	unguard()
}

BOOL cGUI::ScriptCompile( char* file )
{
	guard(cGUI::ScriptCompile)
	if(!file) return TRUE;
	int ret = gs_compilefile(m_vm,file); 
	return (ret==GS_OK);
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// GUI EXPORT
//////////////////////////////////////////////////////////////////////////////////////////////////
#define GS_ERROR( p1,p2,desc )	gs_error( vm, GSE_USER, p1, p2, desc );

#define GET_DLG()																\
	cGUIDlg* dlg=g_gui->DlgGet(g_gui->m_lastdlg);								\
	if(!dlg) { GS_ERROR(0,0,"no selected dialog"); return 0; }
	
#define GET_ITEM()																\
	GET_DLG();																	\
	cGUIItem* item=dlg->ItemGet(g_gui->m_lastitem);								\
	if(!item) { GS_ERROR(0,0,"no selected item"); return 0; }

int gsGuiGetInt( gsVM* vm )
{
	guard(gsGuiGetInt)
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int var = gs_toint(vm, 0);
	if(var<0 || var>=GV_MAX) { GS_ERROR(var,GV_MAX,"invalid gui variable"); GS_RETURNINT(vm,0); }
	gs_pushint( vm, g_gui->GetInt( var ) );
	return 1;
	unguard()
}

int gsGuiSetInt( gsVM* vm )
{
	guard(gsGuiSetInt)
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=GV_MAX) { GS_ERROR(var,GV_MAX,"invalid gui variable"); return 0; }
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_cktype(vm,i,GS_INT))
			g_gui->SetInt(var+i-1, gs_toint(vm,i));
	}
	return 0;
	unguard()
}

int gsGuiGetTxt( gsVM* vm )
{
	guard(gsGuiGetTxt)
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,"");
	int var = gs_toint(vm, 0);
	if(var<0 || var>=GV_MAX) { GS_ERROR(var,GV_MAX,"invalid gui variable"); GS_RETURNSTR(vm,""); }
	gs_pushstr( vm, g_gui->GetTxt( var ) );
	return 1;
	unguard()
}

int gsGuiSetTxt( gsVM* vm )
{
	guard(gsGuiSetTxt)
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=GV_MAX) { GS_ERROR(var,GV_MAX,"invalid gui variable"); return 0; }
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_cktype(vm,i,GS_STR))
			g_gui->SetTxt(var+i-1, gs_tostr(vm,i));
	}
	return 0;
	unguard()
}


int gsDlgFind( gsVM* vm )
{
	guard(gsDlgFind)
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int idx = g_gui->DlgFind(gs_toint(vm,0));
	gs_pushint(vm, idx);
	return 1;
	unguard()
}

int gsItemFind( gsVM* vm )
{
	guard(gsItemFind)
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	GET_DLG();
	int idx = -1;
	if(dlg!=NULL)
		idx = dlg->ItemFind(gs_toint(vm,0));
	gs_pushint(vm, idx);
	return 1;
	unguard()
}

int gsImgLoad( gsVM* vm )
{
	guard(gsImgLoad)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,-1);
	int img = g_gui->ImgLoad(gs_tostr(vm,0));
	gs_pushint(vm,img);
	return 1;
	unguard()
}

int gsImgFind( gsVM* vm )
{
	guard(gsImgFind)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,-1);
	int img = g_gui->ImgFind(gs_tostr(vm,0));
	gs_pushint(vm,img);
	return 1;
	unguard()
}

int gsMouseX( gsVM* vm )
{
	guard(gsMouseX)
	gs_pushint( vm, g_gui->m_mousex); 
	return 1;
	unguard()
}

int gsMouseY( gsVM* vm )
{
	guard(gsMouseY)
	gs_pushint( vm, g_gui->m_mousey); 
	return 1;
	unguard()
}

int gsScrW( gsVM* vm )
{
	guard(gsScrW)
	RECT rc;
	GetClientRect(E9_GetHWND(), &rc); 
	gs_pushint( vm, rc.right - rc.left ); 
	return 1;
	unguard()
}

int gsScrH( gsVM* vm )
{
	guard(gsScrH)
	RECT rc;
	GetClientRect(E9_GetHWND(), &rc); 
	gs_pushint( vm, rc.bottom - rc.top ); 
	return 1;
	unguard()
}

int gsFontH( gsVM* vm )
{
	guard(gsFontH)
	if(!g_gui->m_font) GS_RETURNINT(vm,0);
	gs_pushint( vm, (int)g_gui->m_font->GetSize());
	return 1;
	unguard()
}

int gsTextW( gsVM* vm )
{
	guard(gsTextW)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	if(!g_gui->m_font)			GS_RETURNINT(vm,0);
	float w,h;
	g_gui->m_font->GetTextBox(gs_tostr(vm,0),w,h);
	gs_pushint(vm,(int)w);
	return 1;
	unguard()
}

int gsTextH( gsVM* vm )
{
	guard(gsTextH)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	if(!g_gui->m_font)			GS_RETURNINT(vm,0);
	float w,h;
	g_gui->m_font->GetTextBox(gs_tostr(vm,0),w,h);
	gs_pushint(vm,(int)h);
	return 1;
	unguard()
}

int gsWinDlgOpenFile( gsVM* vm )
{
	guard(gsWinDlgOpenFile)
	if(!gs_cktype(vm,0,GS_REF))		GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT))		GS_RETURNINT(vm,0);
	static char filename[256];
	gsObj* obj = gs_toref(vm,0);

	filename[0]=0;
	if(obj->s!=NULL) strcpy(filename, obj->s);
	
	BOOL ok = WinDlgOpenFile( filename, gs_tostr(vm,1), gs_toint(vm,2) );
	if(ok)
	{
		gso_del(*obj);
		*obj = gso_dup( gsObj(filename,gsstrsize(filename)) );
	}
	
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsWinDlgOpenFolder( gsVM* vm )
{
	guard(gsWinDlgOpenFolder)
	if(!gs_cktype(vm,0,GS_REF))		GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	static char foldername[256];
	gsObj* obj = gs_toref(vm,0);

	foldername[0]=0;
	if(obj->s!=NULL) strcpy(foldername, obj->s);
	
	BOOL ok = WinDlgOpenFolder( foldername );
	if(ok)
	{
		gso_del(*obj);
		*obj = gso_dup( gsObj(foldername,gsstrsize(foldername)) );
	}
	
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsWinDlgOpenColor( gsVM* vm )
{
	guard(gsWinDlgOpenColor)
	if(!gs_cktype(vm,0,GS_REF))		GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_INT))	GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,0);
	dword color = obj->i;
	BOOL ret = WinDlgOpenColor(&color, TRUE);
	if(ret)	obj->i = color;
	gs_pushint(vm,ret);
	return 1;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIDlg EXPORT
//////////////////////////////////////////////////////////////////////////////////////////////////

int gsDlgNew( gsVM* vm )
{
	guard(gsDlgNew)
	char dlgclass[64];
	strcpy(dlgclass,"cGUIDlg");
	if(gs_params(vm)==1 && gs_isstr(vm,0) && gs_tostr(vm,0)!=NULL )
	{
		strncpy(dlgclass,gs_tostr(vm,0),63);
		dlgclass[63]=0;
	}

	cGUIDlg* dlg = (cGUIDlg*)GUICreateClass(dlgclass);
	if(!dlg) { GS_ERROR(0,0,"dialog creation failure"); GS_RETURNINT(vm,-1); }
	
	g_gui->m_lastdlg = g_gui->DlgAdd(dlg); 
	gs_pushint(vm, g_gui->m_lastdlg);
	return 1;
	unguard()
}

int gsDlgSelect( gsVM* vm )
{
	guard(gsDlgSelect)
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=g_gui->DlgCount()) { GS_ERROR(idx,g_gui->DlgCount(),"invalid dialog index"); return 0; }
	g_gui->m_lastdlg = idx;
	g_gui->m_lastitem = -1;
	return 0;
	unguard()
}

int gsDlgGetSelect( gsVM* vm )
{
	guard(gsDlgGetSelect)
	gs_pushint(vm, g_gui->m_lastdlg);
	return 1;
	unguard()
}

int gsDlgClose( gsVM* vm )
{
	guard(gsDlgClose)
	GET_DLG();
	int ret = 0;
	if(gs_params(vm)==1) ret = gs_toint(vm,0);
	dlg->Close(ret);
	return 0;	
	unguard()
}

int gsDlgGetInt( gsVM* vm )
{
	guard(gsDlgGetInt)
	GET_DLG(); 
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int var = gs_toint(vm, 0);
	if(var<0 || var>=DV_MAX) { GS_ERROR(var,DV_MAX,"invalid dialog variable"); GS_RETURNINT(vm,0); }
	gs_pushint( vm, dlg->GetInt( var ) );
	return 1;
	unguard()
}

int gsDlgSetInt( gsVM* vm )
{
	guard(gsDlgSetInt)
	GET_DLG();
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=DV_MAX) { GS_ERROR(var,DV_MAX,"invalid dialog variable"); return 0; }
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_cktype(vm,i,GS_INT))
			dlg->SetInt(var+i-1, gs_toint(vm,i));
	}
	return 0;
	unguard()
}

int gsDlgGetTxt( gsVM* vm )
{
	guard(gsDlgGetTxt)
	GET_DLG(); 
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,"");
	int var = gs_toint(vm, 0);
	if(var<0 || var>=DV_MAX) { GS_ERROR(var,DV_MAX,"invalid dialog variable"); GS_RETURNSTR(vm,""); }
	gs_pushstr( vm, dlg->GetTxt( var ) );
	return 1;
	unguard()
}

int gsDlgSetTxt( gsVM* vm )
{
	guard(gsDlgSetTxt)
	GET_DLG();
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=DV_MAX) { GS_ERROR(var,DV_MAX,"invalid dialog variable"); return 0; }
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_cktype(vm,i,GS_STR))
			dlg->SetTxt(var+i-1, gs_tostr(vm,i));
	}
	return 0;
	unguard()
}

int gsDlgAddKey( gsVM* vm)
{
	guard(gsDlgAddKey)
	GET_DLG();
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_STR)) return 0;
	dlg->AddKey( gs_toint(vm, 0), gs_toint(vm, 1), gs_tostr(vm, 2));
	return 0;
	unguard()
}

int gsDlgCount( gsVM* vm)
{
	guard(gsDlgCount)
	gs_pushint( vm, g_gui->DlgCount());  
	return 1;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIItem EXPORT
//////////////////////////////////////////////////////////////////////////////////////////////////

int gsItemNew( gsVM* vm )
{
	guard(gsItemNew)
	GET_DLG();
	if(dlg->ItemCount()>=MAX_GUIITEMS) { GS_ERROR(0,0,"too many items in a dialog"); GS_RETURNINT(vm,-1); }

	char itemclass[64];
	strcpy(itemclass,"cGUIItem");
	if(gs_params(vm)==1 && gs_isstr(vm,0) && gs_tostr(vm,0)!=NULL )
	{
		strncpy(itemclass,gs_tostr(vm,0),63);
		itemclass[63]=0;
	}

	cGUIItem* item = (cGUIItem*)GUICreateClass(itemclass);
	if(!item) { GS_ERROR(0,0,"item creation failure"); GS_RETURNINT(vm,-1); }

	g_gui->m_lastitem = dlg->ItemAdd(item);
	gs_pushint(vm,g_gui->m_lastitem);
	return 1;
	unguard()
}

int gsItemSelect( gsVM* vm )
{
	guard(gsItemSelect)
	GET_DLG();
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=dlg->ItemCount()) { GS_ERROR(idx,dlg->ItemCount(),"invalid item index"); return 0; }
	sassert(dlg->ItemGet(idx)); // safety
	g_gui->m_lastitem = idx;
	return 0;
	unguard()
}

int gsItemGetSelect( gsVM* vm )
{
	guard(gsItemGetSelect)
	gs_pushint(vm, g_gui->m_lastitem); 
	return 1;
	unguard()
}

int gsItemBuild( gsVM* vm )
{
	guard(gsItemBuild)
	GET_ITEM(); 
	item->Build();
	return 0;
	unguard()
}

int gsItemGetInt( gsVM* vm )
{
	guard(gsItemGetInt)
	GET_ITEM(); 
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IV_MAX) { GS_ERROR(var,IV_MAX,"invalid item variable"); GS_RETURNINT(vm,0); }
	gs_pushint( vm, item->GetInt( var ) );
	return 1;
	unguard()
}

int gsItemSetInt( gsVM* vm )
{
	guard(gsItemSetInt)
	GET_ITEM();
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IV_MAX) { GS_ERROR(var,IV_MAX,"invalid item variable"); return 0; }
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_cktype(vm,i,GS_INT))
			item->SetInt(var+i-1, gs_toint(vm,i));
	}
	return 0;
	unguard()
}

int gsItemGetTxt( gsVM* vm )
{
	guard(gsItemGetTxt)
	GET_ITEM(); 
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,"");
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IV_MAX) { GS_ERROR(var,IV_MAX,"invalid item variable"); GS_RETURNSTR(vm,""); }
	gs_pushstr( vm, item->GetTxt( var ) );
	return 1;
	unguard()
}

int gsItemSetTxt( gsVM* vm )
{
	guard(gsItemSetTxt)
	GET_ITEM();
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IV_MAX) { GS_ERROR(var,IV_MAX,"invalid item variable"); return 0; }
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_cktype(vm,i,GS_STR))
			item->SetTxt(var+i-1, gs_tostr(vm,i));
	}
	return 0;
	unguard()
}

int gsItemCount( gsVM* vm )
{
	guard(gsItemCount)
	GET_DLG();
	gs_pushint( vm, dlg->ItemCount());  
	return 1;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// GUI REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////////
#define GS_REGCONST( c )	gs_regint( m_vm, #c, c );

void cGUI::ScriptRegister()
{
	guard(cGUI::ScriptRegister)

	// GUI
	gs_regfn( m_vm, "DlgFind",			gsDlgFind  );				// int(id) > int(dlg/-1)
	gs_regfn( m_vm, "ItemFind",			gsItemFind );				// int(id) > int(item)
	gs_regfn( m_vm, "ImgLoad",			gsImgLoad );				// str(file) > int(idx/-1)
	gs_regfn( m_vm, "ImgFind",			gsImgFind );				// str(file) > int(idx/-1)
	gs_regfn( m_vm, "GuiGetInt",		gsGuiGetInt );				// int(idx) > int(val/0)
	gs_regfn( m_vm, "GuiSetInt",		gsGuiSetInt );				// int(idx), int, ...
	gs_regfn( m_vm, "GuiGetTxt",		gsGuiGetTxt );				// int(idx) > str(val/"")
	gs_regfn( m_vm, "GuiSetTxt",		gsGuiSetTxt );				// int(idx), str, ...

	// utils
	gs_regfn( m_vm, "MouseX",			gsMouseX );					// > int(mouseposx)
	gs_regfn( m_vm, "MouseY",			gsMouseY );					// > int(mouseposy)
	gs_regfn( m_vm, "ScrW",				gsScrW );					// > int(screen width)
	gs_regfn( m_vm, "ScrH",				gsScrH );					// > int(screen height)
	gs_regfn( m_vm, "FontH",			gsFontH );					// > int(font height/0)
	gs_regfn( m_vm, "TextW",			gsTextW );					// str(text) > int(text width/0)
	gs_regfn( m_vm, "TextH",			gsTextH );					// str(text) > int(text height/0)
	gs_regfn( m_vm, "WinDlgOpenFile",	gsWinDlgOpenFile );			// refstr(file), str(ext), int(mode) > int(1/0)
	gs_regfn( m_vm, "WinDlgOpenFolder",	gsWinDlgOpenFolder );		// refstr(folder) > int(1/0)
	gs_regfn( m_vm, "WinDlgOpenColor",	gsWinDlgOpenColor );		// retint(color) > int(1/0)
		
	// GUIDlg													
	gs_regfn( m_vm, "DlgNew",			gsDlgNew );					// [str(class)="cGUIItem"] > int(dlg/-1)
	gs_regfn( m_vm, "DlgSelect",		gsDlgSelect );				// int(dlg)
	gs_regfn( m_vm, "DlgGetSelect",		gsDlgGetSelect );			// > int(dlg)
	gs_regfn( m_vm, "DlgClose",			gsDlgClose );				// [int(ret)=0]
	gs_regfn( m_vm, "DlgGetInt",		gsDlgGetInt );				// int(var) > int(val/-1)
	gs_regfn( m_vm, "DlgSetInt",		gsDlgSetInt );				// int(var), [int...]
	gs_regfn( m_vm, "DlgGetTxt",		gsDlgGetTxt );				// int(var) > str(val/"")
	gs_regfn( m_vm, "DlgSetTxt",		gsDlgSetTxt );				// int(var), [str...] 
	gs_regfn( m_vm, "DlgAddKey",		gsDlgAddKey );				// int(key), int(flag), str(cmd) 

	gs_regfn( m_vm, "DlgCount",			gsDlgCount );				// > int(dlg)
	
	// GUIItem
	gs_regfn( m_vm, "ItemNew",			gsItemNew );				// [str(class)] > int(item/-1)
	gs_regfn( m_vm, "ItemSelect",		gsItemSelect );				// int(item)
	gs_regfn( m_vm, "ItemGetSelect",	gsItemGetSelect );			// > int(item)
	gs_regfn( m_vm, "ItemBuild",		gsItemBuild );
	gs_regfn( m_vm, "ItemGetInt",		gsItemGetInt );				// int(var) > int(val/0)
	gs_regfn( m_vm, "ItemSetInt",		gsItemSetInt );				// int(var), [int...]
	gs_regfn( m_vm, "ItemGetTxt",		gsItemGetTxt );				// int(var) > str(val/"")
	gs_regfn( m_vm, "ItemSetTxt",		gsItemSetTxt );				// int(var), [str...]

	gs_regfn( m_vm, "ItemCount",		gsItemCount );				// > int(items)

	// aligns
	GS_REGCONST( GUIALIGN_NONE			);
	GS_REGCONST( GUIALIGN_LEFT			);
	GS_REGCONST( GUIALIGN_RIGHT			);
	GS_REGCONST( GUIALIGN_CENTERX		);
	GS_REGCONST( GUIALIGN_TOP			);
	GS_REGCONST( GUIALIGN_BOTTOM		);
	GS_REGCONST( GUIALIGN_CENTERY		);
	GS_REGCONST( GUIALIGN_CENTERXY		);

	// styles
	GS_REGCONST( GUISTYLE_NONE			);
	GS_REGCONST( GUISTYLE_BACKGR		);
	GS_REGCONST( GUISTYLE_GRADIENT		);
	GS_REGCONST( GUISTYLE_BORDER		);
	GS_REGCONST( GUISTYLE_BORDER3D		);
	GS_REGCONST( GUISTYLE_PRESSED		);

	// key flags
	GS_REGCONST( GUIKEYFLAG_NONE		);
	GS_REGCONST( GUIKEYFLAG_SHIFT		);
	GS_REGCONST( GUIKEYFLAG_CTRL		);
	GS_REGCONST( GUIKEYFLAG_ALT			);

	// constant item variables
	GS_REGCONST( IV_ID					);
	GS_REGCONST( IV_STYLE				);
	GS_REGCONST( IV_HIDDEN				);	
	GS_REGCONST( IV_DISABLE				);
	GS_REGCONST( IV_RECT				);
	GS_REGCONST( IV_POS					);
	GS_REGCONST( IV_X					);	
	GS_REGCONST( IV_Y					);	
	GS_REGCONST( IV_POS2				);
	GS_REGCONST( IV_X2					);
	GS_REGCONST( IV_Y2					);
	GS_REGCONST( IV_TXT					);
	GS_REGCONST( IV_TXTALIGN			);
	GS_REGCONST( IV_TXTCOLOR			);
	GS_REGCONST( IV_TXTOFFSET			);
	GS_REGCONST( IV_COLOR				);
	GS_REGCONST( IV_IMG					);
	GS_REGCONST( IV_IMGCOLOR			);
	GS_REGCONST( IV_IMGALIGN			);
	GS_REGCONST( IV_MODE				);
	GS_REGCONST( IV_CMDACTION			);
	GS_REGCONST( IV_CMDACTIONPARAM		);
	GS_REGCONST( IV_VALUE				);
	GS_REGCONST( IV_GROUP				);	
	GS_REGCONST( IV_TOOLTIP				);	
	GS_REGCONST( IV_USER				);	
	GS_REGCONST( IV_MAX					);	

	// constant dialog variables
	GS_REGCONST( DV_ID					);
	GS_REGCONST( DV_HIDDEN				);
	GS_REGCONST( DV_DISABLE				);
	GS_REGCONST( DV_RECT				);
	GS_REGCONST( DV_POS					);
	GS_REGCONST( DV_POS2				);
	GS_REGCONST( DV_X					);
	GS_REGCONST( DV_Y					);
	GS_REGCONST( DV_X2					);
	GS_REGCONST( DV_Y2					);
	GS_REGCONST( DV_MODAL				);
	GS_REGCONST( DV_TESTKEY				);
	GS_REGCONST( DV_CLOSEOUT			);
	GS_REGCONST( DV_CLOSERET			);
	GS_REGCONST( DV_CLOSECMD			);
	GS_REGCONST( DV_USER				);
	GS_REGCONST( DV_MAX					);

	// constant gui variables
	GS_REGCONST( GV_USER				);
	GS_REGCONST( GV_MAX					);

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// replicators
//////////////////////////////////////////////////////////////////////////////////////////////////
#define GUI_REPLICATE(name) if(0==strcmp(classname,#name)) return snew name();

void* GUICreateClass( char* classname )
{
	guard(GUICreateClass);
	GUI_REPLICATE(cGUIItem);
	GUI_REPLICATE(cGUITitle);
	GUI_REPLICATE(cGUIButton);
	GUI_REPLICATE(cGUICheck);
	GUI_REPLICATE(cGUIRadio);
	GUI_REPLICATE(cGUIEdit);
	GUI_REPLICATE(cGUITile);
	GUI_REPLICATE(cGUITileMap);
	GUI_REPLICATE(cGUIColorPick);
	GUI_REPLICATE(cGUIDlg);
	return NULL;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
