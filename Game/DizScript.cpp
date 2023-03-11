//////////////////////////////////////////////////////////////////////////////////////////////////
// DizScript.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E9System.h"
#include "D9Debug.h"
#include "E9App.h"
#include "gslib_default.h"
#include "gslib_math.h"
#include "gslib_file.h"
#include "gslib_win.h"
#include "DizScript.h"
#include "DizGame.h"
#include "DizSound.h"
#include "DizApp.h"
#include "DizDebug.h"

cDizScript g_script;
char* cDizScript::m_handlername[HANDLER_MAX] = 
{ 
		"HandlerGameInit", 
		"HandlerGameStart",
		"HandlerGameUpdate",
		"HandlerGameAfterUpdate",
		"HandlerRoomOpen",
		"HandlerRoomClose",
		"HandlerRoomOut",
		"HandlerCollision",
		"HandlerFall",
		"HandlerJump",
		"HandlerPlayerUpdate",
		"HandlerAction",
		"HandlerMenu",
		"HandlerDrawHud",
		"HandlerMusicLoop",
		"HandlerDebug",
		"HandlerReloadMap",
		"HandlerReloadScript"
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// DizScript class
//////////////////////////////////////////////////////////////////////////////////////////////////
cDizScript::cDizScript()
{
	guard(cDizScript::cDizScript)
	m_env = NULL;
	m_vm0 = NULL;
	m_vm1 = NULL;
	m_request = -1;
	m_running = -1;
	m_killlatent = 0;
	
	for(int i=0; i<HANDLER_MAX; i++) m_handler[i]=-1;
	memset(m_handlerdata,0,sizeof(m_handlerdata));

	unguard()
}

cDizScript::~cDizScript()
{
	guard(cDizScript::~cDizScript)

	unguard()
}

BOOL cDizScript::Init()
{
	guard(cDizScript::Init)
	m_env = gs_initenv();
	m_vm0 = gs_initvm( m_env );
	m_vm1 = gs_initvm( m_env );
	gslib_default(m_vm0);
	gslib_math(m_vm0);
	gslib_file(m_vm0);
	gslib_win(m_vm0);
	
	ScriptRegister(m_vm0);
	int ret = gs_compilefile(m_vm0,"Data\\Scripts\\dizzy.gs");
	if(ret!=GS_OK) return FALSE;
	
	// read handlers
	for(int i=0; i<HANDLER_MAX; i++) 
	{
		m_handler[i] = gs_findfn(m_vm0,m_handlername[i]);
		if(m_handler[i]==-1)
			dlog("Handler function \"%s\" not found.\n", m_handlername[i]);
	}

	// game init
	CallHandler(HANDLER_GAMEINIT);

	return TRUE;
	unguard()
}

void cDizScript::Done()
{
	guard(cDizScript::Done)
	gs_donevm(m_vm0);
	gs_donevm(m_vm1);
	gs_doneenv(m_env);
	unguard()
}
	
void cDizScript::Start()
{
	guard(cDizScript::Start)
	m_request = -1;
	m_running = -1;
	m_killlatent = 0;
	gs_recover(m_vm0);
	gs_recover(m_vm1);
	CallHandler(HANDLER_GAMESTART);
	unguard()
}

void cDizScript::Update()
{
	guard(cDizScript::Update)

	if( IsBusy() && m_killlatent )
	{
		m_killlatent = 0;
		m_running=-1;
		gs_recover(m_vm1); // stop current running
	}

	if( !IsBusy() && m_request!=-1 )
	{
		m_running=m_request;
		gs_recover(m_vm1); // stop current running
		gs_call(m_vm1, m_running, 0);
	}

	m_request=-1; // clear request


	if( gs_isrunning(m_vm1) )
	{
		gs_run(m_vm1);
		if( !gs_isrunning(m_vm1) ) 
		{
			// just finished the job
			gs_pop(m_vm1); // pop returned value
			sverify(m_vm1->m_sb==0 && m_vm1->m_sp==0); // safety check!
			m_running = -1;
		}
	}

	unguard()
}

BOOL cDizScript::Reload()
{
	guard(cDizScript::Reload)
	if(IsBusy()) return TRUE; // can't reload when busy (no error thought)
	
	// init in a temporary enviroment, so we can keep the old one if error 
	gsEnv*	env = gs_initenv();
	gsVM*	vm0 = gs_initvm( env );
	gsVM*	vm1 = gs_initvm( env );
	gslib_default(vm0);
	gslib_math(vm0);
	gslib_file(vm0);
	gslib_win(vm0);
	
	ScriptRegister(vm0);
	int ret = gs_compilefile(vm0,"Data\\Scripts\\dizzy.gs");
	if(ret!=GS_OK) 
	{
		gs_donevm(vm0);
		gs_donevm(vm1);
		gs_doneenv(env);
		ERRORMESSAGE("Script compiling error.");
		return FALSE;
	}
	
	// read handlers
	for(int i=0; i<HANDLER_MAX; i++) 
	{
		m_handler[i] = gs_findfn(vm0,m_handlername[i]);
		if(m_handler[i]==-1)
			dlog("Handler function \"%s\" not found.\n", m_handlername[i]);
	}

	// swap script
	Done();	
	m_env = env;
	m_vm0 = vm0;
	m_vm1 = vm1;

	dlog(LOGAPP,"Script reloaded.\n");

	return TRUE;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ENVIROMENT AND VIRTUAL MACHINES
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cDizScript::Request( char* fnname )
{
	guard(cDizScript::Request)
	int fp = gs_findfn(m_vm1, fnname);
	if(fp==-1) return FALSE;
	m_request = fp;
	return TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// HANDLERS
//////////////////////////////////////////////////////////////////////////////////////////////////

void cDizScript::CallHandler( int handler )
{
	guard(cDizScript::CallHandler)
	sassert(0<=handler && handler<HANDLER_MAX);
	if(m_handler[handler]==-1) return; // invalid handler (function not found)
	gs_runfn(m_vm0,m_handler[handler],0); 
	gs_pop(m_vm0); // pop return
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////

#define GS_REGCONST( c )		gs_regint( vm, #c, c );
#define GS_REGFN( fn )			gs_regfn( vm, #fn, gs##fn );
#define GS_ERROR( p1,p2,desc )	gs_error( vm, GSE_USER, p1, p2, desc );

//////////////////////////////////////////////////////////////////////////////////////////////////
// GAME
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsGameFrame( gsVM* vm )
{
	guard(gsGameFrame)
	gs_pushint(vm, g_game.m_gameframe);
	return 1;
	unguard()
}

int gsGameGet( gsVM* vm )
{
	guard(gsGameGet)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int var = gs_toint(vm,0);
	if(var<0 || var>=G_MAX)		{ GS_ERROR(var,G_MAX,"invalid game variable"); GS_RETURNINT(vm,0); }
	int	val = g_game.Get(var);
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsGameSet( gsVM* vm )
{
	guard(gsGameSet)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	int var = gs_toint(vm,0);
	int val = gs_toint(vm,1);
	if(var<0 || var>=G_MAX)		{ GS_ERROR(var,G_MAX,"invalid game variable"); return 0; }
	g_game.Set(var,val); 
	return 0;
	unguard()
}

int gsGameCommand( gsVM* vm )
{
	guard(gsGameCommand)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	g_game.m_command = gs_toint(vm,0);
	return 0;
	unguard()
}

int gsGameSetName( gsVM* vm )
{
	guard(gsGameSetName)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	char* name = gs_tostr(vm,0);
	if(name) E9_AppSetStr(E9_APP_NAME,name);
	return 0;
	unguard()
}

int gsMapLoad( gsVM* vm )
{
	guard(gsMapLoad)
	if(!gs_ckparams(vm,1)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	g_map.Reset();
	int ok = g_map.Load(gs_tostr(vm,0));
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// MATERIALS
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsMaterialGetDensity( gsVM* vm )
{
	guard(gsMaterialGetDensity)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int mat = gs_toint(vm,0);
	if(mat<0 || mat>=MAT_MAX) { GS_ERROR(mat,MAT_MAX,"invalid material"); GS_RETURNINT(vm,0); }
	int	val = g_game.m_matdensity[mat];
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsMaterialSetDensity( gsVM* vm )
{
	guard(gsMaterialSetDensity)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	int mat = gs_toint(vm,0);
	int val = gs_toint(vm,1);
	if(mat<0 || mat>=MAT_MAX) { GS_ERROR(mat,MAT_MAX,"invalid material"); return 0; }
	g_game.m_matdensity[mat] = val;
	return 0;
	unguard()
}

int gsMaterialGetColor( gsVM* vm )
{
	guard(gsMaterialGetColor)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int mat = gs_toint(vm,0);
	if(mat<0 || mat>=MAT_MAX) { GS_ERROR(mat,MAT_MAX,"invalid material"); GS_RETURNINT(vm,0); }
	int	val = g_game.m_matcolor[mat];
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsMaterialSetColor( gsVM* vm )
{
	guard(gsMaterialSetColor)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	int mat = gs_toint(vm,0);
	int val = gs_toint(vm,1);
	if(mat<0 || mat>=MAT_MAX) { GS_ERROR(mat,MAT_MAX,"invalid material"); return 0; }
	g_game.m_matcolor[mat] = val;
	return 0;
	unguard()
}

int gsMaterialRead( gsVM* vm )
{
	guard(gsMaterialRead)
	if(!gs_ckparams(vm,4))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_INT)) GS_RETURNINT(vm,0);
	int x1 = gs_toint(vm,0);
	int y1 = gs_toint(vm,1);
	int x2 = x1+gs_toint(vm,2);
	int y2 = y1+gs_toint(vm,3);
	int mat = 0;
	for(int y=y1;y<y2;y++)
	{
		for(int x=x1;x<x2;x++)
		{
			mat |= (1<<g_game.MatMap(x,y));
		}
	}
	gs_pushint(vm,mat);
	return 1;
	unguard()
}

int gsMaterialDensityRead( gsVM* vm )
{
	guard(gsMaterialDensityRead)
	if(!gs_ckparams(vm,4))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_INT)) GS_RETURNINT(vm,0);
	int x1 = gs_toint(vm,0);
	int y1 = gs_toint(vm,1);
	int x2 = x1+gs_toint(vm,2);
	int y2 = y1+gs_toint(vm,3);
	int density = 0;
	for(int y=y1;y<y2;y++)
	{
		for(int x=x1;x<x2;x++)
		{
			density |= (1<<(MATDENSITY(g_game.MatMap(x,y))));
		}
	}
	gs_pushint(vm,density);
	return 1;
	unguard()
}

int	gsMaterialCheckFree( gsVM* vm )
{
	guard(gsMaterialCheckFree);
	if(!gs_ckparams(vm,4))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_INT)) GS_RETURNINT(vm,0);
	int x1 = gs_toint(vm,0);
	int y1 = gs_toint(vm,1);
	int x2 = gs_toint(vm,2);
	int y2 = gs_toint(vm,3);
	
	for(int iy=y1;iy<y2;iy++)
	{
		for(int ix=x1;ix<x2;ix++)
		{
			if( MATDENSITY(g_game.MatMap(ix,iy))==MATD_HARD ) 
			{
				gs_pushint(vm,0);
				return 1;
			}
		}
	}

	gs_pushint(vm,1);
	return 1;
	unguard();
}

int	gsMaterialGetFreeDist( gsVM* vm )
{
	guard(gsMaterialGetFreeDist);
	if(!gs_ckparams(vm,6))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,4,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,5,GS_INT)) GS_RETURNINT(vm,0);
	int ix,iy;
	int x1 = gs_toint(vm,0);
	int y1 = gs_toint(vm,1);
	int x2 = gs_toint(vm,2);
	int y2 = gs_toint(vm,3);
	int dir = gs_toint(vm,4);
	BOOL hardonly = gs_toint(vm,5); // hard or void
	if(dir==0) // [top to bottom)
	{
		for( iy=y1; iy<y2; iy++ )
		{
			for(ix=x1;ix<x2;ix++)
			{
				if(hardonly)
				{
					if( MATDENSITY(g_game.MatMap(ix,iy))==MATD_HARD ) 
						GS_RETURNINT(vm,iy-y1); // blocked
				}
				else
				{
					if( MATDENSITY(g_game.MatMap(ix,iy))!=MATD_VOID ) 
						GS_RETURNINT(vm,iy-y1); // blocked
				}
			}
		}

		GS_RETURNINT(vm,y2-y1); // all free
	}
	else
	if(dir==1) // (bottom to top]
	{
		for( iy=y2-1; iy>=y1; iy-- )
		{
			for(ix=x1;ix<x2;ix++)
			{
				if(hardonly)
				{
					if( MATDENSITY(g_game.MatMap(ix,iy))==MATD_HARD ) 
						GS_RETURNINT(vm,y2-1-iy); // blocked
				}
				else
				{
					if( MATDENSITY(g_game.MatMap(ix,iy))!=MATD_VOID ) 
						GS_RETURNINT(vm,y2-1-iy); // blocked
				}
			}
		}
		GS_RETURNINT(vm,y2-y1); // all free
	}
	else 
	{
		GS_ERROR(dir,2,"direction unsuported yet"); 
	}

	gs_pushint(vm,0);
	return 1;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// TILES
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsTileFind( gsVM* vm ) 
{
	guard(gsTileFind)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int idx = g_paint.TileFind(gs_toint(vm,0)); 
	gs_pushint(vm,idx);
	return 1;
	unguard()
}

int gsTileCount( gsVM* vm )  
{
	guard(gsTileCount)
	gs_pushint(vm,g_paint.m_tile.Size());
	return 1;
	unguard()
}

int gsTileGet( gsVM* vm ) 
{
	guard(gsTileGet)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	int idx = gs_toint(vm,0);
	cTile* tile = g_paint.TileGet(idx); 
	if(!tile) { GS_ERROR(idx,g_paint.TileCount(),"invalid tile index"); GS_RETURNINT(vm,0); }
	int ret = 0;
	switch( gs_toint(vm,1) )
	{
		case TILE_ID:		ret = tile->m_id; break;
		case TILE_W:		ret = R9_TextureGetWidth(tile->m_tex); break;
		case TILE_H:		ret = R9_TextureGetHeight(tile->m_tex); break;
		case TILE_FRAMES:	ret = tile->m_frames; break;
		case TILE_NAME:		gs_pushstr(vm,tile->m_name?tile->m_name:""); return 1;
	}
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsTileLoad( gsVM* vm )
{
	guard(gsTileLoad)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int group=0;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
		group = gs_toint(vm,1);
	}
	BOOL ok = g_paint.TileLoad(gs_tostr(vm,0),group);
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsTileUnload( gsVM* vm )
{
	guard(gsTileUnload)
	int group=0;
	if(gs_params(vm)>0)
	{
		if(!gs_cktype(vm,0,GS_INT)) return 0;
		group = gs_toint(vm,0);
	}
	g_paint.TileUnload(group);
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// SCRIPT
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsScrGetRequest( gsVM* vm )
{
	guard(gsScrGetRequest)
	gs_pushint(vm, g_script.m_request);
	return 1;
	unguard()
}
int gsScrGetRunning( gsVM* vm )
{
	guard(gsScrGetRunning)
	gs_pushint(vm, g_script.m_running);
	return 1;
	unguard()
}
int gsScrRequest( gsVM* vm )
{
	guard(gsScrRequest)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	g_script.m_request = gs_toint(vm,0);
	return 0;
	unguard()
}

int gsScrKillLatent( gsVM* vm )
{
	guard(gsScrKillLatent)
	g_script.m_killlatent = 1;
	return 0;
	unguard()
}

int gsScrGetHandlerData( gsVM* vm )
{
	guard(gsScrGetHandlerData)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int idx = gs_toint(vm, 0);
	if(idx<0 || idx>=HANDLER_MAXDATA) { GS_ERROR(idx,HANDLER_MAXDATA,"invalid handler data index"); GS_RETURNINT(vm,0); }
	gs_pushint(vm, g_script.m_handlerdata[idx]);
	return 1;
	unguard()
}

int gsScrSetHandlerData( gsVM* vm )
{
	guard(gsScrSetHandlerData)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	int idx = gs_toint(vm, 0);
	int val = gs_toint(vm, 1);
	if(idx<0 || idx>=HANDLER_MAXDATA) { GS_ERROR(idx,HANDLER_MAXDATA,"invalid handler data index"); return 0; }
	g_script.m_handlerdata[idx] = val;
	return 0;
	unguard()
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// BRUSHES
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsBrushCount( gsVM* vm )
{
	guard(gsBrushCount)
	gs_pushint(vm, g_map.m_brush.Size()); 
	return 1;
	unguard()
}

int gsBrushFind( gsVM* vm )
{
	guard(gsBrushFind)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int id = gs_toint(vm,0);
	int idx = g_map.BrushFind(id);
	gs_pushint(vm, idx); 
	return 1;
	unguard()
}

int gsBrushGet( gsVM* vm )
{
	guard(gsBrushGet)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	int idx = gs_toint(vm,0);
	int var = gs_toint(vm,1);
	if(idx<0 || idx>=g_map.BrushCount()) { GS_ERROR(idx,g_map.BrushCount(),"invalid brush index"); GS_RETURNINT(vm,0); }
	if(var<0 || var>=BRUSH_MAX) { GS_ERROR(var,BRUSH_MAX,"invalid brush variable"); GS_RETURNINT(vm,0); }
	tBrush* brush = g_map.BrushGet(idx); sassert(brush!=NULL); // safety
	int val = brush->Get(var);
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsBrushSet( gsVM* vm )
{
	guard(gsBrushSet)
	if(!gs_ckparams(vm,3))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	int idx = gs_toint(vm,0);
	int var = gs_toint(vm,1);
	int val = gs_toint(vm,2);
	if(idx<0 || idx>=g_map.BrushCount()) { GS_ERROR(idx,g_map.BrushCount(),"invalid brush index"); return 0; }
	if(var<0 || var>=BRUSH_MAX) { GS_ERROR(var,BRUSH_MAX,"invalid brush variable"); return 0; }
	tBrush* brush = g_map.BrushGet(idx); sassert(brush!=NULL); // safety
	brush->Set(var,val); 
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// OBJECTS
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsObjCount( gsVM* vm )
{
	guard(gsObjCount)
	gs_pushint(vm, g_map.ObjCount()); 
	return 1;
	unguard()
}

int gsObjFind( gsVM* vm )
{
	guard(gsObjFind)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int id = gs_toint(vm,0);
	int idx = g_map.ObjFind(id);
	gs_pushint(vm, idx); 
	return 1;
	unguard()
}

int gsObjGet( gsVM* vm )
{
	guard(gsObjGet)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	int idx = gs_toint(vm,0);
	int var = gs_toint(vm,1);
	if(idx<0 || idx>=g_map.ObjCount()) { GS_ERROR(idx,g_map.ObjCount(),"invalid object index"); GS_RETURNINT(vm,0); }
	if(var<0 || var>=BRUSH_MAX) { GS_ERROR(var,BRUSH_MAX,"invalid object variable"); GS_RETURNINT(vm,0); }
	tBrush* obj = g_map.ObjGet(idx); sassert(obj!=NULL); // safety
	int val = obj->Get(var);
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsObjSet( gsVM* vm )
{
	guard(gsObjSet)
	if(!gs_ckparams(vm,3))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	int idx = gs_toint(vm,0);
	int var = gs_toint(vm,1);
	int val = gs_toint(vm,2);
	if(idx<0 || idx>=g_map.ObjCount()) { GS_ERROR(idx,g_map.ObjCount(),"invalid object index"); return 0; }
	if(var<0 || var>=BRUSH_MAX) { GS_ERROR(var,BRUSH_MAX,"invalid object variable"); return 0; }
	tBrush* obj = g_map.ObjGet(idx); sassert(obj!=NULL); // safety
	obj->Set(var,val); 
	return 0;
	unguard()
}

int gsObjGetName( gsVM* vm )
{
	guard(gsObjGetName)
	if(!gs_ckparams(vm,1))		GS_RETURNSTR(vm,"");
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,"");
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=g_map.ObjCount()) { GS_ERROR(idx,g_map.ObjCount(),"invalid object index"); GS_RETURNSTR(vm,""); }
	gs_pushstr(vm, g_map.GetObjName(idx)); 
	return 1;
	unguard()
}

int gsObjSetName( gsVM* vm )
{
	guard(gsObjSetName)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_STR)) return 0;
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=g_map.ObjCount()) { GS_ERROR(idx,g_map.ObjCount(),"invalid object index"); return 0; }
	g_map.SetObjName(idx,gs_tostr(vm,1)); 
	return 0;
	unguard()
}

int gsObjPresent( gsVM* vm )
{
	guard(gsObjPresent)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=g_map.ObjCount()) { GS_ERROR(idx,g_map.ObjCount(),"invalid object index"); return 0; }
	g_game.ObjPresent(idx); 
	return 0;
	unguard()
}

int gsObjPresentCount( gsVM* vm )
{
	guard(gsObjPresentCount)
	gs_pushint(vm,g_game.m_objcount);
	return 1;
	unguard()
}

int gsObjPresentIdx( gsVM* vm )
{
	guard(gsObjPresentIdx)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int presentidx = gs_toint(vm,0);
	if(presentidx<0 || presentidx>=g_game.m_objcount) { GS_ERROR(presentidx,g_game.m_objcount,"invalid present index"); GS_RETURNINT(vm,-1); }
	gs_pushint(vm,g_game.m_obj[presentidx]);
	return 1;
	unguard()
}

int gsObjPresentGather( gsVM* vm )
{
	guard(ObjPresentGather)
	g_game.ObjGather();
	return 0;
	unguard()
}

int gsColliderSnapDistance( gsVM* vm )
{
	guard(gsColliderSnapDistance);
	if(!gs_ckparams(vm,4))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,3,GS_INT)) GS_RETURNINT(vm,0);
	int x1 = gs_toint(vm,0);
	int y1 = gs_toint(vm,1);
	int x2 = gs_toint(vm,2);
	int y2 = gs_toint(vm,3);

	int dist = 0; // max distance from box bottom to collider top (if collider top is inside box)
	int cx1,cy1,cx2,cy2;
	for( int i=0; i<g_game.m_collidercount; i++ )
	{
		int idx = g_game.m_collider[i];
		tBrush* obj = g_map.ObjGet(idx); sassert(obj);
		if( obj->Get(BRUSH_DISABLE)!=0 ) continue; // only enabled objects
		if(!(obj->Get(BRUSH_COLLIDER) & COLLIDER_HARD)) continue; // only those that need it
		obj->MakeBBW(cx1,cy1,cx2,cy2);
		if( x2<=cx1 || x1>=cx2 ) continue; // not intersecting
		if(y1<=cy1 && cy1<y2) dist = MAX(dist,y2-cy1);
	}

	gs_pushint(vm,dist);
	return 1;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ROOMS
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsRoomGet( gsVM* vm )
{
	guard(gsRoomGet)
	if(!gs_ckparams(vm,3))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	int rx = gs_toint(vm,0);
	int ry = gs_toint(vm,1);
	int idx = gs_toint(vm,2);
	if(rx<0 || rx>=g_map.m_mapw || ry<0 || ry>=g_map.m_maph) { GS_ERROR(rx,ry,"invalid room coordinates"); GS_RETURNINT(vm,0); }
	if(idx<0 || idx>=R_MAX) { GS_ERROR(idx,R_MAX,"invalid room property"); GS_RETURNINT(vm,0); }
	gs_pushint( vm, g_map.GetRoomProp( rx, ry, idx ) );
	return 1;
	unguard()
}

int gsRoomSet( gsVM* vm )
{
	guard(gsRoomSet)
	if(!gs_ckparams(vm,4)) return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	if(!gs_cktype(vm,3,GS_INT)) return 0;
	int rx = gs_toint(vm,0);
	int ry = gs_toint(vm,1);
	int idx = gs_toint(vm,2);
	int val = gs_toint(vm,3);
	if(rx<0 || rx>=g_map.m_mapw || ry<0 || ry>=g_map.m_maph) { GS_ERROR(rx,ry,"invalid room coordinates"); return 0; }
	if(idx<0 || idx>=R_MAX) { GS_ERROR(idx,R_MAX,"invalid room property"); return 0; }
	g_map.SetRoomProp( rx, ry, idx, val );
	return 0;
	unguard()
}

int gsRoomGetName( gsVM* vm )
{
	guard(gsRoomGetName)
	if(!gs_ckparams(vm,2))		GS_RETURNSTR(vm,"");
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,"");
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNSTR(vm,"");
	int rx = gs_toint(vm,0);
	int ry = gs_toint(vm,1);
	if(rx<0 || rx>=g_map.m_mapw || ry<0 || ry>=g_map.m_maph) { GS_ERROR(rx,ry,"invalid room coordinates"); GS_RETURNSTR(vm,""); }
	gs_pushstr( vm, g_map.GetRoomName( rx, ry ) );
	return 1;
	unguard()
}

