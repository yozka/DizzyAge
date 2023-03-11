//////////////////////////////////////////////////////////////////////////////////////////////////
// WUIScript.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WUIApp.h"
#include "WUIScript.h"
#include "WndDlg.h"
#include "WUIItemEdit.h"
#include "shlobj.h"
#include "gslib_file.h"
#include "gslib_win.h"

gsVM*	g_vm = NULL;
void	WUIScriptRegister();

//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WUIScriptInit()
{
	guard(WUIScriptInit)
	g_vm = gs_init();
	gslib_file(g_vm);
	gslib_win(g_vm);
	WUIScriptRegister();
	return TRUE;
	unguard()
}

void WUIScriptDone()
{
	guard(WUIScriptDone)
	gs_done(g_vm);
	unguard()
}

BOOL WUIScriptLoad( char* script )
{
	guard(WUIScriptLoad)
	int ret;
	sassert(script && script[0]);
	ret = gs_compilefile(g_vm,script);
	return (ret==GS_OK);
	unguard()
}

void WUIScriptDo( char* szcmd )
{
	guard(WUIScriptDo)
	if(szcmd) gs_dostring(g_vm,szcmd);
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////
cWndDlg* g_lastdlg=NULL;
int	g_lastitem=-1;

#define GS_ERROR( p1,p2,desc )	gs_error( vm, GSE_USER, p1, p2, desc );

#define GET_DLG()																\
	cWndDlg* dlg=g_lastdlg;														\
	if(!dlg) { GS_ERROR(0,0,"no selected dialog"); return 0; }					
	
#define GET_ITEM()																\
	if(g_lastitem<0 || g_lastitem>=dlg->m_itemcount) { GS_ERROR(0,0,"no selected item"); return 0; } \
	cWUIItem* item = dlg->m_item[g_lastitem];									


int gsResBitmap( gsVM* vm )
{
	guard(gsResBitmap)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_STR))	GS_RETURNINT(vm,-1);
	int idx = WUIResource()->LoadBitmap( gs_tostr(vm,0) );
	gs_pushint(vm,idx);
	return 1;
	unguard()
}

int gsResFont( gsVM* vm )
{
	guard(gsResFont)
	if(!gs_ckparams(vm,4))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,1,GS_INT))	GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,2,GS_INT))	GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,3,GS_INT))	GS_RETURNINT(vm,-1);
	int idx = WUIResource()->CreateFont( gs_tostr(vm,0), gs_toint(vm,1), gs_toint(vm,2), gs_toint(vm,3) );
	gs_pushint(vm,idx);
	return 1;
	unguard()
}

int gsResDelete( gsVM* vm )
{
	guard(gsResDelete)
	WUIResource()->Unload( gs_toint(vm,0) );
	return 0;
	unguard()
}

int gsDlgOpen( gsVM* vm )
{
	guard(gsDlgOpen)
	if(!gs_ckparamsmin(vm,6)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,4,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,5,GS_INT))	GS_RETURNINT(vm,0);

	g_lastdlg = NULL;
	g_lastitem = -1;

	cWndDlg* dlg = DlgNew(); sassert(dlg); // safety

	cWndDlg* dlgparent = NULL;
	if(gs_params(vm)==7 && gs_isptr(vm,6)) // if parent hwnd is specified
	{
		dlgparent = DlgFind((HWND)gs_toptr(vm,6));
	}
	
	int x = gs_toint( vm, 2 );
	int y = gs_toint( vm, 3 );
	int w = gs_toint( vm, 4 );
	int h = gs_toint( vm, 5 );
	if(x==-1) x = GetSystemMetrics(SM_CXSCREEN)/2-w/2;
	if(y==-1) y = GetSystemMetrics(SM_CYSCREEN)/2-h/2;

	if(!dlg->CreateDlg(	gs_tostr( vm, 0 ), // name
						gs_toint( vm, 1 ), // style
						x, y, w, h,
						dlgparent ))
	{
		sdelete(dlg);
		GS_ERROR(0,0,"dialog creation failure");
		GS_RETURNINT(vm,0);
	}

	// dlg->ShowWindow(SW_SHOW);
	// dlg->EnableWindow(TRUE);


	if(WUIApp()->GetHWND()==NULL) // first dialog is set as app window
		WUIApp()->SetHWND(dlg->GetHWND());

	g_lastdlg = dlg;
	g_lastitem = -1;
	gs_pushptr(vm, (void*)dlg->GetHWND());
	return 1;
	unguard()
}

