//////////////////////////////////////////////////////////////////////////////////////////////////
// DizCfg.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DizCfg.h"
#include "DizDef.h"

#include <string>

cDizCfg g_cfg;

char* GetIniFile()
{
	static std::string nameTest = "D:/Projects/Smartphones/DizzyAGE/DATA/dizzy.ini";
	return (char*)nameTest.c_str();

	static char name[64] = "dizzy.ini";
	char apppath[MAX_PATH];
	if(!GetModuleFileName( NULL, apppath, MAX_PATH )) return name;
	strcpy(name,file_path2file(apppath));
	strcpy(file_path2ext(name),"ini");
	return name;
}

char* GetPakFile()
{
	static std::string nameTest = "D:/Projects/Smartphones/DizzyAGE/DATA/dizzy_2.pak";
	return (char*)nameTest.c_str();

	static char name[64] = "dizzy.pak";
	char apppath[MAX_PATH];
	if(!GetModuleFileName( NULL, apppath, MAX_PATH )) return name;
	strcpy(name,file_path2file(apppath));
	strcpy(file_path2ext(name),"pak");
	return name;
}

char* GetLogFile()
{
	static char name[64] = "dizzy.log";
	char apppath[MAX_PATH];
	if(!GetModuleFileName( NULL, apppath, MAX_PATH )) return name;
	strcpy(name,file_path2file(apppath));
	strcpy(file_path2ext(name),"log");
	return name;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////
cDizCfg::cDizCfg()
{
	guard(cDizCfg::cDizCfg)

	m_scale					= 0;

	m_volfx					= 100;
	m_volmusic				= 100;
	
	m_key[KEY_LEFT][0]		= I9K_LEFT;
	m_key[KEY_RIGHT][0]		= I9K_RIGHT;
	m_key[KEY_UP][0]		= I9K_UP;
	m_key[KEY_DOWN][0]		= I9K_DOWN;
	m_key[KEY_JUMP][0]		= I9K_SPACE;
	m_key[KEY_ACTION][0]	= I9K_RETURN;
	m_key[KEY_MENU][0]		= I9K_ESCAPE;
	
	m_key[KEY_LEFT][1]		= I9K_Z;
	m_key[KEY_RIGHT][1]		= I9K_X;
	m_key[KEY_UP][1]		= I9K_K;
	m_key[KEY_DOWN][1]		= I9K_M;
	m_key[KEY_JUMP][1]		= I9K_SPACE;
	m_key[KEY_ACTION][1]	= I9K_RETURN;
	m_key[KEY_MENU][1]		= I9K_Q;

	m_rumble				= TRUE;
	m_deadzone[0]			= 500;
	m_deadzone[1]			= 500;
	m_joy[0]				= 0;
	m_joy[1]				= 1;
	m_joy[2]				= 2;
	m_joy[3]				= 3;
	m_joy[4]				= 0;
	m_joy[5]				= 1;

	m_info					= NULL;

	unguard()
}

void cDizCfg::Init()
{
	guard(cDizCfg::Init)
	Load(); Save();
	// read info file
	F9FILE f = F9_FileOpen("Data\\dizzy.inf");
	if(!f) { dlog(LOGAPP, "dizzy.inf not found\n"); return; }
	int size = F9_FileSize(f);
	if(size==0) { F9_FileClose(f); return; }
	m_info = (char*)smalloc(size+1); m_info[size]=0;
	F9_FileRead(m_info,size,f);
	F9_FileClose(f);
	unguard()
}

void cDizCfg::Done()
{
	guard(cDizCfg::Done)
	if(m_info) sfree(m_info);
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD SAVE
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cDizCfg::Load()
{
	guard(cDizCfg::Load)
	char inifile[256];

	// USER
	strcpy( inifile, file_getfullpath(GetIniFile()) );

	// VIDEO
	ini_getint( inifile, "VIDEO",	"scale",		&m_scale				);

	// AUDIO
	ini_getint( inifile, "AUDIO",	"volfx",		&m_volfx				);
	ini_getint( inifile, "AUDIO",	"volmusic",		&m_volmusic				);

	// INPUT
	ini_getint( inifile, "INPUT",	"key1_left",	&m_key[KEY_LEFT][0]		);
	ini_getint( inifile, "INPUT",	"key1_right",	&m_key[KEY_RIGHT][0]	);
	ini_getint( inifile, "INPUT",	"key1_up",		&m_key[KEY_UP][0]		);
	ini_getint( inifile, "INPUT",	"key1_down",	&m_key[KEY_DOWN][0]		);
	ini_getint( inifile, "INPUT",	"key1_jump",	&m_key[KEY_JUMP][0]		);
	ini_getint( inifile, "INPUT",	"key1_action",	&m_key[KEY_ACTION][0]	);
	ini_getint( inifile, "INPUT",	"key1_menu",	&m_key[KEY_MENU][0]		);

	ini_getint( inifile, "INPUT",	"key2_left",	&m_key[KEY_LEFT][1]		);
	ini_getint( inifile, "INPUT",	"key2_right",	&m_key[KEY_RIGHT][1]	);
	ini_getint( inifile, "INPUT",	"key2_up",		&m_key[KEY_UP][1]		);
	ini_getint( inifile, "INPUT",	"key2_down",	&m_key[KEY_DOWN][1]		);
	ini_getint( inifile, "INPUT",	"key2_jump",	&m_key[KEY_JUMP][1]		);
	ini_getint( inifile, "INPUT",	"key2_action",	&m_key[KEY_ACTION][1]	);
	ini_getint( inifile, "INPUT",	"key2_menu",	&m_key[KEY_MENU][1]		);

	ini_getint( inifile, "INPUT",	"rumble",		&m_rumble				);
	ini_getint( inifile, "INPUT",	"joystick_dx",	&m_deadzone[0]			);
	ini_getint( inifile, "INPUT",	"joystick_dy",	&m_deadzone[1]			);
	ini_getint( inifile, "INPUT",	"joystick_b0",	&m_joy[0]				);
	ini_getint( inifile, "INPUT",	"joystick_b1",	&m_joy[1]				);
	ini_getint( inifile, "INPUT",	"joystick_b2",	&m_joy[2]				);
	ini_getint( inifile, "INPUT",	"joystick_b3",	&m_joy[3]				);
	ini_getint( inifile, "INPUT",	"joystick_ax",	&m_joy[4]				);
	ini_getint( inifile, "INPUT",	"joystick_ay",	&m_joy[5]				);

	return TRUE;
	unguard()
}

BOOL cDizCfg::Save()
{
	guard(cDizCfg::Save)
	char inifile[256];

	// USER
	strcpy( inifile, file_getfullpath(GetIniFile()) );

	// VIDEO
	ini_setint( inifile, "VIDEO",	"scale",		m_scale					);

	// AUDIO
	ini_setint( inifile, "AUDIO",	"volfx",		m_volfx					);
	ini_setint( inifile, "AUDIO",	"volmusic",		m_volmusic				);

	// INPUT
	ini_setint( inifile, "INPUT",	"key1_left",	m_key[KEY_LEFT][0]		);
	ini_setint( inifile, "INPUT",	"key1_right",	m_key[KEY_RIGHT][0]		);
	ini_setint( inifile, "INPUT",	"key1_up",		m_key[KEY_UP][0]		);
	ini_setint( inifile, "INPUT",	"key1_down",	m_key[KEY_DOWN][0]		);
	ini_setint( inifile, "INPUT",	"key1_jump",	m_key[KEY_JUMP][0]		);
	ini_setint( inifile, "INPUT",	"key1_action",	m_key[KEY_ACTION][0]	);
	ini_setint( inifile, "INPUT",	"key1_menu",	m_key[KEY_MENU][0]		);

	ini_setint( inifile, "INPUT",	"key2_left",	m_key[KEY_LEFT][1]		);
	ini_setint( inifile, "INPUT",	"key2_right",	m_key[KEY_RIGHT][1]		);
	ini_setint( inifile, "INPUT",	"key2_up",		m_key[KEY_UP][1]		);
	ini_setint( inifile, "INPUT",	"key2_down",	m_key[KEY_DOWN][1]		);
	ini_setint( inifile, "INPUT",	"key2_jump",	m_key[KEY_JUMP][1]		);
	ini_setint( inifile, "INPUT",	"key2_action",	m_key[KEY_ACTION][1]	);
	ini_setint( inifile, "INPUT",	"key2_menu",	m_key[KEY_MENU][1]		);

	ini_setint( inifile, "INPUT",	"rumble",		m_rumble				);
	ini_setint( inifile, "INPUT",	"joystick_dx",	m_deadzone[0]			);
	ini_setint( inifile, "INPUT",	"joystick_dy",	m_deadzone[1]			);
	ini_setint( inifile, "INPUT",	"joystick_b0",	m_joy[0]				);
	ini_setint( inifile, "INPUT",	"joystick_b1",	m_joy[1]				);
	ini_setint( inifile, "INPUT",	"joystick_b2",	m_joy[2]				);
	ini_setint( inifile, "INPUT",	"joystick_b3",	m_joy[3]				);
	ini_setint( inifile, "INPUT",	"joystick_ax",	m_joy[4]				);
	ini_setint( inifile, "INPUT",	"joystick_ay",	m_joy[5]				);

	return TRUE;
	unguard()
}

void cDizCfg::LoadRenderCfg( r9Cfg& cfg, int& api )
{
	guard(cDizCfg::LoadRenderCfg);
	char inifile[256];
	strcpy( inifile, file_getfullpath(GetIniFile()) );

	api				= R9_API_DEFAULT;
	cfg.m_windowed	= 1;
	cfg.m_bpp		= 32;
	cfg.m_width		= 640;
	cfg.m_height	= 480;
	cfg.m_refresh	= 85;
	cfg.m_vsync		= 0;

	ini_getint( inifile, "VIDEO", "api",		&api );
	ini_getint( inifile, "VIDEO", "windowed",	&cfg.m_windowed );
	ini_getint( inifile, "VIDEO", "bpp",		&cfg.m_bpp );
	ini_getint( inifile, "VIDEO", "width",		&cfg.m_width );
	ini_getint( inifile, "VIDEO", "height",		&cfg.m_height );
	ini_getint( inifile, "VIDEO", "refresh",	&cfg.m_refresh );
	ini_getint( inifile, "VIDEO", "vsync",		&cfg.m_vsync );

	unguard();
}

char* cDizCfg::GetInfoValue( char* name )
{
	guard(cDizCfg::GetInfoValue);
	int p;
	char* sz;
	static char szret[128];
	szret[0]=0;
	if(m_info==NULL) return szret;
	if(name==NULL || name[0]==0) return szret;

	sz = parser_skiptotoken(m_info,name,p);
	if(!sz) return szret;
	sz = parser_skiptochar(sz,"=\r\n",p);
	if(!sz) return szret;
	if(*sz!='=') return szret;
	sz = parser_skipchar(sz+1," \t",p);
	parser_readline(sz,szret,127,p);
	parser_trimbackspace(szret,p);
	
	return szret;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
