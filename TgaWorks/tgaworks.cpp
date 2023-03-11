//////////////////////////////////////////////////////////////////////////////////////////////////
// tgaworks.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include "conio.h"
#include "malloc.h"
#include "string.h"
#include "shlobj.h"
#include "resource.h"
#include "util.h"

#include "tgaoperations.h"

#define APPTITLE						"TGA Works v2.2"
#define MSGINFO( text )					MessageBox( g_hwnd, text, "TGA Works", MB_OK | MB_ICONINFORMATION )
#define MSGERROR( text )				MessageBox( g_hwnd, text, "TGA Works", MB_OK | MB_ICONERROR )
#define MSGQUESTION( text )				MessageBox( g_hwnd, text, "TGA Works", MB_YESNO | MB_ICONQUESTION )

#define MAX_PROPS 3
#define BROWSE_FILE		0
#define BROWSE_FOLDER	1
#define BROWSE_COLOR	2
#define BROWSE_TYPES	3

#define	OPERATION_EXPLODE					0
#define	OPERATION_IMPLODE					1
#define	OPERATION_BUILD_ALPHA				2
#define	OPERATION_MULTI_BUILD_ALPHA			3
#define	OPERATION_CHECK_COLORS				4
#define	OPERATION_MULTI_CHECK_COLORS		5
#define	OPERATION_LAST						6

HWND		 g_hwnd = NULL;
HWND		 g_hwndlog = NULL;
HINSTANCE	 g_hinstance = NULL;
HFONT		 g_hfont = NULL;
int			 g_lastOperation = -1;
const char  *g_operations[] = { "EXPLODE" , "IMPLODE", "BUILD ALPHA", "MULTI BUILD ALPHA", "CHECK COLORS", "MULTI CHECK COLORS" };
extern dword g_OpMultiBuildAlphaColor;
extern char  g_OpFolder[MAX_PATH];

struct tGUIProp
{
	int		m_ctrl_name;
	int		m_ctrl_edit;
	int		m_ctrl_browse;
	int		m_browser;
	int		m_browserfilter;
	char	m_value[256];
};

tGUIProp g_prop[MAX_PROPS] =
{
	{ IDC_TEXT1, IDC_EDIT1, IDC_BROWSE1, 0, -1, "" },
	{ IDC_TEXT2, IDC_EDIT2, IDC_BROWSE2, 0, -1, "" },
	{ IDC_TEXT3, IDC_EDIT3, IDC_BROWSE3, 0, -1, "" }
};


void SetPropCount( int count )
{
	sassert(0<=count && count<=MAX_PROPS);
	for(int i=0;i<MAX_PROPS;i++)
	{
		HWND hwnd;
		hwnd = GetDlgItem(g_hwnd,g_prop[i].m_ctrl_name);
		ShowWindow(hwnd,i<count);
		hwnd = GetDlgItem(g_hwnd,g_prop[i].m_ctrl_edit);
		ShowWindow(hwnd,i<count);
		hwnd = GetDlgItem(g_hwnd,g_prop[i].m_ctrl_browse);
		ShowWindow(hwnd,i<count);
	}
}

int GetPropCount()
{
	for(int i=0;i<MAX_PROPS;i++)
	{
		HWND hwnd = GetDlgItem(g_hwnd,g_prop[i].m_ctrl_name);
		if(!IsWindowVisible(hwnd))
			return (i+1);
	}
	return MAX_PROPS;
}

void SetPropName( int idx, char* name )
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_name);
	SetWindowText(hwnd,name);
}

void GetPropName( int idx, char* name)
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_name);
	GetWindowText(hwnd,name,255);
}

void SetPropValue( int idx, char* value )
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_edit);
	SetWindowText(hwnd,value);
}

char* GetPropValue( int idx )
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_edit);
	GetWindowText(hwnd,g_prop[idx].m_value,255);
	return g_prop[idx].m_value;
}

dword GetPropValueColor( int idx )
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_edit);
	char value[32];
	GetWindowText(hwnd,value,32);
	dword dw=0;
	sscanf(value,"%x",&dw);
	return dw;
}

void SetPropBrowser( int idx, int browse, int filter)
{
	sassert(0<=idx && idx<MAX_PROPS);
	sassert(0<=browse && browse<BROWSE_TYPES);
	g_prop[idx].m_browser = browse;
	g_prop[idx].m_browserfilter = filter;
}