int gsDlgSelect( gsVM* vm )
{
	guard(gsDlgSelect)
	if(!gs_ckparams(vm,1)) return 0;
	if(!gs_cktype(vm,0,GS_PTR)) return 0;
	g_lastdlg = DlgFind( (HWND)gs_toptr(vm,0) );
	g_lastitem = -1;
	GET_DLG();
	return 0;
	unguard()
}

int gsDlgClose( gsVM* vm )
{
	guard(gsDlgClose)
	GET_DLG();
	dlg->PostMessage( WM_CLOSE,0,0 ); // prepare to terminate
	g_lastdlg = NULL;
	g_lastitem = -1;
	return 0;
	unguard()
}

int gsDlgClear( gsVM* vm )
{
	guard(gsDlgClear)
	GET_DLG();
	dlg->PageClear();
	g_lastitem = -1;
	return 0;
	unguard()
}

int gsDlgVisible( gsVM* vm )
{
	guard(gsDlgVisible)
	if(!gs_ckparams(vm,1)) return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	GET_DLG();
	dlg->ShowWindow( gs_toint(vm,0) ? SW_SHOW : SW_HIDE );
	dlg->EnableWindow(TRUE);
	return 0;
	unguard()
}

int gsDlgRepaint( gsVM* vm )
{
	guard(gsDlgRepaint)
	GET_DLG();
	dlg->InvalidateRect(NULL);
	HWND hwnd = dlg->GetHWND(); sassert(hwnd); // safety
	BOOL ok = UpdateWindow(hwnd);
	return 0;
	unguard()
}

int gsDlgSetRgn( gsVM* vm )
{
	guard(gsDlgSetRgn)
	if(!gs_ckparams(vm,2)) return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	GET_DLG();
	dlg->DlgSetRgn( gs_toint(vm,0), gs_toint(vm,1) );
	return 0;
	unguard()
}

int	gsItemNew( gsVM* vm )
{
	guard(gsItemNew)
	GET_DLG();
	if(dlg->m_itemcount>=MAX_WUIITEMS) { GS_ERROR(0,0,"too many items in a dialog"); GS_RETURNINT(vm,-1); }

	char itemclass[64];
	strcpy(itemclass,"cWUIItem");
	if(gs_params(vm)==1 && gs_isstr(vm,0) && gs_tostr(vm,0)!=NULL )
	{
		strncpy(itemclass,gs_tostr(vm,0),63);
		itemclass[63]=0;
	}

	cWUIItem* item = NULL;
	if(strcmp(itemclass,"cWUIItem")==0)
		item = (cWUIItem*) snew cWUIItem();
	else	
	if(strcmp(itemclass,"cWUIItemEdit")==0)
		item = (cWUIItem*) snew cWUIItemEdit();
	if(!item) { GS_ERROR(0,0,"item creation failure"); GS_RETURNINT(vm,-1); }

	dlg->m_item[dlg->m_itemcount] = item;
	g_lastitem = dlg->m_itemcount;
	dlg->m_itemcount++;
	
	item->m_idx = g_lastitem;
	item->m_dlg = dlg;
	gs_pushint(vm,g_lastitem);
	return 1;
	unguard()
}

int gsItemSelect( gsVM* vm )
{
	guard(gsItemSelect)
	GET_DLG();
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=dlg->m_itemcount) { GS_ERROR(idx,dlg->m_itemcount,"invalid item index"); return 0; }
	g_lastitem = idx;
	return 0;
	unguard()
}

int	gsItemBuild( gsVM* vm )
{
	guard(gsItemBuild)
	GET_DLG(); GET_ITEM(); 
	item->Build();
	return 0;
	unguard()
}

int	gsItemGet( gsVM* vm )
{
	guard(gsItemGetInt)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IIMAX)		{ GS_ERROR(var,IIMAX,"invalid item variable"); GS_RETURNINT(vm,0); }
	GET_DLG(); GET_ITEM(); 
	gs_pushint( vm, item->GetInt( var ) );
	return 1;
	unguard()
}


