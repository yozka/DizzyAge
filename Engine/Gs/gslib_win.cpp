#include "stdafx.h"
#include "gslib_win.h"
#include "gs.h"
#include "shellapi.h"


/////////////////////////////////////////////////////////////////////////////////////////
// execute
/////////////////////////////////////////////////////////////////////////////////////////
int gsShell( gsVM* vm )
{
	guard(gsShell)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	char* app	= gs_tostr(vm,0);
	char* dir	= NULL;
	char* cmd	= NULL;
	if(gs_params(vm)>=2 && gs_cktype(vm,1,GS_STR)) dir = gs_tostr(vm,1);
	if(gs_params(vm)>=3 && gs_cktype(vm,2,GS_STR)) cmd = gs_tostr(vm,2);

	BOOL ret = (intptr)ShellExecute(NULL, "open", app, cmd, dir, SW_SHOW) > 32;
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsLaunch( gsVM* vm )
{
	guard(gsLaunch)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	char* app	= gs_tostr(vm,0);
	char* dir	= NULL;
	char* cmd	= NULL;
	if(gs_params(vm)>=2 && gs_cktype(vm,1,GS_STR)) dir = gs_tostr(vm,1);
	if(gs_params(vm)>=3 && gs_cktype(vm,2,GS_STR)) cmd = gs_tostr(vm,2);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb = sizeof(si);

	BOOL ret = CreateProcess(	app, 
								cmd,
								NULL, NULL,
								FALSE,
								0,
								NULL,
								dir,
								&si, &pi );
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsCommandLine( gsVM* vm )
{
	guard(gsCommandLine);
	char* cmd = GetCommandLine();
	gs_pushstr(vm,cmd);
	return 1;
	unguard();
}


/////////////////////////////////////////////////////////////////////////////////////////
// messages
/////////////////////////////////////////////////////////////////////////////////////////
int gsMessageBox( gsVM* vm )
{
	guard(gsMessageBox)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	char* text	= gs_tostr(vm,0);
	char* title	= NULL;
	int type = MB_OK;
	if(gs_params(vm)>=2 && gs_cktype(vm,1,GS_STR)) title = gs_tostr(vm,1);
	if(gs_params(vm)>=3 && gs_cktype(vm,2,GS_INT)) type = gs_toint(vm,2);

	int ret = MessageBox(NULL,text,title,type);
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsMessageBeep( gsVM* vm )
{
	guard(gsMessageBeep)
	dword type = 0xFFFFFFFF; // default
	if(gs_params(vm)==1 && gs_cktype(vm,0,GS_INT)) type = gs_toint(vm,1);
	int ret = MessageBeep(type);
	gs_pushint(vm,ret);
	return 1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// register lib
/////////////////////////////////////////////////////////////////////////////////////////
#define GS_REGCONST( c )	gs_regint( vm, #c, c );

int gslib_win( gsVM* vm )
{
	guard(gslib_win)
	
	// constants
	GS_REGCONST( MB_ABORTRETRYIGNORE	);
	GS_REGCONST( MB_OK					);
	GS_REGCONST( MB_OKCANCEL			);
	GS_REGCONST( MB_RETRYCANCEL			);
	GS_REGCONST( MB_YESNO				);
	GS_REGCONST( MB_YESNOCANCEL			);
	GS_REGCONST( MB_ICONEXCLAMATION		);
	GS_REGCONST( MB_ICONQUESTION		);
	GS_REGCONST( MB_ICONERROR			);
	GS_REGCONST( MB_SYSTEMMODAL			);
	GS_REGCONST( IDABORT				);
	GS_REGCONST( IDCANCEL				);
	GS_REGCONST( IDIGNORE				);
	GS_REGCONST( IDNO					);
	GS_REGCONST( IDOK					);
	GS_REGCONST( IDRETRY				);
	GS_REGCONST( IDYES					);
		
	// execute
	gs_regfn( vm, "gs_shell",			gsShell );			// str(app) [,str(dir) [,str(cmd)]] > int(1/0)
	gs_regfn( vm, "gs_launch",			gsLaunch );			// str(app) [,str(dir) [,str(cmd)]] > int(1/0)
	gs_regfn( vm, "gs_commandline",		gsCommandLine );	// > str(command line)

	// messages
	gs_regfn( vm, "gs_messagebox",		gsMessageBox );		// str(text) [,str(title) [,int(type)]] > int(ret)
	gs_regfn( vm, "gs_messagebeep",		gsMessageBeep );	// [int(type)] > int(1/0)

	return 0;	

	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