int GetPropBrowserCtrl( int idx )
{
	sassert(0<=idx && idx<MAX_PROPS);
	return g_prop[idx].m_ctrl_browse;
}

int GetPropBrowserType( int idx )
{
	sassert(0<=idx && idx<MAX_PROPS);
	return g_prop[idx].m_browser ;
}

int GetPropBrowserFilter( int idx )
{
	sassert(0<=idx && idx<MAX_PROPS);
	return g_prop[idx].m_browserfilter;
}

int GetOperation()
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_COMBO1);
	return SendMessage(hwnd,CB_GETCURSEL,0,0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 Functions
//////////////////////////////////////////////////////////////////////////////////////////////////
void OnOperationChange()
{
	if(g_lastOperation==GetOperation()) return;
	switch(GetOperation())
	{
	case OPERATION_EXPLODE:
		if(g_lastOperation!=OPERATION_IMPLODE)
		{
			SetPropName(0,"RGBA File");
			SetPropValue(0,"");
			SetPropBrowser(0,BROWSE_FILE,TGA_FILTER);
			SetPropName(1,"RGB File");
			SetPropValue(1,"");
			SetPropBrowser(1,BROWSE_FILE,BMP_FILTER);
			SetPropName(2,"ALPHA File");
			SetPropValue(2,"");
			SetPropBrowser(2,BROWSE_FILE,BMP_FILTER);
			SetPropCount(3);
		}
		break;
	case OPERATION_IMPLODE:
		if(g_lastOperation!=OPERATION_EXPLODE)
		{
			SetPropName(0,"RGBA File");
			SetPropValue(0,"");
			SetPropBrowser(0,BROWSE_FILE,TGA_FILTER);
			SetPropName(1,"RGB File");
			SetPropValue(1,"");
			SetPropBrowser(1,BROWSE_FILE,BMP_FILTER);
			SetPropName(2,"ALPHA File");
			SetPropValue(2,"");
			SetPropBrowser(2,BROWSE_FILE,BMP_FILTER);
			SetPropCount(3);
		}
		break;
	case OPERATION_BUILD_ALPHA:
		SetPropName(0,"RGBA File");
		SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FILE,TGA_FILTER);
		if(g_lastOperation!=OPERATION_MULTI_BUILD_ALPHA)
		{
			SetPropName(1,"RGB Color");
			SetPropValue(1,"");
			SetPropBrowser(1,BROWSE_COLOR,-1);
		}
		SetPropCount(2);
		break;
	case OPERATION_MULTI_BUILD_ALPHA:
		SetPropName(0,"RGBA Folder");
		SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FOLDER,-1);
		if(g_lastOperation!=OPERATION_BUILD_ALPHA)
		{
			SetPropName(1,"RGB Color");
			SetPropValue(1,"");
			SetPropBrowser(1,BROWSE_COLOR,-1);
		}
		SetPropCount(2);
		break;
	case OPERATION_CHECK_COLORS:
		SetPropName(0,"RGBA File");
		SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FILE,TGA_FILTER);
		SetPropCount(1);
		break;
	case OPERATION_MULTI_CHECK_COLORS:
		SetPropName(0,"RGBA Folder");
		SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FOLDER,-1);
		SetPropCount(1);
		break;
	}
	g_lastOperation = GetOperation();
}

void OnInitDialog( HWND hwnd )
{
	g_hwnd = hwnd;
	g_hwndlog = GetDlgItem(hwnd,IDC_LOG);

	SetWindowText(hwnd,APPTITLE);
	HICON hIcon = LoadIcon(g_hinstance,MAKEINTRESOURCE(IDI_ICON1));
	PostMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM) (HICON) hIcon);

	HWND hwnded = GetDlgItem(hwnd,IDC_LOG);
	sassert(hwnded!=NULL);
	g_hfont = CreateFont(6,0,0,0,0,0,0,0,0,0,0,0,0,"Courier");
	SendMessage(hwnded,WM_SETFONT,(WPARAM)g_hfont,0);

	// operations
	HWND hwndcb = GetDlgItem(g_hwnd,IDC_COMBO1);
	for(int i=0;i<OPERATION_LAST;i++)
		SendMessage(hwndcb,CB_ADDSTRING,0,(LPARAM)((LPCTSTR)g_operations[i]));
	SendMessage(hwndcb,CB_SETCURSEL,0,0);
	OnOperationChange();
}