int	gsItemSet( gsVM* vm )
{
	guard(gsItemSet)
	if(!gs_ckparamsmin(vm,1))	return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IIMAX)		{ GS_ERROR(var,IIMAX,"invalid item variable"); return 0; }

	GET_DLG(); GET_ITEM();
	for(int i=1;i<gs_params(vm);i++)
	{
		if(gs_isstr(vm,i))
			item->SetText(var+i-1, gs_tostr(vm,i));
		else
		if(gs_isint(vm,i))
			item->SetInt(var+i-1, gs_toint(vm,i));
		else
			gs_error(vm,GSE_BADARGTYPE,gs_type(vm,i),0);
	}
	return 0;
	unguard()
}

int gsItemGetText( gsVM* vm )
{
	guard(gsItemGetText)
	if(!gs_ckparams(vm,1))		GS_RETURNSTR(vm,"");
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,"");
	int var = gs_toint(vm, 0);
	if(var<0 || var>=IIMAX)		{ GS_ERROR(var,IIMAX,"invalid item variable"); GS_RETURNSTR(vm,""); }
	GET_DLG(); GET_ITEM(); 
	gs_pushstr( vm, item->GetText( var ) );
	return 1;
	unguard()
}

int gsDesktopW( gsVM* vm )
{
	guard(gsDesktopW)
	gs_pushint(vm, GetSystemMetrics(SM_CXSCREEN));
	return 1;
	unguard()
}

int gsDesktopH( gsVM* vm )
{
	guard(gsDesktopH)
	gs_pushint(vm, GetSystemMetrics(SM_CYSCREEN));
	return 1;
	unguard()
}

int gsMouseX( gsVM* vm )
{
	guard(gsMouseX)
	GET_DLG();
	POINT p;
	GetCursorPos(&p);
	p = dlg->Point2Client(p);
	gs_pushint(vm,p.x);
	return 1;
	unguard()
}

int gsMouseY( gsVM* vm )
{
	guard(gsMouseY)
	GET_DLG();
	POINT p;
	GetCursorPos(&p);
	p = dlg->Point2Client(p);
	gs_pushint(vm,p.y);
	return 1;
	unguard()
}

int gsOpenFolder( gsVM* vm )
{
	guard(gsOpenFolder)

	char folder[MAX_PATH];
	char title[] = "Select installation directory";

	folder[0]=0;

	BROWSEINFO bi;
	bi.hwndOwner = WUIApp()->GetHWND();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = folder;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl;
	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL) GS_RETURNSTR(vm,"");
	if(!SHGetPathFromIDList(pidl, folder)) GS_RETURNSTR(vm,"");

	int s=strlen(folder);
	if(s==0) GS_RETURNSTR(vm,"");
	if(folder[s-1]=='\\') folder[s-1]=0;

	gs_pushstr(vm,folder);
	return 1;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
#define GS_REGCONST( c )	gs_regint( g_vm, #c, c );

