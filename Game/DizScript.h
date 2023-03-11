//////////////////////////////////////////////////////////////////////////////////////////////////
// DizScript.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DIZSCRIPT_H__
#define __DIZSCRIPT_H__

#include "gs.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Handlers (non-latent)
//////////////////////////////////////////////////////////////////////////////////////////////////
#define HANDLER_GAMEINIT		0
#define HANDLER_GAMESTART		1
#define HANDLER_GAMEUPDATE		2
#define HANDLER_GAMEAFTERUPDATE	3
#define HANDLER_ROOMOPEN		4
#define HANDLER_ROOMCLOSE		5
#define HANDLER_ROOMOUT			6
#define HANDLER_COLLISION		7
#define HANDLER_FALL			8
#define HANDLER_JUMP			9
#define HANDLER_PLAYERUPDATE		10
#define HANDLER_ACTION			11
#define HANDLER_MENU			12
#define HANDLER_DRAWHUD			13
#define HANDLER_MUSICLOOP		14
#define HANDLER_DEBUG			15
#define HANDLER_RELOADMAP		16
#define HANDLER_RELOADSCRIPT		17
#define HANDLER_MAX				18

#define	HANDLER_MAXDATA			8



#define B_LAYER			0	
#define B_X				1	
#define B_Y				2	
#define B_W				3	
#define B_H				4	
#define B_TILE			5	
#define B_FRAME			6
#define B_MAP			7	
#define B_FLIP			11	
#define B_COLOR			12	
#define B_SHADER		13	
#define B_SCALE			14
#define B_ID			17	
#define B_MATERIAL		18	
#define B_DRAW			19	
#define B_DELAY			21
#define B_ANIM			22
#define B_MAX			48	

#define O_LAYER			0	
#define O_X				1	
#define O_Y				2	
#define O_W				3	
#define O_H				4	
#define O_TILE			5	
#define O_FRAME			6	
#define O_MAP			7	
#define O_FLIP			11	
#define O_COLOR			12	
#define O_SHADER		13	
#define O_SCALE			14
#define O_ID			17	
#define O_MATERIAL		18	
#define O_DRAW			19	
#define O_DISABLE		20	
#define O_DELAY			21	
#define O_ANIM			22
#define O_COLLIDER		23	
#define O_CLASS			24	
#define O_STATUS		25	
#define O_TARGET		26	
#define O_DEATH			27
#define O_COLLISION		31
#define O_USER			32	
#define O_MAX			48	

#define R_MAX			ROOM_PROPS

//////////////////////////////////////////////////////////////////////////////////////////////////
// Script class
//////////////////////////////////////////////////////////////////////////////////////////////////
class cDizScript
{
public:
						cDizScript			();
						~cDizScript			();

		BOOL			Init				();					// init
		void			Done				();					// done
		void			Start				();					// call this to start a new game
		void			Update				();					// update and run current code
		BOOL			Reload				();					// for debug purposes

		// enviroment and virtual machines
		gsEnv*			m_env;									// script enviroment
		gsVM*			m_vm0;									// virtual machine 0 (used for non-latent functions like handlers)
		gsVM*			m_vm1;									// virtual machine 1 (used for latent functions like cutscenes)
		int				m_request;								// requested latent function to run
		int				m_running;								// running latent function (-1=not busy)
		int				m_killlatent;							// request kill latent function if running
		BOOL			Request( char* fnname );				// request to ran a latent function
inline	BOOL			IsBusy()								{ return m_running!=-1; }

		// handlers
		int				m_handler[HANDLER_MAX];					// handlers function indexes for fast call
static	char*			m_handlername[HANDLER_MAX];				// handlers names
		int				m_handlerdata[HANDLER_MAXDATA];			// handler data parameters (temporary transport)
		void			CallHandler			( int handler );	// call a handler

		// register
		void			ScriptRegister		( gsVM* vm );		// register script functions and constants
};

extern cDizScript g_script;

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