void OnHelp()
{
	switch(GetOperation())
	{
	case OPERATION_EXPLODE:
	MessageBox(g_hwnd,
		"\n" \
		"  TGA Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you with your tga files.\n" \
		"\n" \
		"  Operation EXPLODE splits a TGA file in two BMP files, containing\n" \
		"RGB color and ALPHA channel value. Use it as conversion tool from\n"\
		"32bit TGA to 24bit BMP file. This is reverse for IMPLODE operation.\n"\
		"\n" \
		"  1. Select a TGA file from your tiles.\n" \
		"  2. Output BMP file(24bit) with RGB color from given TGA.\n" \
		"Use this for converting  a 32bit TGA file to 24bit BMP file.\n" \
		"If the field is empty, RGB color file will not be generated.\n"\
		"  3. Output BMP file(24bit) with ALPHA value from given TGA.\n" \
		"This field may be empty, in this case a file with ALPAHA value\n"\
		"as color in RGB channel will not be generated.\n"\
		"  4. Procced with operation.\n" \
		"\n",
		
		"TGA Works HELP", MB_OK);
	break;
	case OPERATION_IMPLODE:
	MessageBox(g_hwnd,
		"\n" \
		"  TGA Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you with your tga files.\n" \
		"\n" \
		"  Operation IMPLODE creates a TGA file from two BMP files, containing\n" \
		"RGB color and ALPHA channel value. Use it as conversion tool from\n"\
		"24bit BMP to 24bit TGA file. This is the reverse of EXPLODE operation.\n"\
		"\n" \
		"  1. Output TGA file to be created from two BMP files.\n" \
		"  2. Select BMP file(24bit) with RGB color.\n" \
		"  3. Select BMP file(24bit) with ALPHA value.\n" \
		"This field may be empty, in this case the operation is a simple\n"\
		"conversion from BMP to TGA file.\n"\
		"  4. Procced with operation.\n" \
		"\n",
		
		"TGA Works HELP", MB_OK);
	break;
	case OPERATION_BUILD_ALPHA:
	MessageBox(g_hwnd,
		"\n" \
		"  TGA Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you with your tga files.\n" \
		"\n" \
		"  Operation BUILD ALPHA generates the ALPHA channel\n" \
		" for a TGA file, using selected color for transparacy.\n"\
		"\n" \
		"  1. Select file for which you want alpha channel to be created.\n" \
		"  2. Select color to be transparent.\n" \
		"  3. Procced with operation.\n" \
		"\n",
		
		"TGA Works HELP", MB_OK);
	break;
	case OPERATION_MULTI_BUILD_ALPHA:
	MessageBox(g_hwnd,
		"\n" \
		"  TGA Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you with your tga files.\n" \
		"\n" \
		"  Operation MULTI BUILD ALPHA generates the ALPHA channel\n" \
		"for all TGA files, from a selected folder. Same as BUILD ALPHA\n"\
		"applyed on more files.\n"\
		"\n" \
		"  1. Select folder where to look for files.\n" \
		"  2. Select color to be transparent.\n" \
		"  3. Procced with operation.\n" \
		"\n",
		
		"TGA Works HELP", MB_OK);
	break;
	case OPERATION_CHECK_COLORS:
	MessageBox(g_hwnd,
		"\n" \
		"  TGA Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you with your tga files.\n" \
		"\n" \
		"  Operation CHECK COLORS verifies colors for a TGA file,\n" \
		"in both RGB and ALPHA channels and display their number (with a limit).\n" \
		"  It is useful to find mistakes in tiles that were supposed to have only\n" \
		"a fixed number of colors, like black and white tiles.\n" \
		"\n" \
		"  1. Select TGA file to be checked.\n" \
		"  2. Procced with operation.\n" \
		"\n",
		
		"TGA Works HELP", MB_OK);
	break;
	case OPERATION_MULTI_CHECK_COLORS:
	MessageBox(g_hwnd,
		"\n" \
		"  TGA Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you with your tga files.\n" \
		"\n" \
		"  Operation MULTI CHECK COLORS verifies colors for all TGA file,\n"\
		"from a selected folder, in both RGB and ALPHA channels.\n" \
		"Same as CHECK COLORS applyed on more files.\n"\
		"\n" \
		"  1. Select folder where to look for files.\n" \
		"  2. Procced with operation.\n" \
		"\n",
		
		"TGA Works HELP", MB_OK);
	break;
	}

}


