//////////////////////////////////////////////////////////////////////////////////////////////////
// audioworks.cpp
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

#include "audiooperations.h"

#define APPTITLE						"AUDIO Works v2.2"
#define MSGINFO( text )					MessageBox( g_hwnd, text, "AUDIO Works", MB_OK | MB_ICONINFORMATION )
#define MSGERROR( text )				MessageBox( g_hwnd, text, "AUDIO Works", MB_OK | MB_ICONERROR )
#define MSGQUESTION( text )				MessageBox( g_hwnd, text, "AUDIO Works", MB_YESNO | MB_ICONQUESTION )

#define MAX_PROPS 5
#define BROWSE_FILE		0
#define BROWSE_FOLDER	1
#define BROWSE_COLOR	2
#define BROWSE_TYPES	3

#define	OPERATION_INFO						0
#define	OPERATION_CROP						1
#define	OPERATION_DETECT_LOOP				2
#define	OPERATION_LAST						3

HWND		 g_hwnd = NULL;
HWND		 g_hwndlog = NULL;
HINSTANCE	 g_hinstance = NULL;
int			 g_lastOperation = -1;
const char  *g_operations[] = { "INFO", "CROP" , "DETECT LOOP" };

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
	{ IDC_TEXT3, IDC_EDIT3, -1, 0, -1, "" },
	{ IDC_TEXT4, IDC_EDIT4, -1, 0, -1, "" },
	{ IDC_TEXT5, IDC_COMBO2, -1, 0, -1, "" }
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
		if(g_hwnd,g_prop[i].m_ctrl_browse!=-1)
		{
			hwnd = GetDlgItem(g_hwnd,g_prop[i].m_ctrl_browse);
			ShowWindow(hwnd,i<count);	
		}
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

void SetPropComboValue( int idx, int value)
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_edit);
	int items = SendMessage(hwnd,CB_GETCOUNT,0,0);
	if(value<0 || value>=items) value = 0;
	SendMessage(hwnd,CB_SETCURSEL,value,0);
}

int GetPropComboValue( int idx)
{
	sassert(0<=idx && idx<MAX_PROPS);
	HWND hwnd = GetDlgItem(g_hwnd,g_prop[idx].m_ctrl_edit);
	return SendMessage(hwnd,CB_GETCURSEL,0,0);
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

int GetPositionType()
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_COMBO2);
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
	case OPERATION_INFO:
		SetPropName(0,"YM File");
		//SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FILE,YM_FILTER);
		SetPropCount(1);
		break;
	case OPERATION_CROP:
		SetPropName(0,"YM File");
		//SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FILE,YM_FILTER);
		SetPropName(1,"Output");
		SetPropValue(1,"");
		SetPropBrowser(1,BROWSE_FILE,YM_FILTER);
		SetPropName(2,"Start");
		SetPropValue(2,"");
		SetPropName(3,"End");
		SetPropValue(3,"");
		SetPropName(4,"Position Type");
		SetPropComboValue(4,0);
		SetPropCount(5);
		break;
	case OPERATION_DETECT_LOOP:
		SetPropName(0,"YM File");
		//SetPropValue(0,"");
		SetPropBrowser(0,BROWSE_FILE,YM_FILTER);
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

	// operations
	HWND hwndcb = GetDlgItem(g_hwnd,IDC_COMBO1);
	for(int i=0;i<OPERATION_LAST;i++)
		SendMessage(hwndcb,CB_ADDSTRING,0,(LPARAM)((LPCTSTR)g_operations[i]));
	SendMessage(hwndcb,CB_SETCURSEL,0,0);

	hwndcb = GetDlgItem(g_hwnd,IDC_COMBO2);
	SendMessage(hwndcb,CB_ADDSTRING,0,(LPARAM)("seconds"));
	SendMessage(hwndcb,CB_ADDSTRING,0,(LPARAM)("frames"));
	SendMessage(hwndcb,CB_SETCURSEL,0,0);

	OnOperationChange();
}

void OnHelp()
{
	switch(GetOperation())
	{
	case OPERATION_INFO:
	MessageBox(g_hwnd,
		"\n" \
		"  Audio Works v2.2 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you work with music files, which have .ym extension.\n" \
		"\n" \
		"  Operation INFO displays detailed information about a file, such as\n" \
		"name, author, frames number, frequency, duration and others.\n" \
		"\n" \
		"  1. Select YM File you want to see.\n" \
		"  2. Proceed with operation.\n" \
		"\n" \
		"\n",
	
		"AUDIO Works HELP", MB_OK);
		break;
	case OPERATION_CROP:
	MessageBox(g_hwnd,
		"\n" \
		"  Audio Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you work with music files, which have .ym extension.\n" \
		"\n" \
		"  Operation CROP cuts the file, keeping only the wanted part.\n" \
		"\n" \
		"  1. Select YM File you want to crop.\n" \
		"  2. Select Output file where to save the result.\n" \
		"  3. Choose the Position Type for cropping, \"seconds\" or \"frames\" as you like.\n" \
		"  4. Edit Start point from where to crop (in the choosen unit).\n"\
		"  5. Edit End position where to stop the cropping (in the choosen unit).\n"\
		"  6. Proceed with operation.\n" \
		"\n" \
		"\n",
	
		"AUDIO Works HELP", MB_OK);
		break;
	case OPERATION_DETECT_LOOP:
	MessageBox(g_hwnd,
		"\n" \
		"  Audio Works v2.0 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you work with music files, which have .ym extension.\n" \
		"\n" \
		"  Operation DETECT LOOP checks the file for a looping part, reports it and \n" \
		"sugests values for an eventual crop. A loop is a sequence of frames which \n"\
		"will repeat itself from the first appearance to the end of file.\n"\
		"\n" \
		"  1. Select YM File you want to check.\n" \
		"  2. Proceed with operation.\n" \
		"\n" \
		"\n",
	
		"AUDIO Works HELP", MB_OK);
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
		GetPropName(browse,browsetitle);
		if(!OnBrowseFile(browsetitle,GetPropBrowserFilter(browse),browsepath))
			return;
		SetPropValue(browse,browsepath);

		if(browse==0 &&
		   (GetOperation()==OPERATION_CROP) && strlen(browsepath)>3)
		{
			char path[MAX_PATH];
			browsepath[strlen(browsepath)-3]=0;
			strcpy(path,browsepath);
			strcat(path,"_crop.ym");
			SetPropValue(browse+1,path);
		}
	}
}

void OnProceed()
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	Log(hwnd,NULL);
	if(GetOperation()==OPERATION_INFO)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: INFO\r\n------------------------------------------------\r\n");
		OpInfo(GetPropValue(0));
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_CROP)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: CROP\r\n------------------------------------------------\r\n");
		OpCrop(GetPropValue(0),GetPropValue(1),atoi(GetPropValue(2)),atoi(GetPropValue(3)),(GetPositionType()==1));
		Log(hwnd,"\r\n");
	}
	if(GetOperation()==OPERATION_DETECT_LOOP)
	{
		Log(hwnd,"------------------------------------------------\r\nOperation: DETECT LOOP\r\n------------------------------------------------\r\n");
		OpDetectLoop(GetPropValue(0));
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
