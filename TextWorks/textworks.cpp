//////////////////////////////////////////////////////////////////////////////////////////////////
// checkdialogs.cpp
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
#include "checkoperations.h"

#define APPTITLE						"Text Works v2.2"
#define MSGINFO( text )					MessageBox( g_hwnd, text, "Text Works", MB_OK | MB_ICONINFORMATION )
#define MSGERROR( text )				MessageBox( g_hwnd, text, "Text Works", MB_OK | MB_ICONERROR )
#define MSGQUESTION( text )				MessageBox( g_hwnd, text, "Text Works", MB_YESNO | MB_ICONQUESTION )

HWND		g_hwnd = NULL;
HWND		g_hwndlog = NULL;
HINSTANCE	g_hinstance = NULL;
HFONT		g_hfont = NULL;

char		g_outputfile[MAX_PATH];
char 		g_intagprefix[MAX_PATH];
char 		g_intagsufix[MAX_PATH];
char 		g_outtagprefix[MAX_PATH];
char 		g_outtagsufix[MAX_PATH];
char		g_extensions[3][64];

int  		g_tagid = 0;
int			g_tagidmax = 0;
int			g_tagcount = 0;
int			g_errors = 0;
int			g_processedfiles = 0;

// filter
/*
#define		MAX_TOKENS 256
int			g_tokencount = 0;
char*		g_tokenlist[MAX_TOKENS];
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
// Browse
//////////////////////////////////////////////////////////////////////////////////////////////////
#define GSFILTER		"gs files\0*.gs\0\0"
#define TXTFILTER		"txt files\0*.txt\0\0"

int OnBrowseOutputFile()
{
	char path[MAX_PATH];
	path[0]=0;

	char folder[MAX_PATH];
	folder[0] = 0;
	HWND hwnd = GetDlgItem(g_hwnd,IDC_EDIT1);
	GetWindowText(hwnd,folder,MAX_PATH);

	dword flags=0;
	//flags |= OFN_NOCHANGEDIR;
	//flags |= OFN_PATHMUSTEXIST;

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= g_hwnd;
	ofn.hInstance			= g_hinstance;
	ofn.lpstrFilter			= GSFILTER;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= path; // in+out
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= folder;
	ofn.lpstrTitle			= "Output File";
	ofn.Flags				= flags;
	ofn.nFileExtension		= 0; //out
	//ofn.lpstrDefExt			= ext;

	if(!GetOpenFileName( &ofn ))
		return 0;

	if(strstr(path,".gs")==NULL) // add extension
		strcat(path,".gs");
	
	hwnd = GetDlgItem(g_hwnd,IDC_EDIT2);
	SetWindowText(hwnd,path);

	return 1;
}

int OnBrowseDir()
{
	char path[MAX_PATH];
	path[0] = 0;

	BROWSEINFO bi;
	bi.hwndOwner = g_hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;
	bi.lpszTitle = "Input Folder";
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl;
	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL) return 0;
	if(!SHGetPathFromIDList(pidl, path)) return 0;

	int s=strlen(path);
	if(s==0) return 0;
	if(path[s-1]=='\\') path[s-1]=0;

	HWND hwnd = GetDlgItem(g_hwnd,IDC_EDIT1);
	SetWindowText(hwnd,path);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 Functions
//////////////////////////////////////////////////////////////////////////////////////////////////
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

	SetDlgItemInt(g_hwnd,IDC_TAGNO,1,FALSE);
	strcpy(g_intagprefix,"TXT_");
	SetDlgItemText(g_hwnd,IDC_TAGSRCPREFIX,g_intagprefix);
	strcpy(g_intagsufix,"");
	SetDlgItemText(g_hwnd,IDC_TAGSRCSUFIX,g_intagsufix);
	strcpy(g_outtagprefix,"#def TXT_");
	SetDlgItemText(g_hwnd,IDC_TAGOUTPREFIX,g_outtagprefix);
	strcpy(g_outtagsufix," ");
	SetDlgItemText(g_hwnd,IDC_TAGOUTSUFIX,g_outtagsufix);

	SetDlgItemText(g_hwnd,IDC_EXT1,"gs");
	SetDlgItemText(g_hwnd,IDC_EXT2,"gs1");
	SetDlgItemText(g_hwnd,IDC_EXT3,"gs2");
}

void OnHelp()
{
	MessageBox(g_hwnd,
		"\n" \
		"  Text Works v2.2 (2008)\n" \
		"  by Cristina Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you to export and reimport the texts from your game's scripts.\n" \
		"It can be used for spellcheck, translate, or localize the game.\n" \
		"\n" \
		"  EXPORT parses each file with extension #1, from the INPUT folder.\n" \
		"It looks for texts inside quotation marks and replaces them with input tags.\n" \
		"The result is saved in a similar file, with extension #2.\n" \
		"It also lists output tags and the coresponding texts, in the TAG file.\n" \
		"\n" \
		"  IMPORT parses each file with extension2, from the INPUT folder.\n" \
		"These are files exported in the previous process.\n" \
		"They contain input tags, instead of texts.\n" \
		"This process looks for these input tags and replaces them \n" \
		"with the coresponding texts, from the TAG file.\n" \
		"The result is saved in a similar file, with extension #3.\n" \
		"\n" \
		"  If texts in the TAG file remain unchanged, the files resulted from the IMPORT\n" \
		"process, should be the same to the original input files, from the EXPORT process.\n" \
		"\n" \
		"  The tool allows use of different file extensions for each process,\n" \
		" to prevent damage to the original source files. This requires later \n" \
		" renaming from the user.\n" \
		"\n" \
		"  Make a BACKUP of all your source files, before using this tool!\n" \
		"\n",
		
		"Text Works HELP", MB_OK);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
BOOL LoadTokenList()
{
	char filterfile[MAX_PATH];
	HWND hwndfilter = GetDlgItem(g_hwnd,IDC_EDIT3);
	GetWindowText(hwndfilter,filterfile,MAX_PATH);
	FILE *filter = fopen(filterfile,"rb");
	if(!filter) 
	{
		MSGERROR("Filter file not found.");
		return FALSE;
	}
	
	char *filterbuffer = NULL;
	fseek(filter,0,2);
	int filtersize = ftell(filter);
	fseek(filter,0,0);
	filterbuffer = (char*)smalloc(filtersize+1);
	fread(filterbuffer,filtersize,1,filter);
	filterbuffer[filtersize]=0;
	fclose(filter);
	
	BOOL ret = TRUE;
	char token[256];
	char* buffer=filterbuffer;
	int len;
	while(TRUE)
	{
		token[0]=0;
		if(!parser_skipspace(buffer,len)) break;
		buffer+=len;
		if(!parser_readword(buffer,token,256,len)) break;
		buffer+=len;
		if(token[0])
		{
			if(g_tokencount==MAX_TOKENS)
			{
				ret = FALSE;
				MSGERROR("Too many filters.");
				break;
			}
			g_tokenlist[g_tokencount] = strdup(token);
			g_tokencount++;
		}
	}

	sfree(filterbuffer);
	return ret;
}

void UnloadTokenList()
{
	for(int i=0;i<g_tokencount;i++)
		sfree(g_tokenlist[i]);
	g_tokencount=0;
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
void OnExport()
{
	g_tagid = GetDlgItemInt(g_hwnd,IDC_TAGNO,NULL,FALSE);
	if(g_tagid<=0) g_tagid = 1;
	GetDlgItemText(g_hwnd,IDC_TAGSRCPREFIX,g_intagprefix,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_TAGSRCSUFIX,g_intagsufix,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_TAGOUTPREFIX,g_outtagprefix,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_TAGOUTSUFIX,g_outtagsufix,MAX_PATH);

	LOG(NULL);
	LOGLN("------------------------------------------------");
	LOGLN("EXPORT");
	LOGLN("------------------------------------------------");

	char folder[MAX_PATH],filename[MAX_PATH],msg[1024];
	GetDlgItemText(g_hwnd,IDC_EDIT1,folder,MAX_PATH);	
	GetDlgItemText(g_hwnd,IDC_EDIT2,filename,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_EXT1,g_extensions[0],64);
	GetDlgItemText(g_hwnd,IDC_EXT2,g_extensions[1],64);
	GetDlgItemText(g_hwnd,IDC_EXT3,g_extensions[2],64);
	if(folder[0]==0)
	{
		MSGINFO("Input folder invalid.");
		LOGLN("Failed!");
		return;
	}
	if(filename[0]==0)
	{
		MSGINFO("Tag file invalid.");
		LOGLN("Failed!");
		return;
	}
	int l=strlen(folder);
	if(l==0 || folder[l-1]!='\\') strcat(folder,"\\");
	strcpy(g_outputfile,filename);
	
	/*
	if(IsDlgButtonChecked(g_hwnd,IDC_FILTER))
	{
		if(!LoadTokenList())
		{
			UnloadTokenList();
			return;
		}
	}

	// log tokens
	if(g_tokencount)
	{
		LOGLN("Filter functions:");
		for(int i=0;i<g_tokencount;i++)
			LOGLN(g_tokenlist[i]);
	}
	*/

	// check output file
	FILE* file = fopen(g_outputfile,"rb");
	if(file)
	{
		fclose(file);
		sprintf(msg,"File %s already exists!\nOverwrite it?\n",g_outputfile);
		if(MSGQUESTION(msg)==IDNO) { LOGLN("Aborted."); return; }
		file = NULL;
	}
	if(!file)
	{
		file = fopen(g_outputfile, "wb");
		if(!file) { MSGERROR("Invalid output file!"); LOGLN("Failed."); return; }
		fclose(file);
	}
	
	// get last tag id for all files from folder
	g_tagidmax = 0;
	g_tagcount = 0;
	file_findfiles(folder, OpGetLastTag, FILE_FINDREC);
    if(g_tagid<=g_tagidmax) 
	{
		sprintf(msg,"Input contains a tag with a higher value\nthan the one requested to start from.\nInput max tag is %i.\nContinue from this value?",g_tagidmax);
		if(MSGQUESTION(msg)==IDNO) { LOGLN("Aborted."); return; }
		g_tagid = g_tagidmax+1;
	}
	else
	if(g_tagcount>0)
	{
		sprintf(msg,"Input contains %i similar tags.\nInput max tag is %i.\nContinue?",g_tagcount,g_tagidmax);
		if(MSGQUESTION(msg)==IDNO) { LOGLN("Aborted."); return; }
	}

	sprintf(msg,"Starting from tag ID %i",g_tagid);
	LOGLN(msg);

	// process
	g_errors = 0;
	g_tagcount = 0;
	g_processedfiles = 0;
	file_findfiles(folder, OpExportFile, FILE_FINDREC);
	LOGLN("");

	// report
	sprintf(msg,"Processed %i files.",g_processedfiles); LOGLN(msg);
	if(g_tagcount>0)
	{
		sprintf(msg,"Exported %i texts (last tag=%i)", g_tagcount, g_tagid );
		LOGLN(msg);
	}
	else
	{
		LOGLN("No texts have been exported!");
	}

	if(g_errors)
	{
		sprintf(msg,"Encountered %i errors!", g_errors);
		LOGLN(msg);
		MSGINFO(msg);
	}
	else
		MSGINFO("Done.");

	// UnloadTokenList();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void OnImport()
{
	GetDlgItemText(g_hwnd,IDC_TAGSRCPREFIX,g_intagprefix,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_TAGSRCSUFIX,g_intagsufix,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_TAGOUTPREFIX,g_outtagprefix,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_TAGOUTSUFIX,g_outtagsufix,MAX_PATH);

	LOG(NULL);
	LOGLN("------------------------------------------------");
	LOGLN("IMPORT");
	LOGLN("------------------------------------------------");

	char folder[MAX_PATH],filename[MAX_PATH],msg[1024];
	GetDlgItemText(g_hwnd,IDC_EDIT1,folder,MAX_PATH);	
	GetDlgItemText(g_hwnd,IDC_EDIT2,filename,MAX_PATH);
	GetDlgItemText(g_hwnd,IDC_EXT1,g_extensions[0],64);
	GetDlgItemText(g_hwnd,IDC_EXT2,g_extensions[1],64);
	GetDlgItemText(g_hwnd,IDC_EXT3,g_extensions[2],64);
	int l=strlen(folder);
	if(l==0 || folder[l-1]!='\\') strcat(folder,"\\");
	strcpy(g_outputfile,filename);

	// check output file
	FILE* file = fopen(g_outputfile,"rb");
	if(!file)
	{
		MSGINFO("Output file not found!");
		LOGLN("Failed!");
		return;
	}
	else fclose(file);

	// process
	g_errors = 0;
	g_tagcount = 0; // count tags
	g_processedfiles = 0;
	file_findfiles(folder, OpImportFile, FILE_FINDREC);
	LOGLN("");

	// report
	sprintf(msg,"Processed %i files, %i tags.",g_processedfiles, g_tagcount);
	LOGLN(msg);
	if(g_errors)
	{
		sprintf(msg,"Encountered %i errors!", g_errors);
		LOGLN(msg);
		MSGINFO("Failed!");
	}
	else
		MSGINFO("Done.");
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
				case IDC_BROWSE1:
					OnBrowseDir();
					break;
				case IDC_BROWSE2:	
					OnBrowseOutputFile();	
					break;

				case IDC_EXPORT:		OnExport(); return TRUE;
				case IDC_IMPORT:		OnImport(); return TRUE;
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