int gsRoomSetName( gsVM* vm )
{
	guard(gsRoomSetName)
	if(!gs_ckparams(vm,3)) return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_STR)) return 0;
	int rx = gs_toint(vm,0);
	int ry = gs_toint(vm,1);
	char* name = gs_tostr(vm,2);
	if(rx<0 || rx>=g_map.m_mapw || ry<0 || ry>=g_map.m_maph) { GS_ERROR(rx,ry,"invalid room coordinates"); return 0; }
	g_map.SetRoomName( rx, ry, name );
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// PLAYER
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsPlayerGet( gsVM* vm )
{
	guard(gsPlayerGet)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int var = gs_toint(vm,0);
	if(var<0 || var>=P_MAX) { GS_ERROR(var,P_MAX,"invalid player variable"); GS_RETURNINT(vm,0); }
	int	val = g_player.Get(var);
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsPlayerSet( gsVM* vm )
{
	guard(gsPlayerSet)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	int var = gs_toint(vm,0);
	int val = gs_toint(vm,1);
	if(var<0 || var>=P_MAX) { GS_ERROR(var,P_MAX,"invalid player variable"); return 0; }
	g_player.Set(var,val); 
	return 0;
	unguard()
}

int gsPlayerMakeBB( gsVM* vm )
{
	guard(gsPlayerMakeBB);
	if(!gs_ckparams(vm,4))			return 0;
	if(!gs_ckreftype(vm,0,GS_INT))	return 0;
	if(!gs_ckreftype(vm,1,GS_INT))	return 0;
	if(!gs_ckreftype(vm,2,GS_INT))	return 0;
	if(!gs_ckreftype(vm,3,GS_INT))	return 0;
	int rx = g_game.Get(G_ROOMX);
	int ry = g_game.Get(G_ROOMY);
	int x1,y1,x2,y2;
	g_player.MakeBBW(x1,y1,x2,y2);
	x1 = x1 - rx*ROOM_W;
	x2 = x2 - rx*ROOM_W;
	y1 = y1 - ry*ROOM_H;
	y2 = y2 - ry*ROOM_H;
	gs_toref(vm,0)->i = x1;
	gs_toref(vm,1)->i = y1;
	gs_toref(vm,2)->i = x2;
	gs_toref(vm,3)->i = y2;
	return 0;
	unguard();
}

int gsPlayerMakeBBW( gsVM* vm )
{
	guard(gsPlayerMakeBBW);
	if(!gs_ckparams(vm,4))			return 0;
	if(!gs_ckreftype(vm,0,GS_INT))	return 0;
	if(!gs_ckreftype(vm,1,GS_INT))	return 0;
	if(!gs_ckreftype(vm,2,GS_INT))	return 0;
	if(!gs_ckreftype(vm,3,GS_INT))	return 0;
	int x1,y1,x2,y2;
	g_player.MakeBBW(x1,y1,x2,y2);
	gs_toref(vm,0)->i = x1;
	gs_toref(vm,1)->i = y1;
	gs_toref(vm,2)->i = x2;
	gs_toref(vm,3)->i = y2;
	return 0;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Dialogs
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsDlgPush( gsVM* vm )
{
	guard(gsDlgPush)
	g_game.DlgPush();
	return 0;
	unguard()
}

int gsDlgPop( gsVM* vm )
{
	guard(gsDlgPop)
	g_game.DlgPop();
	return 0;
	unguard()
}

int gsDlgPopAll( gsVM* vm )
{
	guard(gsDlgPopAll)
	g_game.DlgPopAll();
	return 0;
	unguard()
}

int gsDlgCount( gsVM* vm )
{
	guard(gsDlgCount)
	int count = g_game.m_dlg.Size();
	gs_pushint(vm,count);
	return 1;
	unguard()
}

int gsDlgGet( gsVM* vm )
{
	guard(gsDlgGet)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	int idx = gs_toint(vm,0);
	int var = gs_toint(vm,1);
	if(idx<0 || idx>=g_game.m_dlg.Size()) { GS_ERROR(idx,g_game.m_dlg.Size(),"invalid dialog index"); GS_RETURNINT(vm,0); }
	if(var<0 || var>=DLG_MAX) { GS_ERROR(var,DLG_MAX,"invalid dialog variable"); GS_RETURNINT(vm,0); }
	tDialog* dlg = g_game.m_dlg.Get(idx); sassert(dlg!=NULL); // safety
	int val = dlg->Get(var);
	gs_pushint(vm, val); 
	return 1;
	unguard()
}

int gsDlgSet( gsVM* vm )
{
	guard(gsDlgSet)
	if(!gs_ckparams(vm,3))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	int idx = gs_toint(vm,0);
	int var = gs_toint(vm,1);
	int val = gs_toint(vm,2);
	if(idx<0 || idx>=g_game.m_dlg.Size()) { GS_ERROR(idx,g_game.m_dlg.Size(),"invalid dialog index"); GS_RETURNINT(vm,0); }
	if(var<0 || var>=DLG_MAX) { GS_ERROR(var,DLG_MAX,"invalid dialog variable"); GS_RETURNINT(vm,0); }
	tDialog* dlg = g_game.m_dlg.Get(idx); sassert(dlg!=NULL); // safety
	dlg->Set(var,val);
	return 0;
	unguard()
}

int gsDlgGetText( gsVM* vm )
{
	guard(gsDlgGetText)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int idx = gs_toint(vm,0);
	if(idx<0 || idx>=g_game.m_dlg.Size()) { GS_ERROR(idx,g_game.m_dlg.Size(),"invalid dialog index"); GS_RETURNINT(vm,0); }
	tDialog* dlg = g_game.m_dlg.Get(idx); sassert(dlg!=NULL); // safety
	char* text = dlg->GetText();
	gs_pushstr(vm, text?text:""); 
	return 1;
	unguard()
}

int gsDlgSetText( gsVM* vm )
{
	guard(gsDlgSetText)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_STR)) return 0;
	int idx = gs_toint(vm,0);
	char* text = gs_tostr(vm,1);
	if(idx<0 || idx>=g_game.m_dlg.Size()) { GS_ERROR(idx,g_game.m_dlg.Size(),"invalid dialog index"); GS_RETURNINT(vm,0); }
	tDialog* dlg = g_game.m_dlg.Get(idx); sassert(dlg!=NULL); // safety
	dlg->SetText(text?text:"");
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Sound
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsSampleLoad( gsVM* vm )
{
	guard(gsSampleLoad)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int group=0;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
		group = gs_toint(vm,1);
	}
	BOOL ok = g_sound.SampleLoad( gs_tostr(vm,0), group );
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsSampleUnload( gsVM* vm )
{
	guard(gsSampleUnload)
	int group=0;
	if(gs_params(vm)>0)
	{
		if(!gs_cktype(vm,0,GS_INT)) return 0;
		group = gs_toint(vm,0);
	}
	g_sound.SampleUnload(group);
	return 0;
	unguard()
}

int gsSamplePlay( gsVM* vm )
{
	guard(gsSamplePlay)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int loop=0;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,-1);
		loop = gs_toint(vm,1);
	}
	int voiceidx = g_sound.SamplePlay( gs_toint(vm,0), loop );
	gs_pushint(vm,voiceidx);
	return 1;
	unguard()
}

