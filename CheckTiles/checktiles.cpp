//////////////////////////////////////////////////////////////////////////////////////////////////
// CheckTiles.cpp
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

#define APPTITLE						"DizzyAGE CheckTiles v2.2"
#define MSGINFO( text )					MessageBox( g_hwnd, text, "CheckTiles", MB_OK | MB_ICONINFORMATION )
#define MSGERROR( text )				MessageBox( g_hwnd, text, "CheckTiles", MB_OK | MB_ICONERROR )
#define MSGQUESTION( text )				MessageBox( g_hwnd, text, "CheckTiles", MB_YESNO | MB_ICONQUESTION )

HWND		g_hwnd = NULL;
HWND		g_hwndlog = NULL;
HINSTANCE	g_hinstance = NULL;

#define MAP_ID							"dizzymap"
#define MAP_CHUNKID						0x11111111
#define MAP_CHUNKBRUSHES				0x88888889
#define	BRUSH_MAX						48
#define	BRUSH_TILE						5

//////////////////////////////////////////////////////////////////////////////////////////////////
// UTIL
//////////////////////////////////////////////////////////////////////////////////////////////////

// map tiles
int		g_maptilecount;
int		g_maptilemax;
int*	g_maptile;

// files
int		g_filecount;
int		g_filemax;
char**	g_file;

void FindFileCallback( char* filepath, BOOL dir )
{
	char input[MAX_PATH]; input[0]=0;
	GetDlgItemText(g_hwnd,IDC_EDIT2,input,MAX_PATH);
	if(0!=strstr(filepath,input)) return; // weird
	filepath += strlen(input); // use relative path

	if(g_filecount==g_filemax)
	{
		g_filemax += 32;
		g_file = (char**)realloc(g_file, g_filemax*sizeof(char*));
	}
	g_file[g_filecount] = strdup(filepath);
	g_filecount++;
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
}

void OnHelp()
{
	MessageBox(g_hwnd,
		"\n" \
		"  DizzyAGE CheckTiles v2.2 (2008)\n" \
		"  by Alexandru Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  This tool helps you find unused or lost tiles,\n" \
		"  before releasing your game, to minimize it's size.\n" \
		"\n" \
		"  1. Select the map file of your game.\n" \
		"  2. Select the tiles folder location of your game.\n" \
		"  3. Proceed with checking.\n" \
		"  4. Edit results by removing those files you don't want to delete.\n" \
		"  5. Delete unused tile files.\n" \
		"\n" \
		"Consider that some of the tile files, even if not used in map, should not be deleted.\n" \
		"Some of them may be used by engine or by scripts, like those from the menu or the player himself.\n" \
		"Edit with care the results before you delete.\n"
		"\n" \
		"The lost tiles are also reported.\n" \
		"Those are tiles are used in the map but not found in the selected tiles folder.\n" \
		"You will have to add those manually to have a complete map.\n" \
		"\n",
		
		"CheckTiles HELP", MB_OK);
}

int OnBrowseFile()
{
	char path[MAX_PATH];
	path[0]=0;
	

	dword flags=0;
	flags |= OFN_NOCHANGEDIR;
	flags |= OFN_PATHMUSTEXIST;
	char title[128]; title[0]=0;

	strcpy(title,"Select map file");
		
	char ext[] = "map";
	char filter[64] = "*.map\0*.map\0\0";

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
	if(!ok) return 0;

	// write
	HWND hwnd = GetDlgItem(g_hwnd,IDC_EDIT1);
	SetWindowText(hwnd,path);
		
	return 1;
}

int OnBrowseDir()
{
	char folder[MAX_PATH];
	folder[0]=0;

	char title[128];
	strcpy( title, "Select tiles directory" );
	
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
	HWND hwnd = GetDlgItem(g_hwnd,IDC_EDIT2);
	SetWindowText(hwnd,folder);
	return 1;
}

