//////////////////////////////////////////////////////////////////////////////////////////////////
// DizDef.h
//
// Versions releases:
// v0.0		prerelease 
// v1.0b	beta testing release
// v1.0		first release version
// v1.2		major release (huddraw,customload,etc)
// v1.3		vertexbuffer support (to solve flickering pbl)
// v1.31	thread problems fixed (on dual core)
// v2.0		major release (free updated sources)
// v2.1b	beta (bug fixes and upgrades)
// v2.1		important release (bug fixes and upgrades)
// v2.1.1	bug fixed to support gamepad (engine)
// v2.2		major release (font,languages,render reset,audio optimization)
// v2.2.1	small features (joystick, custom game name)
// v2.3		native scrolling and flip
// v2.3.1	editor fix import old maps
// v2.4		custom game resolution, get system time, fix block brush
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DIZDEF_H__
#define __DIZDEF_H__

#include "E9System.h"
#include "E9Math.h"
#include "E9List.h"
#include "E9String.h"
#include "E9Hash.h"
#include "D9Debug.h"
#include "F9Files.h"
#include "I9Input.h"
#include "A9Audio.h"
#include "R9Img.h"
#include "R9ImgLoader.h"
#include "R9Render.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// System
//////////////////////////////////////////////////////////////////////////////////////////////////
#define GAME_NAME			"DizzyAGE"					// application name
#define GAME_VERSION		"2.4"						// application version (change this with each change, at least 2 digits)

#define GAME_SCRWB			320							// default screen width with borders
#define GAME_SCRHB			240							// default screen height with borders
#define GAME_SCRW			256							// default screen width
#define GAME_SCRH			192							// default screen height
#define GAME_ROOMW			240							// default room width
#define GAME_ROOMH			136							// default room height
//#define GAME_MENUH		40							// menu top height
#define GAME_VIEWX			8							// view (room) pos x
#define GAME_VIEWY			48							// view (room) pos y
//#define GAME_VIEWW			240							// view (room) width
//#define GAME_VIEWH			136							// view (room) height
//#define GAME_VIEWB			16							// border size
//#define GAME_VIEWWB			(GAME_VIEWW+2*GAME_VIEWB)	// view width bordered
//#define GAME_VIEWHB			(GAME_VIEWH+2*GAME_VIEWB)	// view height bordered
#define GAME_LAYERS			8							// as many as in editor

#define GAME_FPS			36							// update every 27 ms => logic fps=37

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