int gsSamplePlaying( gsVM* vm )
{
	guard(gsSamplePlaying)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int id = g_sound.SamplePlaying( gs_toint(vm,0) );
	gs_pushint(vm,id);
	return 1;
	unguard()
}

int gsSampleStop( gsVM* vm )
{
	guard(gsSampleStop)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	g_sound.SampleStop( gs_toint(vm,0) );
	return 0;
	unguard()
}

int gsSampleStopAll( gsVM* vm )
{
	guard(gsSampleStop)
	int id=-1;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) return 0;
		id = gs_toint(vm,1);
	}
	g_sound.SampleStopAll(id);
	return 0;
	unguard()
}

int gsSampleVolume( gsVM* vm )
{
	guard(gsSampleVolume)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	g_sound.SampleVolume( gs_toint(vm,0) );
	return 0;
	unguard()
}

int gsMusicLoad( gsVM* vm )
{
	guard(gsMusicLoad)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int group=0;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
		group = gs_toint(vm,1);
	}
	BOOL ok = g_sound.MusicLoad( gs_tostr(vm,0), group );
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsMusicUnload( gsVM* vm )
{
	guard(gsMusicUnload)
	int group=0;
	if(gs_params(vm)>0)
	{
		if(!gs_cktype(vm,0,GS_INT)) return 0;
		group = gs_toint(vm,0);
	}
	g_sound.MusicUnload(group);
	return 0;
	unguard()
}

