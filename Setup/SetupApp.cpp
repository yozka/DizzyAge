//////////////////////////////////////////////////////////////////////////////////////////////////
// SetupApp.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <tchar.h>
#include "D9Debug.h"
#include "SetupApp.h"
#include "resource.h"
#include "shlobj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cSetupApp
//////////////////////////////////////////////////////////////////////////////////////////////////
cSetupApp::cSetupApp()
{
	guard(cSetupApp::cSetupApp)
	SetName("Setup");
	SetVersion("v2.3");
	unguard()
}

cSetupApp::~cSetupApp()
{
	guard(cSetupApp::~cSetupApp)
	unguard()
}

BOOL cSetupApp::Init( HINSTANCE hinst, HINSTANCE hinst2, LPSTR cmdline, int cmdshow )
{
	guard(cSetupApp::Init)
	cWUIApp::Init(hinst,hinst2,cmdline,cmdshow);

	BOOL ok = WUIScriptLoad("setup.gs"); sassert(ok);
	RegisterFunc();

	char sz[320];
	sprintf(sz,"main(`%s`);",GetCmdLine());
	WUIScriptDo(sz);

	// obs: script is responsable for opening the pak file, if need

	if(cWnd::GetWindowCount()==0) return FALSE; // if no dialog window, exit app

	return TRUE;
	unguard()
}

