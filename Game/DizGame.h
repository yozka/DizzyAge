//////////////////////////////////////////////////////////////////////////////////////////////////
// DizGame.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DIZGAME_H__
#define __DIZGAME_H__

#include "E9System.h"
#include "D9Debug.h"
#include "DizCfg.h"
#include "DizPaint.h"
#include "DizMap.h"
#include "DizPlayer.h"
#include "DizScript.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////////////////////////

#define	G_FPS							0			// fps
#define	G_KEYS							1			// input keys: bit 0=KEY_LEFT, 1=KEY_RIGHT, 2=KEY_UP, 3=KEY_DOWN, 4=KEY_JUMP, 5=KEY_ACTION, 6=KEY_MENU
#define	G_KEYSHIT						2			// input keys: bit 0=KEY_LEFT, 1=KEY_RIGHT, 2=KEY_UP, 3=KEY_DOWN, 4=KEY_JUMP, 5=KEY_ACTION, 6=KEY_MENU; tells if the key was just hitted down this frame !
#define	G_ROOMX							3			// current room x
#define	G_ROOMY							4			// current room y
#define	G_ROOMW							5			// room width
#define	G_ROOMH							6			// room height
#define	G_MAPW							7			// current map width (in rooms)
#define	G_MAPH							8			// current map height (in rooms)
#define	G_VIEWX							9			// position of room's view on the hud
#define	G_VIEWY							10			// position of room's view on the hud
#define	G_PAUSE							11			// if game is paused
#define	G_SHAKEX						12			// view x offset used for shaking
#define	G_SHAKEY						13			// view y offset used for shaking
#define	G_MAPCOLOR						14			// map background color
#define	G_BORDERCOLOR					15			// window border color
#define	G_FFMAGNITUDE					16			// force feedback magnitude [0..100]
#define	G_FFPERIOD						17			// force feedback period (miliseconds)
#define	G_VIEWPORTMODE					18			// viewport mode: 0=normal, 1=extended to 3x3 rooms for scrolling
#define	G_VIEWPORTX						19			// viewport x offset
#define	G_VIEWPORTY						20			// viewport y offset
#define	G_VIEWPORTFLIP					21			// viewport flip options (bit 0=x, 1=y)
#define	G_FULLMATERIALMAP				22			// material map size: 0=normal (current room with small border), 1=extended to 3x3 rooms for scrolling

#define	G_INVENTORY						50			// inventory slots (max 10, up to 60)
#define	G_USER							64			// user defined
#define G_MAX							256			// dummy

#define CMD_NONE						0			// nothing to do
#define CMD_START						1			// must start game
#define CMD_EXIT						2			// must exit game
#define CMD_REFRESH						3			// refresh room material map

#define DRAWMODE_NORMAL					0			// normal
#define DRAWMODE_MATERIAL				1			// show materials colors
#define DRAWMODE_DENSITY				2			// show materials densities
#define DRAWMODE_NONE					3			// don't draw
#define DRAWMODE_MAX					4			// dummy

//////////////////////////////////////////////////////////////////////////////////////////////////
// MATERIALS
//////////////////////////////////////////////////////////////////////////////////////////////////
#define	MAT_MAX				32		// dummy

#define MATD_VOID			0		// fall through
#define MATD_SOFT			1		// stop fall
#define MATD_HARD			2		// stop fall, collide
#define MATD_JUMP			3		// jumper
#define MATD_MAX			4		// dummy

#define MATDENSITY( mat )	g_game.m_matdensity[mat]
#define MATCOLOR( mat )		g_game.m_matcolor[mat]


//////////////////////////////////////////////////////////////////////////////////////////////////
// DIALOGS
//////////////////////////////////////////////////////////////////////////////////////////////////
#define	DLG_STYLE		0				// style
#define	DLG_X			1				// x in scr pixels
#define	DLG_Y			2				// y in scr pixels
#define	DLG_W			3				// w in scr pixels
#define	DLG_H			4				// h in scr pixels
#define	DLG_COLOR		5				// user color (usualy for border)
#define	DLG_FONT		6				// font tile id
#define DLG_USER		8				// user defined
#define DLG_MAX			16

