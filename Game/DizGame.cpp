//////////////////////////////////////////////////////////////////////////////////////////////////
// DizGame.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DizGame.h"
#include "DizApp.h"	// for error msg

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////
cDizGame g_game;

cDizGame::cDizGame()
{
	guard(cDizGame::cDizGame)

	m_screen_bw		= GAME_SCRWB;
	m_screen_bh		= GAME_SCRHB;
	m_screen_w		= GAME_SCRW;
	m_screen_h		= GAME_SCRH;

	m_gameframe		= 0;
	m_command		= CMD_NONE;
	m_matmap		= NULL;
	m_viewx			= 0;
	m_viewy			= 0;
	m_drawmode		= DRAWMODE_NORMAL;
				
	m_objcount		= 0;
	m_objsize		= 0;
	m_obj			= NULL;

	m_collidercount = 0;
	m_collidersize	= 0;
	m_collider		= NULL;

	memset(m_data,0,sizeof(m_data));
	memset(m_matdensity,0,sizeof(m_matdensity));
	memset(m_matcolor,0,sizeof(m_matcolor));

	m_data[G_VIEWX]	= GAME_VIEWX;
	m_data[G_VIEWY]	= GAME_VIEWY;
	
	m_fffx_magnitude	= 0;
	m_fffx_period		= 50;

	m_visible_brushes	= 0;
	unguard()
}

cDizGame::~cDizGame()
{
	guard(cDizGame::~cDizGame)
	unguard()
}

BOOL cDizGame::Init()
{
	guard(cDizGame::Init)

	m_objsize		= 128;
	m_obj			= (int*)smalloc( m_objsize*sizeof(int) );
	m_collidersize	= 128;
	m_collider		= (int*)smalloc( m_collidersize*sizeof(int) );

	Set(G_FPS,GAME_FPS);

	// load game resolution settings from inf
	sscanf( g_cfg.GetInfoValue( "game_screen_bw" ), "%i", &m_screen_bw );
	sscanf( g_cfg.GetInfoValue( "game_screen_bh" ), "%i", &m_screen_bh );
	sscanf( g_cfg.GetInfoValue( "game_screen_w" ), "%i", &m_screen_w );
	sscanf( g_cfg.GetInfoValue( "game_screen_h" ), "%i", &m_screen_h );
	g_paint.Layout(); // refresh layout

	return CheckVersion();
	unguard()
}

void cDizGame::Done()
{
	guard(cDizGame::Done)
	if(m_obj)		{ sfree(m_obj); m_obj=NULL; }
	if(m_collider)	{ sfree(m_collider); m_collider=NULL; }
	if(m_matmap)	{ sfree(m_matmap); m_matmap=NULL; }
	m_dlg.Done();
	unguard()
}

BOOL cDizGame::Start()
{
	guard(cDizGame::Start)

	// dialogs
	DlgPopAll();

	// map reset
	g_map.Reset();

	// game
	m_gameframe = 0;
	m_collidercount = 0;
//	memset(&m_data[G_CFG_MAX], 0, sizeof(int)*(G_MAX-G_CFG_MAX)); // keep cfg
//	Set(G_FFPERIOD,50);
//	Set(G_PAUSE,1); // start paused

	// player
	g_player.Reset();
	g_player.Set(P_X, 0);
	g_player.Set(P_Y, 0);
	g_player.Set(P_DISABLE, 1 );

	// fffx
	if(I9_IsReady()) FFFXStop();

	// room
	SetRoom( 0, 0 );

	// script
	g_script.Start();

	return TRUE;
	unguard()
}