void cSetupApp::Done()
{
	guard(cSetupApp::Done);
	F9_ArchiveClose(0);
	cWUIApp::Done();
	unguard();
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// SHELL LINK
// apppath = full app path (to the .exe)
// appdir = startup dir
// apparg = command line arguments
// linkpath = full link path (to the .lnk)
// linkdesc = link description name
// CoInitalize must have been called
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CreateShellLink( char* apppath, char* appdir, char* apparg, char* linkpath, char* linkdesc )
{
	guard(CreateShellLink)
	BOOL ret=FALSE;
    IShellLink* psl; 
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,  IID_IShellLink, (PVOID *) &psl); 
    if(SUCCEEDED(hres))
	{
		 
		IPersistFile* ppf; 
			
		psl->SetPath((LPCSTR)apppath); 
		psl->SetWorkingDirectory((LPCSTR)appdir);
		psl->SetArguments ((LPCSTR)apparg); 
		psl->SetDescription((LPCSTR)linkdesc); 
			
		hres = psl->QueryInterface(IID_IPersistFile, (PVOID *) &ppf);
		if(SUCCEEDED(hres)) 
		{ 
			word wsz[MAX_PATH]; 
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR) linkpath, -1, (LPWSTR)wsz, MAX_PATH); 
			hres = ppf->Save((LPCOLESTR)wsz, TRUE); 
			ppf->Release(); 
		} 
		psl->Release(); 
    } 

	return(SUCCEEDED(hres));
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// DRIVE SPACE
//////////////////////////////////////////////////////////////////////////////////////////////////
float GetDriveFreeBytes(char drive)
{
	guard(GetDriveFreeBytes)
	drive = (char)toupper(drive);
	sassert(drive >= 'A' && drive <= 'Z');
	char szdrive[8];
	sprintf(szdrive,"%c:\\",drive);
	
	float free = 0.0f;
	DWORD dwSectorsPerCluster, dwBytesPerSector, dwNumberOfFreeClusters, dwTotalNumberOfClusters;
	if (GetDiskFreeSpace((LPCSTR) szdrive,
						 &dwSectorsPerCluster,
						 &dwBytesPerSector,
						 &dwNumberOfFreeClusters,
						 &dwTotalNumberOfClusters))
	{
		free =	(float) dwNumberOfFreeClusters *
				(float) dwSectorsPerCluster    *
				(float) dwBytesPerSector;
	}
	
	return free;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// CREATE PATH
// create all the directories from the path
// can't know for shure if succeded since directory may already exist
//////////////////////////////////////////////////////////////////////////////////////////////////
void CreatePath( char* path )
{
	guard(CreatePath)
	if(!path || !path[0]) return;
	int len = strlen(path);
	int p=0;
	while(p<len)
	{
		if(path[p]=='\\')
		{
			path[p]=0;
			CreateDirectory(path,NULL);
			path[p]='\\';
		}
		p++;
	}
	CreateDirectory(path,NULL);
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// DELETE FILES
// delete executable and it's folder if empty;
// after calling this the programm needs to exit for the deletion to proceed
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL DelExe()
{
	guard(DelExe)

	HANDLE hfile;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	char  szBatFile[1000];		// batch file content
	char szExePath[MAX_PATH];
	char szExeDir[MAX_PATH];
	char szBatPath[MAX_PATH];
	char szBatDir[MAX_PATH];

	// Get the full pathname of our executable file.
	GetModuleFileName(NULL, szExePath, _MAX_PATH);
	strcpy(szExeDir, szExePath);
	*_tcsrchr(szExeDir, '\\') = 0;     // Chop off the file

	// Get the full pathname to temporary batch file; destination is the desktop
	HRESULT hr;
	LPITEMIDLIST pidl;
	hr = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
	if(FAILED(hr)) return FALSE;
	if(!SHGetPathFromIDList(pidl, szBatDir)) return FALSE;
	strcpy(szBatPath,szBatDir);
	strcat(szBatPath,"\\_deltmp.bat");

	// Create a batch file that continuously attempts to delete our executable file
	// When the executable no longer exists, remove its subdirectory (if empty), 
	// and then delete the batch file too.
	hfile = CreateFile(szBatPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hfile == INVALID_HANDLE_VALUE) return FALSE;

	// Construct the lines for the batch file.
	wsprintf(szBatFile,
	 __TEXT(":Repeat\r\n")
	 __TEXT("del \"%s\"\r\n")
	 __TEXT("if exist \"%s\" goto Repeat\r\n")
	 __TEXT("rmdir \"%s\"\r\n")						/* use /s /q for force remove */
	 __TEXT("del \"%s\"\r\n"), 
	 szExePath, szExePath, szExeDir, szBatPath);

	// Write the batch file and close it.
	DWORD writen;
	WriteFile(hfile, szBatFile, lstrlen(szBatFile) * sizeof(TCHAR), &writen, NULL);
	CloseHandle(hfile);

	// Get ready to spawn the batch file we just created.
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// We want its console window to be invisible to the user.
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	// Spawn the batch file with low-priority.
	if(!CreateProcess(	NULL, szBatPath, NULL, NULL, FALSE,
						IDLE_PRIORITY_CLASS, NULL, szBatDir, 
						&si, &pi)) 
	{
		dlog(LOGAPP,"DelExe: create process error.\n");
		return FALSE;
	}

	return TRUE;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Script functions
//////////////////////////////////////////////////////////////////////////////////////////////////
#define GS_ERROR( p1,p2,desc )	gs_error( vm, GSE_USER, p1, p2, desc );

int gsAppGetFreeSpace( gsVM* vm )
{
	guard(gsAppGetFreeSpace)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	char* path = gs_tostr(vm,0);
	int free = 0;
	if(strlen(path)>=2 && path[1]==':')  
	{
		free = (int)(GetDriveFreeBytes(path[0])/(1024*1024)); // in Mb
	}
	gs_pushint(vm,free);
	return 1;
	unguard()
}

int gsAppFileExist( gsVM* vm )
{
	guard(gsAppFileExist)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	char* filename = gs_tostr(vm,0);
	if(!filename) return 0;
	FILE* f=fopen(filename,"rb");
	int ok=(f!=NULL);
	if(f) fclose(f);
	gs_pushint(vm,ok);
	return 1;
	unguard()
}


int gsAppCopyFile( gsVM* vm )
{
	guard(gsAppCopyFile)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_STR)) GS_RETURNINT(vm,0);

	char* filename = gs_tostr(vm,0);
	char* path = gs_tostr(vm,1);
	if(!filename || !filename[0] || !path || !path[0]) { GS_ERROR(0,0,"invalide file or path"); GS_RETURNINT(vm,0); }

	CreatePath(path);

	char target[256];
	strcpy(target,path);

	strcat(target,filename);
	
	FILE* src=NULL;
	FILE* dst=NULL;
	int filesize=0;
	byte* buffer = NULL;
	
	src=fopen(filename,"rb"); if(!src) goto error;
	dst=fopen(target,"wb"); if(!dst) goto error;
	fseek(src,0,2); filesize = ftell(src); fseek(src,0,0);
	buffer = (byte*)smalloc(filesize);
	if(fread(buffer,1,filesize,src)!=filesize) goto error;
	if(fwrite(buffer,1,filesize,dst)!=filesize) goto error;
	fclose(src);
	fclose(dst);
	sfree(buffer);
	
	gs_pushint(vm,1); 
	return 1;

error:
	dlog(LOGAPP,"file copy failed '%s' to '%s'\n",filename,target);
	if(src) fclose(src);
	if(dst) fclose(dst);
	if(buffer) sfree(buffer);
	gs_pushint(vm,0);
	return 1;
	unguard()
}

int gsAppAddLink( gsVM* vm )
{
	guard(gsAppAddLinks)
	if(!gs_ckparams(vm,7))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,4,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,5,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,6,GS_STR))	GS_RETURNINT(vm,0);

	HRESULT hr;
	LPITEMIDLIST pidl;

	int target		= gs_toint(vm,0)?CSIDL_PROGRAMS:CSIDL_DESKTOP;
	char* linkdir	= gs_tostr(vm,1);
	char* linkname	= gs_tostr(vm,2);
	char* linkdesc	= gs_tostr(vm,3);
	char* apppath	= gs_tostr(vm,4);
	char* appdir	= gs_tostr(vm,5);
	char* apparg	= gs_tostr(vm,6);

	char linkpath[MAX_PATH];

	BOOL ok=FALSE;

	hr = SHGetSpecialFolderLocation(NULL, target, &pidl);
	if(SUCCEEDED(hr))
	{
		if(SHGetPathFromIDList(pidl, linkpath))
		{
			// link directory
			if(linkdir[0])
			{
				strcat(linkpath,"\\"); strcat(linkpath, linkdir);
				CreateDirectory(linkpath,0); // try to create
			}

			strcat(linkpath,"\\"); strcat(linkpath, linkname);
			
			ok = CreateShellLink( apppath, appdir, apparg, linkpath, linkdesc );
		}
	}

	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsAppDel( gsVM* vm )
{
	guard(gsAppDel)
	if(!gs_ckparams(vm,3))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT))	GS_RETURNINT(vm,0);

	int type = gs_toint(vm,2);
	int target = gs_toint(vm,0);
	char path[MAX_PATH]; path[0]=0;

	HRESULT hr;
	LPITEMIDLIST pidl;

	if(target!=0) // links
	{
		if(target==1)	hr = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
		else			hr = SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAMS, &pidl);
		char basepath[MAX_PATH];
		if(SHGetPathFromIDList(pidl, basepath))
		strcpy(path,basepath);
		strcat(path,"\\");
	}

	strcat(path,gs_tostr(vm,1));

	dlog(LOGAPP,"deleting %s ... ", path);
	BOOL ret; 
	if(type==0) // file
		ret = DeleteFile(path);
	else
		ret = RemoveDirectory(path);

	dlog(LOGAPP,ret?"ok\n":"fail\n");
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsAppDelExe( gsVM* vm )
{
	guard(gsAppDelExe)
	BOOL ret=DelExe();
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsAppGetGameInfo( gsVM* vm )
{
	guard(gsAppGetGameInfo)
	
	// load game inf file
	char* szbuffer = NULL;
	F9FILE f = F9_FileOpen("data\\dizzy.inf",0);
	if(!f) goto error;
	int size = F9_FileSize(f);
	if(size==0) goto error;
	szbuffer = (char*)smalloc(size+1);
	szbuffer[size]=0;
	F9_FileRead(szbuffer,size,f);
	F9_FileClose(f); f=NULL;
	
	// parse
	int pos;
	char* buffer = szbuffer;
	char* szkey = gs_tostr(vm,0);
	buffer = parser_skiptotoken(buffer,szkey,pos); if(!buffer) goto error;
	char szvalue[256];
	if(!parser_readvarstr(buffer,szkey,szvalue,256,pos)) goto error;
	sfree(szbuffer);

	// format
	size = strlen(szvalue);
	for(int i=0;i<size-1;i++)
	{
		if(szvalue[i]=='\\' && szvalue[i+1]=='n')
		{
			szvalue[i]=' ';
			szvalue[i+1]='\n';
		}
	}
	gs_pushstr(vm,szvalue);
	return 1;
	
	error:
	if(szbuffer) sfree(szbuffer);
	if(f) F9_FileClose(f);
	gs_pushstr(vm,""); 
	return 1;
	unguard()
}

int gsAppOpenPak( gsVM* vm )
{
	guard(gsAppOpenPak);
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	char* pakfile = gs_tostr(vm,0);
	int idx = F9_ArchiveOpen(pakfile, F9_READ | F9_ARCHIVE_PAK );
	gs_pushint(vm,idx!=-1);
	return 1;
	unguard();
}

void cSetupApp::RegisterFunc()
{
	guard(cSetupApp::RegisterFunc)

	gs_regfn( g_vm, "AppGetFreeSpace",		gsAppGetFreeSpace );	// str(path) > int(free MB/0)
	gs_regfn( g_vm, "AppCopyFile",			gsAppCopyFile );		// str(filename), str(path+'\') > int(1/0)
	gs_regfn( g_vm, "AppFileExist",			gsAppFileExist );		// str(filename) > int(1/0)
	gs_regfn( g_vm, "AppAddLink",			gsAppAddLink );			// int(target: 0=desktop, 1=programs), str(linkdir optional), str(linkname), str(linkdesc), str(apppath), str(appdir) > int(1/0)
	gs_regfn( g_vm, "AppDel",				gsAppDel );				// int(target: 0=current, 1=desktop, 2=programs), str(path), int(type:0=file,1=dir) > int(1/0)
	gs_regfn( g_vm, "AppDelExe",			gsAppDelExe );			// > int(1/0)
	gs_regfn( g_vm, "AppGetGameInfo",		gsAppGetGameInfo );		// str(filename), str(key) > str(value)
	gs_regfn( g_vm, "AppOpenPak",			gsAppOpenPak );			// str(filename) > int(1/0)
	unguard()
}
//////////////////////////////////////////////////////////////////////////////////////////////////
