//////////////////////////////////////////////////////////////////////////////////////////////////
// DizPlayer.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DizPlayer.h"
#include "DizGame.h"

cDizPlayer	g_player;

#define DATA(idx)					( m_data[idx] )
#define MATINSIDE(mat)				( DATA(P_MATINSIDE) & (1<<mat) )
#define KEY(key)					( (m_input) ? g_game.Key(key) : FALSE )
#define KEYHIT(key)					( (m_input) ? g_game.KeyHit(key) : FALSE )

#define	MAKEBB( x1, y1, x2, y2 )	\
{									\
	int rx = g_game.Get(G_ROOMX);	\
	int ry = g_game.Get(G_ROOMY);	\
	MakeBBW(x1,y1,x2,y2);			\
	x1 = x1 - rx*ROOM_W;			\
	x2 = x2 - rx*ROOM_W;			\
	y1 = y1 - ry*ROOM_H;			\
	y2 = y2 - ry*ROOM_H;			\
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////

cDizPlayer::cDizPlayer()
{
	guard(cDizPlayer::cDizPlayer)
	Reset();
	unguard()
}

cDizPlayer::~cDizPlayer()
{
	guard(cDizPlayer::~cDizPlayer)
	unguard()
}

void cDizPlayer::Reset()
{
	guard(cDizPlayer::Reset)

	memset( m_data, 0, sizeof(m_data) );

	DATA(P_SHADER)		= SHADER_BLEND;
	DATA(P_STATUS)		= STATUS_IDLE;
	DATA(P_DELAY)		= 3;
	DATA(P_LAYER)		= 7;
	DATA(P_W)			= DIZ_BOXW;
	DATA(P_H)			= DIZ_BOXH;

	DATA(P_TILE)		= TILE_IDLE;
	DATA(P_ANIM)		= 2;
	DATA(P_COLOR)		= 0xffffffff;

	DATA(P_TILEIDLE)	= TILE_IDLE;
	DATA(P_TILEWALK)	= TILE_WALK;
	DATA(P_TILEUP)		= TILE_UP;
	DATA(P_TILEJUMP)	= TILE_JUMP;

	DATA(P_LIFE)		= 100;
	DATA(P_CREDITS)		= 3;

	m_input	= TRUE;
	m_debug	= FALSE;

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////////////////////////////////////////

void cDizPlayer::Update()
{
	guard(cDizPlayer::Update)

	//Sleep(200); // @T

	if( DATA(P_DISABLE) || m_debug ) return; // disabled

	// delay
	if( !g_game.IsUpdate(DATA(P_DELAY)) ) return;

	// read mat info
	ReadMatInfo(); // read material info

	// objects collison
	CheckColliders();

	// script
	g_script.CallHandler(HANDLER_PLAYERUPDATE);

	if( DATA(P_CUSTOMMOVE)!=0 )	return; // player was custom moved on the player update handler

	// input status
	if( DATA(P_STATUS)==STATUS_IDLE || DATA(P_STATUS)==STATUS_WALK )
	{
		EnterKeyState();
	}

	// states update
	if( DATA(P_STATUS)==STATUS_IDLE ) 
		UpdateIdle();
	else
	if( DATA(P_STATUS)==STATUS_WALK )
		UpdateWalk();
	else
	if( DATA(P_STATUS)==STATUS_JUMP )
		UpdateJump();
	else
	if( DATA(P_STATUS)==STATUS_FALL )
		UpdateFall();
	else
	if( DATA(P_STATUS)==STATUS_SCRIPTED )
		UpdateScripted();

	if( DATA(P_STATUS)!=STATUS_SCRIPTED )
	{
		BOOL snap = CheckCollidersSnap();

		// stand check only if not already snapped to collider
		if( !snap && (DATA(P_STATUS)==STATUS_IDLE || DATA(P_STATUS)==STATUS_WALK) )
		{
			int h = CheckFallY(1); // see if it can fall 
			if(h>0) // if any space below then enter in fall
			{
				EnterFall();
				DATA(P_Y)++; 
				DATA(P_POW)++; // force one step down (DIZZYMATCH)
			}
		}
		
		// fix collision by rising dizzy
		CheckCollision();
	}

	unguard()
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// ENTER STATES
// don't seem to need to change tile when enter states
//////////////////////////////////////////////////////////////////////////////////////////////////

void cDizPlayer::EnterIdle()
{
	guard(cDizPlayer::EnterIdle)
	DATA(P_STATUS)	= STATUS_IDLE;
	DATA(P_DIR)		= 0;
	DATA(P_POW)		= 0;
	DATA(P_FLIP)	= DATA(P_FLIP) & R9_FLIPY;

	if( DATA(P_TILE)!=DATA(P_COSTUME)+DATA(P_TILEIDLE)+DATA(P_EMOTION) )
	{
		DATA(P_FRAME) = 0;
		DATA(P_TILE) = DATA(P_COSTUME)+DATA(P_TILEIDLE)+DATA(P_EMOTION);
	}

	unguard()
}

void cDizPlayer::EnterWalk( int dir )
{
	guard(cDizPlayer::EnterWalk)
	DATA(P_STATUS)	= STATUS_WALK; 
	DATA(P_DIR)		= dir;
	DATA(P_POW)		= 0;
	DATA(P_FLIP)	= (DATA(P_FLIP) & R9_FLIPY) | (DATA(P_DIR)==-1);

	if( DATA(P_TILE)!=DATA(P_COSTUME)+DATA(P_TILEWALK) )
	{
		DATA(P_FRAME) = 0;
		DATA(P_TILE) = DATA(P_COSTUME)+DATA(P_TILEWALK);
	}

	unguard()
}

void cDizPlayer::EnterJump( int dir, int pow )
{
	guard(cDizPlayer::EnterJump)
	DATA(P_STATUS)	= STATUS_JUMP; 
	DATA(P_DIR)		= dir;
	DATA(P_POW)		= pow;
	DATA(P_FLIP)	= (DATA(P_FLIP) & R9_FLIPY) | (DATA(P_DIR)==-1);

	if( DATA(P_DIR)==0 && DATA(P_TILE)!=DATA(P_COSTUME)+DATA(P_TILEUP) )
	{
		DATA(P_FRAME) = 0;
		DATA(P_TILE) = DATA(P_COSTUME)+DATA(P_TILEUP);
	}
	else 
	if( DATA(P_DIR)!=0 && DATA(P_TILE)!=DATA(P_COSTUME)+DATA(P_TILEJUMP) )
	{
		DATA(P_FRAME) = 0;
		DATA(P_TILE) = DATA(P_COSTUME)+DATA(P_TILEJUMP);
	}
	
	unguard()
}

void cDizPlayer::EnterFall()
{
	guard(cDizPlayer::EnterFall)
	DATA(P_STATUS)		= STATUS_FALL; 
	DATA(P_POW)			= 1;
	// don't change tile and frame
	unguard()
}

void cDizPlayer::EnterRoll()
{
	guard(cDizPlayer::EnterRoll)
	
	DATA(P_POW)	= 1; // cut fall power to roll on ground
	
	if( DATA(P_TILE)==DATA(P_COSTUME)+DATA(P_TILEUP) || DATA(P_TILE)==DATA(P_COSTUME)+DATA(P_TILEJUMP) ) // only when jumping
	{
		cTile* tile = FindTile();
		if(tile)
		{
			int frame = ComputeFrame( DATA(P_FRAME), tile->m_frames, DATA(P_ANIM) );
			//if( frame < tile->m_frames-1 ) return; // don't enter idle unless last frame reached; untill then stay in roll
			if( frame != 0 ) return; // don't enter idle unless last frame reached; untill then stay in roll
		}
	}

	EnterKeyState(); // be sure to stop the fall, just in case the fall handler doesn't

	g_script.CallHandler( HANDLER_FALL );
	
	DATA(P_STUNLEVEL) = 0; // clear stun level

	unguard()
}

void cDizPlayer::EnterJumper( int mat )
{
	guard(cDizPlayer::EnterJumper)
	if(DATA(P_LIFE)==0) { EnterIdle(); return; } // no more jumps for mr. dead guy

	int dir = 0;
	int pow = 0;

	// direction
	if(KEY(KEY_RIGHT))	dir++;
	if(KEY(KEY_LEFT))	dir--;

	// call jump handler to determine the power of the jump
	g_script.m_handlerdata[0] = mat; // send the material
	g_script.m_handlerdata[1] = 0;	 // clean return for safety
	g_script.CallHandler(HANDLER_JUMP);
	pow = g_script.m_handlerdata[1]; // receive power

	if(pow>0)
		EnterJump(dir,pow); // still jumpy
	else
		EnterRoll(); // roll

	unguard()
}

void cDizPlayer::EnterSpin( int dir )
{
	guard(cDizPlayer::EnterSpin)
	EnterFall();
	DATA(P_DIR)		= dir;
	DATA(P_TILE)	= DATA(P_COSTUME)+DATA(P_TILEJUMP);
	DATA(P_FLIP)	= (DATA(P_FLIP) & R9_FLIPY) | (DATA(P_DIR)==-1);
	DATA(P_FRAME)	= 1;
	unguard()
}

void cDizPlayer::EnterKeyState()
{
	guard(cDizPlayer::EnterKeyState)
	if( DATA(P_LIFE)<=0 ) {	EnterIdle(); return; } // prepare to die

	int dir = 0;
	if( KEY(KEY_RIGHT) )	dir++;
	if( KEY(KEY_LEFT) )		dir--;
	if( KEY(KEY_JUMP) )		
	{
		// call jump handler to determine the power of the jump
		g_script.m_handlerdata[0] = -1; // send no material
		g_script.m_handlerdata[1] = 0;  // clean return for safety
		g_script.CallHandler(HANDLER_JUMP);
		int pow = g_script.m_handlerdata[1]; // receive power
		if(pow>0) EnterJump(dir,pow); // 7 would be the default jump power
	}
	else
	if(dir!=0)
	{
		EnterWalk(dir);
	}
	else 
	{
		EnterIdle();
	}
	unguard()
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE STATES
//////////////////////////////////////////////////////////////////////////////////////////////////

void cDizPlayer::UpdateIdle()
{
	guard(cDizPlayer::UpdateIdle)

	DATA(P_FRAME)++;

	unguard()
}

void cDizPlayer::UpdateWalk()
{
	guard(cDizPlayer::UpdateWalk)

	if( CheckWalkX() )
		DATA(P_X) += DATA(P_DIR)*DIZ_STEPX;

	DATA(P_FRAME)++;
	
	unguard()
}

void cDizPlayer::UpdateJump()
{
	guard(cDizPlayer::UpdateJump)
	
	if( CheckJumpX() )
		DATA(P_X) += DATA(P_DIR)*DIZ_STEPX;
	int step = MIN(DATA(P_POW),DIZ_STEPYMAX);
	step = CheckJumpY(step);
	DATA(P_Y) -= step;
	DATA(P_POW)--;

	DATA(P_FRAME)++;

	if( DATA(P_POW)<0 ) // done jumping - see where to go idle or fall
	{
		int under = CheckFallY(1);
		if(under==0)
			EnterKeyState();
		else
			EnterFall();
	}

	unguard()
}

void cDizPlayer::UpdateFall()
{
	guard(cDizPlayer::UpdateFall)

	if( CheckFallX() )
		DATA(P_X) += DATA(P_DIR)*DIZ_STEPX;

	int step = MIN(DATA(P_POW),DIZ_STEPYMAX);
	int step2 = CheckFallY(step);
	DATA(P_Y) += step2;

	DATA(P_FRAME)++; // keep last tile
	DATA(P_STUNLEVEL)++;

	// stopping fall if fall step was reduced
	if(step2<step)
	{
		// check for jumpers
		int mat = CheckJumper();
		if( mat>=0 ) // it's a jumper!
			EnterJumper( mat );
		else
			EnterRoll();
	}
	else
		DATA(P_POW)++;

	unguard()
}

void cDizPlayer::UpdateScripted()
{
	guard(cDizPlayer::UpdateScripted)
	
	if(DATA(P_ANIM)!=0) DATA(P_FRAME)++;

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// CHECKS
//////////////////////////////////////////////////////////////////////////////////////////////////

// check if rectangle have no hard materials in side
BOOL cDizPlayer::CheckFree( int x1, int y1, int x2, int y2 )
{
	guard(cDizPlayer::CheckFree)
	for(int iy=y1;iy<y2;iy++)
		for(int ix=x1;ix<x2;ix++)
			if( MATDENSITY(g_game.MatMap(ix,iy))==MATD_HARD ) 
				return FALSE;
	return TRUE;
	unguard()
}

// check side, only above 8 bottom pixels. if bottom is blocked it will step-up on it
BOOL cDizPlayer::CheckWalkX()
{
	guard(cDizPlayer::CheckWalkX)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	if(DATA(P_DIR)==1)
		return CheckFree( x2,y1,x2+DIZ_STEPX,y2-8 );
	else
	if(DATA(P_DIR)==-1)
		return CheckFree( x1-DIZ_STEPX,y1,x1,y2-8 );
	else
		return FALSE;
	unguard()
}

BOOL cDizPlayer::CheckJumpX()
{
	guard(cDizPlayer::CheckJumpX)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	if(DATA(P_DIR)==1)
		return CheckFree( x2,y1,x2+DIZ_STEPX,y2-8 );
	else
	if(DATA(P_DIR)==-1)	
		return CheckFree( x1-DIZ_STEPX,y1,x1,y2-8 );
	else
		return FALSE;
	unguard()
}

// check material above the box and see how far can it go
int cDizPlayer::CheckJumpY( int step )
{
	guard(cDizPlayer::CheckJumpY)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	for(int iy=y1-1;iy>y1-1-step;iy--)
	{
		for(int ix=x1;ix<x2;ix++)
			if( MATDENSITY(g_game.MatMap(ix,iy))==MATD_HARD ) 
				return ((y1-1)-iy);
	}
	return step;
	unguard()
}

BOOL cDizPlayer::CheckFallX()
{
	guard(cDizPlayer::CheckFallX)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	if(DATA(P_DIR)==1)	
		return CheckFree( x2,y1,x2+DIZ_STEPX,y2-8 );
	else
	if(DATA(P_DIR)==-1)	
		return CheckFree( x1-DIZ_STEPX,y1,x1,y2-8 );
	else
		return FALSE;
	unguard()
}

// check material under the box and see how far can it go
int cDizPlayer::CheckFallY( int step )
{
	guard(cDizPlayer::CheckFallY)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	for( int iy=y2; iy<y2+step; iy++ )
		for(int ix=x1;ix<x2;ix++)
			if( MATDENSITY(g_game.MatMap(ix,iy))!=MATD_VOID ) 
				return (iy-y2); // return minimized step if block found
	return step;
	unguard();
}

// collision inside box bottom will rise dizzy up with maximum DIZ_STEPY 
void cDizPlayer::CheckCollision()
{
	guard(cDizPlayer::CheckCollision)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	
	// going from up to bottom until the first hard line found
	for( int iy=y2-DIZ_STEPY; iy<y2; iy++ )
	{
		BOOL hard = FALSE;
		for(int ix=x1;ix<x2;ix++)
		{
			if( MATDENSITY(g_game.MatMap(ix,iy))==MATD_HARD ) 
			{
				DATA(P_Y) -= y2-iy;
				return;
			}
		}
	}
	
	unguard()
}

int cDizPlayer::CheckJumper()
{
	guard(cDizPlayer::CheckJumper)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);

	for(int ix=x1;ix<x2;ix++)
	{
		int mat = g_game.MatMap(ix,y2);
		if( MATDENSITY(mat)==MATD_JUMP ) return mat;
	}

	return -1;
	unguard()
}


void cDizPlayer::CheckColliders()
{
	guard(cDizPlayer::CheckColliders)
	int x1,y1,x2,y2;
	int cx1,cy1,cx2,cy2;
	MakeBBW(x1,y1,x2,y2);
	
	for( int i=0; i<g_game.m_collidercount; i++ )
	{
		int idx = g_game.m_collider[i];
		tBrush* obj = g_map.ObjGet(idx); sassert(obj);
		if( obj->Get(BRUSH_DISABLE)!=0 ) continue; // only enabled objects
		if(!(obj->Get(BRUSH_COLLIDER) & COLLIDER_HANDLE)) continue; // just those that request it
		cx1 = obj->Get(BRUSH_X);
		cy1 = obj->Get(BRUSH_Y);
		cx2 = cx1+obj->Get(BRUSH_W);
		cy2 = cy1+obj->Get(BRUSH_H);

		BOOL collision = !( x2<=cx1 || x1>=cx2 || y2<=cy1 || y1>=cy2 );
		if(!collision && !obj->Get(BRUSH_COLLISION)) continue; // no collision event

		// call
		g_script.m_handlerdata[0] = idx; // send object index
		if(!collision)					g_script.m_handlerdata[1] = 0;   // exit from collision
		else
		if(obj->Get(BRUSH_COLLISION))	g_script.m_handlerdata[1] = 2;   // keep colliding (touch)
		else							g_script.m_handlerdata[1] = 1;   // entering collision
		g_script.CallHandler(HANDLER_COLLISION);

		obj->Set(BRUSH_COLLISION,collision);
	}

	unguard()
}

BOOL cDizPlayer::CheckCollidersSnap()
{
	guard(cDizPlayer::CheckCollidersSnap)
	int x1,y1,x2,y2;
	int cx1,cy1,cx2,cy2;
	MakeBBW(x1,y1,x2,y2);

	int stepu=0;			// move player up (deepest entering collider)
	int stepd=DIZ_STEPY+1;	// move player down (space below to the closest collider)

	BOOL snap=FALSE;

	for( int i=0; i<g_game.m_collidercount; i++ )
	{
		int idx = g_game.m_collider[i];
		tBrush* obj = g_map.ObjGet(idx); sassert(obj);
		if( obj->Get(BRUSH_DISABLE)!=0 ) continue; // only enabled objects
		if(!(obj->Get(BRUSH_COLLIDER) & COLLIDER_HARD)) continue; // only those that need it
		obj->MakeBBW(cx1,cy1,cx2,cy2);
		if( x2<=cx1 || x1>=cx2 ) continue;
		
		if(y2<=cy1 && cy1<y2+DIZ_STEPY+1)	// collider's top is inside player's box
			stepd = MIN(stepd,cy1-y2);

		if(y1<=cy1 && cy1<y2)				// collider's top is not too far under the player's bottom ( a smaller interval would be (y2-DIZ_STEPY<=cy1 && cy1<y2) )
			stepu = MAX(stepu,y2-cy1);
	}	

	if(stepu>0)
	{
		snap = TRUE;
		DATA(P_Y) -= MIN(stepu,DIZ_STEPY);
	}
	else
	if(stepd<=DIZ_STEPY)
	{
		int stepd2 = CheckFallY(stepd);
		if(stepd2==stepd)
		{
			snap = TRUE;
			DATA(P_Y) += stepd;
		}
	}

	if(snap && DATA(P_STATUS)==STATUS_FALL) // stop falls
	{
		EnterRoll();
	}

	return snap;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// MATERIALS INFO
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizPlayer::ReadMatInfo()
{
	guard(cDizPlayer::ReadMatInfo)
	int x1,y1,x2,y2;
	MAKEBB(x1,y1,x2,y2);
	
	// inside
	DATA(P_MATINSIDE) = 0;
	for(int y=y1;y<y2;y++)
	{
		for(int x=x1;x<x2;x++)
		{
			DATA(P_MATINSIDE) |= (1<<g_game.MatMap(x,y));
		}
	}	

	// under
	DATA(P_MATUNDER) = 0;
	for(int x=x1;x<x2;x++)
	{
		DATA(P_MATUNDER) |= (1<<g_game.MatMap(x,y2));
	}

	// center
	DATA(P_MATCENTER) = g_game.MatMap((x1+x2)/2,(y1+y2)/2);

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// DRAW
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizPlayer::Draw()
{
	guard(cDizPlayer::Draw)
	
	if( DATA(P_DISABLE)) return; // disabled

	int rx = MAP_POSX2ROOMX(DATA(P_X));
	int ry = MAP_POSY2ROOMY(DATA(P_Y));
	
	// tile	
	cTile* tile = FindTile();
	if(tile)
	{
		int frame = ComputeFrame(DATA(P_FRAME),tile->m_frames,DATA(P_ANIM));
		int blend = DATA(P_SHADER);
		int w = tile->GetWidth();
		int h = tile->GetHeight();
		int x = DATA(P_X)-rx*ROOM_W - w/2; // @TODO need -1 to the MatchX offset because of the 25 vs 24 width bla bla
		int y = DATA(P_Y)-ry*ROOM_H + DATA(P_H)/2 - h;
		x += g_game.m_viewx;
		y += g_game.m_viewy;
		fRect src;
		src.x1 = (float)(frame*w);
		src.x2 = (float)((frame+1)*w);
		src.y1 = 0.0f;
		src.y2 = (float)h;
		fV2 pos(SCALEX(x), SCALEY(y));
		R9_SetState(R9_STATE_BLEND,blend);
		R9_DrawSprite( pos, src, tile->m_tex, DATA(P_COLOR), DATA(P_FLIP), (float)SCALE );
	}

	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