int gsMusicFade( gsVM* vm )
{
	guard(gsMusicFade);
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	g_sound.MusicFade( gs_toint(vm,0), gs_toint(vm,1) );
	return 0;
	unguard();
}

int gsMusicPlay( gsVM* vm )
{
	guard(gsMusicPlay)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,-1);
	int start=0;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,-1);
		start = gs_toint(vm,1);
	}
	int ret = g_sound.MusicPlay( gs_toint(vm,0), start );
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsMusicPlaying( gsVM* vm )
{
	guard(gsMusicPlaying)
	int id = g_sound.MusicPlaying();
	gs_pushint(vm,id);
	return 1;
	unguard()
}

int gsMusicPosition( gsVM* vm )
{
	guard(gsMusicPosition)
	int musicpos = g_sound.MusicPosition();
	gs_pushint(vm,musicpos);
	return 1;
	unguard()
}

int gsMusicStop( gsVM* vm )
{
	guard(gsMusicStop)
	g_sound.MusicStop();
	g_sound.m_musicnext = -1;
	g_sound.m_musicvol = 0.0;
	return 0;
	unguard()
}

int gsMusicVolume( gsVM* vm )
{
	guard(gsMusicVolume)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	g_sound.MusicVolume( gs_toint(vm,0) );
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// FONT
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsFontLoad( gsVM* vm )
{
	guard(gsFontLoad)
	if(!gs_ckparamsmin(vm,1))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int group=0;
	if(gs_params(vm)>1)
	{
		if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
		group = gs_toint(vm,1);
	}
	BOOL ok = g_paint.FontLoad( gs_tostr(vm,0), group );
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsFontUnload( gsVM* vm )
{
	guard(gsFontUnload)
	int group=0;
	if(gs_params(vm)>0)
	{
		if(!gs_cktype(vm,0,GS_INT)) return 0;
		group = gs_toint(vm,0);
	}
	g_paint.FontUnload(group);
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// HUD
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsHudDrawTile( gsVM* vm )
{
	guard(gsHudDrawTile)
	if(!gs_ckparams(vm,1+4+4+2)) return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	if(!gs_cktype(vm,3,GS_INT)) return 0;
	if(!gs_cktype(vm,4,GS_INT)) return 0;
	if(!gs_cktype(vm,5,GS_INT)) return 0;
	if(!gs_cktype(vm,6,GS_INT)) return 0;
	if(!gs_cktype(vm,7,GS_INT)) return 0;
	if(!gs_cktype(vm,8,GS_INT)) return 0;
	if(!gs_cktype(vm,9,GS_INT)) return 0;
	if(!gs_cktype(vm,10,GS_INT)) return 0;
	int tileid = gs_toint(vm,0);
	iRect dst( gs_toint(vm,1), gs_toint(vm,2), gs_toint(vm,3), gs_toint(vm,4) );
	dst.x2 += dst.x1;
	dst.y2 += dst.y1;
	iRect src( gs_toint(vm,5), gs_toint(vm,6), gs_toint(vm,7), gs_toint(vm,8) );
	src.x2 += src.x1;
	src.y2 += src.y1;
	g_paint.HUDDrawTile( tileid, dst, src, gs_toint(vm,9), gs_toint(vm,10) );
	return 0;
	unguard()
}

int gsHudDrawText( gsVM* vm )
{
	guard(gsHudDrawText)
	if(!gs_ckparams(vm,7))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	if(!gs_cktype(vm,3,GS_INT)) return 0;
	if(!gs_cktype(vm,4,GS_INT)) return 0;
	if(!gs_cktype(vm,5,GS_STR)) return 0;
	if(!gs_cktype(vm,6,GS_INT)) return 0;
	iRect dst( gs_toint(vm,1), gs_toint(vm,2), gs_toint(vm,3), gs_toint(vm,4) );
	dst.x2 += dst.x1;
	dst.y2 += dst.y1;
	g_paint.HUDDrawText( gs_toint(vm,0), dst, gs_tostr(vm,5), gs_toint(vm,6) );
	return 0;
	unguard()
}

int gsHudGetTextWidth( gsVM* vm )
{
	guard(gsHudGetTextWidth)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	char* text = gs_tostr(vm,0);
	int w,h,c,r;
	g_paint.HUDGetTextSize(text,w,h,c,r);
	gs_pushint(vm,w);
	return 1;
	unguard()
}

int gsHudGetTextHeight( gsVM* vm )
{
	guard(gsHudGetTextHeight)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	char* text = gs_tostr(vm,0);
	int w,h,c,r;
	g_paint.HUDGetTextSize(text,w,h,c,r);
	gs_pushint(vm,h);
	return 1;
	unguard()
}

int gsHudGetTextColumns( gsVM* vm )
{
	guard(gsHudGetTextColumns)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	char* text = gs_tostr(vm,0);
	int w,h,c,r;
	g_paint.HUDGetTextSize(text,w,h,c,r);
	gs_pushint(vm,c);
	return 1;
	unguard()
}

int gsHudGetTextRows( gsVM* vm )
{
	guard(gsHudGetTextRows)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	char* text = gs_tostr(vm,0);
	int w,h,c,r;
	g_paint.HUDGetTextSize(text,w,h,c,r);
	gs_pushint(vm,r);
	return 1;
	unguard()
}

int gsHudClipping( gsVM* vm )
{
	guard(gsHudClipping)
	if(!gs_ckparams(vm,4))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	if(!gs_cktype(vm,3,GS_INT)) return 0;	
	iRect dst( gs_toint(vm,0), gs_toint(vm,1), gs_toint(vm,2), gs_toint(vm,3) );
	dst.x2 += dst.x1;
	dst.y2 += dst.y1;
	g_paint.HudClipping( dst );
	return 0;
	unguard()
}

int gsHudFont( gsVM* vm )
{
	guard(gsHudFont)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int fontid = gs_toint(vm,0);
	g_paint.m_hudfont = fontid;
	return 0;
	unguard()
}

int gsHudShader( gsVM* vm )
{
	guard(gsHudShader)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	int shd = gs_toint(vm,0);
	if(shd<0 || shd<SHADER_MAX) shd=SHADER_BLEND;
	g_paint.m_hudshader = shd;
	return 0;
	unguard()
}

int gsHudColor( gsVM* vm )
{
	guard(gsHudColor)
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	g_paint.m_hudcolor = gs_toint(vm,0);
	return 0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// KEYBOARD
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsKeyboardRead( gsVM* vm )
{
	guard(gsKeyboardRead)
	if(!gs_ckparams(vm,0))	GS_RETURNINT(vm,0);
	if(!I9_IsReady())		GS_RETURNINT(vm,0);
	int qcount = I9_GetKeyQCount();
	int key = qcount ? I9_GetKeyQValue(0) : 0;
	gs_pushint(vm,key);
	return 1;
	unguard()
}

int gsKeyboardStatus( gsVM* vm )
{
	guard(gsKeyboardStatus)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!I9_IsReady())			GS_RETURNINT(vm,0);
	int status = 0;
	int key = gs_toint(vm,0);
	if( key<0 || key>=I9_KEYBOARD_KEYS ) GS_RETURNINT(vm,0);
	status = I9_GetKeyValue(key);
	gs_pushint(vm,status);
	return 1;
	unguard()
}

int gsKeyboardCodeToChar( gsVM* vm )
{
	guard(gsKeyboardCodeToChar)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!I9_IsReady())			GS_RETURNINT(vm,0);
	int key = gs_toint(vm,0);
	if( key<0 || key>=I9_KEYBOARD_KEYS ) GS_RETURNINT(vm,0);
	gs_pushint(vm,I9_GetKeyAscii(key));
	return 1;
	unguard()
}

int gsKeyboardCharToCode( gsVM* vm )
{
	guard(gsKeyboardCharToCode)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!I9_IsReady())			GS_RETURNINT(vm,0);
	int chr = gs_toint(vm,0);
	gs_pushint(vm,I9_FindKeyByAscii(chr));
	return 1;
	unguard()
}

int gsJoystickStatus( gsVM* vm )
{
	guard(gsKeyboardStatus)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!I9_IsReady())			GS_RETURNINT(vm,0);
	int status = 0;
	int key = gs_toint(vm,0);
	if( key<0 || key>=I9_JOY_KEYS ) GS_RETURNINT(vm,0);
	status = I9_GetKeyValue(I9_JOY_FIRSTKEY(0)+key);
	gs_pushint(vm,status);
	return 1;
	unguard()
}

