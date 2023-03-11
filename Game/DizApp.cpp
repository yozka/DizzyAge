///////////////////////////////////////////////////////////////////////////////////////////////////
// DizApp.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Resource.h"
#include "E9App.h"
#include "DizApp.h"
#include "DizDebug.h"

#include "DizCfg.h"
#include "DizGame.h"
#include "DizSound.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DialogProcInfo( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
	guard(DialogProcInfo)
	if( uMsg==WM_INITDIALOG ) 
	{
		// create info content
		char sz[512];
		strcpy(sz,g_cfg.GetInfoValue("game_title"));
		strcat(sz," v"); strcat(sz, g_cfg.GetInfoValue("game_version"));
		strcat(sz,"\r\n");
		strcat(sz,"by "); strcat(sz, g_cfg.GetInfoValue("game_author"));
		strcat(sz,"\r\n");
		strcat(sz,g_cfg.GetInfoValue("game_website"));
		SetDlgItemText(hwndDlg,IDC_TEXT1,sz);
		strcpy(sz,"Created with DizzyAGE v"); strcat(sz,g_cfg.GetInfoValue("dizzyage_version"));
		strcat(sz,"\r\n");
		strcat(sz,"by Alexandru and Cristina Simion\r\nhttp://www.yolkfolk.com/dizzyage");
		SetDlgItemText(hwndDlg,IDC_TEXT2,sz);
		return TRUE; // autofocus
	}
	if( uMsg==WM_CLOSE || (uMsg==WM_COMMAND && (LOWORD(wParam)==IDOK || LOWORD(wParam)==IDCANCEL) ) )
	{ EndDialog(hwndDlg,0); return TRUE; }
	return FALSE;
	unguard()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cDizApp::cDizApp()
{
	guard(cDizApp::cDizApp);
	m_gamefps = 0;
	m_drawstats = 0;
	m_musicwaspaused = FALSE;
	unguard();
}

BOOL cDizApp::Init()
{
	guard(cDizApp::Init);
	dlog(LOGAPP,"App init.\n");
	
	// engine
	if(!InitApp())	 { ERRORMESSAGE("Init app error.");   return FALSE; }
	if(!InitFiles()) { ERRORMESSAGE("Init files error."); return FALSE; }
	if(!InitInput()) { ERRORMESSAGE("Init input device error."); return FALSE; }
	if(!InitAudio()) { ERRORMESSAGE("Init audio device error."); } // allow to run without audio
	if(!InitVideo()) { ERRORMESSAGE("Init video device error."); return FALSE; }

	// game init
	g_cfg.Init();
	g_dizdebug.Init();
	g_paint.Init();
	g_map.Init();
	g_game.Init();
	g_sound.Init();
	if(!g_script.Init()) { ERRORMESSAGE("Script compiling error."); return FALSE; }

	// game start
	g_game.Start();

	return TRUE;
	unguard();
}

BOOL cDizApp::InitApp()
{
	guard(cDizApp::InitApp);
	E9_AppSetStr(E9_APP_NAME,GAME_NAME);
	E9_AppSetStr(E9_APP_ICON,MAKEINTRESOURCE(IDI_ICON));

	BOOL cool = TRUE;
	ini_getint( file_getfullpath(GetIniFile()), "ADVANCED", "cool",	&cool );
	E9_AppSetInt(E9_APP_COOL,cool);
	
	return TRUE;
	unguard();
}

BOOL cDizApp::InitFiles()
{
	guard(cDizApp::InitFiles);
	BOOL ok = F9_Init();
	if(!ok) return FALSE;
	// add pak archive
	char* pakfile = GetPakFile();
	int arc = F9_ArchiveOpen(pakfile, F9_READ | F9_ARCHIVE_PAK );
	if(arc==-1)
		dlog(LOGAPP,"Pak archive %s not found, running from data folder.\n",pakfile);
	else
		dlog(LOGAPP,"Pak archive %s opened, data folder is ignored.\n",pakfile);
	return TRUE;
	unguard();
}

BOOL cDizApp::InitInput()
{
	guard(cDizApp::InitInput);
	char inifile[256];
	strcpy( inifile, file_getfullpath(GetIniFile()) );

	int input_enabled = 1;
	ini_getint( inifile, "INPUT", "enabled",	&input_enabled );
	if(!input_enabled) return TRUE; // no input
	
	BOOL ok = I9_Init(E9_GetHWND(),E9_GetHINSTANCE(),I9_API_DEFAULT);
	if(!ok) return FALSE;

	// init devices
	int keyboard	= 1;
	int mouse		= 0;
	int joystick	= 1;
	ini_getint( inifile, "INPUT", "keyboard",	&keyboard );
	ini_getint( inifile, "INPUT", "mouse",		&mouse );
	ini_getint( inifile, "INPUT", "joystick",	&joystick );
	if(keyboard)	ok = I9_DeviceInit(I9_DEVICE_KEYBOARD);
	if(mouse)		ok = I9_DeviceInit(I9_DEVICE_MOUSE);
	if(joystick)	ok = I9_DeviceInit(I9_DEVICE_JOYSTICK1);

	// rumble support
	if(I9_DeviceIsPresent(I9_DEVICE_JOYSTICK1))
		I9_DeviceFFInit(I9_DEVICE_JOYSTICK1);
	
	return TRUE;
	unguard();
}

