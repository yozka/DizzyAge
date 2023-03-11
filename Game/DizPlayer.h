//////////////////////////////////////////////////////////////////////////////////////////////////
// DizPlayer.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DIZPLAYER_H__
#define __DIZPLAYER_H__

#include "E9System.h"
#include "D9Debug.h"
#include "DizPaint.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

#define STATUS_IDLE			0
#define STATUS_WALK			1
#define STATUS_JUMP			2
#define STATUS_FALL			3
#define STATUS_SCRIPTED		4

#define TILE_IDLE			10			// default 
#define TILE_WALK			11			// default 
#define TILE_UP				12			// default 
#define TILE_JUMP			13			// default 
#define TILE_STUN			14			// default 
#define TILE_DEAD			15			// default 

#define DIZ_STEPX			4			// move step x
#define DIZ_STEPY			4			// move step y; used in adjustments
#define DIZ_STEPYMAX		7			// move step y; used in jumps and falls
#define DIZ_BOXW			16			// collision box width
#define DIZ_BOXH			20			// collision box height

//////////////////////////////////////////////////////////////////////////////////////////////////

#define	P_DISABLE			0			// if disabled no update and no draw
#define	P_STATUS			1			// player status
#define	P_DELAY				2			// update skips 'delay' frames (different from the animation delay that is in tile)
#define	P_LAYER				3			// player's layer
#define	P_X					4			// position x in world
#define	P_Y					5			// position y in world
#define	P_W					6			// width of the object (usually the sprite width); read-only
#define	P_H					7			// height of the object (usually the sprite height); read-only

#define	P_TILE				8			// the current tile used to draw
#define	P_FRAME				9			// the animation frame in the tile (starts with 0)
#define	P_ANIM				10			// animation playing mode (0=stop,1=normal,2=loop)
#define	P_FLIP				11			// flip: 0=none, 1=x (not yet: 2=y, 3=xy)
#define	P_COLOR				12			// color used to draw
#define	P_SHADER			13			// color used to draw
#define	P_EMOTION			14			// emotion (tile id offset from tileidle); 0=no emmotion, just idle

#define	P_DIR				15			// direction -1,0,1
#define	P_POW				16			// power jump/fall (positive value used to measure jumps or falls)

#define	P_COSTUME			17			// tile costume (id offset)
#define	P_TILEIDLE			18			// tile for idle status
#define	P_TILEWALK			19			// tile for walk status
#define	P_TILEUP			20			// tile for jump up status
#define	P_TILEJUMP			21			// tile for jump side status

#define	P_AIRLEVEL			22			// normal air 0 (full); critical AIR_LEVEL=100 (empty)
#define P_STUNLEVEL			23			// normal stun 0 (none); critical STUN_LEVEL=20 (stunned)
#define P_MATINSIDE			24			// materials inside player's box on bits
#define P_MATUNDER			25			// materials right under player's box on bits
#define P_MATCENTER			26			// material in center of player's box (at mouth point)

#define P_LIFE				27			// life energy [0,100]
#define P_CREDITS			28			// life credits [0,2]
#define P_COINS				29			// collected coins

#define P_XSAFE				30			// last safe position x in world (stable)
#define P_YSAFE				31			// last safe position x in world (stable)
#define P_SAFE				32			// if player is safe to update safe position (stable)

#define P_CUSTOMMOVE		33			// custom movement for player
#define P_DEATH				34			// cause of death

#define	P_USER				64			// user defined
#define	P_MAX				128


#define COLLIDER_HANDLE					(1<<0)
#define COLLIDER_HARD					(1<<1)

//////////////////////////////////////////////////////////////////////////////////////////////////
// cDizPlayer
//////////////////////////////////////////////////////////////////////////////////////////////////
class cDizPlayer
{
public:
					cDizPlayer			();
					~cDizPlayer			();

		// init
		void		Reset				();											// reset when start game
virtual	void		Update				();
virtual	void		Draw				();

		// enter states
		void		EnterIdle			();											// enter idle status
		void		EnterWalk			( int dir );								// enter walk status
		void		EnterJump			( int dir, int pow );						// enter jump status
		void		EnterFall			();											// enter fall status
		void		EnterRoll			();											// enter a roll (keep fall) or idle if ready
		void		EnterJumper			( int mat );								// enter on a jumper with mat material
		void		EnterSpin			( int dir );								// enter a forced roll
		void		EnterKeyState		();											// enter state requested by input keys

		// update states
		void		UpdateIdle			();
		void		UpdateWalk			();
		void		UpdateJump			();
		void		UpdateFall			();
		void		UpdateScripted		();
		
		// checks
		BOOL		CheckFree			( int x1, int y1, int x2, int y2 );			// general hard material in-box test
		BOOL		CheckWalkX			();											// test horizontal movement space when walking
		BOOL		CheckJumpX			();											// test horizontal movement space when jumping
		int			CheckJumpY			( int step );								// test for space above the player when jumping for a specified number of lines; return reduced step if collision detected
		BOOL		CheckFallX			();											// test horizontal movement space when falling
		int			CheckFallY			( int step );								// test material under player for a specified number of lines; return reduced step if collision detected
		void		CheckCollision		();											// test material hard collision and push player up
		int			CheckJumper			();											// test for jumper right under player; return jumper material or -1
		void		CheckColliders		();											// call collision handler for touched colliders that want it
		BOOL		CheckCollidersSnap	();											// test colliders collision and snap player if necessary

		void		ReadMatInfo			();											// read materials info
		
		// utils
inline	void		MakeBBW				( int &x1, int &y1, int &x2, int &y2 )		{ x1 = m_data[P_X]-m_data[P_W]/2; x2 = m_data[P_X]+m_data[P_W]/2;	y1 = m_data[P_Y]-m_data[P_H]/2; y2 = m_data[P_Y]+m_data[P_H]/2; }
inline	cTile*		FindTile			()											{ return g_paint.TileGet(g_paint.TileFind(m_data[P_TILE])); }
		
		// data
inline	int*		Data				( int idx )									{ return &m_data[idx]; }
inline	int			Get					( int idx )									{ if(idx>=0 && idx<P_MAX) return m_data[idx]; else return 0; }
inline	void		Set					( int idx, int val )						{ if(idx>=0 && idx<P_MAX) m_data[idx] = val; }

		int			m_data[P_MAX];		// user data

		// not exported
		BOOL		m_input;			// user input accepted
		BOOL		m_debug;			// prevent player's update while debug movement
};

extern	cDizPlayer	g_player;

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