void OnProceed()
{
	int unusedfiles = 0;
	int losttiles	= 0;

	// read file
	char input[MAX_PATH];
	GetDlgItemText(g_hwnd,IDC_EDIT1,input,MAX_PATH);
	if(input[0]==0) { MSGERROR("Select a DizzyAGE map file first."); return; }
	FILE* file=fopen(input,"rb");
	if(!file) { MSGERROR("Failed to open the map file.\nBrowse for a valid DizzyAGE map."); return; }
	fseek(file,0,2);
	int filesize = ftell(file);
	fseek(file,0,0);
	if(filesize==0) { fclose(file); MSGERROR("Invalid DizzyAGE map file."); return; }

	// map tiles
	g_maptilecount	= 0;
	g_maptilemax	= 0;
	g_maptile		= NULL;

	// read chunks
	int i,j;
	int size;
	int chunkid=0;
	int chunksize=0;
	int brushcount=0;

	while(true)
	{
		if( fread(&chunkid,1,4,file)!=4 )	goto errormap;
		if( fread(&chunksize,1,4,file)!=4 )	goto errormap;
		
		switch(chunkid)
		{
			case MAP_CHUNKID:
			{
				if( chunksize!=(int)strlen(MAP_ID) )	goto errormap;
				char* buffer = (char*)malloc(chunksize);
				size = 0;
				size += fread(buffer, 1,chunksize, file);
				if(size!=chunksize) { free(buffer);  goto errormap; }
				if(memcmp(buffer,MAP_ID,chunksize)!=0) { free(buffer);  goto errormap; }
				free(buffer);
				break;
			}

			case MAP_CHUNKBRUSHES:
			{
				if(chunksize%(BRUSH_MAX*4)!=0) goto errormap;
				int data[BRUSH_MAX];
				size = 0;
				while(size<chunksize)
				{
					int ret = fread(data, 1, BRUSH_MAX*4, file);
					if(ret!=BRUSH_MAX*4) goto errormap;
					size += ret;
					brushcount++;
					int tile = data[BRUSH_TILE];
					
					// add tile (no duplicates)
					for(i=0;i<g_maptilecount;i++)
						if(g_maptile[i]==tile) break;

					if(i==g_maptilecount)
					{
						if(g_maptilecount==g_maptilemax)
						{
							g_maptilemax += 32;
							g_maptile = (int*)realloc(g_maptile,g_maptilemax*sizeof(int));
						}
						g_maptile[g_maptilecount] = tile;
						g_maptilecount++;
					}
				}				   
				break;
			}
			
			default:
			{
				// skip other chunks
				if(chunksize>0) fseek(file,chunksize,1);
			}
		}
		if( ftell(file)>=filesize) break;
	}

	fclose(file);

	// find files
	g_filecount	= 0;
	g_filemax	= 0;
	g_file		= NULL;
	
	input[0]=0;
	GetDlgItemText(g_hwnd,IDC_EDIT2,input,MAX_PATH);
	if(input[0]==0) { MSGERROR("Select the path to the tiles folder.\n Named \"Tiles\"."); goto errorexit; }
	if(input[strlen(input)-1]!='\\') { strcat(input,"\\"); SetDlgItemText(g_hwnd,IDC_EDIT2,input); }
	file_findfiles( input, FindFileCallback, FINDFILE_REC );

	// process unused
	Log(g_hwndlog,NULL);
	Log(g_hwndlog,"\r\n> TILE FILES (UNUSED IN MAP)\r\n");
	for(i=0;i<g_filecount;i++)
	{
		char path[MAX_PATH];
		strcpy(path,g_file[i]);
		char* filename = file_path2file(path);
		char* fileext = file_path2ext(path);
		if(!filename || !fileext) continue; // weird

		int id = -1;
		char szt[128];
		int ret = sscanf(filename,"%i %s",&id,szt);

		if( ret!=0 && id!=-1 && 0==stricmp(fileext,"tga") ) // acceptable tile file
		{
			// test used
			for(j=0;j<g_maptilecount;j++)
				if(g_maptile[j]==id) break;
			if(j<g_maptilecount) continue; // if used, don't report it
		}
		
		// report as unused
		strcat(path,"\r\n");
		Log(g_hwndlog,path);
		unusedfiles++;
	}
	if(unusedfiles==0) Log(g_hwndlog,"> none\r\n");

	// process lost
	Log(g_hwndlog,"\r\n> LOST TILES (USED IN MAP)\r\n");
	for(i=0;i<g_maptilecount;i++)
	{
		for(j=0;j<g_filecount;j++)
		{
			char* filename = file_path2file(g_file[j]);
			char* fileext = file_path2ext(g_file[j]);
			if(!filename || !fileext) continue; // weird

			int id = -1;
			char szt[128];
			int ret = sscanf(filename,"%i %s",&id,szt);
			if( ret==0 || id==-1 || 0!=stricmp(fileext,"tga") ) continue; // unacceptable tile file
			
			if( id==g_maptile[i] ) break; // found
		}		
		if(j<g_filecount) continue;	// found
		char szid[64];
		sprintf(szid,"> %i\r\n",g_maptile[i]);
		Log(g_hwndlog,szid);
		losttiles++;
	}
	if(losttiles==0) Log(g_hwndlog,"> none\r\n");

	char sz[256];
	Log(g_hwndlog,"\r\n> PROCESS RESULTS\r\n");
	sprintf(sz,"> %i total brushes in map\r\n> %i total tiles in map\r\n> %i total files on disk\r\n> %i used files\r\n> %i unused files\r\n> %i lost tiles\r\n",
			brushcount,g_maptilecount,g_filecount,g_filecount-unusedfiles,unusedfiles,losttiles);
	Log(g_hwndlog,sz);
	MSGINFO("Map processed successfully!");
	return;

errormap:
	MSGERROR("Invalid DizzyAGE map.");
errorexit:
	if(file) fclose(file);
	if(g_maptilecount) free(g_maptile);
	if(g_filecount)
	{
		for(i=0;i<g_filecount;i++) free(g_file[i]);
		free(g_file);
	}
	g_maptilecount = 0;
	g_maptile = NULL;
	g_filecount = 0;
	g_file = NULL;
	return;
}