BOOL cDizApp::InitAudio()
{
	guard(cDizApp::InitAudio);
	char inifile[256];
	strcpy( inifile, file_getfullpath(GetIniFile()) );

	int audio_enabled = 1;
	ini_getint( inifile, "AUDIO", "enabled",	&audio_enabled  );
	if(!audio_enabled) return TRUE; // no audio
	
	BOOL ok = A9_Init(E9_GetHWND(),A9_API_DEFAULT);
	if(!ok) return FALSE;
	return TRUE;	
	unguard();
}

BOOL cDizApp::InitVideo()
{
	guard(cDizApp::InitVideo);
	char inifile[256];
	strcpy( inifile, file_getfullpath(GetIniFile()) );

	// load config
	int api;
	r9Cfg cfg;
	g_cfg.LoadRenderCfg(cfg,api);

	// init interface
	if(!R9_InitInterface(api)) return FALSE;

	BOOL ok = R9_Init(E9_GetHWND(),&cfg,api);
	if(!ok) // try the other api
	{
		dlog(LOGERR,"RENDER: init %s (api %i) failed, try the other api.\n",api?"OpenGL":"DirectX9",api);
		ok = R9_Init(E9_GetHWND(),&cfg,!api);
		if(!ok)	return FALSE;
	}

	R9_SetState(R9_STATE_FILTER,FALSE);
	E9_AppSetInt(E9_APP_WINDOWED,cfg.m_windowed);

	return TRUE;
	unguard();
}

void cDizApp::Done()
{
	guard(cDizApp::Done);
	// must be able to destroy partial init too, in case Init has failed

	// game
	g_script.Done();
	g_sound.Done();
	g_game.Done();
	g_map.Done();
	g_paint.Done();
	g_cfg.Done();

	// debug
	g_dizdebug.Done();

	// engine
	R9_Done();
	R9_DoneInterface();
	A9_Done();
	I9_Done();
	F9_ArchiveClose(0); // close first archive if found
	F9_Done();

	dlog(LOGAPP,"App done.\n");
	unguard();
}

void cDizApp::Activate( BOOL active )
{
	guard(cDizApp::Activate);
	if(active)
	{
		if(I9_IsReady()) I9_Acquire();
		g_game.FFFXUpdate();
		if(!m_musicwaspaused) g_sound.MusicPause(FALSE); // unpause
	}
	else
	{
		if(I9_IsReady()) I9_Unacquire();
		m_musicwaspaused = g_sound.m_musicpaused;
		if(!m_musicwaspaused) g_sound.MusicPause(TRUE); // pause
	}
	unguard();
}

BOOL cDizApp::ToggleVideo()
{
	guard(cDizApp::ToggleVideo);
	if(!R9_GetCfg().m_windowed) return FALSE; // toggle only in windowed mode (not a hw restriction though)

	dlog(LOGAPP,"Toggle video.\n");
	int scrwidth = sys_desktopwidth();
	int scrheight = sys_desktopheight();
	int w = R9_GetCfg().m_width;
	int h = R9_GetCfg().m_height;
	static BOOL maximized = FALSE;

	maximized = !maximized; // toggle

	// make cfg
	int api;
	r9Cfg cfg;
	g_cfg.LoadRenderCfg(cfg,api);

	if(maximized) // overwrite width and height - pseudo full screen
	{
		cfg.m_width		= scrwidth;
		cfg.m_height	= scrheight;
	}

	//unaquire
	g_paint.Unacquire();
	R9_Done();
	
	// re-init render
	BOOL ok;
	ok = R9_Init(E9_GetHWND(),&cfg,api);
	if(!ok) // try to go back
	{
		dlog(LOGERR,"RENDER: re-init failed; trying to restore original cfg.\n");
		g_cfg.LoadRenderCfg(cfg,api);
		BOOL ok = R9_Init(E9_GetHWND(),&cfg,api);
		if(!ok)	{ dlog(LOGERR, "RENDER: critical error!\n"); return FALSE; }
	}

	g_cfg.m_scale = 0; // full scale

	// reacquire
	R9_SetState(R9_STATE_FILTER,FALSE);
	E9_AppSetInt(E9_APP_WINDOWED,R9_GetCfg().m_windowed);
	E9_AppSetCursor(R9_GetCfg().m_windowed ? E9_CURSOR_ARROW : E9_CURSOR_NONE);
	g_dizdebug.Layout();
	g_paint.Reacquire();
	g_paint.Layout();

	return TRUE;
	unguard();
}