void OnBrowse(int browse)
{
	char browsetitle[MAX_PATH];
	browsetitle[0] = 0;
	char browsepath[MAX_PATH];
	browsepath[0] = 0;
	if(GetPropBrowserType(browse)==BROWSE_FILE)
	{
		if(!OnBrowseFile(browsetitle,GetPropBrowserFilter(browse),browsepath))
			return;
		SetPropValue(browse,browsepath);

		if(browse==0 &&
		   (GetOperation()==OPERATION_EXPLODE || GetOperation()==OPERATION_IMPLODE))
		{
			char path[MAX_PATH];
			browsepath[strlen(browsepath)-4]=0;
			strcpy(path,browsepath);
			strcat(path," rgb.bmp");
			SetPropValue(browse+1,path);
			strcpy(path,browsepath);
			strcat(path," a.bmp");
			SetPropValue(browse+2,path);
		}
	}
	if(GetPropBrowserType(browse)==BROWSE_FOLDER)
	{
		GetPropName(browse,browsetitle);
		if(!OnBrowseDir(browsetitle,browsepath))
			return;
		SetPropValue(browse,browsepath);
	}
	if(GetPropBrowserType(browse)==BROWSE_COLOR)
	{
		dword color=0;
		char szcolor[16];
		if(!OnBrowseColor(&color,TRUE))
			return;
		sprintf(szcolor,"%X",color);
		SetPropValue(browse,szcolor);
	}
}

void OnProceed()
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	Log(hwnd,NULL);
	if(GetOperation()==OPERATION_EXPLODE)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: EXPLODE\r\n------------------------------------------------\r\n");
		OpExplode(GetPropValue(0),GetPropValue(1),GetPropValue(2));
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_IMPLODE)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: IMPLODE\r\n------------------------------------------------\r\n");
		OpImplode(GetPropValue(1),GetPropValue(2),GetPropValue(0));
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_BUILD_ALPHA)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: BUILD ALPHA\r\n------------------------------------------------\r\n");
		OpBuildAlpha(GetPropValue(0),GetPropValueColor(1));
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_MULTI_BUILD_ALPHA)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: MULTI BUILD ALPHA\r\n------------------------------------------------\r\n");
		char folder[256];
		strcpy(folder,GetPropValue(0));
		strcat(folder,"\\");
		g_OpMultiBuildAlphaColor = GetPropValueColor(1);
		file_findfiles(folder, OpMultiBuildAlpha, FILE_FINDREC);
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_CHECK_COLORS)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: CHECK COLOR\r\n------------------------------------------------\r\n");
		Log(hwnd,"RGB A  Files\r\n");
		g_OpFolder[0] = 0;
		OpCheckColor(GetPropValue(0));
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_MULTI_CHECK_COLORS)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: MULTI CHECK COLOR\r\n------------------------------------------------\r\n");
		Log(hwnd,"RGB A  Files\r\n");
		char folder[256];
		strcpy(folder,GetPropValue(0));
		strcat(folder,"\\");
		strcpy(g_OpFolder,folder);
		file_findfiles(folder, OpMultiCheckColor, FILE_FINDREC);
		Log(hwnd,"\r\n");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 Proc
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DialogProc1( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
		case WM_INITDIALOG:	OnInitDialog( hwndDlg ); return TRUE;
		case WM_COMMAND:
		{
			switch(LOWORD(wParam)) // by ID
			{
				case IDC_COMBO1:
					switch(HIWORD(wParam)) 
					{
					case CBN_SELCHANGE:
						OnOperationChange();
						break;
					}
					break;
				case IDC_BROWSE1:
				case IDC_BROWSE2:
				case IDC_BROWSE3:
					{
						for(int i=0;i<MAX_PROPS;i++)
							if(GetPropBrowserCtrl(i)==LOWORD(wParam))
							{
								OnBrowse(i);
								break;
							}
					}
					break;
				case IDC_PROCEED:		OnProceed(); return TRUE;
				case IDC_EXIT:			EndDialog(hwndDlg,0); return TRUE;
				case IDC_HLP:			OnHelp(); return TRUE;
			}
			break;
		}
		break;
		case WM_CLOSE: EndDialog(hwndDlg,0); return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK WndProcDummy( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// WINMAIN
//////////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	InitCommonControls();
	g_hinstance = hInstance;

	int ret = DialogBox( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc1 );
	dword err = GetLastError();

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
