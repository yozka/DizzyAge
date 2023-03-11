//////////////////////////////////////////////////////////////////////////////////////////////////
// Pak.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include "conio.h"
#include "malloc.h"
#include "string.h"
#include "shlobj.h"
#include "resource.h"
#include "PakUtil.h"

#define APPTITLE						"PAK DizzyAGE v2.2"
#define MSGINFO( text )					MessageBox( g_hwnd, text, "PAK", MB_OK | MB_ICONINFORMATION )
#define MSGERROR( text )				MessageBox( g_hwnd, text, "PAK", MB_OK | MB_ICONERROR )
#define MSGQUESTION( text )				MessageBox( g_hwnd, text, "PAK", MB_YESNO | MB_ICONQUESTION )

HWND		g_hwnd = NULL;
HWND		g_hwndlog = NULL;
HWND		g_hwndprogress = NULL;
HINSTANCE	g_hinstance = NULL;

int			g_mode = 0;					// 0=pack 1=unpack
char		g_password[256] = "";		// password

//////////////////////////////////////////////////////////////////////////////////////////////////
// PACK
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 Functions
//////////////////////////////////////////////////////////////////////////////////////////////////
void OnInitDialog( HWND hwnd )
{
	g_hwnd = hwnd;
	g_hwndlog = GetDlgItem(hwnd,IDC_LOG);
	g_hwndprogress = GetDlgItem(hwnd,IDC_PROGRESS);

	CenterDialog(hwnd);

	SetWindowText(hwnd,APPTITLE);
	HICON hIcon = LoadIcon(g_hinstance,MAKEINTRESOURCE(IDI_ICON1));
	PostMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM) (HICON) hIcon);

	LONG style;
	hwnd = GetDlgItem(g_hwnd,IDC_UNPACK);
	style = GetWindowLong(hwnd,GWL_STYLE);
	style |= BS_PUSHLIKE | BS_AUTOCHECKBOX;
	SetWindowLong(hwnd,GWL_STYLE, style);

	hwnd = GetDlgItem(g_hwnd,IDC_PACK);
	style = GetWindowLong(hwnd,GWL_STYLE);
	style |= BS_PUSHLIKE | BS_AUTOCHECKBOX;
	SetWindowLong(hwnd,GWL_STYLE, style);

	CheckDlgButton(g_hwnd, IDC_PACK, 1);
	CheckDlgButton(g_hwnd, IDC_UNPACK, 0);

	SetDlgItemText(g_hwnd, IDC_EXT, "wav ogg ym jpg png zip");

	g_mode=0;

	LOG("Welcome!");
}

void OnHelp()
{
	MessageBox(g_hwnd,
		"\n" \
		"  PAK Tool for DizzyAGE v2.2 (2008)\n" \
		"  by Alexandru Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  PACK operation will create a .pak archive with all the files\n" \
		"from the input folder, including the folder itself. All files will be\n" \
		"compressed, except those with the specified extentions.\n" \
		"  Do not compress files used intensively by the game,\n" \
		"like streaming music or already compressed files.\n" \
		"  Do not compress huge files because they are read entirely\n"
		"in memory at open time.\n" \
		"\n" \
		"  UNPACK operation will extract files from a .pak archive\n" \
		"into the output folder.\n" \
		"\n",
		
		"PAK HELP", MB_OK);
}

int OnBrowseDir()
{
	char folder[MAX_PATH];
	folder[0]=0;

	char title[128];
	if(g_mode)	strcpy( title, "Select directory to unpack files in" );
	else		strcpy( title, "Select directory to pack" );

	BROWSEINFO bi;
	bi.hwndOwner = g_hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = folder;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl;
	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL) return 0;
	if(!SHGetPathFromIDList(pidl, folder)) return 0;

	int s=strlen(folder);
	if(s==0) return 0;
	if(folder[s-1]=='\\') folder[s-1]=0;

	// write
	HWND hwnd = (g_mode)?GetDlgItem(g_hwnd,IDC_TO):GetDlgItem(g_hwnd,IDC_FROM);
	SetWindowText(hwnd,folder);
	return 1;
}

int OnBrowseFile()
{
	char path[MAX_PATH];
	path[0]=0;
	strcat(path,"unnamed.pak");
	

	dword flags=0;
	flags |= OFN_NOCHANGEDIR;
	flags |= OFN_PATHMUSTEXIST;
	char title[128]; title[0]=0;

	if(g_mode)
	{
		strcpy(title,"Select archive file to unpack");
		flags |= OFN_FILEMUSTEXIST;
	}
	else		
	{
		strcpy(title,"Select archive to pack files in");
	}
		

	char ext[] = "pak";
	char filter[64];
	if(ext)
	{
		int extsize=strlen(ext);
		strcpy(filter,"*.");
		strcat(filter,ext);
		strcpy(filter+extsize+3,"*.");
		strcat(filter+extsize+3,ext);
		filter[(extsize+3)*2] = 0;
	}

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= g_hwnd;
	ofn.hInstance			= g_hinstance;
	ofn.lpstrFilter			= ext ? filter : NULL;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= path; // in+out
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= title;
	ofn.Flags				= flags;
	ofn.nFileExtension		= 0; //out
	ofn.lpstrDefExt			= ext;

	BOOL ok;
	ok = GetOpenFileName( &ofn );
	if(ok)
	{
		// write
		HWND hwnd = (!g_mode)?GetDlgItem(g_hwnd,IDC_TO):GetDlgItem(g_hwnd,IDC_FROM);
		SetWindowText(hwnd,path);
		return 1;
	}
	return 0;
}