BOOL cDizGame::CheckVersion()
{
	return true;
	guard(CheckVersion)
	// requested version must be in the folowing formats: 2.0, 2.01, 2.01b, etc
	// the match is done on first 2 digits (2.0)
	char* reqv = g_cfg.GetInfoValue("dizzyage_version");
	char* engv = GAME_VERSION;
	if(strlen(reqv)>=3) // enough digits
	{
		if( reqv[0]==engv[0] && reqv[1]==engv[1] && reqv[2]==engv[2] ) return TRUE; // match
		sys_msgbox( E9_GetHWND(), sprint("This game was made with DizzyAGE v%s\nBut, you are running it with DizzyAGE v%s\nIf you experience malfunctions, contact the author.",reqv,engv), "WARNING",MB_OK );
	}
	else
	{
		sys_msgbox( E9_GetHWND(), sprint("This game doesn't specify the version of DizzyAGE it was made for.\nYou are running it with DizzyAGE v%s\nIf you experience malfunctions, contact the author.",engv), "WARNING",MB_OK );
	}
	return TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cDizGame::Update()
{
	guard(cDizGame::Update)

	int i;
	int rx, ry;

	// command process
	if(m_command==CMD_START) // start game
	{
		BOOL ok = Start();
		if(!ok) return FALSE;
	}
	else
	if(m_command==CMD_EXIT) // exit game
	{
		return FALSE;
	}
	if(m_command==CMD_REFRESH) // refresh room
	{
		MatMapUpdate();
	}
	m_command = CMD_NONE;

	// game frame
	m_gameframe++;

	// key input
	int keys = 0;
	static int oldkeys = 0;

	if(I9_IsReady())
	{
		for(i=0;i<KEY_MAX;i++)
		{
			if( I9_GetKeyValue(g_cfg.m_key[i][0]) ||
				I9_GetKeyValue(g_cfg.m_key[i][1]) )	
				keys |= (1<<i);
		}
		
		// joystick bogus values are higher on y because up can be used as jump...
		int dzx = g_cfg.m_deadzone[0];
		int dzy = g_cfg.m_deadzone[1];
		int jax = I9_JOY_FIRSTAXE(0)+g_cfg.m_joy[4];
		int jay = I9_JOY_FIRSTAXE(0)+g_cfg.m_joy[5];
		int	jb0 = I9_JOY_FIRSTKEY(0)+g_cfg.m_joy[0];
		int	jb1 = I9_JOY_FIRSTKEY(0)+g_cfg.m_joy[1];
		int	jb2 = I9_JOY_FIRSTKEY(0)+g_cfg.m_joy[2];

		if( I9_GetAxeValue(jax)<-dzx || I9_GetJoystickHAT(0,I9_HUT_LEFT)  )	keys |= (1<<KEY_LEFT);
		if( I9_GetAxeValue(jax)> dzx || I9_GetJoystickHAT(0,I9_HUT_RIGHT) )	keys |= (1<<KEY_RIGHT);
		if( I9_GetAxeValue(jay)<-dzy || I9_GetJoystickHAT(0,I9_HUT_UP)    )	keys |= (1<<KEY_UP);
		if( I9_GetAxeValue(jay)> dzy || I9_GetJoystickHAT(0,I9_HUT_DOWN)  )	keys |= (1<<KEY_DOWN);
		if( I9_GetKeyValue(jb1) )											keys |= (1<<KEY_JUMP);		// xbox360 B
		if( I9_GetKeyValue(jb0) )											keys |= (1<<KEY_ACTION);	// xbox360 A
		if( I9_GetKeyValue(jb2) )											keys |= (1<<KEY_MENU);		// xbox360 X
	}

	// avoid keys during debug
	if(g_player.m_debug) keys=0;

	Set(G_KEYS,keys);
	int keyshit = (keys ^ oldkeys) & keys;
	Set(G_KEYSHIT,keyshit);
	oldkeys = keys;

	// game update
	g_script.CallHandler(HANDLER_GAMEUPDATE);

	// script update
	g_script.Update();

	// update present objects
	if(!Get(G_PAUSE))
	{
		for( i=0; i<m_objcount; i++ )
		{
			tBrush* obj = g_map.m_obj.Get(m_obj[i]); sassert(obj!=NULL);
			if( obj->Get(BRUSH_DISABLE)!=0 ) continue; // only enabled objects
			if( obj->Get(BRUSH_ANIM)!=0 )
			{
				if(IsUpdate(obj->Get(BRUSH_DELAY)))
				{
					int frame=obj->Get(BRUSH_FRAME);
					obj->Set(BRUSH_FRAME,frame+1);
				}
			}
		}
	}

	// player update
	if(!Get(G_PAUSE)) g_player.Update();

	// map update
	if( MAP_W>0 && MAP_H>0 ) // if map size is valid
	{
		// room bound check
		rx = Get(G_ROOMX);
		ry = Get(G_ROOMY);
		BOOL outroom = FALSE;
		outroom |= ( g_player.Get(P_X) < rx*ROOM_W );
		outroom |= ( g_player.Get(P_X) >= (rx+1)*ROOM_W );
		outroom |= ( g_player.Get(P_Y) < ry*ROOM_H );
		outroom |= ( g_player.Get(P_Y) >= (ry+1)*ROOM_H );
		if(outroom)	g_script.CallHandler(HANDLER_ROOMOUT); // users may change player's pos on this handler

		// world bound check
		rx = MAP_POSX2ROOMX( g_player.Get(P_X) );
		ry = MAP_POSY2ROOMY( g_player.Get(P_Y) );
		if( rx<0 )				{ rx=0; g_player.Set(P_X,0); }
		if( rx>MAP_W-1 )	{ rx=MAP_W-1; g_player.Set(P_X,MAP_W*ROOM_W-4); }
		if( ry<0 )				{ ry=0; g_player.Set(P_Y,0); }
		if( ry>MAP_H-1 )	{ ry=MAP_H-1; g_player.Set(P_Y,MAP_H*ROOM_H-1); }

		// room tranzit
		if( rx!=Get(G_ROOMX) ||	ry!=Get(G_ROOMY) )
		{
			g_script.CallHandler(HANDLER_ROOMCLOSE);
			SetRoom( rx, ry );
			g_script.CallHandler(HANDLER_ROOMOPEN);
		}
	}

	// menu
	if( KeyHit(KEY_MENU) && m_dlg.Size()==0 && !Get(G_PAUSE) )
	{
		g_script.CallHandler(HANDLER_MENU);
	}

	// action
	if( KeyHit(KEY_ACTION) && m_dlg.Size()==0 && !Get(G_PAUSE) )
	{
		if(	g_player.Get(P_LIFE)>0 && 
			(g_player.Get(P_STATUS)==STATUS_IDLE || g_player.Get(P_STATUS)==STATUS_WALK) &&
			!g_player.Get(P_DISABLE) )
		{
			g_script.CallHandler(HANDLER_ACTION);
		}
	}

	// fffx rumble
	BOOL update = FALSE;
	if(m_fffx_period!=Get(G_FFPERIOD))
	{
		m_fffx_period = Get(G_FFPERIOD);
		update = TRUE;
	}
	if(m_fffx_magnitude!=Get(G_FFMAGNITUDE))
	{
		m_fffx_magnitude = Get(G_FFMAGNITUDE);
		update = TRUE;
	}
	if(update) FFFXUpdate();

	// game after update
	g_script.CallHandler(HANDLER_GAMEAFTERUPDATE);

	return TRUE;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//DRAW
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizGame::Draw()
{
	guard(cDizGame::Draw)
	int i;

	// clipping
	fRect rect; // visible room area
	fRect clip; // draw room area (for viewportmode=1)
	rect.x1 = (float)(g_paint.m_scrx+Get(G_VIEWX)*g_paint.m_scale);
	rect.x2 = (float)(g_paint.m_scrx+(Get(G_VIEWX)+ROOM_W)*g_paint.m_scale);
	rect.y1 = (float)(g_paint.m_scry+Get(G_VIEWY)*g_paint.m_scale);
	rect.y2 = (float)(g_paint.m_scry+(Get(G_VIEWY)+ROOM_H)*g_paint.m_scale);

	// view ofset with shake option and optional viewport for scrolling
	m_viewx = Get(G_VIEWX) + Get(G_SHAKEX);
	m_viewy = Get(G_VIEWY) + Get(G_SHAKEY);
	if( Get(G_VIEWPORTMODE) )
	{
		m_viewx += Get(G_VIEWPORTX);
		m_viewy += Get(G_VIEWPORTY);
	}
	
	// viewport flipping
	dword flip = Get(G_VIEWPORTFLIP);
	if( flip )
	{
		int viewx = 0;
		int viewy = 0;

		if( flip & R9_FLIPX )
		{
			int vx = g_paint.m_scrx + Get(G_VIEWX)*g_paint.m_scale;
			viewx = R9_GetWidth() - vx - Get(G_ROOMW)*g_paint.m_scale - vx + 1; // magic +1
		}
		if( flip & R9_FLIPY )
		{
			int vy = g_paint.m_scry + Get(G_VIEWY)*g_paint.m_scale;
			viewy = R9_GetHeight() - vy - Get(G_ROOMH)*g_paint.m_scale - vy + 1; // magic +1
		}

		R9_SetView( viewx, viewy, flip );
	}

	m_visible_brushes = 0;

	// for each layer
	for(int layer=0;layer<GAME_LAYERS;layer++)
	{
		// room
		if( Get(G_VIEWPORTMODE) )
		{
			// full matmap 3x3 rooms
			for( int ry=0; ry<3; ry++ )
			{
				for( int rx=0; rx<3; rx++ )
				{
					// clip here to avoid duplicate draw (brushes shared in neighbour rooms)
					// Note: brushes order must also be perserved (so the drawframe trick didn't work)
					R9_SetClipping( rect );
					clip.x1 = (float)g_paint.m_scrx + (m_viewx+(rx-1)*ROOM_W)*g_paint.m_scale,
					clip.y1 = (float)g_paint.m_scry + (m_viewy+(ry-1)*ROOM_H)*g_paint.m_scale,
					clip.x2 = clip.x1 + ROOM_W*g_paint.m_scale;
					clip.y2 = clip.y1 + ROOM_H*g_paint.m_scale;
					R9_AddClipping( clip );
					g_map.DrawRoom( Get(G_ROOMX)+rx-1, Get(G_ROOMY)+ry-1, layer, m_drawmode, m_viewx+(rx-1)*ROOM_W, m_viewy+(ry-1)*ROOM_H );
				}
			}
		}
		else
		{
			// classic style
			R9_SetClipping( rect );
			g_map.DrawRoom( Get(G_ROOMX), Get(G_ROOMY), layer, m_drawmode, m_viewx, m_viewy );
		}

		// objects present
		R9_SetClipping( rect );
		for( i=0; i<m_objcount; i++ )
		{
			tBrush* obj = g_map.m_obj.Get( m_obj[i] );
			if( obj->Get(BRUSH_LAYER)!=layer ) continue;
			if( obj->Get(BRUSH_DISABLE)!=0 ) continue;
			if((obj->Get(BRUSH_DRAW) & 1)==0 ) continue;
			ObjDraw(obj);
		}

		// dizzy
		if(g_player.Get(P_LAYER)==layer) g_player.Draw();
	}

	if( flip )
		R9_SetView( 0, 0, 0 );

	// borders
	R9_DrawBar( fRect( 0.0f, 0.0f, (float)R9_GetWidth(), rect.y1 ),					Get(G_BORDERCOLOR)|0xff000000);
	R9_DrawBar( fRect( 0.0f, rect.y2, (float)R9_GetWidth(), (float)R9_GetHeight() ),Get(G_BORDERCOLOR)|0xff000000);
	R9_DrawBar( fRect( 0.0f, rect.y1, rect.x1, rect.y2 ),							Get(G_BORDERCOLOR)|0xff000000);
	R9_DrawBar( fRect( rect.x2, rect.y1, (float)R9_GetWidth(),rect.y2 ),			Get(G_BORDERCOLOR)|0xff000000);

/*	// clip
	rect.x1 = g_paint.m_scrx;
	rect.x2 = g_paint.m_scrx+g_game.m_screen_w*g_paint.m_scale;
	rect.y1 = g_paint.m_scry;
	rect.y2 = g_paint.m_scry+g_game.m_screen_h*g_paint.m_scale;
	R9_SetClipping(rect);
*/
	// HUD (cover, dialogs, menus, etc)
	R9_ResetClipping();
	g_paint.m_huddraw = TRUE;
	g_script.CallHandler(HANDLER_DRAWHUD);
	g_paint.m_huddraw = FALSE;
	R9_ResetClipping();

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// DIALOGS
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizGame::DlgPush()
{
	guard(cDizGame::DlgPush)
	tDialog* dlg = snew tDialog();
	m_dlg.Add(dlg);
	unguard()
}

void cDizGame::DlgPop()
{
	guard(cDizGame::DlgPop)
	if(m_dlg.Size()>0)
		m_dlg.Del(m_dlg.Size()-1);
	unguard()
}

void cDizGame::DlgPopAll()
{
	guard(cDizGame::DlgPopAll)
	while(m_dlg.Size()>0) DlgPop();
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ROOM
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizGame::SetRoom( int x, int y )
{
	guard(cDizGame::SetRoom)
	Set(G_ROOMX, x);
	Set(G_ROOMY, y);
	MatMapUpdate();
	ObjGather();
	unguard()
}

void cDizGame::MatMapAlloc()
{
	guard(cDizGame::MatMapAlloc);
	if(m_matmap) sfree(m_matmap);
	m_matmap = (byte*)smalloc(9*ROOM_W*ROOM_H);
	unguard();
}

void cDizGame::MatMapUpdate()
{
	guard(cDizGame::MatMapUpdate)
	if(!m_matmap) return; // not allocated yet
	
	// clear
	memset(m_matmap,0,9*ROOM_W*ROOM_H);
	
	// prepare coordinates
	int scrx = g_paint.m_scrx;
	int scry = g_paint.m_scry;
	int scale = g_paint.m_scale;
	g_paint.m_scrx = 0;
	g_paint.m_scry = 0;
	g_paint.m_scale = 1;

	// clipping
	fRect oldclip = R9_GetClipping();
	
	// prepare for software rendering
	g_paint.m_drawtilesoft = TRUE;
	g_paint.m_imgtarget.m_pf = R9_PF_A;
	g_paint.m_imgtarget.m_width = 3*ROOM_W;
	g_paint.m_imgtarget.m_height = 3*ROOM_H;
	g_paint.m_imgtarget.m_size = 9*ROOM_W*ROOM_H;
	g_paint.m_imgtarget.m_data = m_matmap;

	// draw room
	for(int layer=0; layer<GAME_LAYERS; layer++)
	{
		// room
		if( Get(G_FULLMATERIALMAP) )
		{
			// full matmap 3x3 rooms
			for( int ry=0; ry<3; ry++ )
			{
				for( int rx=0; rx<3; rx++ )
				{
					// clip here to avoid duplicate draw (brushes shared in neighbour rooms)
					R9_SetClipping( fRect(rx*ROOM_W,ry*ROOM_H,rx*ROOM_W+ROOM_W,ry*ROOM_H+ROOM_H) );
					g_map.DrawRoom( Get(G_ROOMX)+rx-1, Get(G_ROOMY)+ry-1, layer, DRAWMODE_MATERIAL, rx*ROOM_W, ry*ROOM_H );
				}
			}
		}
		else
		{
			// classic style
			R9_SetClipping( fRect(ROOM_W-ROOM_B,ROOM_H-ROOM_B,2*ROOM_W+ROOM_B,2*ROOM_H+ROOM_B) );
			g_map.DrawRoom( Get(G_ROOMX), Get(G_ROOMY), layer, DRAWMODE_MATERIAL, ROOM_W, ROOM_H);
		}
	}

	// rollback
	g_paint.m_drawtilesoft = FALSE;
	g_paint.m_scrx = scrx;
	g_paint.m_scry = scry;
	g_paint.m_scale = scale;
	R9_SetClipping(oldclip);

	// @DBG material (dark colors)
	// R9_ImgSaveFile("matdump.tga",&g_paint.m_imgtarget);

	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// OBJECTS
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizGame::ObjPresent( int idx )
{
	guard(cDizGame::ObjPresent)
	for(int i=0;i<m_objcount;i++)
		if(m_obj[i]==idx) return;
	ObjAdd(idx);
	unguard()
}

void cDizGame::ObjGather()
{
	guard(cDizGame::ObjGather)

	int i;
	m_objcount = 0;
	m_collidercount = 0;

	iRect objbb;
	iRect roombb;
	if( Get(G_VIEWPORTMODE) )
	{
		// extended room bound to 3x3 rooms
		MakeRoomBBW(roombb.x1,roombb.y1,roombb.x2,roombb.y2,0);
		roombb.x1 -= ROOM_W;
		roombb.x2 += ROOM_W;
		roombb.y1 -= ROOM_H;
		roombb.y2 += ROOM_H;
	}
	else
	{
		// room bound with small border
		MakeRoomBBW(roombb.x1,roombb.y1,roombb.x2,roombb.y2,ROOM_B);
	}

	for( i=0; i<g_map.ObjCount(); i++ )
	{
		tBrush* obj = g_map.m_obj.Get(i);
		obj->MakeBBW(objbb.x1,objbb.y1,objbb.x2,objbb.y2);
		if(RECT2RECT(objbb,roombb)) ObjAdd(i); // object is present in current bordered room
	}

	unguard()
}

void cDizGame::ObjDraw( tBrush* brush )
{
	guard(cDizGame::ObjDraw)
	
	// MAKEBBR
	int rx = Get(G_ROOMX);
	int ry = Get(G_ROOMY);
	int x1,y1,x2,y2;
	brush->MakeBBW(x1,y1,x2,y2);
	x1 = x1 - rx*ROOM_W;
	x2 = x2 - rx*ROOM_W;
	y1 = y1 - ry*ROOM_H;
	y2 = y2 - ry*ROOM_H;

	// draw current tile frame
	int idx = g_paint.TileFind(brush->Get(BRUSH_TILE));
	cTile* tile = g_paint.TileGet(idx); if(!tile) return;
	int frame = ComputeFrame(brush->Get(BRUSH_FRAME),tile->m_frames,brush->Get(BRUSH_ANIM));
	g_paint.DrawBrush( brush, m_viewx+x1, m_viewy+y1, frame );

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// FFFX Rumble
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizGame::FFFXUpdate()
{
	guard(cDizGame::FFFXUpdate)
	if(!I9_IsReady()) return;
	if(!I9_DeviceIsPresent(I9_DEVICE_JOYSTICK1)) return; // no joystick
	BOOL isrunning = I9_DeviceFFIsPlaying(I9_DEVICE_JOYSTICK1);
	if(g_cfg.m_rumble)
	{
		I9_DeviceFFSet(I9_DEVICE_JOYSTICK1, m_fffx_magnitude*100, m_fffx_period);
		if(m_fffx_magnitude==0 && isrunning) I9_DeviceFFStop(I9_DEVICE_JOYSTICK1);
		else
		if(m_fffx_magnitude!=0 && !isrunning) I9_DeviceFFPlay(I9_DEVICE_JOYSTICK1);
	}
	else
	{
		I9_DeviceFFSet(I9_DEVICE_JOYSTICK1, 0, 0);
		if(isrunning) I9_DeviceFFStop(I9_DEVICE_JOYSTICK1);
	}
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