void OnDelete()
{
	char input[MAX_PATH];
	GetDlgItemText(g_hwnd,IDC_EDIT2,input,MAX_PATH);
	if(input[0]==0) { MSGERROR("Tiles folder missing."); return; }
	if(input[strlen(input)-1]!='\\') { strcat(input,"\\"); SetDlgItemText(g_hwnd,IDC_EDIT2,input); }
	int inputlen = strlen(input);

	// parce log text
	char* buffer;
	int buffersize;
	buffersize = GetWindowTextLength(g_hwndlog)+1;
	if(buffersize==1) { MSGERROR("No files to delete.\nProceed with checking first.\n"); return; }
	buffer = (char*)malloc(buffersize); buffer[0]=0;
	GetWindowText(g_hwndlog,buffer,buffersize);

	int ret=MSGQUESTION("Are you sure you want to delete the selected files?\n\n" \
						"Only tile files that are not used by map nor by scripts should be selected.\n" \
						"Make sure you have removed from the selection those tiles you use in scripts,\n" \
						"like menu or player tiles. It is also advised to have a backup.");
	if(ret!=IDYES) { free(buffer); return; }



	Log(g_hwndlog,"\r\n> DELETE RESULTS\r\n");
	int filecount = 0;
	int deletecount = 0;
	char* sz = buffer;
	while(sz[0])
	{
		int ok;
		char* sz2 = strstr(sz,"\r\n");
		if(sz2==NULL) break; // done
		*sz2=0; // tmp
		if(strlen(sz)==0) goto nextone; // invalid
		if(*sz=='>') goto nextone; // invalid
		input[inputlen]=0;
		strcat(input,sz);
		filecount++;
		ok = DeleteFile(input);
		if(!ok) { Log(g_hwndlog,"> failed: "); Log(g_hwndlog,input); Log(g_hwndlog,"\r\n"); }
		else	deletecount++;
	nextone:
		sz =sz2+2;
	}
	char sztmp[128];
	if(filecount>deletecount)
		sprintf(sztmp,"> %i failed to be deleted from a total of %i files\r\n",filecount-deletecount,filecount);
	else
		sprintf(sztmp,"> %i files deleted\r\n",deletecount);
	Log(g_hwndlog,sztmp);
	
	free(buffer);
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
				case IDC_BROWSE1:		OnBrowseFile(); return TRUE;
				case IDC_BROWSE2:		OnBrowseDir(); return TRUE;
				case IDC_PROCEED:		OnProceed(); return TRUE;
				case IDC_DELETE:		OnDelete(); return TRUE;
				case IDC_EXIT:			EndDialog(hwndDlg,0); return TRUE;
				case IDC_HLP:			OnHelp(); return TRUE;
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
			if(ext==NULL) // consider it the Tiles folder
				SetDlgItemText(g_hwnd,IDC_EDIT2,filepath);
			else
				if(0==strcmp(ext,"map")) // map file
					SetDlgItemText(g_hwnd,IDC_EDIT1,filepath);
				else
					MSGERROR("Invalid file format.\nDrag and drop the map file or the tiles folder.");
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