struct tDialog
{
		int			m_data[DLG_MAX];	// dialog data
		char*		m_text;				// dialog text
					tDialog()												{ m_text=NULL; memset(m_data,0,sizeof(m_data)); }
					~tDialog()												{ if(m_text) sfree(m_text); m_text=NULL; }
inline	int			Get( int idx )											{ return m_data[idx]; }
inline	void		Set( int idx, int val )									{ m_data[idx] = val; }
inline	char*		GetText()												{ return m_text; }
inline	void		SetText( char* text )									{ if(m_text) sfree(m_text); m_text = sstrdup(text); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// cDizGame
//////////////////////////////////////////////////////////////////////////////////////////////////

class cDizGame
{
public:
						cDizGame			();
						~cDizGame			();

		// init
		BOOL			Init				();					// init
		void			Done				();					// done
		BOOL			Start				();					// call this to start a new game
		BOOL			Update				();					// update game (called periodical to match the game fps) return FALSE to exit game
		void			Draw				();					// draw game
		BOOL			CheckVersion		();					// check game version (first 2 digits)
		
		// settings
		int				m_screen_bw;							// game resolution bordered width (320)
		int				m_screen_bh;							// game resolution bordered height (200)
		int				m_screen_w;								// game resolution width (256 - Z80 res)
		int				m_screen_h;								// game resolution height (192 - Z80 res)

		// game
		int				m_gameframe;							// game frame index
		int				m_command;								// game command request
inline	BOOL			IsUpdate			( int delay )		{ return (delay==0 || (m_gameframe%delay==0)); }
inline	BOOL			Key( int key )							{ return (Get(G_KEYS) & (1<<key)) ? 1 : 0; }			// test a key
inline	BOOL			KeyHit( int key )						{ return (Get(G_KEYSHIT) & (1<<key)) ? 1 : 0; }			// test a key hit

		// materials
		int				m_matdensity[MAT_MAX];					// density of materials
		int				m_matcolor[MAT_MAX];					// colors of materials

		// menu		
//		void			MenuDraw			();					// draw menu top and borders
//		void			MenuDrawCover		();					// draw menu center cover


		// dialogs
		void			DlgPush				();					// create a new dialog and push it on the stack
		void			DlgPop				();					// destroy top dialog from the stack
		void			DlgPopAll			();					// pop all dialogs
inline	tDialog*		DlgTop				()					{ if(m_dlg.Size()>0) return m_dlg.Get(m_dlg.Size()-1); else return NULL; }
		cPList<tDialog>	m_dlg;									// dialogs stack


		// map room
		void			SetRoom				( int x, int y );	// set current room (load)
inline	void			MakeRoomBBW			( int &x1, int &y1, int &x2, int &y2, int border=0 )	{ x1=m_data[G_ROOMX]*ROOM_W-border; y1=m_data[G_ROOMY]*ROOM_H-border; x2=(m_data[G_ROOMX]+1)*ROOM_W+border; y2=(m_data[G_ROOMY]+1)*ROOM_H+border; }

		void			MatMapAlloc			();					// call after map is loaded and roomw and roomh are set
inline	byte			MatMap				( int x, int y );
		void			MatMapUpdate		();					// paint material map SOFTWARE
		byte*			m_matmap;								// material map (3x3 rooms, with current room in the middle)
		int				m_viewx;								// view position (used in draw, set from G_VIEW, G_SHAKE, and G_VIEWPORT)
		int				m_viewy;								// view position (used in draw, set from G_VIEW, G_SHAKE, and G_VIEWPORT)
		int				m_drawmode;								// 0=imgmap (normal), 1=matmap, 2=densitymap, 3=none
			
		// objects
inline	void			ObjAdd				( int idx );		// add object to present lists (objects and coliders)
		void			ObjGather			();					// gather objects present in current room
		void			ObjPresent			( int idx );		// add object to present list if not in it already
		void			ObjDraw				( tBrush* brush	);	// draw one object
		int				m_objcount;								// current objects count in list
		int				m_objsize;								// objects list size
		int*			m_obj;									// objects list with objects indexes (present in current room)
		int				m_collidercount;						// current coliders count in list
		int				m_collidersize;							// colliders list size
		int*			m_collider;								// colliders list with objects indexes (present in current room)


		// globals variables
		int				m_data[G_MAX];							// global data
inline	int				Get					( int idx )			{ return m_data[idx]; }
inline	void			Set					( int idx, int val ){ m_data[idx] = val; }

		// FFFX Rumble
		int				m_fffx_magnitude;						// force magnitude [0,100] (0=stopped)
		int				m_fffx_period;							// force period in miliseconds (50=default)
		void			FFFXUpdate();							// update force feedback
inline	void			FFFXStop()								{ m_fffx_magnitude=0; Set(G_FFMAGNITUDE,0); FFFXUpdate(); }

		// stats
		int				m_visible_brushes;
};

inline void cDizGame::ObjAdd( int idx )
{
	tBrush* obj = g_map.m_obj.Get(idx);

	if(m_objcount==m_objsize) // resize list
	{
		m_objsize += 64;
		m_obj = (int*)srealloc(m_obj,m_objsize*sizeof(int));
	}
	m_obj[m_objcount] = idx;
	m_objcount++;

	if( obj->Get(BRUSH_COLLIDER) )
	{
		if(m_collidercount==m_collidersize) // resize list
		{
			m_collidersize += 64;
			m_collider = (int*)srealloc(m_collider,m_collidersize*sizeof(int));
		}
		m_collider[m_collidercount] = idx;
		m_collidercount++;
	}

	obj->Set(BRUSH_COLLISION,0); // reset collision
}

extern cDizGame g_game;

inline byte cDizGame::MatMap( int x, int y )	
{ 
	if(x>=-ROOM_W && x<2*ROOM_W && y>=-ROOM_H && y<2*ROOM_H && m_matmap) 
		return *(m_matmap+(x+ROOM_W)+(y+ROOM_H)*ROOM_W*3); 
	else 
		return 0; 
}

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