int gsJoystickAxe( gsVM* vm )
{
	guard(gsJoystickAxe)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!I9_IsReady())			GS_RETURNINT(vm,0);
	int value = 0;
	int axe = gs_toint(vm,0);
	if( axe<0 || axe>=I9_JOY_AXES ) GS_RETURNINT(vm,0);
	value = I9_GetAxeValue(I9_JOY_FIRSTAXE(0)+axe);
	gs_pushint(vm,value);
	return 1;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG
//////////////////////////////////////////////////////////////////////////////////////////////////
int gsDebugData( gsVM* vm )
{
	guard(gsDebugData)
	if(!gs_ckparams(vm,2))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	if(!gs_cktype(vm,1,GS_STR)) return 0;
	g_dizdebug.SlotSet(gs_toint(vm,0), gs_tostr(vm,1));
	return 0;
	unguard()
}

int gsDebugCool( gsVM* vm )
{
	guard(gsDebugCool)
	if(gs_params(vm)==0)
	{
		gs_pushint(vm,E9_AppGetInt(E9_APP_COOL));
		return 1;
	}
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	dlog(LOGAPP,"DebugCool %s\n", gs_toint(vm,0)?"on":"off");
	int val = gs_toint(vm,0);
	E9_AppSetInt(E9_APP_COOL,val);
	return 0;
	unguard()
}