BOOL cDizApp::Update()
{
	guard(cDizApp::Update);

	// timing
	static int timergame = 0;	// timer for game
	static int timersec=0;		// timer for one sec
	static int gameframecount = 0;
	
	timersec += E9_AppGetInt(E9_APP_DELTATIME);
	if(timersec>=1000)
	{
		timersec %= 1000;
		m_gamefps = gameframecount;
		gameframecount=0;
	}

	// input
	float dtime = (float)E9_AppGetInt(E9_APP_DELTATIME) / 1000.0f;
	if(I9_IsReady()) I9_Update(dtime);

	g_sound.Update(); // update sounds

	timergame += E9_AppGetInt(E9_APP_DELTATIME);
	int gamefps = g_game.Get(G_FPS); // game fps
	if(gamefps<1) gamefps=1;
	int gameframetime = 1000/gamefps;
	if(timergame>=gameframetime)
	{
		timergame %= gameframetime;
		gameframecount++;

		// game
		if(!g_game.Update()) return FALSE;

		if( IS_DEVELOPER() )
			g_script.CallHandler(HANDLER_DEBUG); // debug script callback

	}

	// debug
	if(!g_dizdebug.Update()) return FALSE;

	// functional keys
	if( I9_IsReady() )
	{
		BOOL ctrl  = I9_GetKeyValue(I9K_LCONTROL) || I9_GetKeyValue(I9K_RCONTROL);
		if( I9_GetKeyDown(I9K_F1) && E9_AppGetInt(E9_APP_WINDOWED) ) DialogBox( E9_GetHINSTANCE(), MAKEINTRESOURCE(IDD_INFO), E9_GetHWND(), DialogProcInfo );
		if( I9_GetKeyDown(I9K_F10) ) 
		{
			// obsolete
			// R9_ToggleVideoMode(); // toggle windowed mode (F10)
			// E9_AppSetInt(E9_APP_WINDOWED,R9_GetCfg().m_windowed);
			// g_paint.Layout();

			BOOL ok = ToggleVideo();
			if(!ok) return FALSE;
		}
		if( I9_GetKeyDown(I9K_F11) ) m_drawstats = !m_drawstats;
		if( I9_GetKeyDown(I9K_F9) && A9_IsReady() )	// toggle volume
		{
			static int volume=-1;
			int vol;
			if(volume==-1)
			{
				volume = A9_VolumeDecibelToPercent( A9_Get(A9_MASTERVOLUME) );
				vol = 0;
				dlog(LOGAPP,"sound off\n");
			}
			else
			{
				vol = volume;
				volume = -1;
				dlog(LOGAPP,"sound on\n");
			}
			A9_Set(A9_MASTERVOLUME, A9_VolumePercentToDecibel(vol) );
		}
		if(I9_GetKeyDown(I9K_SYSRQ)) // print screen
		{
			fRect r(0,0,R9_GetWidth(),R9_GetHeight());
			R9_SaveScreenShot(&r,!ctrl);
		}
	}

	return TRUE;
	unguard();
}

void cDizApp::Draw()
{
	guard(cDizApp::Draw);
	if(!R9_IsReady()) return; // avoid painting if render is not ready
	R9_CheckDevice(); // check for lost device
	if(R9_BeginScene())
	{
		R9_Clear(g_game.Get(G_MAPCOLOR)|0xff000000);
		// game
		if(g_game.m_drawmode!=DRAWMODE_NONE) g_game.Draw();

		// stats
		if(m_drawstats) DrawStats();

		// debug
		g_dizdebug.Draw();

		R9_EndScene();
		R9_Present();
	}

	unguard();
}

void cDizApp::DrawStats()
{
	guard(cDizApp::DrawStats)
	char sz[128];
	sprintf(sz, "obj:%i, brs:%i, fps:%i/%i", g_game.m_objcount, g_game.m_visible_brushes, (int)m_gamefps, E9_AppGetInt(E9_APP_FPS));

	float w = (float)R9_CHRW*(int)strlen(sz)+4;
	float h = (float)R9_CHRH+4;
	float x=R9_GetWidth()-w-2;
	float y=2;
	R9_DrawBar( fRect(x,y,x+w,y+h), 0xa0000000 );
	R9_DrawText( fV2(x+2,y+2), sz, 0xffffff80 );
	
	R9_Flush();
	unguard()
}

void cDizApp::ErrorMessage( char* msg )
{
	dlog(LOGERR, "DizzyAGE ERROR:\n%s\n", msg);
	sys_msgbox( E9_GetHWND(), msg, "DizzyAGE ERROR", MB_OK );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