void OnToggle( int mode )
{
	g_mode = mode;
	SetDlgItemText(g_hwnd, IDC_PROCEED, g_mode?"UNPACK":"PACK");

	CheckDlgButton(g_hwnd, IDC_PACK, !g_mode);
	CheckDlgButton(g_hwnd, IDC_UNPACK, g_mode);

	SetDlgItemText(g_hwnd,IDC_FROM,"");
	SetDlgItemText(g_hwnd,IDC_TO,"");

	HWND hwnd = GetDlgItem(g_hwnd,IDC_EXT);
	EnableWindow(hwnd,g_mode==0);

	hwnd = GetDlgItem(g_hwnd,IDC_PASS);
	EnableWindow(hwnd,g_mode==0);
}


void OnProceed()
{
	int i;
	int ret=0;
	int size;
	char input[256];
	char output[256];
	char olddir[256];

	LOG(NULL);

	PROGRESSINIT(0);	

	if(g_mode==0) // PACK
	{
		// get input
		size = GetDlgItemText(g_hwnd,IDC_FROM,input,256);
		if(size<=0) { LOG("ERROR: invalid input.\r\n"); goto pakerror; }
		if(input[size-1]=='\\') { size--; input[size]=0; }
		i=size-1;
		while(i>0 && input[i]!='\\' ) i--;
		if(i==0) { LOG("ERROR: invalid input.\r\n"); goto pakerror; }
		input[i]=0;

		// get output
		size = GetDlgItemText(g_hwnd,IDC_TO,output,256);
		if(size<=0) { LOG("ERROR: invalid output.\r\n"); goto pakerror; }

		// get extentions
		char extentions[256]; extentions[0]=0;
		size = GetDlgItemText(g_hwnd,IDC_EXT,extentions,256);

		// directory
		olddir[0]=0;
		GetCurrentDirectory(256,olddir);
		if(!SetCurrentDirectory(input)) { LOG("ERROR: could not switch to input directory: "); LOG(input); LOG("\r\n"); goto pakerror; }

		// proceed
		LOG("Pack\r\n");
		LOG("From: "); LOG(input); LOG("\\"); LOG(input+i+1); LOG("\r\n");
		LOG("To: "); LOG(output); LOG("\r\n");
		ret = PackProceed( input+i+1, output, extentions, 1, g_password );

		SetCurrentDirectory(olddir);
		
pakerror:
		if(ret)	MSGINFO("Pack successful!");
		else	MSGERROR("Pack failure!");

	}
	else // UNPACK
	{
		// get input
		size = GetDlgItemText(g_hwnd,IDC_FROM,input,256);
		if(size<=0) { LOG("ERROR: invalid input.\r\n"); goto unpackerror; }

		// get output
		size = GetDlgItemText(g_hwnd,IDC_TO,output,256);
		if(size<=0) { LOG("ERROR: invalid output.\r\n"); goto unpackerror; }
		if(output[size-1]=='\\') { size--; output[size]=0; }

		// directory
		olddir[0]=0;
		GetCurrentDirectory(256,olddir);
		if(!SetCurrentDirectory(output)) { LOG("ERROR: could not switch to output directory: "); LOG(output); LOG("\r\n"); goto unpackerror; }

		// proceed
		LOG("Unpack\r\n");
		LOG("From: "); LOG(input); LOG("\r\n");
		LOG("To: "); LOG(output); LOG("\r\n");
		ret = UnpackProceed( input, 1 );

		SetCurrentDirectory(olddir);
		
unpackerror:
		if(ret)	MSGINFO("Unpack successful!");
		else	MSGERROR("Unpack failure!");
	}
}

void OnPass()
{
	if(!InputBox(g_hinstance,g_hwnd,"Input password","Your archive will require this\r\npassword to be unpacked.\r\nMake sure you remember it.", g_password)) return;
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
				case IDC_PACK:		OnToggle(0); return TRUE;
				case IDC_UNPACK:	OnToggle(1); return TRUE;
				case IDC_FROMB:		if(g_mode) OnBrowseFile(); else OnBrowseDir(); return TRUE;
				case IDC_TOB:		if(g_mode) OnBrowseDir(); else OnBrowseFile(); return TRUE;
				case IDC_EXIT:		EndDialog(hwndDlg,0); return TRUE;
				case IDC_PASS:		OnPass(); return TRUE;
				case IDC_HLP:		OnHelp(); return TRUE;
				case IDC_PROCEED:	OnProceed(); return TRUE;
			}
			break;
		}
		case WM_CLOSE: EndDialog(hwndDlg,0); return TRUE;
		case WM_DROPFILES:
		{
			char filepath[MAX_PATH];
			if(DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0)<1) return 0; // count
			if(DragQueryFile((HDROP)wParam, 0, filepath, MAX_PATH)==0) return 0; // error
			char* ext = file_path2ext(filepath);
			if(ext==NULL) // folder
			{
				if(g_mode==0) // pack: set data folder
					SetDlgItemText(g_hwnd,IDC_FROM,filepath);
				else // unpack: set destination folder
					SetDlgItemText(g_hwnd,IDC_TO,filepath);
			}
			else
			{
				if(0==strcmp(ext,"pak")) // map file
				{
					if(g_mode==0) // pack: set destination pak file
						SetDlgItemText(g_hwnd,IDC_TO,filepath);
					else // unpack: set source pak file
						SetDlgItemText(g_hwnd,IDC_FROM,filepath);
				}
				else
					MSGERROR("Invalid file format.\nDrag and drop the map file or the tiles folder.");
			}
			return 0;
		}
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
