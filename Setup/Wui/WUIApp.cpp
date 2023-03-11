//////////////////////////////////////////////////////////////////////////////////////////////////
// WUIApp.cpp
// Uses resources: IDI_ICON, IDC_CURSOR_HAND, IDC_CURSOR_FINGER
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Debug.h"
#include "WUIApp.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cWUIApp
//////////////////////////////////////////////////////////////////////////////////////////////////
cWUIApp*	cWUIApp::m_app						= NULL;
char		cWUIApp::m_name[MAX_PATH];
char		cWUIApp::m_version[32];
char	    cWUIApp::m_path[MAX_PATH];
HINSTANCE	cWUIApp::m_hinst					= NULL;
HINSTANCE	cWUIApp::m_hinst2					= NULL;
char*		cWUIApp::m_cmdline					= NULL;
int			cWUIApp::m_cmdshow					= 0;
HWND		cWUIApp::m_hwnd						= NULL;
BOOL		cWUIApp::m_rib						= FALSE;
HICON		cWUIApp::m_hicon					= NULL;
BOOL		cWUIApp::m_active					= FALSE;
BOOL		cWUIApp::m_minimized				= FALSE;
BOOL		cWUIApp::m_inmenuloop				= FALSE;
BOOL		cWUIApp::m_processcommand			= FALSE;

cWUIApp::cWUIApp()
{
	guard(cWUIApp::cWUIApp)
	sassert(m_app==NULL);
	m_app = this;
	m_path[0] = 0;
	GetModuleFileName( NULL, m_path, MAX_PATH );
	SetName("WUI App");
	SetVersion("v0.0");
	m_rib = TRUE;
	unguard()
}

cWUIApp::~cWUIApp()
{
	guard(cWUIApp::~cWUIApp)
	sassert(m_app==this);
	m_app = NULL;
	unguard()
}

BOOL cWUIApp::Init( HINSTANCE hinst, HINSTANCE hinst2, LPSTR cmdline, int cmdshow )
{
	guard(cWUIApp::Init)
	
	SetIcon( LoadIcon( hinst, MAKEINTRESOURCE(IDI_ICON) ) );
	m_hinst		= hinst;
	m_hinst2	= hinst2;
	m_cmdline	= cmdline;
	m_cmdshow	= cmdshow;

	// init ifile
	F9_Init();
	F9_ResourcesOpen(); // use resources

	// init resources
	InitWUIResource();

	// load default cursors
	WUIResource()->SetCursor(CURSOR_ARROW,	 LoadCursor(NULL,	IDC_ARROW));
	WUIResource()->SetCursor(CURSOR_HAND,	 LoadCursor(hinst,	MAKEINTRESOURCE(IDC_CURSOR_HAND)));
	WUIResource()->SetCursor(CURSOR_FINGER,	 LoadCursor(hinst,	MAKEINTRESOURCE(IDC_CURSOR_FINGER)));
	WUIResource()->SetCursor(CURSOR_WAIT,	 LoadCursor(NULL,	IDC_WAIT));

	// register class
	WndRegisterClass(
			HWNDCLASS_CWND, 
			CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
			hinst,
			m_hicon,
			WUIResource()->GetCursor(CURSOR_ARROW),
			NULL ); // (HBRUSH)(COLOR_WINDOW+1) using a brush here will mess the bkdc paint

	WUIScriptInit();

	return TRUE;
	unguard()
}

void cWUIApp::Done()
{
	guard(cWUIApp::Done);
	WUIScriptDone();
	DoneWUIResource();
	F9_ResourcesClose();
	F9_Done();
	unguard();
}

void cWUIApp::Run()
{
	guard(cWUIApp::Run)
	dlog(LOGAPP, "Main loop start.\n" );

	MSG		msg;
	BOOL	finished = FALSE;

	while( TRUE )
	{
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( GetMessage( &msg, NULL, 0, 0) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				finished = TRUE; break;
			}
		}

		if( finished ) break;

		if( (m_active && !m_minimized) || m_rib )
		{
			if( !MainLoop() ) break;
		}
		else
		{
			Sleep(10); // do something for windows
		}

		Sleep(1); // cpu cool

	}

	dlog(LOGAPP, "Main loop finished.\n");
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