int gsDebugLog( gsVM* vm )
{
	guard(gsDebugLog)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	int i;
	if(gs_toint(vm,0))
	{
		for(i=0;i<=D9_LOG_CHANNELMAX;i++) D9_LogOpenChannel(i,TRUE);
		dlog(LOGAPP,"DebugLog on\n");
	}
	else
	{
		dlog(LOGAPP,"DebugLog off\n");
		for(i=0;i<=D9_LOG_CHANNELMAX;i++) D9_LogOpenChannel(i,FALSE);
		D9_LogOpenChannel(LOGSYS,TRUE); // always open
		D9_LogOpenChannel(LOGERR,TRUE); // always open
	}
	return 0;
	unguard()
}

int gsDebugDev( gsVM* vm )
{
	guard(gsDebugDev)
	if(gs_params(vm)==0)
	{
		gs_pushint(vm,cDizDebug::m_developer);
		return 1;
	}
	if(!gs_ckparams(vm,1))		return 0;
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	dlog(LOGAPP,"DebugDev %s\n", gs_toint(vm,0)?"on":"off");
	int val = gs_toint(vm,0);
	cDizDebug::m_developer = val;
	g_paint.Layout();
	return 0;
	unguard()
}

int gsDebugCallstack( gsVM* vm )
{
	guard(gsDebugCallstack)
	char sz[256]; sz[0]=0;
	gs_cpycall(g_script.m_vm1,sz,256);
	gs_pushstr(vm,sz);
	return 1;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Script Register
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizScript::ScriptRegister( gsVM* vm )
{
	guard(ScrRegisterObj)
	sassert(vm!=NULL);

	// GAME ......................................................................................
	GS_REGCONST( G_FPS					);
	GS_REGCONST( G_KEYS					);
	GS_REGCONST( G_KEYSHIT				);
	GS_REGCONST( G_ROOMX				);
	GS_REGCONST( G_ROOMY				);
	GS_REGCONST( G_ROOMW				);
	GS_REGCONST( G_ROOMH				);
	GS_REGCONST( G_MAPW					);
	GS_REGCONST( G_MAPH					);
	GS_REGCONST( G_VIEWX				);
	GS_REGCONST( G_VIEWY				);
	GS_REGCONST( G_PAUSE				);
	GS_REGCONST( G_SHAKEX				);
	GS_REGCONST( G_SHAKEY				);
	GS_REGCONST( G_MAPCOLOR				);
	GS_REGCONST( G_BORDERCOLOR			);
	GS_REGCONST( G_FFMAGNITUDE			);
	GS_REGCONST( G_FFPERIOD				);
	GS_REGCONST( G_VIEWPORTMODE			);
	GS_REGCONST( G_VIEWPORTX			);
	GS_REGCONST( G_VIEWPORTY			);
	GS_REGCONST( G_VIEWPORTFLIP			);
	GS_REGCONST( G_FULLMATERIALMAP		);
	GS_REGCONST( G_INVENTORY			);
	GS_REGCONST( G_USER					);
	GS_REGCONST( G_MAX					);

	GS_REGCONST( CMD_START				);
	GS_REGCONST( CMD_EXIT				);
	GS_REGCONST( CMD_REFRESH			);
	
	GS_REGFN( GameFrame );				// > int
	GS_REGFN( GameGet );				// int(idx) > int(value/0)
	GS_REGFN( GameSet );				// int(idx), int(value)
	GS_REGFN( GameCommand );			// int(idx)
	GS_REGFN( GameSetName );			// str(name)
	GS_REGFN( MapLoad );				// str(map file) > int(1/0)

	// MATERIALS .................................................................................
	GS_REGCONST( MAT_MAX				);
	GS_REGCONST( MATD_VOID				);
	GS_REGCONST( MATD_SOFT				);
	GS_REGCONST( MATD_HARD				);
	GS_REGCONST( MATD_JUMP				);

	GS_REGFN( MaterialGetDensity );		// int(idx) > int(value/0)
	GS_REGFN( MaterialSetDensity );		// int(idx), int(value)
	GS_REGFN( MaterialGetColor );		// int(idx) > int(value/0)
	GS_REGFN( MaterialSetColor );		// int(idx), int(value)
	GS_REGFN( MaterialRead )			// int(x), int(y), int(w), int(h) > int(materials/0)
	GS_REGFN( MaterialDensityRead );	// int(x), int(y), int(w), int(h) > int(densities/0)
	GS_REGFN( MaterialCheckFree );		// int(x1), int(y1), int(x2), int(y2) > int(1/0)
	GS_REGFN( MaterialGetFreeDist );	// int(x1), int(y1), int(x2), int(y2), int(dir 0,1,2,3), int(hardonly) > int(free dist/0)

	// TILES .....................................................................................
	GS_REGCONST( TILE_ID		);
	GS_REGCONST( TILE_W			);
	GS_REGCONST( TILE_H			);
	GS_REGCONST( TILE_FRAMES	);
	GS_REGCONST( TILE_NAME		);

	GS_REGFN( TileFind );				// int(id) > int(idx/-1)
	GS_REGFN( TileCount );				// > int(count)
	GS_REGFN( TileGet );				// int(idx), int(data) > int/str/0
	GS_REGFN( TileLoad );				// str(tiles path) [,int(group)] > int(0/1)
	GS_REGFN( TileUnload );				// [int(group)]
	
	// SCRIPT ....................................................................................
	GS_REGFN( ScrGetHandlerData );		// int(idx) > int(value/0)
	GS_REGFN( ScrSetHandlerData );		// int(idx), int(value)
	GS_REGFN( ScrGetRequest );			// > int(request fp)
	GS_REGFN( ScrGetRunning );			// > int(running fp)
	GS_REGFN( ScrRequest );				// int(request fp)
	GS_REGFN( ScrKillLatent );			//

	// BRUSHES ...................................................................................
	GS_REGCONST( B_LAYER	);
	GS_REGCONST( B_X		);	
	GS_REGCONST( B_Y		);	
	GS_REGCONST( B_W		);	
	GS_REGCONST( B_H		);	
	GS_REGCONST( B_TILE		);
	GS_REGCONST( B_FRAME	);
	GS_REGCONST( B_MAP		);
	GS_REGCONST( B_FLIP		);
	GS_REGCONST( B_COLOR	);	
	GS_REGCONST( B_SHADER	);
	GS_REGCONST( B_SCALE	);
	GS_REGCONST( B_ID		);
	GS_REGCONST( B_MATERIAL	);
	GS_REGCONST( B_DRAW		);
	GS_REGCONST( B_DELAY	);
	GS_REGCONST( B_ANIM		);
	GS_REGCONST( B_MAX		);

	GS_REGFN( BrushCount );			// > int(count)
	GS_REGFN( BrushFind );			// int(id) > int(idx/-1)
	GS_REGFN( BrushGet );			// int(idx) > int(value/0)
	GS_REGFN( BrushSet );			// int(idx), int(value)


	// OBJECTS ...................................................................................
	GS_REGCONST( O_LAYER	);	
	GS_REGCONST( O_X		);		
	GS_REGCONST( O_Y		);		
	GS_REGCONST( O_W		);		
	GS_REGCONST( O_H		);
	GS_REGCONST( O_TILE		);	
	GS_REGCONST( O_FRAME	);	
	GS_REGCONST( O_MAP		);	
	GS_REGCONST( O_FLIP		);	
	GS_REGCONST( O_COLOR	);
	GS_REGCONST( O_SHADER	);
	GS_REGCONST( O_SCALE	);
	GS_REGCONST( O_ID		);
	GS_REGCONST( O_MATERIAL	);
	GS_REGCONST( O_DRAW		);
	GS_REGCONST( O_DISABLE	);	
	GS_REGCONST( O_DELAY	);
	GS_REGCONST( O_ANIM		);	
	GS_REGCONST( O_COLLIDER	);
	GS_REGCONST( O_CLASS	);	
	GS_REGCONST( O_STATUS	);	
	GS_REGCONST( O_TARGET	);
	GS_REGCONST( O_DEATH	);
	GS_REGCONST( O_COLLISION);
	GS_REGCONST( O_USER		);
	GS_REGCONST( O_MAX		);

	GS_REGFN( ObjCount );			// > int(count)
	GS_REGFN( ObjFind );			// int(id) > int(idx/-1)
	GS_REGFN( ObjGet );				// int(idx), int(var) > int(val/0)
	GS_REGFN( ObjSet );				// int(idx), int(var), int(val)
	GS_REGFN( ObjGetName );			// int(idx) > str(name/NULL/"")
	GS_REGFN( ObjSetName );			// int(idx), str(name)
	GS_REGFN( ObjPresent );			// int(idx)
	GS_REGFN( ObjPresentCount );	// > int(count)
	GS_REGFN( ObjPresentIdx );		// int(presentidx) > int(idx)
	GS_REGFN( ObjPresentGather );	//

	GS_REGFN( ColliderSnapDistance );	// int(x1), int(y1), int(x2), int(y2) > int(maxdist/0)

	// ROOMS .....................................................................................
	GS_REGCONST( R_MAX );	
	GS_REGFN( RoomGet );			// int(rx), int(ry), int(idx) > int(val/0)
	GS_REGFN( RoomSet );			// int(rx), int(ry), int(idx), int(val)
	GS_REGFN( RoomGetName );		// int(rx), int(ry) > str(name/"")
	GS_REGFN( RoomSetName );		// int(rx), int(ry), str(name)

	// PLAYER ...................................................................................
	GS_REGCONST( P_DISABLE		);
	GS_REGCONST( P_STATUS		);
	GS_REGCONST( P_DELAY		);
	GS_REGCONST( P_LAYER		);
	GS_REGCONST( P_X			);
	GS_REGCONST( P_Y			);
	GS_REGCONST( P_W			);
	GS_REGCONST( P_H			);
	GS_REGCONST( P_TILE			);
	GS_REGCONST( P_FRAME		);
	GS_REGCONST( P_ANIM			);
	GS_REGCONST( P_FLIP			);
	GS_REGCONST( P_COLOR		);
	GS_REGCONST( P_SHADER		);
	GS_REGCONST( P_EMOTION		);
	GS_REGCONST( P_DIR			);
	GS_REGCONST( P_POW			);
	GS_REGCONST( P_COSTUME		);
	GS_REGCONST( P_TILEIDLE		);
	GS_REGCONST( P_TILEWALK		);
	GS_REGCONST( P_TILEUP		);
	GS_REGCONST( P_TILEJUMP		);
	GS_REGCONST( P_AIRLEVEL		);
	GS_REGCONST( P_STUNLEVEL	);
	GS_REGCONST( P_MATINSIDE	);
	GS_REGCONST( P_MATUNDER		);
	GS_REGCONST( P_MATCENTER	);
	GS_REGCONST( P_LIFE			);
	GS_REGCONST( P_CREDITS		);
	GS_REGCONST( P_COINS		);
	GS_REGCONST( P_XSAFE		);
	GS_REGCONST( P_YSAFE		);
	GS_REGCONST( P_SAFE			);
	GS_REGCONST( P_CUSTOMMOVE	);
	GS_REGCONST( P_DEATH		);
	GS_REGCONST( P_USER			);
	GS_REGCONST( P_MAX			);

	GS_REGCONST( STATUS_IDLE	);
	GS_REGCONST( STATUS_WALK	);
	GS_REGCONST( STATUS_JUMP	);
	GS_REGCONST( STATUS_FALL	);
	GS_REGCONST( STATUS_SCRIPTED);

	GS_REGFN( PlayerGet );				// int(var) > int(val/0)
	GS_REGFN( PlayerSet );				// int(var), int(val)
	GS_REGFN( PlayerMakeBB );			// refint(x1), refint(x2), refint(y1), refint(y2)
	GS_REGFN( PlayerMakeBBW );			// refint(x1), refint(x2), refint(y1), refint(y2)

	// DIALOGS ...................................................................................
	GS_REGCONST( DLG_STYLE	);
	GS_REGCONST( DLG_X		);
	GS_REGCONST( DLG_Y		);
	GS_REGCONST( DLG_W		);
	GS_REGCONST( DLG_H		);
	GS_REGCONST( DLG_COLOR	);
	GS_REGCONST( DLG_FONT	);
	GS_REGCONST( DLG_USER	);
	GS_REGCONST( DLG_MAX	);

	GS_REGFN( DlgPush );				//
	GS_REGFN( DlgPop );					//
	GS_REGFN( DlgPopAll );				//
	GS_REGFN( DlgCount );				// > int(count)
	GS_REGFN( DlgGet );					// int(idx), int(var) > int(val/0)
	GS_REGFN( DlgSet );					// int(idx), int(var), int(val)
	GS_REGFN( DlgGetText );				// int(idx) > str(text/"")
	GS_REGFN( DlgSetText );				// int(idx), str(text)

	// SOUND .....................................................................................
	GS_REGCONST( SOUND_VOICES	);

	GS_REGFN( SampleLoad );				// str(path) [,int(group)] > (1/0)
	GS_REGFN( SampleUnload );			// [int(group)]
	GS_REGFN( SamplePlay );				// int(id) [,int(loop=0)] > int(voiceidx or -1)
	GS_REGFN( SamplePlaying );			// int(voiceidx) > int(id/-1)
	GS_REGFN( SampleStop );				// int(voiceidx)
	GS_REGFN( SampleStopAll );			//
	GS_REGFN( SampleVolume );			// int(percent)

	GS_REGFN( MusicLoad );				// str(file) [,int(group)] > int(1/0)
	GS_REGFN( MusicUnload );			// [int(group)]
	GS_REGFN( MusicFade );				// int(fadeout), int(fadein)
	GS_REGFN( MusicPlay );				// int(id) [,int(pos=0)] > int(0/-1)
	GS_REGFN( MusicPlaying );			// > int(id/-1)
	GS_REGFN( MusicPosition );			// > int(position ms/-1)
	GS_REGFN( MusicStop );				//
	GS_REGFN( MusicVolume );			// int(percent)

	// FONT ......................................................................................
	GS_REGFN( FontLoad );				// str(file) [,int(group)] > int(0/1)
	GS_REGFN( FontUnload );				// [int(group)]

	// HUD .......................................................................................
	GS_REGCONST( SHADER_OPAQUE );
	GS_REGCONST( SHADER_BLEND );
	GS_REGCONST( SHADER_ADD );
	GS_REGCONST( SHADER_MOD );
	GS_REGCONST( SHADER_MOD2 );

	GS_REGFN( HudDrawTile );			// int(tileid), int(x), int(y), int(w), int(h), int(map x), int(map y), int(map w), int(map h), int(flip), int(frame)
	GS_REGFN( HudDrawText );			// int(tileid), int(x), int(y), int(w), int(h), str(text), int(align)
	GS_REGFN( HudGetTextWidth );		// str(text) > int(horizontal size in pixels)
	GS_REGFN( HudGetTextHeight );		// str(text) > int(veritcal size in pixels)
	GS_REGFN( HudGetTextRows );			// str(text) > int(horizontal size in characters)
	GS_REGFN( HudGetTextColumns );		// str(text) > int(veritcal size in characters)
	GS_REGFN( HudClipping );			// int(x), int(y), int(w), int(h)
	GS_REGFN( HudFont );				// int(fontid)
	GS_REGFN( HudShader );				// int(shader)
	GS_REGFN( HudColor );				// int(color)

	// KEY .......................................................................................
	GS_REGCONST( KEY_LEFT );
	GS_REGCONST( KEY_RIGHT );	
	GS_REGCONST( KEY_UP );
	GS_REGCONST( KEY_DOWN );
	GS_REGCONST( KEY_JUMP );
	GS_REGCONST( KEY_ACTION );
	GS_REGCONST( KEY_MENU );

	GS_REGFN( KeyboardRead );			// > int(keycode)
	GS_REGFN( KeyboardStatus);			// int(keycode) > int(0=up,1=down)
	GS_REGFN( KeyboardCodeToChar );		// int(keycode) > int(char)
	GS_REGFN( KeyboardCharToCode );		// int(char) > int(keycode)
	GS_REGFN( JoystickStatus );			// int(keycode) > int(0=up,1=down)
	GS_REGFN( JoystickAxe );			// int(axe) > int(value -1000 to 1000)

	// DEBUG .....................................................................................
	GS_REGFN( DebugData );				// int(slot), str(data)
	GS_REGFN( DebugCool );				// int(1/0)
	GS_REGFN( DebugLog );				// int(1/0)
	GS_REGFN( DebugDev );				// int(1/0)
	GS_REGFN( DebugCallstack );			// > str(callstack)

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