void WUIScriptRegister()
{
	guard(WUIScriptRegister)

	gs_regfn( g_vm, "ResBitmap",		gsResBitmap );				// str(file) > int(idx/-1)
	gs_regfn( g_vm, "ResFont",			gsResFont );				// str(face), int(w), int(h), int(flags) > int(idx/-1)
	gs_regfn( g_vm, "ResDelete",		gsResDelete );				// int(idx)

	gs_regfn( g_vm, "DlgOpen",			gsDlgOpen );				// str(name), int(style), int(x), int(y), int(w), int(h), [ptr(hwnd parent)] > ptr(hwnd/NULL)
	gs_regfn( g_vm, "DlgSelect",		gsDlgSelect );				// ptr(hwnd)
	gs_regfn( g_vm, "DlgClose",			gsDlgClose );
	gs_regfn( g_vm, "DlgClear",			gsDlgClear );
	gs_regfn( g_vm, "DlgVisible",		gsDlgVisible );				// int(1/0)
	gs_regfn( g_vm, "DlgRepaint",		gsDlgRepaint );
	gs_regfn( g_vm, "DlgSetRgn",		gsDlgSetRgn );				// int(img idx), int(colorkey)
	
	gs_regfn( g_vm, "ItemNew",			gsItemNew );				// [str(class)]> int(idx/-1)
	gs_regfn( g_vm, "ItemSelect",		gsItemSelect );				// int(idx)
	gs_regfn( g_vm, "ItemBuild",		gsItemBuild );
	gs_regfn( g_vm, "ItemGet",			gsItemGet );				// int > int(val/0)
	gs_regfn( g_vm, "ItemSet",			gsItemSet );				// int, [int/str...]
	gs_regfn( g_vm, "ItemGetText",		gsItemGetText );			// int > str(val/"")

	// api
	gs_regfn( g_vm,	"DesktopW",			gsDesktopW);				// > int; width of the desktop
	gs_regfn( g_vm,	"DesktopH",			gsDesktopH);				// > int; height of the desktop
	gs_regfn( g_vm, "MouseX",			gsMouseX);					// > int; relative to selected dlg
	gs_regfn( g_vm, "MouseY",			gsMouseY);					// > int; relative to selected dlg
	gs_regfn( g_vm, "OpenFolder",		gsOpenFolder);				// > str(folder/"")

	// constant resources
	gs_regint( g_vm, "BOLD",			FONTFLAG_BOLD			);
	gs_regint( g_vm, "ITALIC",			FONTFLAG_ITALIC			);
	gs_regint( g_vm, "UNDERLINE",		FONTFLAG_UNDERLINE		);
	gs_regint( g_vm, "STRIKEOUT",		FONTFLAG_STRIKEOUT		);
	gs_regint( g_vm, "QUALITY",			FONTFLAG_QUALITY		);

	// constant variables
	GS_REGCONST( IITYPE					);
	GS_REGCONST( IIINVISIBLE			);	
	GS_REGCONST( IIINTERACTIVE			);
	GS_REGCONST( IIRECT					);
	GS_REGCONST( IIPOS					);
	GS_REGCONST( IIX					);	
	GS_REGCONST( IIY					);	
	GS_REGCONST( IIPOS2					);
	GS_REGCONST( IIX2					);
	GS_REGCONST( IIY2					);
	GS_REGCONST( IITEXT					);
	GS_REGCONST( IICOLOR				);
	GS_REGCONST( IITOOLTIP				);

	GS_REGCONST( IITXTRES				);	
	GS_REGCONST( IITXTALIGN				);
	GS_REGCONST( IITXTSINGLELINE		);

	GS_REGCONST( IIIMGRES				);
	GS_REGCONST( IIIMG9					);
	GS_REGCONST( IIIMGALIGN				);
	GS_REGCONST( IIIMGBLEND				);
	GS_REGCONST( IIIMGMAP				);
	GS_REGCONST( IIIMGWRAP				);
	GS_REGCONST( IIIMGWRAPLEFT			);
	GS_REGCONST( IIIMGWRAPTOP			);
	GS_REGCONST( IIIMGWRAPRIGHT			);
	GS_REGCONST( IIIMGWRAPBOTTOM		);	
	GS_REGCONST( IIIMGBORDER			);	
	GS_REGCONST( IIIMGBORDERLEFT		);	
	GS_REGCONST( IIIMGBORDERTOP			);
	GS_REGCONST( IIIMGBORDERRIGHT		);
	GS_REGCONST( IIIMGBORDERBOTTOM		);
	GS_REGCONST( IIIMGSTRETCHMODE		);

	GS_REGCONST( IIBUTCHECK				);
	GS_REGCONST( IIBUTPRESSED			);
	GS_REGCONST( IIBUTCMDTEXT			);


	// constant values
	GS_REGCONST( IITYPE_RECT			);
	GS_REGCONST( IITYPE_COLOR			);
	GS_REGCONST( IITYPE_GRADIENT		);
	GS_REGCONST( IITYPE_IMG				);
	GS_REGCONST( IITYPE_TEXT			);
	GS_REGCONST( IITYPE_BUTTON			);

	GS_REGCONST( IIWRAP_NORMAL			);
	GS_REGCONST( IIWRAP_STRETCH			);	
	GS_REGCONST( IIWRAP_REPEAT			);	
										
	GS_REGCONST( IIALIGN_NONE			);
	GS_REGCONST( IIALIGN_LEFT			);
	GS_REGCONST( IIALIGN_RIGHT			);
	GS_REGCONST( IIALIGN_CENTERX		);	
	GS_REGCONST( IIALIGN_TOP			);	
	GS_REGCONST( IIALIGN_BOTTOM			);
	GS_REGCONST( IIALIGN_CENTERY		);	
										
	GS_REGCONST( IIBLEND_NORMAL			);
	GS_REGCONST( IIBLEND_COLORKEY		);
	GS_REGCONST( IIBLEND_ALPHA			);


	// constant winapi
	GS_REGCONST( WS_OVERLAPPEDWINDOW	);
	GS_REGCONST( WS_CLIPCHILDREN		);
	GS_REGCONST( WS_CHILD				);
	GS_REGCONST( WS_POPUP				);
	
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
